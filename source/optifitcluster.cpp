//
//  optifitcluster.cpp
//  Mothur
//
//  Created by Sarah Westcott on 5/10/18.
//  Copyright © 2018 Schloss Lab. All rights reserved.
//

#include "optifitcluster.hpp"


/***********************************************************************/
OptiFitCluster::OptiFitCluster(OptiData* mt, ClusterMetric* met, long long ns) : Cluster(), matrix(mt), metric(met), numComboSingletons(ns) {
    m = MothurOut::getInstance();
    maxRefBinNumber = 0;

    numFitSeqs = 0; numFitSingletons = 0; fittruePositives = 0; fitfalsePositives = 0; fitfalseNegatives = 0; fittrueNegatives = 0;
    numComboSeqs = 0; numComboSingletons = 0; combotruePositives = 0; combofalsePositives = 0; combofalseNegatives = 0; combotrueNegatives = 0;
}
/***********************************************************************/
int OptiFitCluster::initialize(double& value, bool randomize, vector<vector< string > > existingBins, vector<string> bls) {
    try {
        long long reftruePositives, reftrueNegatives, reffalsePositives, reffalseNegatives, numRefSeqs, numRefSingletons;
        numRefSeqs = 0; numRefSingletons = 0; reftruePositives = 0; reffalsePositives = 0; reffalseNegatives = 0; reftrueNegatives = 0;
        
        vector< vector< int> > translatedBins;
        randomizeSeqs = matrix->getNumSeqs(existingBins, translatedBins); //otus in existingBins, otus with matrix names
        numFitSeqs = randomizeSeqs.size();
        for (int i = 0; i < numFitSeqs; i++) { fitSeqs.insert(randomizeSeqs[i]); }
        
        int binNumber = 0;
        int placeHolderIndex = -1;
        for (int i = 0; i < translatedBins.size(); i++) {
            binLabels[binNumber] = bls[i];
            bins.push_back(translatedBins[i]);
            numRefSeqs += translatedBins[i].size();
            
            for (int j = 0; j < translatedBins[i].size(); j++) {
                for (int k = 0; k < j; k++) {
                    if (translatedBins[i][j] < 0) { //no dists in matrix
                        translatedBins[i][j] = placeHolderIndex; placeHolderIndex--;
                        reffalsePositives++;
                    }else { //j has distances in the matrix, but is it close to k?
                        if (matrix->isClose(translatedBins[i][j], translatedBins[i][k])) {
                            reftruePositives++;
                        }else { reffalsePositives++; }
                    }
                }
                seqBin[translatedBins[i][j]] = binNumber;
            }
            binNumber++;
        }
        
        maxRefBinNumber = binNumber;
        reffalseNegatives = matrix->getNumRefDists() - reftruePositives; //number of distance in matrix for reference seqs - reftruePositives
        reftrueNegatives = numRefSeqs * (numRefSeqs-1)/2 - (reffalsePositives + reffalseNegatives + reftruePositives);
        
        double refValue = metric->getValue(reftruePositives, reftrueNegatives, reffalsePositives, reffalseNegatives);
        
        cout << "reference mcc " << refValue << endl;
        /*
         //randomly assigns fit seqs to an OTU
         for (int i = 0; i < numSeqs; i++) {
         set<int> refBinsToTry;
         set<int> closeSeqs = matrix->getCloseSeqs(randomizeSeqs[i]);
         for (set<int>::iterator itClose = closeSeqs.begin(); itClose != closeSeqs.end(); itClose++) {
         if (fitSeqs.count(*itClose) == 0) {  refBinsToTry.insert(seqBin[*itClose]);  } //if you aren't a fit seq you are a ref seq
         }
         
         int assignedBin = 0;
         if (refBinsToTry.size() == 0) { assignedBin = util.getRandomIndex(bins.size()-1); } //you aren't close to any reference seqs so just randomly pick one
         else { //randomly select a reference bin to assign the seq to
         int location = util.getRandomIndex(refBinsToTry.size()-1);
         set<int>::const_iterator it(refBinsToTry.begin());
         advance(it,location);
         assignedBin = *it;
         }
         
         bins[assignedBin].push_back(randomizeSeqs[i]);
         seqBin[randomizeSeqs[i]] = assignedBin;
         }
         */
        
        //add fit seqs as singletons
        int numRefBins = translatedBins.size()-1;
        //put every fit seq in own bin
        for (int i = 0; i < numFitSeqs; i++) {
            vector<int> thisBin;
            thisBin.push_back(randomizeSeqs[i]);
            bins.push_back(thisBin);
            seqBin[randomizeSeqs[i]] = numRefBins+i;
            
            long long numCloseSeqs = (matrix->getNumFitClose(randomizeSeqs[i])); //does not include self
            fitfalseNegatives += numCloseSeqs;
        }
        
        fitfalseNegatives /= 2; //square matrix
        fittrueNegatives = numFitSeqs * (numFitSeqs-1)/2 - (fitfalsePositives + fitfalseNegatives + fittruePositives); //since everyone is a singleton no one clusters together. True negative = num far apart
        
        double fitValue = metric->getValue(fittruePositives, fittrueNegatives, fitfalsePositives, fitfalseNegatives);
        
        cout << "fit intial mcc " << fitValue << endl;
        numComboSeqs = numRefSeqs + numFitSeqs;
        
        combofalseNegatives = matrix->getNumDists() - reftruePositives; //number of distance in matrix for reference seqs - reftruePositives
        combotrueNegatives = numComboSeqs * (numComboSeqs-1)/2 - (reffalsePositives + reffalseNegatives + reftruePositives);
        combotruePositives = reftruePositives;
        combofalsePositives = reffalsePositives;
        
        double comboValue = metric->getValue(combotruePositives, combotrueNegatives, combofalsePositives, combofalseNegatives);
        
        cout << "combo intial mcc " << comboValue << endl;
        
        //add insert location
        seqBin[bins.size()] = -1;
        insertLocation = bins.size();
        vector<int> temp;
        bins.push_back(temp);
        
        if (randomize) { util.mothurRandomShuffle(randomizeSeqs); }
        
        for (map<int, int>::iterator it = seqBin.begin(); it != seqBin.end(); it++) { cout << it->first << '\t' << it->second <<endl; }
        
        return value;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "initialize");
        exit(1);
    }
}
/***********************************************************************/
/* for each sequence with mutual information (close)
 * remove from current OTU and calculate MCC when sequence forms its own OTU or joins one of the other OTUs where there is a sequence within the `threshold` (no need to calculate MCC if the paired sequence is already in same OTU and no need to try every OTU - just those where there's a close sequence)
 * keep or move the sequence to the OTU where the `metric` is the largest - flip a coin on ties */
