#ifndef SFFINFOCOMMAND_H
#define SFFINFOCOMMAND_H

/*
 *  sffinfocommand.h
 *  Mothur
 *
 *  Created by westcott on 7/7/10.
 *  Copyright 2010 Schloss Lab. All rights reserved.
 *
 */

#include "command.hpp"


/**********************************************************/
struct CommonHeader {
	unsigned int magicNumber;
	string version;
	unsigned long int indexOffset;
	unsigned int indexLength;
	unsigned int numReads;
	unsigned short headerLength;
	unsigned short keyLength;
	unsigned short numFlowsPerRead;
	int flogramFormatCode;
	string flowChars; //length depends on number flow reads
	string keySequence; //length depends on key length
	
	CommonHeader(){ magicNumber=0; indexOffset=0; indexLength=0; numReads=0; headerLength=0; keyLength=0; numFlowsPerRead=0; flogramFormatCode='s'; } 
	~CommonHeader() { }
};
/**********************************************************/
struct Header {
	unsigned short headerLength;
	unsigned short nameLength;
	unsigned int numBases;
	unsigned short clipQualLeft;
	unsigned short clipQualRight;
	unsigned short clipAdapterLeft;
	unsigned short clipAdapterRight;
	string name; //length depends on nameLength
	string timestamp;
	string region;
	string xy;
	
	Header() { headerLength=0; nameLength=0; numBases=0; clipQualLeft=0; clipQualRight=0; clipAdapterLeft=0; clipAdapterRight=0; }
	~Header() { } 
};
/**********************************************************/
struct seqRead {
	vector<unsigned short> flowgram;
	vector<unsigned int> flowIndex;
	string bases;
	vector<unsigned int> qualScores;
	
	seqRead() { } 
	~seqRead() { } 
};
/**********************************************************/

class SffInfoCommand : public Command {
	
public:
	SffInfoCommand(string);
	SffInfoCommand();
	~SffInfoCommand(){}
	
	vector<string> setParameters();
	string getCommandName()			{ return "sffinfo";					}
	string getCommandCategory()		{ return "Sequence Processing";		}
	string getHelpString();	
	string getCitation() { return "http://www.mothur.org/wiki/Sffinfo"; }
	
	int execute(); 
	void help() { m->mothurOut(getHelpString()); }	
	
private:
	string sffFilename, sfftxtFilename, outputDir, accnosName;
	vector<string> filenames, outputNames, accnosFileNames;
	bool abort, fasta, qual, trim, flow, sfftxt, hasAccnos;
	int mycount;
	set<string> seqNames;
	
	//extract sff file functions
	int extractSffInfo(string, string);
	int readCommonHeader(ifstream&, CommonHeader&);
	int readHeader(ifstream&, Header&);
	int readSeqData(ifstream&, seqRead&, int, int);
	int decodeName(string&, string&, string&, string);
	
	int printCommonHeader(ofstream&, CommonHeader&); 
	int printHeader(ofstream&, Header&);
	int printSffTxtSeqData(ofstream&, seqRead&, Header&);
	int printFlowSeqData(ofstream&, seqRead&, Header&);
	int printFastaSeqData(ofstream&, seqRead&, Header&);
	int printQualSeqData(ofstream&, seqRead&, Header&);
	int readAccnosFile(string);
	int parseSffTxt();
	
	//parsesfftxt file functions
	int parseHeaderLineToInt(ifstream&);
	vector<unsigned short> parseHeaderLineToFloatVector(ifstream&, int);
	vector<unsigned int> parseHeaderLineToIntVector(ifstream&, int);
	string parseHeaderLineToString(ifstream&);
};

/**********************************************************/
 
#endif


