#ifndef TREEGROUPCOMMAND_H
#define TREEGROUPCOMMAND_H

/*
 *  treegroupscommand.h
 *  Mothur
 *
 *  Created by Sarah Westcott on 4/8/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */
 
#include "command.hpp"
#include "inputdata.h"
#include "groupmap.h"
#include "validcalculator.h"
#include "tree.h"
#include "treemap.h"
#include "readmatrix.hpp"
#include "readcolumn.h"
#include "readphylip.h"
#include "sparsematrix.hpp"


/* This command create a tree file for each similarity calculator at distance level, using various calculators to find the similiarity between groups. 
	The user can select the lines or labels they wish to use as well as the groups they would like included.
	They can also use as many or as few calculators as they wish. */
	

typedef list<PCell>::iterator MatData;

class TreeGroupCommand : public Command {
	
public:
	TreeGroupCommand(string);	
	TreeGroupCommand();
	~TreeGroupCommand();
	
	vector<string> setParameters();
	string getCommandName()			{ return "tree.shared";				}
	string getCommandCategory()		{ return "OTU-Based Approaches";	}
	string getHelpString();	
	string getCitation() { return "http://www.mothur.org/wiki/Tree.shared"; }
	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
private:
	int createTree();
	void printSims(ostream&);
	int makeSimsShared();
	int makeSimsDist();
	
	ReadMatrix* readMatrix;
	SparseMatrix* matrix;
	NameAssignment* nameMap;
	ListVector* list;
	TreeMap* tmap;
	Tree* t;
	vector<Calculator*> treeCalculators;
	vector< vector<float> > simMatrix;
	map<int, int> index;  //maps row in simMatrix to vector index in the tree	
	InputData* input;
	vector<SharedRAbundVector*> lookup;
	string lastLabel;
	string format, outputFile, groupNames, filename, sharedfile, inputfile;
	int numGroups;
	ofstream out;
	float precision, cutoff;

	bool abort, allLines;
	set<string> labels; //holds labels to be used
	string phylipfile, columnfile, namefile, calc, groups, label, outputDir;
	vector<string>  Estimators, Groups, outputNames; //holds estimators to be used
	
	//if the users enters label "0.06" and there is no "0.06" in their file use the next lowest label.
	int process(vector<SharedRAbundVector*>);
	
	

};
	
	
#endif