bool OptiFitCluster::update(double& listMetric) {
    try {
        
        //for each sequence (singletons removed on read)
        for (int i = 0; i < randomizeSeqs.size(); i++) {
            
            if (m->getControl_pressed()) { break; }
            
            map<int, int>::iterator it = seqBin.find(randomizeSeqs[i]);
            
            int seqNumber = it->first;
            int binNumber = it->second;
            
            cout << seqNumber << '\t' << binNumber << endl;
            
            if (binNumber == -1) { }
            else {
                vector<long long> bestBin; bestBin.resize(2, binNumber);
                vector<long long> tn; tn.push_back(fittrueNegatives); tn.push_back(combotrueNegatives);
                vector<long long> tp; tp.push_back(fittruePositives); tp.push_back(combotruePositives);
                vector<long long> fp; fp.push_back(fitfalsePositives); fp.push_back(combofalsePositives);
                vector<long long> fn; fn.push_back(fitfalseNegatives); fn.push_back(combofalseNegatives);
                vector<double> bestMetric; bestMetric.resize(2, -1);  //bestMetric[0] = fitSeqs alone, bestMetric[1] = combo or ref and fit
                
                vector<long long> bestTp; bestTp.resize(2, 0);
                vector<long long> bestTn; bestTn.resize(2, 0);
                vector<long long> bestFp; bestFp.resize(2, 0);
                vector<long long> bestFn; bestFn.resize(2, 0);
            
                //close / far count in current bin
                vector<long long> results = getCloseFarCounts(seqNumber, binNumber);
                long long combocCount = results[0];  long long combofCount = results[1];
                
                //close / far count in current bin for fit seqs
                vector<long long> fitresults = getCloseFarFitCounts(seqNumber, binNumber);
                long long fitcCount = fitresults[0];  long long fitfCount = fitresults[1];
                
                //fit metrics in current bin
                bestMetric[0] = metric->getValue(tp[0], tn[0], fp[0], fn[0]);
                bestTp[0] = tp[0]; bestTn[0] = tn[0]; bestFp[0] = fp[0]; bestFn[0] = fn[0];
                
                //combo metric in current bin
                bestMetric[1] = metric->getValue(tp[1], tn[1], fp[1], fn[1]);
                bestTp[1] = tp[1]; bestTn[1] = tn[1]; bestFp[1] = fp[1]; bestFn[1] = fn[1];
                
                //if not already singleton, then calc value if singleton was created
                if (!((bins[binNumber].size()) == 1)) {
                    //make a singleton
                    fn[0]+=fitcCount; tn[0]+=fitfCount; fp[0]-=fitfCount; tp[0]-=fitcCount;
                    fn[1]+=combocCount; tn[1]+=combofCount; fp[1]-=combofCount; tp[1]-=combocCount;
                    
                    double singleFitMetric = metric->getValue(tp[0], tn[0], fp[0], fn[0]);
                    if (singleFitMetric > bestMetric[0]) {
                        bestBin[0] = -1; bestTp[0] = tp[0]; bestTn[0] = tn[0]; bestFp[0] = fp[0]; bestFn[0] = fn[0];
                        bestMetric[0] = singleFitMetric;
                    }
                    
                    double singleComboMetric = metric->getValue(tp[1], tn[1], fp[1], fn[1]);
                    if (singleComboMetric > bestMetric[1]) {
                        bestBin[1] = -1; bestTp[1] = tp[1]; bestTn[1] = tn[1]; bestFp[1] = fp[1]; bestFn[1] = fn[1];
                        bestMetric[1] = singleComboMetric;
                    }
                }
                
                set<int> binsToTry;
                set<int> closeSeqs = matrix->getCloseRefSeqs(seqNumber);
                for (set<int>::iterator itClose = closeSeqs.begin(); itClose != closeSeqs.end(); itClose++) {
                    cout << "close to " << *itClose << " try bin " << seqBin[*itClose] << endl;
                    binsToTry.insert(seqBin[*itClose]);
                }
                
                //merge into each "close" otu
                for (set<int>::iterator it = binsToTry.begin(); it != binsToTry.end(); it++) {
                    //reset tn, tp,fp,fn values to original bin
                    tn[0] = fittrueNegatives; tp[0] = fittruePositives; fp[0] = fitfalsePositives; fn[0] = fitfalseNegatives;
                    tn[1] = combotrueNegatives; tp[1] = combotruePositives; fp[1] = combofalsePositives; fn[1] = combofalseNegatives;
                    
                    //move out of old bin
                    fn[0]+=fitcCount; tn[0]+=fitfCount; fp[0]-=fitfCount; tp[0]-=fitcCount;
                    fn[1]+=combocCount; tn[1]+=combofCount; fp[1]-=combofCount; tp[1]-=combocCount;
                    
                    results = getCloseFarCounts(seqNumber, *it);
                    fn[1]-=results[0]; tn[1]-=results[1];  tp[1]+=results[0]; fp[1]+=results[1]; //move into new bin
                    
                    results = getCloseFarFitCounts(seqNumber, *it);
                    fn[0]-=results[0]; tn[0]-=results[1];  tp[0]+=results[0]; fp[0]+=results[1]; //move into new bin - only consider fit seqs
                    
                    double newComboMetric = metric->getValue(tp[1], tn[1], fp[1], fn[1]); //score when sequence is moved
                    double newFitMetric = metric->getValue(tp[0], tn[0], fp[0], fn[0]); //score when sequence is moved
                    //new best
                    if (newFitMetric > bestMetric[0]) { bestMetric[0] = newFitMetric; bestBin[0] = (*it); bestTp[0] = tp[0]; bestTn[0] = tn[0]; bestFp[0] = fp[0]; bestFn[0] = fn[0]; }
                    if (newComboMetric > bestMetric[1]) { bestMetric[1] = newComboMetric; bestBin[1] = (*it); bestTp[1] = tp[1]; bestTn[1] = tn[1]; bestFp[1] = fp[1]; bestFn[1] = fn[1]; }
                    
                    cout << "trying bin " << *it << '\t' << bestMetric[0] << '\t' << bestMetric[1] << endl;
                }
                
                //how to choose the best bin if they differ????
                long long newBin = bestBin[1];
                
                bool usedInsert = false;
                if (newBin == -1) {  newBin = insertLocation;  usedInsert = true;  }
                
                if (newBin != binNumber) {
                    combotruePositives = bestTp[1]; combotrueNegatives = bestTn[1]; combofalsePositives = bestFp[1]; combofalseNegatives = bestFn[1];
                    fittruePositives = bestTp[0]; fittrueNegatives = bestTn[0]; fitfalsePositives = bestFp[0]; fitfalseNegatives = bestFn[0];
                    
                    //move seq from i to j
                    bins[newBin].push_back(seqNumber); //add seq to bestbin
                    bins[binNumber].erase(remove(bins[binNumber].begin(), bins[binNumber].end(), seqNumber), bins[binNumber].end()); //remove from old bin i
                }
                
                if (usedInsert) { insertLocation = findInsert(); }
                
                //update seqBins
                seqBin[seqNumber] = newBin; //set new OTU location
            }
        }
        
        listMetric = metric->getValue(combotruePositives, combotrueNegatives, combofalsePositives, combofalseNegatives);
        
        double fitListMetric = metric->getValue(fittruePositives, fittrueNegatives, fitfalsePositives, fitfalseNegatives);
        
        cout << "fit mcc value " << fitListMetric << endl << "combo mcc value " << listMetric << endl;
        
        if (m->getDebug()) { ListVector* list = getList(); list->print(cout); delete list; }
        
        return 0;
        
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "update");
        exit(1);
    }
}
/***********************************************************************/
vector<long long> OptiFitCluster::getCloseFarCounts(int seq, int newBin) {
    try {
        vector<long long> results; results.push_back(0); results.push_back(0);
        
        if (newBin == -1) { }  //making a singleton bin. Close but we are forcing apart.
        else { //merging a bin
            for (int i = 0; i < bins[newBin].size(); i++) {
                if (seq == bins[newBin][i]) {} //ignore self
                else if (!matrix->isClose(seq, bins[newBin][i])) { results[1]++; }  //this sequence is "far away" from sequence i - above the cutoff
                else { results[0]++;  }  //this sequence is "close" to sequence i - distance between them is less than cutoff
            }
        }
        
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getCloseFarCounts");
        exit(1);
    }
}
/***********************************************************************/
vector<long long> OptiFitCluster::getCloseFarFitCounts(int seq, int newBin) {
    try {
        vector<long long> results; results.push_back(0); results.push_back(0);
        
        if (newBin == -1) { }  //making a singleton bin. Close but we are forcing apart.
        else { //merging a bin
            for (int i = 0; i < bins[newBin].size(); i++) {
                bool isFit = true;
                if (seq == bins[newBin][i]) {} //ignore self
                else if (!matrix->isCloseFit(seq, bins[newBin][i], isFit)) {  //this sequence is "far away" from sequence i - above the cutoff
                    if (isFit) { results[1]++; }
                }else { results[0]++;  }  //this sequence is "close" to sequence i - distance between them is less than cutoff
            }
        }
        
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getCloseFarCounts");
        exit(1);
    }
}

