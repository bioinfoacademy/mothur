/*
 *  parselistcommand.cpp
 *  Mothur
 *
 *  Created by westcott on 2/24/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "parselistscommand.h"

//**********************************************************************************************************************
vector<string> ParseListCommand::setParameters(){	
	try {
		CommandParameter plist("list", "InputTypes", "", "", "none", "none", "none",false,true); parameters.push_back(plist);
		CommandParameter pgroup("group", "InputTypes", "", "", "none", "none", "none",false,true); parameters.push_back(pgroup);
		CommandParameter plabel("label", "String", "", "", "", "", "",false,false); parameters.push_back(plabel);
		CommandParameter pinputdir("inputdir", "String", "", "", "", "", "",false,false); parameters.push_back(pinputdir);
		CommandParameter poutputdir("outputdir", "String", "", "", "", "", "",false,false); parameters.push_back(poutputdir);
		
		vector<string> myArray;
		for (int i = 0; i < parameters.size(); i++) {	myArray.push_back(parameters[i].name);		}
		return myArray;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseListCommand", "setParameters");
		exit(1);
	}
}
//**********************************************************************************************************************
string ParseListCommand::getHelpString(){	
	try {
		string helpString = "";
		helpString += "The parse.list command reads a list and group file and generates a list file for each group in the groupfile. \n";
		helpString += "The parse.list command parameters are list, group and label.\n";
		helpString += "The list and group parameters are required.\n";
		helpString += "The label parameter is used to read specific labels in your input you want to use.\n";
		helpString += "The parse.list command should be used in the following format: parse.list(list=yourListFile, group=yourGroupFile, label=yourLabels).\n";
		helpString += "Example: parse.list(list=abrecovery.fn.list, group=abrecovery.groups, label=0.03).\n";
		helpString += "Note: No spaces between parameter labels (i.e. list), '=' and parameters (i.e.yourListfile).\n";
		return helpString;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseListCommand", "getHelpString");
		exit(1);
	}
}
//**********************************************************************************************************************
ParseListCommand::ParseListCommand(){	
	try {
		abort = true; calledHelp = true; 
		setParameters();
		vector<string> tempOutNames;
		outputTypes["list"] = tempOutNames;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseListCommand", "ParseListCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
ParseListCommand::ParseListCommand(string option)  {
	try {
		abort = false; calledHelp = false;   
		allLines = 1;
			
		//allow user to run help
		if(option == "help") { help(); abort = true; calledHelp = true; }
		else if(option == "citation") { citation(); abort = true; calledHelp = true;}
		
		else {
			vector<string> myArray = setParameters();
			
			OptionParser parser(option);
			map<string, string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string, string>::iterator it;
		
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//initialize outputTypes
			vector<string> tempOutNames;
			outputTypes["list"] = tempOutNames;			
												
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("list");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["list"] = inputDir + it->second;		}
				}
				
				it = parameters.find("group");
				//user has given a template file
				if(it != parameters.end()){ 
					path = m->hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["group"] = inputDir + it->second;		}
				}
			}

			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";	}

			//check for required parameters
			listfile = validParameter.validFile(parameters, "list", true);
			if (listfile == "not open") { abort = true; }
			else if (listfile == "not found") { 
				listfile = m->getListFile(); 
				if (listfile != "") {  m->mothurOut("Using " + listfile + " as input file for the list parameter."); m->mothurOutEndLine(); }
				else { 
					m->mothurOut("No valid current list file. You must provide a list file."); m->mothurOutEndLine(); 
					abort = true;
						
				}
			}	
			
			groupfile = validParameter.validFile(parameters, "group", true);
			if (groupfile == "not open") { abort = true; }	
			else if (groupfile == "not found") { 
				groupfile = m->getListFile(); 
				if (groupfile != "") {  
					m->mothurOut("Using " + groupfile + " as input file for the group parameter."); m->mothurOutEndLine(); 
					groupMap = new GroupMap(groupfile);
					
					int error = groupMap->readMap();
					if (error == 1) { abort = true; }
				}else { m->mothurOut("No valid current group file. You must provide a group file."); m->mothurOutEndLine();  abort = true; } 
			}else {  
				groupMap = new GroupMap(groupfile);
				
				int error = groupMap->readMap();
				if (error == 1) { abort = true; }
			}
			
			//do you have all files needed
			if ((listfile == "") || (groupfile == "")) { m->mothurOut("You must enter both a listfile and groupfile for the parse.list command. "); m->mothurOutEndLine(); abort = true;  }
			
			//check for optional parameter and set defaults
			// ...at some point should added some additional type checking...
			label = validParameter.validFile(parameters, "label", false);			
			if (label == "not found") { label = "";  allLines = 1; }
			else { 
				if(label != "all") {  m->splitAtDash(label, labels);  allLines = 0;  }
				else { allLines = 1;  }
			}
		}

	}
	catch(exception& e) {
		m->errorOut(e, "ParseListCommand", "ParseListCommand");
		exit(1);
	}
}
//**********************************************************************************************************************
int ParseListCommand::execute(){
	try {
	
		if (abort == true) { if (calledHelp) { return 0; }  return 2;	}
		
		//set fileroot
		string fileroot = outputDir + m->getRootName(m->getSimpleName(listfile));
		
		//fill filehandles with neccessary ofstreams
		int i;
		ofstream* temp;
		for (i=0; i<groupMap->namesOfGroups.size(); i++) {
			temp = new ofstream;
			filehandles[groupMap->namesOfGroups[i]] = temp;
			
			string filename = fileroot +  groupMap->namesOfGroups[i] + ".list";
			outputNames.push_back(filename); outputTypes["list"].push_back(filename);
			m->openOutputFile(filename, *temp);
		}
		
		//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
		set<string> processedLabels;
		set<string> userLabels = labels;	
	
		input = new InputData(listfile, "list");
		list = input->getListVector();
		string lastLabel = list->getLabel();
		
		if (m->control_pressed) { 
			delete input; delete list; delete groupMap;
			for (i=0; i<groupMap->namesOfGroups.size(); i++) {  (*(filehandles[groupMap->namesOfGroups[i]])).close();  delete filehandles[groupMap->namesOfGroups[i]]; } 
			for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); } outputTypes.clear();
			return 0;
		}
		
		while((list != NULL) && ((allLines == 1) || (userLabels.size() != 0))) {
		
			if (m->control_pressed) { 
				delete input; delete list; delete groupMap;
				for (i=0; i<groupMap->namesOfGroups.size(); i++) {  (*(filehandles[groupMap->namesOfGroups[i]])).close();  delete filehandles[groupMap->namesOfGroups[i]]; } 
				for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); } outputTypes.clear();
				return 0;
			}
			
			if(allLines == 1 || labels.count(list->getLabel()) == 1){
					
					m->mothurOut(list->getLabel()); m->mothurOutEndLine();
					parse(list);
										
					processedLabels.insert(list->getLabel());
					userLabels.erase(list->getLabel());
			}
			
			if ((m->anyLabelsToProcess(list->getLabel(), userLabels, "") == true) && (processedLabels.count(lastLabel) != 1)) {
					string saveLabel = list->getLabel();
					
					delete list;
					list = input->getListVector(lastLabel); //get new list vector to process
					
					m->mothurOut(list->getLabel()); m->mothurOutEndLine();
					parse(list);
					
					processedLabels.insert(list->getLabel());
					userLabels.erase(list->getLabel());
					
					//restore real lastlabel to save below
					list->setLabel(saveLabel);
			}
			
		
			lastLabel = list->getLabel();
				
			delete list;
			list = input->getListVector(); //get new list vector to process
		}
		
		if (m->control_pressed) { 
				delete input; delete groupMap;
				for (i=0; i<groupMap->namesOfGroups.size(); i++) { (*(filehandles[groupMap->namesOfGroups[i]])).close();  delete filehandles[groupMap->namesOfGroups[i]]; } 
				for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); } outputTypes.clear();
				return 0;
		}
		
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
		
		if (m->control_pressed) { 
				delete input; delete groupMap;
				for (i=0; i<groupMap->namesOfGroups.size(); i++) {  (*(filehandles[groupMap->namesOfGroups[i]])).close();  delete filehandles[groupMap->namesOfGroups[i]]; } 
				for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); } outputTypes.clear();
				return 0;
		}
		
		//run last label if you need to
		if (needToRun == true)  {
			if (list != NULL) {	delete list;	}
			list = input->getListVector(lastLabel); //get new list vector to process
			
			m->mothurOut(list->getLabel()); m->mothurOutEndLine();
			parse(list);		
			
			delete list;
		}
		
		for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
			(*(filehandles[it3->first])).close();
			delete it3->second;
		}
		
		
		delete groupMap;
		delete input;
		
		if (m->control_pressed) { 
			for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); } outputTypes.clear();
			return 0;
		}
		
		//set fasta file as new current fastafile
		string current = "";
		itTypes = outputTypes.find("list");
		if (itTypes != outputTypes.end()) {
			if ((itTypes->second).size() != 0) { current = (itTypes->second)[0]; m->setListFile(current); }
		}
			
		m->mothurOutEndLine();
		m->mothurOut("Output File Names: "); m->mothurOutEndLine();
		for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
		m->mothurOutEndLine();
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "ParseListCommand", "execute");
		exit(1);
	}
}
/**********************************************************************************************************************/
int ParseListCommand::parse(ListVector* thisList) {
	try {
	
		map<string, string> groupVector;
		map<string, string>::iterator itGroup;
		map<string, int> groupNumBins;
		
		//print label
		for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
			groupNumBins[it3->first] = 0;
			groupVector[it3->first] = "";
		}

		
		for (int i = 0; i < thisList->getNumBins(); i++) {
			if (m->control_pressed) { return 0; }
			
			map<string, string> groupBins;
			string bin = list->get(i); 
			
			vector<string> names;
			m->splitAtComma(bin, names);  //parses bin into individual sequence names
			
			//parse bin into list of sequences in each group
			for (int j = 0; j < names.size(); j++) {
				string group = groupMap->getGroup(names[j]);
				
				if (group == "not found") { m->mothurOut(names[j] + " is not in your groupfile. please correct."); m->mothurOutEndLine(); exit(1); }
				
				itGroup = groupBins.find(group);
				if(itGroup == groupBins.end()) {
					groupBins[group] = names[j];  //add first name
					groupNumBins[group]++;
				}else{ //add another name
					groupBins[group] = groupBins[group] + "," + names[j];
				}
			}
			
			//print parsed bin info to files
			for (itGroup = groupBins.begin(); itGroup != groupBins.end(); itGroup++) {
				groupVector[itGroup->first] +=  itGroup->second + '\t'; 
			}
		
		}
		
		//end list vector
		for (it3 = filehandles.begin(); it3 != filehandles.end(); it3++) {
			(*(filehandles[it3->first])) << thisList->getLabel() << '\t' << groupNumBins[it3->first] << '\t' << groupVector[it3->first] << endl;  // label numBins  listvector for that group
		}
		
		return 0;

	}
	catch(exception& e) {
		m->errorOut(e, "ParseListCommand", "parse");
		exit(1);
	}
}

/**********************************************************************************************************************/


