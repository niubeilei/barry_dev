////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommandManTester.h
// Description:
//   
//
// Modification History:
// 12/06/2006      Created by Harry Long
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_CommandMan_Tester_CommandManTorturer_h
#define Omn_CommandMan_Tester_CommandManTorturer_h

#include "CommandMan/CommandMan.h"
#include "Tester/TestPkg.h"

struct Dataset
{
	char path[30];
	char md5[33];
	Dataset* next;
};

class Datasets
{
	private:
		Dataset Head;
	public:
		Datasets()
		{
			Head.next = NULL;
		}
		~Datasets(){}
		bool addDataset(Dataset* dataset)
		{
			Dataset* p = Head.next;
			Head.next = dataset;
			Head.next->next = p; 
			return true;
		}
		
		bool deleteDataset(char* path)
		{
			Dataset *p,*q;
			p = &Head;
			q = Head.next;
			while(q)
			{
				if(!strcmp(q->path,path))
			  {
			  	p->next = q->next;
			  	delete q;
			  	return true;
			  }
			  p = q;
			  q = q->next;
			}
			return false;
		}
		
		bool getMD5(char* path, char* md5)
		{
			Dataset *p,*q;
			p = &Head;
			q = Head.next;
			while(q)
			{
				if(!strcmp(q->path,path))
			  {
			  	strcpy(md5,q->md5);
			  	return true;
			  }
			  p = q;
			  q = q->next;
			}
			return false;
		}
};



class CommandManTorturer : public OmnTestPkg
{
private:
       OmnString   mFilePath;
public:
        CommandManTorturer(const OmnString  &FilePath);
        ~CommandManTorturer() {}
        bool setSystemKey();
        bool testMD5Generater();
        bool testAuthenticate();
        bool testProcess();

        virtual bool       start();
        Datasets CommandInCmd;
        Datasets CommandInPolicy;

        int CommandNumCmd;
        int CommandNumPolicy;
        
        OmnString ValidCommands[100];
        int ValidCommandNum;
        OmnString InvalidCommands[100];
        int InvalidCommandNum;
        
        bool setValid(char* command);
        bool setInvalid(char* command);
        
        bool Executed(const char* rslt);
};
#endif
