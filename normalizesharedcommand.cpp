/*
 *  normalizesharedcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 9/15/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "normalizesharedcommand.h"

//**********************************************************************************************************************
vector<string> NormalizeSharedCommand::getValidParameters(){	
	try {
		string Array[] =  {"groups","label","method","makerelabund","outputdir","inputdir","norm"};
		vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "getValidParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
NormalizeSharedCommand::NormalizeSharedCommand(){	
	try {
		abort = true;
		//initialize outputTypes
		vector<string> tempOutNames;
		outputTypes["shared"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "NormalizeSharedCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> NormalizeSharedCommand::getRequiredParameters(){	
	try {
		vector<string> myArray;
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "getRequiredParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
vector<string> NormalizeSharedCommand::getRequiredFiles(){	
	try {
		string Array[] =  {"shared"};
		vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "getRequiredFiles");
		exit(1);
	}
}
//**********************************************************************************************************************

NormalizeSharedCommand::NormalizeSharedCommand(string option) {
	try {
		globaldata = GlobalData::getInstance();
		abort = false;
		allLines = 1;
		labels.clear();
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string AlignArray[] =  {"groups","label","method","makerelabund","outputdir","inputdir","norm"};
			vector<string> myArray (AlignArray, AlignArray+(sizeof(AlignArray)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			
			//check to make sure all parameters are valid for command
			for (map<string,string>::iterator it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["shared"] = tempOutNames;
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	
				outputDir = "";	
				outputDir += m->hasPath(globaldata->inputFileName); //if user entered a file with a path then preserve it	
			}
			
			//make sure the user has already run the read.otu command
			if ((globaldata->getSharedFile() == "") && (globaldata->getRelAbundFile() == "")) {
				 m->mothurOut("You must read a list and a group, shared or relabund file before you can use the normalize.shared command."); m->mothurOutEndLine(); abort = true; 
			}
			
			if ((globaldata->getSharedFile() != "") && (globaldata->getRelAbundFile() != "")) {
				m->mothurOut("You may not use both a shared and relabund file as input for normalize.shared command."); m->mothurOutEndLine(); abort = true; 
			}
			

			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			label = validParameter.validFile(parameters, "label", false);			
			if (label == "not found") { label = ""; }
			else { 
				if(label != "all") {  m->splitAtDash(label, labels);  allLines = 0;  }
				else { allLines = 1;  }
			}
			
			//if the user has not specified any labels use the ones from read.otu
			if (label == "") {  
				allLines = globaldata->allLines; 
				labels = globaldata->labels; 
			}
			
			groups = validParameter.validFile(parameters, "groups", false);			
			if (groups == "not found") { groups = ""; pickedGroups = false; }
			else { 
				pickedGroups = true;
				m->splitAtDash(groups, Groups);
				globaldata->Groups = Groups;
			}
			
			method = validParameter.validFile(parameters, "method", false);				if (method == "not found") { method = "totalgroup"; }
			if ((method != "totalgroup") && (method != "zscore")) {  m->mothurOut(method + " is not a valid scaling option for the normalize.shared command. The options are totalgroup and zscore. We hope to add more ways to normalize in the future, suggestions are welcome!"); m->mothurOutEndLine(); abort = true; }
		
			string temp = validParameter.validFile(parameters, "norm", false);				
			if (temp == "not found") {  
				norm = 0;  //once you have read, set norm to smallest group number
			}else { 
				convert(temp, norm);
				if (norm < 0) { m->mothurOut("norm must be positive."); m->mothurOutEndLine(); abort=true; }
			}
			
			temp = validParameter.validFile(parameters, "makerelabund", false);	if (temp == "") { temp = "f"; }
			makeRelabund = m->isTrue(temp);
			
			if ((globaldata->getFormat() != "sharedfile") && makeRelabund) { m->mothurOut("makerelabund can only be used with a shared file."); m->mothurOutEndLine(); }
			
		}

	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "NormalizeSharedCommand");
		exit(1);
	}
}

//**********************************************************************************************************************

void NormalizeSharedCommand::help(){
	try {
		m->mothurOut("The normalize.shared command can only be executed after a successful read.otu command of a list and group, shared or relabund file.\n");
		m->mothurOut("The normalize.shared command parameters are groups, method, norm, makerelabund and label.  No parameters are required.\n");
		m->mothurOut("The groups parameter allows you to specify which of the groups in your groupfile you would like included. The group names are separated by dashes.\n");
		m->mothurOut("The label parameter allows you to select what distance levels you would like, and are also separated by dashes.\n");
		m->mothurOut("The method parameter allows you to select what method you would like to use to normalize. The options are totalgroup and zscore. We hope to add more ways to normalize in the future, suggestions are welcome!\n");
		m->mothurOut("The makerelabund parameter allows you to convert a shared file to a relabund file before you normalize. default=f.\n");
		m->mothurOut("The norm parameter allows you to number you would like to normalize to. By default this is set to the number of sequences in your smallest group.\n");
		m->mothurOut("The normalize.shared command should be in the following format: normalize.shared(groups=yourGroups, label=yourLabels).\n");
		m->mothurOut("Example normalize.shared(groups=A-B-C, scale=totalgroup).\n");
		m->mothurOut("The default value for groups is all the groups in your groupfile, and all labels in your inputfile will be used.\n");
		m->mothurOut("The normalize.shared command outputs a .norm.shared file.\n");
		m->mothurOut("Note: No spaces between parameter labels (i.e. groups), '=' and parameters (i.e.yourGroups).\n\n");

	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "help");
		exit(1);
	}
}

//**********************************************************************************************************************

NormalizeSharedCommand::~NormalizeSharedCommand(){}

//**********************************************************************************************************************

int NormalizeSharedCommand::execute(){
	try {
	
		if (abort == true) { return 0; }
		
		string outputFileName = outputDir + m->getRootName(m->getSimpleName(globaldata->inputFileName)) + "norm.shared";
		ofstream out;
		m->openOutputFile(outputFileName, out);
		
		if (globaldata->getFormat() == "sharedfile") {  input = new InputData(globaldata->inputFileName, "sharedfile"); }
		else { input = new InputData(globaldata->inputFileName, "relabund"); }

		//you are reading a sharedfile and you do not want to make relabund
		if ((globaldata->getFormat() == "sharedfile") && (!makeRelabund)) {
			lookup = input->getSharedRAbundVectors();
			string lastLabel = lookup[0]->getLabel();
			
			//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
			set<string> processedLabels;
			set<string> userLabels = labels;
			
			if (method == "totalgroup") {
				//set norm to smallest group number
				if (norm == 0) { 
					norm = lookup[0]->getNumSeqs();
					for (int i = 1; i < lookup.size(); i++) {
						if (lookup[i]->getNumSeqs() < norm) { norm = lookup[i]->getNumSeqs();  }
					}  
				}
				
				m->mothurOut("Normalizing to " + toString(norm) + "."); m->mothurOutEndLine();
			}
			
			//as long as you are not at the end of the file or done wih the lines you want
			while((lookup[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
				
				if (m->control_pressed) { outputTypes.clear();  for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  } globaldata->Groups.clear();   out.close(); remove(outputFileName.c_str()); return 0; }
				
				if(allLines == 1 || labels.count(lookup[0]->getLabel()) == 1){			
					
					m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
					normalize(lookup, out);
					
					processedLabels.insert(lookup[0]->getLabel());
					userLabels.erase(lookup[0]->getLabel());
				}
				
				if ((m->anyLabelsToProcess(lookup[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = lookup[0]->getLabel();
					
					for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }  
					lookup = input->getSharedRAbundVectors(lastLabel);
					m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
					
					normalize(lookup, out);
					
					processedLabels.insert(lookup[0]->getLabel());
					userLabels.erase(lookup[0]->getLabel());
					
					//restore real lastlabel to save below
					lookup[0]->setLabel(saveLabel);
				}
				
				lastLabel = lookup[0]->getLabel();
				//prevent memory leak
				for (int i = 0; i < lookup.size(); i++) {  delete lookup[i]; lookup[i] = NULL; }
				
				if (m->control_pressed) {  outputTypes.clear(); globaldata->Groups.clear();  out.close(); remove(outputFileName.c_str()); return 0; }
				
				//get next line to process
				lookup = input->getSharedRAbundVectors();				
			}
			
			if (m->control_pressed) { outputTypes.clear(); globaldata->Groups.clear();  out.close(); remove(outputFileName.c_str());  return 0; }
			
			//output error messages about any remaining user labels
			set<string>::iterator it;
			bool needToRun = false;
			for (it = userLabels.begin(); it != userLabels.end(); it++) {  
				m->mothurOut("Your file does not include the label " + *it); 
				if (processedLabels.count(lastLabel) != 1) {
					m->mothurOut(". I will use " + lastLabel + "."); m->mothurOutEndLine();
					needToRun = true;
				}else {
					m->mothurOut(". Please refer to " + lastLabel + "."); m->mothurOutEndLine();
				}
			}
			
			//run last label if you need to
			if (needToRun == true)  {
				for (int i = 0; i < lookup.size(); i++) { if (lookup[i] != NULL) { delete lookup[i]; } }  
				lookup = input->getSharedRAbundVectors(lastLabel);
				
				m->mothurOut(lookup[0]->getLabel()); m->mothurOutEndLine();
				
				normalize(lookup, out);
				
				for (int i = 0; i < lookup.size(); i++) {  delete lookup[i];  }
			}
			
		}else{ //relabund values
			lookupFloat = input->getSharedRAbundFloatVectors();
			string lastLabel = lookupFloat[0]->getLabel();
			
			//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
			set<string> processedLabels;
			set<string> userLabels = labels;
			
			//set norm to smallest group number
			if (method == "totalgroup") {
				if (norm == 0) { 
					norm = lookupFloat[0]->getNumSeqs();
					for (int i = 1; i < lookupFloat.size(); i++) {
						if (lookupFloat[i]->getNumSeqs() < norm) { norm = lookupFloat[i]->getNumSeqs();  }
					}  
				}
				
				m->mothurOut("Normalizing to " + toString(norm) + "."); m->mothurOutEndLine();
			}
			
			//as long as you are not at the end of the file or done wih the lines you want
			while((lookupFloat[0] != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
				
				if (m->control_pressed) { outputTypes.clear();  for (int i = 0; i < lookupFloat.size(); i++) {  delete lookupFloat[i];  } globaldata->Groups.clear();   out.close(); remove(outputFileName.c_str()); return 0; }
				
				if(allLines == 1 || labels.count(lookupFloat[0]->getLabel()) == 1){			
					
					m->mothurOut(lookupFloat[0]->getLabel()); m->mothurOutEndLine();
					normalize(lookupFloat, out);
					
					processedLabels.insert(lookupFloat[0]->getLabel());
					userLabels.erase(lookupFloat[0]->getLabel());
				}
				
				if ((m->anyLabelsToProcess(lookupFloat[0]->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = lookupFloat[0]->getLabel();
					
					for (int i = 0; i < lookupFloat.size(); i++) {  delete lookupFloat[i];  }  
					lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);
					m->mothurOut(lookupFloat[0]->getLabel()); m->mothurOutEndLine();
					
					normalize(lookupFloat, out);
					
					processedLabels.insert(lookupFloat[0]->getLabel());
					userLabels.erase(lookupFloat[0]->getLabel());
					
					//restore real lastlabel to save below
					lookupFloat[0]->setLabel(saveLabel);
				}
				
				lastLabel = lookupFloat[0]->getLabel();
				//prevent memory leak
				for (int i = 0; i < lookupFloat.size(); i++) {  delete lookupFloat[i]; lookupFloat[i] = NULL; }
				
				if (m->control_pressed) {  outputTypes.clear(); globaldata->Groups.clear();  out.close(); remove(outputFileName.c_str()); return 0; }
				
				//get next line to process
				lookupFloat = input->getSharedRAbundFloatVectors();				
			}
			
			if (m->control_pressed) { outputTypes.clear(); globaldata->Groups.clear();  out.close(); remove(outputFileName.c_str());  return 0; }
			
			//output error messages about any remaining user labels
			set<string>::iterator it;
			bool needToRun = false;
			for (it = userLabels.begin(); it != userLabels.end(); it++) {  
				m->mothurOut("Your file does not include the label " + *it); 
				if (processedLabels.count(lastLabel) != 1) {
					m->mothurOut(". I will use " + lastLabel + "."); m->mothurOutEndLine();
					needToRun = true;
				}else {
					m->mothurOut(". Please refer to " + lastLabel + "."); m->mothurOutEndLine();
				}
			}
			
			//run last label if you need to
			if (needToRun == true)  {
				for (int i = 0; i < lookupFloat.size(); i++) { if (lookupFloat[i] != NULL) { delete lookupFloat[i]; } }  
				lookupFloat = input->getSharedRAbundFloatVectors(lastLabel);
				
				m->mothurOut(lookupFloat[0]->getLabel()); m->mothurOutEndLine();
				
				normalize(lookupFloat, out);
				
				for (int i = 0; i < lookupFloat.size(); i++) {  delete lookupFloat[i];  }
			}
			
		}
		//reset groups parameter
		globaldata->Groups.clear();  
		delete input;
		out.close();
		
		if (m->control_pressed) { outputTypes.clear(); remove(outputFileName.c_str()); return 0;}
		
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		m->mothurOut(outputFileName); m->mothurOutEndLine(); outputNames.push_back(outputFileName); outputTypes["shared"].push_back(outputFileName);
		m->mothurOutEndLine();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "execute");
		exit(1);
	}
}
//**********************************************************************************************************************

int NormalizeSharedCommand::normalize(vector<SharedRAbundVector*>& thisLookUp, ofstream& out){
	try {
		if (pickedGroups) { eliminateZeroOTUS(thisLookUp); }
				
		if (method == "totalgroup") { 
					
			for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {
						
					for (int i = 0; i < thisLookUp.size(); i++) {
							
						if (m->control_pressed) { return 0; }
							
						int abund = thisLookUp[i]->getAbundance(j);
							
						float relabund = relabund = abund / (float) thisLookUp[i]->getNumSeqs();
						float newNorm = relabund * norm;
						
						//round to nearest int
						int finalNorm = (int) floor((newNorm + 0.5));
						
						thisLookUp[i]->set(j, finalNorm, thisLookUp[i]->getGroup());
					}
				}
					
		}else if (method == "zscore") {
			
			for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {
				
				if (m->control_pressed) { return 0; }
				
				//calc mean
				float mean = 0.0;
				for (int i = 0; i < thisLookUp.size(); i++) {  mean += thisLookUp[i]->getAbundance(j); }
				mean /= (float) thisLookUp.size();
					
				//calc standard deviation
				float sumSquared = 0.0;
				for (int i = 0; i < thisLookUp.size(); i++) { sumSquared += (((float)thisLookUp[i]->getAbundance(j) - mean) * ((float)thisLookUp[i]->getAbundance(j) - mean)); }
				sumSquared /= (float) thisLookUp.size();
				
				float standardDev = sqrt(sumSquared);
					
				for (int i = 0; i < thisLookUp.size(); i++) {
					int finalNorm = 0;
					if (standardDev != 0) { // stop divide by zero
						float newNorm = ((float)thisLookUp[i]->getAbundance(j) - mean) / standardDev;
						//round to nearest int
						finalNorm = (int) floor((newNorm + 0.5));
					}
					
					thisLookUp[i]->set(j, finalNorm, thisLookUp[i]->getGroup());
				}
			}
						
		}else{ m->mothurOut(method + " is not a valid scaling option."); m->mothurOutEndLine(); m->control_pressed = true; return 0; }
				
				
						
		eliminateZeroOTUS(thisLookUp);
		 
		for (int i = 0; i < thisLookUp.size(); i++) {
			out << thisLookUp[i]->getLabel() << '\t' << thisLookUp[i]->getGroup() << '\t';
			thisLookUp[i]->print(out);
		}
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "normalize");
		exit(1);
	}
}
//**********************************************************************************************************************

int NormalizeSharedCommand::normalize(vector<SharedRAbundFloatVector*>& thisLookUp, ofstream& out){
	try {
		if (pickedGroups) { eliminateZeroOTUS(thisLookUp); }
		
		if (method == "totalgroup") { 
			
			for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {
				
				for (int i = 0; i < thisLookUp.size(); i++) {
					
					if (m->control_pressed) { return 0; }
					
					float abund = thisLookUp[i]->getAbundance(j);
					
					float relabund = relabund = abund / (float) thisLookUp[i]->getNumSeqs();
					float newNorm = relabund * norm;
					
					thisLookUp[i]->set(j, newNorm, thisLookUp[i]->getGroup());
				}
			}
			
		}else if (method == "zscore") {
			for (int j = 0; j < thisLookUp[0]->getNumBins(); j++) {
				
				if (m->control_pressed) { return 0; }
				
				//calc mean
				float mean = 0.0;
				for (int i = 0; i < thisLookUp.size(); i++) {  mean += thisLookUp[i]->getAbundance(j); }
				mean /= (float) thisLookUp.size();
				
				//calc standard deviation
				float sumSquared = 0.0;
				for (int i = 0; i < thisLookUp.size(); i++) { sumSquared += ((thisLookUp[i]->getAbundance(j) - mean) * (thisLookUp[i]->getAbundance(j) - mean)); }
				sumSquared /= (float) thisLookUp.size();
				
				float standardDev = sqrt(sumSquared);
				
				for (int i = 0; i < thisLookUp.size(); i++) {
					float newNorm = 0.0;
					if (standardDev != 0) { // stop divide by zero
						newNorm = (thisLookUp[i]->getAbundance(j) - mean) / standardDev;
					}
					thisLookUp[i]->set(j, newNorm, thisLookUp[i]->getGroup());
				}
			}			
			
		}else{ m->mothurOut(method + " is not a valid scaling option."); m->mothurOutEndLine(); m->control_pressed = true; return 0; }
		
		
		eliminateZeroOTUS(thisLookUp);
		
		for (int i = 0; i < thisLookUp.size(); i++) {
			out << thisLookUp[i]->getLabel() << '\t' << thisLookUp[i]->getGroup() << '\t';
			thisLookUp[i]->print(out);
		}
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "normalize");
		exit(1);
	}
}
//**********************************************************************************************************************
int NormalizeSharedCommand::eliminateZeroOTUS(vector<SharedRAbundVector*>& thislookup) {
	try {
		
		vector<SharedRAbundVector*> newLookup;
		for (int i = 0; i < thislookup.size(); i++) {
			SharedRAbundVector* temp = new SharedRAbundVector();
			temp->setLabel(thislookup[i]->getLabel());
			temp->setGroup(thislookup[i]->getGroup());
			newLookup.push_back(temp);
		}
		
		//for each bin
		for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
			if (m->control_pressed) { for (int j = 0; j < newLookup.size(); j++) {  delete newLookup[j];  } return 0; }
		
			//look at each sharedRabund and make sure they are not all zero
			bool allZero = true;
			for (int j = 0; j < thislookup.size(); j++) {
				if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break;  }
			}
			
			//if they are not all zero add this bin
			if (!allZero) {
				for (int j = 0; j < thislookup.size(); j++) {
					newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
				}
			}
		}

		for (int j = 0; j < thislookup.size(); j++) {  delete thislookup[j];  }

		thislookup = newLookup;
		
		return 0;
 
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "eliminateZeroOTUS");
		exit(1);
	}
}
//**********************************************************************************************************************
int NormalizeSharedCommand::eliminateZeroOTUS(vector<SharedRAbundFloatVector*>& thislookup) {
	try {
		
		vector<SharedRAbundFloatVector*> newLookup;
		for (int i = 0; i < thislookup.size(); i++) {
			SharedRAbundFloatVector* temp = new SharedRAbundFloatVector();
			temp->setLabel(thislookup[i]->getLabel());
			temp->setGroup(thislookup[i]->getGroup());
			newLookup.push_back(temp);
		}
		
		//for each bin
		for (int i = 0; i < thislookup[0]->getNumBins(); i++) {
			if (m->control_pressed) { for (int j = 0; j < newLookup.size(); j++) {  delete newLookup[j];  } return 0; }
			
			//look at each sharedRabund and make sure they are not all zero
			bool allZero = true;
			for (int j = 0; j < thislookup.size(); j++) {
				if (thislookup[j]->getAbundance(i) != 0) { allZero = false;  break;  }
			}
			
			//if they are not all zero add this bin
			if (!allZero) {
				for (int j = 0; j < thislookup.size(); j++) {
					newLookup[j]->push_back(thislookup[j]->getAbundance(i), thislookup[j]->getGroup());
				}
			}
		}
		
		for (int j = 0; j < thislookup.size(); j++) {  delete thislookup[j];  }
		
		thislookup = newLookup;
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "NormalizeSharedCommand", "eliminateZeroOTUS");
		exit(1);
	}
}

//**********************************************************************************************************************