/***********************************************************************/
vector<double> OptiFitCluster::getStats(long long& tp,  long long& tn,  long long& fp,  long long& fn) {
    try {
        
        long long singletn = matrix->getNumSingletons();
        long long tempnumSeqs = numComboSeqs + singletn;
        
        tp = combotruePositives;
        fp = combofalsePositives;
        fn = combofalseNegatives;
        tn = tempnumSeqs * (tempnumSeqs-1)/2 - (combofalsePositives + combofalseNegatives + combotruePositives); //adds singletons to tn
        
        vector<double> results;
        
        Sensitivity sens;   double sensitivity = sens.getValue(tp, tn, fp, fn); results.push_back(sensitivity);
        Specificity spec;   double specificity = spec.getValue(tp, tn, fp, fn); results.push_back(specificity);
        PPV ppv;            double positivePredictiveValue = ppv.getValue(tp, tn, fp, fn); results.push_back(positivePredictiveValue);
        NPV npv;            double negativePredictiveValue = npv.getValue(tp, tn, fp, fn); results.push_back(negativePredictiveValue);
        FDR fdr;            double falseDiscoveryRate = fdr.getValue(tp, tn, fp, fn); results.push_back(falseDiscoveryRate);
        Accuracy acc;       double accuracy = acc.getValue(tp, tn, fp, fn); results.push_back(accuracy);
        MCC mcc;            double matthewsCorrCoef = mcc.getValue(tp, tn, fp, fn); results.push_back(matthewsCorrCoef);
        F1Score f1;         double f1Score = f1.getValue(tp, tn, fp, fn); results.push_back(f1Score);
        
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiCluster", "getStats");
        exit(1);
    }
}
/***********************************************************************/
vector<double> OptiFitCluster::getFitStats(long long& tp,  long long& tn,  long long& fp,  long long& fn) {
    try {
        long long singletn = matrix->getNumFitSingletons();
        long long tempnumSeqs = numFitSeqs + singletn;
        
        tp = fittruePositives;
        fp = fitfalsePositives;
        fn = fitfalseNegatives;
        tn = tempnumSeqs * (tempnumSeqs-1)/2 - (fitfalsePositives + fitfalseNegatives + fittruePositives); //adds singletons to tn
        
        vector<double> results;
        
        Sensitivity sens;   double sensitivity = sens.getValue(tp, tn, fp, fn); results.push_back(sensitivity);
        Specificity spec;   double specificity = spec.getValue(tp, tn, fp, fn); results.push_back(specificity);
        PPV ppv;            double positivePredictiveValue = ppv.getValue(tp, tn, fp, fn); results.push_back(positivePredictiveValue);
        NPV npv;            double negativePredictiveValue = npv.getValue(tp, tn, fp, fn); results.push_back(negativePredictiveValue);
        FDR fdr;            double falseDiscoveryRate = fdr.getValue(tp, tn, fp, fn); results.push_back(falseDiscoveryRate);
        Accuracy acc;       double accuracy = acc.getValue(tp, tn, fp, fn); results.push_back(accuracy);
        MCC mcc;            double matthewsCorrCoef = mcc.getValue(tp, tn, fp, fn); results.push_back(matthewsCorrCoef);
        F1Score f1;         double f1Score = f1.getValue(tp, tn, fp, fn); results.push_back(f1Score);
        
        return results;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getFitStats");
        exit(1);
    }
}
/***********************************************************************/
ListVector* OptiFitCluster::getList() {
    try {
        ListVector* list = new ListVector();
        ListVector* singleton = matrix->getListSingle();
        
        if (singleton != NULL) { //add in any sequences above cutoff in read. Removing these saves clustering time.
            for (int i = 0; i < singleton->getNumBins(); i++) {
                if (singleton->get(i) != "") {
                    list->push_back(singleton->get(i));
                }
            }
            delete singleton;
        }
        
        for (int i = 0; i < bins.size(); i++) {
            vector<int> thisBin;
            for (int j = 0; j < bins[i].size(); j++) {  if (bins[i][j] >= 0) { thisBin.push_back(bins[i][j]); } }
            
            if (thisBin.size() != 0) {
                
                string otu = matrix->getName(thisBin[0]);
                
                for (int j = 1; j < thisBin.size(); j++) { otu += "," + matrix->getName(thisBin[j]); }
                list->push_back(otu);
            }
        }
        
        return list;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getList");
        exit(1);
    }
}
/***********************************************************************/
ListVector* OptiFitCluster::getFittedList(long long& unnumFitted) {
    try {
        ListVector* list = new ListVector();
        vector<string> newLabels;
        
        map<int, string> newBins;
        for (int i = 0; i < randomizeSeqs.size(); i++) { //build otus
            
            if (m->getControl_pressed()) { break; }
            
            map<int, int>::iterator it = seqBin.find(randomizeSeqs[i]);
            
            int seqNumber = it->first;
            int binNumber = it->second;
            
            map<int, string>::iterator itBinLabels = binLabels.find(binNumber); //do we have a label for this bin.  If the seq maps to existing bin then we should, otherwise we couldn't "fit" this sequence
            
            if (itBinLabels != binLabels.end()) {
                map<int, string>::iterator itBin = newBins.find(binNumber); // have we seen this otu yet?
                
                if (itBin == newBins.end()) { //create bin
                    newBins[binNumber] = matrix->getName(seqNumber);
                }else { //append bin
                    newBins[binNumber] += "," + matrix->getName(seqNumber);
                }
            }
        }
        
        for (map<int, string>::iterator itBin = newBins.begin(); itBin != newBins.end(); itBin++) {
            if (itBin->first < maxRefBinNumber) {
                list->push_back(itBin->second);
                newLabels.push_back(binLabels[itBin->first]);
            }
        }
        
        list->setLabels(newLabels);
        unnumFitted = (numFitSeqs - list->getNumSeqs()) + matrix->getNumFitSingletons();
        
        /*ListVector* singleton = matrix->getFitListSingle();
        
        if (singleton != NULL) { //add in any sequences above cutoff in read. Removing these saves clustering time.
            for (int i = 0; i < singleton->getNumBins(); i++) {
                string bin = singleton->get(i);
                if (bin != "") { list->push_back(singleton->get(i)); }
            }
            delete singleton;
        }*/
        
        return list;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getFittedList");
        exit(1);
    }
}
/***********************************************************************/
long long OptiFitCluster::getNumBins() {
    try {
        
        long long singletn = 0;
        
        singletn = matrix->getNumSingletons();
            
        for (int i = 0; i < bins.size(); i++) { if (bins[i].size() != 0) { singletn++; } }
        
        return singletn;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getNumBins");
        exit(1);
    }
}
/***********************************************************************/
long long OptiFitCluster::getNumFitBins() {
    try {
        long long unnumFitted;
        ListVector* list = getFittedList(unnumFitted);
        
        int numBins = 0;
        if (list != NULL) {
            numBins = list->getNumBins() + unnumFitted;
            delete list;
        }
        
        return numBins;
        
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "getNumFitBins");
        exit(1);
    }
}
/***********************************************************************/
int OptiFitCluster::findInsert() {
    try {
        
        //initially there are bins for each sequence (excluding singletons removed on read)
        for (int i = 0; i < bins.size(); i++) {
            
            if (m->getControl_pressed()) { break; }
            
            if (bins[i].size() == 0) { return i;  } //this bin is empty
        }
        
        return -1;
    }
    catch(exception& e) {
        m->errorOut(e, "OptiFitCluster", "findInsert");
        exit(1);
    }
}

/***********************************************************************/