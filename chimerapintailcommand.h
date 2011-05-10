#ifndef CHIMERAPINTAILCOMMAND_H
#define CHIMERAPINTAILCOMMAND_H

/*
 *  chimerapintailcommand.h
 *  Mothur
 *
 *  Created by westcott on 4/1/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "mothur.h"
#include "command.hpp"
#include "chimera.h"


/***********************************************************/

class ChimeraPintailCommand : public Command {

public:

	ChimeraPintailCommand(string);
	ChimeraPintailCommand();
	~ChimeraPintailCommand(){}
	
	vector<string> setParameters();
	string getCommandName()			{ return "chimera.pintail";		}
	string getCommandCategory()		{ return "Sequence Processing"; }
	string getHelpString();	
	string getCitation() { return "At Least 1 in 20 16S rRNA Sequence Records Currently Held in the Public Repositories is Estimated To Contain Substantial Anomalies' paper by Kevin E. Ashelford 1, Nadia A. Chuzhanova 3, John C. Fry 1, Antonia J. Jones 2 and Andrew J. Weightman 1. http://www.mothur.org/wiki/Chimera.pintail"; }

	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }		
private:

	struct linePair {
		unsigned long int start;
		unsigned long int end;
		linePair(unsigned long int i, unsigned long int j) : start(i), end(j) {}
	};

	vector<int> processIDS;   //processid
	vector<linePair*> lines;
	
	int driver(linePair*, string, string, string);
	int createProcesses(string, string, string);
	
	#ifdef USE_MPI
	int driverMPI(int, int, MPI_File&, MPI_File&, MPI_File&, vector<unsigned long int>&);
	#endif

	bool abort, filter;
	string fastafile, templatefile, consfile, quanfile, maskfile, outputDir, inputDir;
	int processors, window, increment, numSeqs, templateSeqsLength;
	Chimera* chimera;
	vector<string> outputNames;
	vector<string> fastaFileNames;
};

/***********************************************************/

#endif


