/*
 *  removeseqscommand.cpp
 *  Mothur
 *
 *  Created by Sarah Westcott on 7/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

#include "removeseqscommand.h"
#include "sequence.hpp"
#include "listvector.hpp"

//**********************************************************************************************************************

RemoveSeqsCommand::RemoveSeqsCommand(string option)  {
	try {
		abort = false;
		
		//allow user to run help
		if(option == "help") { help(); abort = true; }
		
		else {
			//valid paramters for this command
			string Array[] =  {"fasta","name", "group", "alignreport", "accnos", "list","taxonomy","outputdir","inputdir", "dups" };
			vector<string> myArray (Array, Array+(sizeof(Array)/sizeof(string)));
			
			OptionParser parser(option);
			map<string,string> parameters = parser.getParameters();
			
			ValidParameters validParameter;
			map<string,string>::iterator it;
			
			//check to make sure all parameters are valid for command
			for (it = parameters.begin(); it != parameters.end(); it++) { 
				if (validParameter.isValidParameter(it->first, myArray, it->second) != true) {  abort = true;  }
			}
			
			//if the user changes the output directory command factory will send this info to us in the output parameter 
			outputDir = validParameter.validFile(parameters, "outputdir", false);		if (outputDir == "not found"){	outputDir = "";		}
			
			//if the user changes the input directory command factory will send this info to us in the output parameter 
			string inputDir = validParameter.validFile(parameters, "inputdir", false);		
			if (inputDir == "not found"){	inputDir = "";		}
			else {
				string path;
				it = parameters.find("alignreport");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["alignreport"] = inputDir + it->second;		}
				}
				
				it = parameters.find("fasta");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["fasta"] = inputDir + it->second;		}
				}
				
				it = parameters.find("accnos");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["accnos"] = inputDir + it->second;		}
				}
				
				it = parameters.find("list");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["list"] = inputDir + it->second;		}
				}
				
				it = parameters.find("name");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["name"] = inputDir + it->second;		}
				}
				
				it = parameters.find("group");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["group"] = inputDir + it->second;		}
				}
				
				it = parameters.find("taxonomy");
				//user has given a template file
				if(it != parameters.end()){ 
					path = hasPath(it->second);
					//if the user has not given a path then, add inputdir. else leave path alone.
					if (path == "") {	parameters["taxonomy"] = inputDir + it->second;		}
				}
			}

			
			//check for required parameters
			accnosfile = validParameter.validFile(parameters, "accnos", true);
			if (accnosfile == "not open") { abort = true; }
			else if (accnosfile == "not found") {  accnosfile = "";  m->mothurOut("You must provide an accnos file."); m->mothurOutEndLine(); abort = true; }	
			
			fastafile = validParameter.validFile(parameters, "fasta", true);
			if (fastafile == "not open") { abort = true; }
			else if (fastafile == "not found") {  fastafile = "";  }	
			
			namefile = validParameter.validFile(parameters, "name", true);
			if (namefile == "not open") { abort = true; }
			else if (namefile == "not found") {  namefile = "";  }	
			
			groupfile = validParameter.validFile(parameters, "group", true);
			if (groupfile == "not open") { abort = true; }
			else if (groupfile == "not found") {  groupfile = "";  }	
			
			alignfile = validParameter.validFile(parameters, "alignreport", true);
			if (alignfile == "not open") { abort = true; }
			else if (alignfile == "not found") {  alignfile = "";  }
			
			listfile = validParameter.validFile(parameters, "list", true);
			if (listfile == "not open") { abort = true; }
			else if (listfile == "not found") {  listfile = "";  }
			
			taxfile = validParameter.validFile(parameters, "taxonomy", true);
			if (taxfile == "not open") { abort = true; }
			else if (taxfile == "not found") {  taxfile = "";  }

			
			string usedDups = "true";
			string temp = validParameter.validFile(parameters, "dups", false);	if (temp == "not found") { temp = "false"; usedDups = ""; }
			dups = isTrue(temp);
			
			if ((fastafile == "") && (namefile == "") && (groupfile == "") && (alignfile == "") && (listfile == "") && (taxfile == ""))  { m->mothurOut("You must provide at least one of the following: fasta, name, group, taxonomy, alignreport or list."); m->mothurOutEndLine(); abort = true; }
			
			if ((usedDups != "") && (namefile == "")) {  m->mothurOut("You may only use dups with the name option."); m->mothurOutEndLine();  abort = true; }			
		}

	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "RemoveSeqsCommand");
		exit(1);
	}
}
//**********************************************************************************************************************

void RemoveSeqsCommand::help(){
	try {
		m->mothurOut("The remove.seqs command reads an .accnos file and at least one of the following file types: fasta, name, group, list, taxonomy or alignreport file.\n");
		m->mothurOut("It outputs a file containing the sequences NOT in the .accnos file.\n");
		m->mothurOut("The remove.seqs command parameters are accnos, fasta, name, group, list, taxonomy, alignreport and dups.  You must provide accnos and at least one of the file parameters.\n");
		m->mothurOut("The dups parameter allows you to remove the entire line from a name file if you remove any name from the line. default=false. \n");
		m->mothurOut("The remove.seqs command should be in the following format: remove.seqs(accnos=yourAccnos, fasta=yourFasta).\n");
		m->mothurOut("Example remove.seqs(accnos=amazon.accnos, fasta=amazon.fasta).\n");
		m->mothurOut("Note: No spaces between parameter labels (i.e. fasta), '=' and parameters (i.e.yourFasta).\n\n");
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "help");
		exit(1);
	}
}

//**********************************************************************************************************************

int RemoveSeqsCommand::execute(){
	try {
		
		if (abort == true) { return 0; }
		
		//get names you want to keep
		readAccnos();
		
		if (m->control_pressed) { return 0; }
		
		//read through the correct file and output lines you want to keep
		if (namefile != "")			{		readName();		}
		if (fastafile != "")		{		readFasta();	}
		if (groupfile != "")		{		readGroup();	}
		if (alignfile != "")		{		readAlign();	}
		if (listfile != "")			{		readList();		}
		if (taxfile != "")			{		readTax();		}
		
		if (m->control_pressed) { for (int i = 0; i < outputNames.size(); i++) {	remove(outputNames[i].c_str()); } return 0; }
		
		if (outputNames.size() != 0) {
			m->mothurOutEndLine();
			m->mothurOut("Output File Names: "); m->mothurOutEndLine();
			for (int i = 0; i < outputNames.size(); i++) {	m->mothurOut(outputNames[i]); m->mothurOutEndLine();	}
			m->mothurOutEndLine();
		}
		
		return 0;		
	}

	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "execute");
		exit(1);
	}
}

//**********************************************************************************************************************
int RemoveSeqsCommand::readFasta(){
	try {
		if (outputDir == "") {  outputDir += hasPath(fastafile);  }
		string outputFileName = outputDir + getRootName(getSimpleName(fastafile)) + "pick" + getExtension(fastafile);
		ofstream out;
		openOutputFile(outputFileName, out);
		
		ifstream in;
		openInputFile(fastafile, in);
		string name;
		
		bool wroteSomething = false;
		
		while(!in.eof()){
			if (m->control_pressed) { in.close();  out.close();  remove(outputFileName.c_str());  return 0; }
			
			Sequence currSeq(in);
			name = currSeq.getName();
			
			if (name != "") {
				//if this name is in the accnos file
				if (names.count(name) == 0) {
					wroteSomething = true;
					
					currSeq.printSequence(out);
				}
			}
			gobble(in);
		}
		in.close();	
		out.close();
		
		if (wroteSomething == false) {  m->mothurOut("Your file contains only sequences from the .accnos file."); m->mothurOutEndLine();  }
		outputNames.push_back(outputFileName); 
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readFasta");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveSeqsCommand::readList(){
	try {
		if (outputDir == "") {  outputDir += hasPath(listfile);  }
		string outputFileName = outputDir + getRootName(getSimpleName(listfile)) + "pick" +  getExtension(listfile);
		ofstream out;
		openOutputFile(outputFileName, out);
		
		ifstream in;
		openInputFile(listfile, in);
		
		bool wroteSomething = false;
		
		while(!in.eof()){
			//read in list vector
			ListVector list(in);
			
			//make a new list vector
			ListVector newList;
			newList.setLabel(list.getLabel());
			
			//for each bin
			for (int i = 0; i < list.getNumBins(); i++) {
				if (m->control_pressed) { in.close();  out.close();  remove(outputFileName.c_str());  return 0; }
			
				//parse out names that are in accnos file
				string binnames = list.get(i);
				
				string newNames = "";
				while (binnames.find_first_of(',') != -1) { 
					string name = binnames.substr(0,binnames.find_first_of(','));
					binnames = binnames.substr(binnames.find_first_of(',')+1, binnames.length());
					
					//if that name is in the .accnos file, add it
					if (names.count(name) == 0) {  newNames += name + ",";  }
				}
			
				//get last name
				if (names.count(binnames) == 0) {  newNames += binnames + ",";  }

				//if there are names in this bin add to new list
				if (newNames != "") {  
					newNames = newNames.substr(0, newNames.length()-1); //rip off extra comma
					newList.push_back(newNames);	
				}
			}
				
			//print new listvector
			if (newList.getNumBins() != 0) {
				wroteSomething = true;
				newList.print(out);
			}
			
			gobble(in);
		}
		in.close();	
		out.close();
		
		if (wroteSomething == false) {  m->mothurOut("Your file contains only sequences from the .accnos file."); m->mothurOutEndLine();  }
		outputNames.push_back(outputFileName); 
				
		return 0;

	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readList");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveSeqsCommand::readName(){
	try {
		if (outputDir == "") {  outputDir += hasPath(namefile);  }
		string outputFileName = outputDir + getRootName(getSimpleName(namefile)) + "pick" + getExtension(namefile);

		ofstream out;
		openOutputFile(outputFileName, out);

		ifstream in;
		openInputFile(namefile, in);
		string name, firstCol, secondCol;
		
		bool wroteSomething = false;
		
		while(!in.eof()){
			if (m->control_pressed) { in.close();  out.close();  remove(outputFileName.c_str());  return 0; }

			in >> firstCol;				
			in >> secondCol;			

			vector<string> parsedNames;
			//parse second column saving each name
			while (secondCol.find_first_of(',') != -1) { 
				name = secondCol.substr(0,secondCol.find_first_of(','));
				secondCol = secondCol.substr(secondCol.find_first_of(',')+1, secondCol.length());
				parsedNames.push_back(name);

			}
			
			//get name after last ,
			parsedNames.push_back(secondCol);

			vector<string> validSecond;  validSecond.clear();
			for (int i = 0; i < parsedNames.size(); i++) {
				if (names.count(parsedNames[i]) == 0) {
					validSecond.push_back(parsedNames[i]);
				}
			}
			
			if ((dups) && (validSecond.size() != parsedNames.size())) {  //if dups is true and we want to get rid of anyone, get rid of everyone
				for (int i = 0; i < parsedNames.size(); i++) {  names.insert(parsedNames[i]);  }
			}else {
					//if the name in the first column is in the set then print it and any other names in second column also in set
				if (names.count(firstCol) == 0) {
					
					wroteSomething = true;
					
					out << firstCol << '\t';
					
					//you know you have at least one valid second since first column is valid
					for (int i = 0; i < validSecond.size()-1; i++) {  out << validSecond[i] << ',';  }
					out << validSecond[validSecond.size()-1] << endl;
					
					//make first name in set you come to first column and then add the remaining names to second column
				}else {
					
					//you want part of this row
					if (validSecond.size() != 0) {
						
						wroteSomething = true;
						
						out << validSecond[0] << '\t';
						
						//you know you have at least one valid second since first column is valid
						for (int i = 0; i < validSecond.size()-1; i++) {  out << validSecond[i] << ',';  }
						out << validSecond[validSecond.size()-1] << endl;
					}
				}
			}
			gobble(in);
		}
		in.close();
		out.close();

		if (wroteSomething == false) {  m->mothurOut("Your file contains only sequences from the .accnos file."); m->mothurOutEndLine();  }
		outputNames.push_back(outputFileName); 
				
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readName");
		exit(1);
	}
}

//**********************************************************************************************************************
int RemoveSeqsCommand::readGroup(){
	try {
		if (outputDir == "") {  outputDir += hasPath(groupfile);  }
		string outputFileName = outputDir + getRootName(getSimpleName(groupfile)) + "pick" + getExtension(groupfile);
		ofstream out;
		openOutputFile(outputFileName, out);

		ifstream in;
		openInputFile(groupfile, in);
		string name, group;
		
		bool wroteSomething = false;
		
		while(!in.eof()){
			if (m->control_pressed) { in.close();  out.close();  remove(outputFileName.c_str());  return 0; }
			
			in >> name;				//read from first column
			in >> group;			//read from second column
			
			//if this name is in the accnos file
			if (names.count(name) == 0) {
				wroteSomething = true;
				out << name << '\t' << group << endl;
			}
					
			gobble(in);
		}
		in.close();
		out.close();
		
		if (wroteSomething == false) {  m->mothurOut("Your file contains only sequences from the .accnos file."); m->mothurOutEndLine();  }
		outputNames.push_back(outputFileName); 
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readGroup");
		exit(1);
	}
}
//**********************************************************************************************************************
int RemoveSeqsCommand::readTax(){
	try {
		if (outputDir == "") {  outputDir += hasPath(taxfile);  }
		string outputFileName = outputDir + getRootName(getSimpleName(taxfile)) + "pick" + getExtension(taxfile);
		ofstream out;
		openOutputFile(outputFileName, out);

		ifstream in;
		openInputFile(taxfile, in);
		string name, tax;
		
		bool wroteSomething = false;
		
		while(!in.eof()){
			if (m->control_pressed) { in.close();  out.close();  remove(outputFileName.c_str());  return 0; }
			
			in >> name;				//read from first column
			in >> tax;			//read from second column
			
			//if this name is in the accnos file
			if (names.count(name) == 0) {
				wroteSomething = true;
				out << name << '\t' << tax << endl;
			}
					
			gobble(in);
		}
		in.close();
		out.close();
		
		if (wroteSomething == false) {  m->mothurOut("Your file contains only sequences from the .accnos file."); m->mothurOutEndLine();  }
		outputNames.push_back(outputFileName); 
		
		return 0;
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readTax");
		exit(1);
	}
}
//**********************************************************************************************************************
//alignreport file has a column header line then all other lines contain 16 columns.  we just want the first column since that contains the name
int RemoveSeqsCommand::readAlign(){
	try {
		if (outputDir == "") {  outputDir += hasPath(alignfile);  }
		string outputFileName = outputDir + getRootName(getSimpleName(alignfile)) + "pick.align.report";
		ofstream out;
		openOutputFile(outputFileName, out);

		ifstream in;
		openInputFile(alignfile, in);
		string name, junk;
		
		bool wroteSomething = false;
		
		//read column headers
		for (int i = 0; i < 16; i++) {  
			if (!in.eof())	{	in >> junk;	 out << junk << '\t';	}
			else			{	break;			}
		}
		out << endl;
		
		while(!in.eof()){
			if (m->control_pressed) { in.close();  out.close();  remove(outputFileName.c_str());  return 0; }
			
			in >> name;				//read from first column
			
			//if this name is in the accnos file
			if (names.count(name) == 0) {
				wroteSomething = true;
				
				out << name << '\t';
				
				//read rest
				for (int i = 0; i < 15; i++) {  
					if (!in.eof())	{	in >> junk;	 out << junk << '\t';	}
					else			{	break;			}
				}
				out << endl;
				
			}else {//still read just don't do anything with it
				
				//read rest
				for (int i = 0; i < 15; i++) {  
					if (!in.eof())	{	in >> junk;		}
					else			{	break;			}
				}
			}
			
			gobble(in);
		}
		in.close();
		out.close();
		
		if (wroteSomething == false) {  m->mothurOut("Your file contains only sequences from the .accnos file."); m->mothurOutEndLine();  }
		outputNames.push_back(outputFileName); 
		
		return 0;
		
	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readAlign");
		exit(1);
	}
}
//**********************************************************************************************************************
void RemoveSeqsCommand::readAccnos(){
	try {
		
		ifstream in;
		openInputFile(accnosfile, in);
		string name;
		
		while(!in.eof()){
			in >> name;
						
			names.insert(name);
			
			gobble(in);
		}
		in.close();		

	}
	catch(exception& e) {
		m->errorOut(e, "RemoveSeqsCommand", "readAccnos");
		exit(1);
	}
}

//**********************************************************************************************************************


