////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.cpp
// Description:
//   
//
// Modification History:
// 12/06/2006      Created by Harry Long
//
////////////////////////////////////////////////////////////////////////////
#include "CommandMan/Tester/CommandManTorturer.h"

#include "Tester/Test.h"
#include "Tester/TestMgr.h"

#include "Debug/Debug.h"

#include "Tracer/Tracer.h"
#include "Util/Random.h"
#include "Util/RandomSelector.h"

#include "keyman/keyman.h"

#include "Util/Random.h"
#include "Util/RandomSelector.h"

#include "KernelInterface/CliSysCmd.h"

#include "sqlite3.h"
#include <stdlib.h>

#define FILE_NAME1 "/bin"
#define FILE_NAME2 "/bin/cp"
#define FILE_NAME3 "/bin/env"

#define DB_ERROR 32256

CommandManTorturer::CommandManTorturer(const OmnString  &FilePath): mFilePath(FilePath)
{
  CommandNumCmd = 0;
  CommandNumPolicy = 0;
 
 	ValidCommandNum = 0;
 	InvalidCommandNum = 0;
}

bool CommandManTorturer::start()
{
        OmnBeginTest << "Begin the CommandMan Torturer Testing";
        mTcNameRoot = "CommandMan_Torturer";
				
				//if(setSystemKey() == false) return false;
				
				//testMD5Generater();
				//testAuthenticate();
			
		ValidCommandNum = 0;
		InvalidCommandNum = 0;
				testProcess();
        return true;
}

bool CommandManTorturer::setSystemKey()
{
	if(!aos_keymagmt_add_key("SystemKey","12345678"))
		return true;
	else return false;	
}

bool CommandManTorturer::testMD5Generater()
{
	char filename[50];
	int NumOfFile = 1;
	// 1. create a file in CommandMan/Tester/File/
	while(NumOfFile < 2)
	{
		sprintf(filename,"%s%d","File/file",NumOfFile);
		cout<<"file is : "<<filename<<endl;
		FILE* file = 0;
		file = fopen(filename, "a");
		if(!file)
		{
			cout<<"fail to create file! "<<endl;
			return false;
		}
		char data[20000];
		int datalen = OmnRandom::nextStr(data, 10000, 20000); 
		int datalen_file = fwrite(data,sizeof(char),datalen,file);
		fclose(file);
		if(datalen != datalen_file) return false;
		char md5[33];
		//cout<<"start!"<<endl;
		int rslt = aos_Generate_MD5(FILE_NAME1,md5);
		if(rslt == 0) cout<<"md5: "<<md5<<endl;
			else cout<<"error!"<<endl;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << "Failed to generate MD5 : " << rslt <<endtc;
		
		NumOfFile++;	
	}
	return true;
}

bool CommandManTorturer::testAuthenticate()
{
	int rslt = aos_comman_Authenticate(FILE_NAME1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	return true;
}

bool CommandManTorturer::testProcess()
{
	///////////   Description    /////////////////////
	//  this function simulate the executable command process ,
	///////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////
	//  first , open DB and clear it 
	
	cout<<"starting test process!"<<endl;
	
	sqlite3 *db=NULL;
	char *zErrMsg = 0;
	char sql[1024];
	int rc;
	rc = sqlite3_open("../../aos_secure/securecmd.db", &db);
	if(rc)
	{
		cout<<"Can't open database: "<< sqlite3_errmsg(db)<<endl;
		sqlite3_close(db);
		return false;
	}
	
	sprintf(sql, "delete from cmdtable");
	sqlite3_exec(db, sql,0,0,&zErrMsg);
	
	cout<<"DB cleared!"<<endl;
	///////////////////////////////////////////////////////////
	
	// 1.generate data
	//  the data is stored in file "command.txt" ,  
	//OmnFile commandfile("command.txt", OmnFile::eReadOnly);
	OmnFile commandfile(mFilePath.data(), OmnFile::eReadOnly);
	if(!commandfile.isGood())
	{
		cout<<"file open error"<<endl;
		sqlite3_close(db);	
		return false;
	}
	
	OmnString bufline;
	bool finished = false;
	while(!finished)
	{
		//cout<<"the commandNum is : "<<CommandNum<<endl;
		bufline = commandfile.getLine(finished);
		////////////// Tip: about the function OmnString::getLine() //////////////
		// when call this function , the caller with get a string added "\n" at the tail
		// but we don't want to get a "\n" most of the times.
		// so use this statement:  str[strlen(str)-1] = 0;
		//
		Dataset p;
		strcpy(p.path,bufline.data());
		p.path[strlen(p.path)-1] = 0;
		
		/*
		cout<<"path is : [path]"<<p->path;
		cout<<"[path]"<<endl;
		cout<<"length = "<<strlen(p->path)<<endl;
		*/
		
		int ret = aos_Generate_MD5(p.path, p.md5);
		if(ret != 0)
		{
			cout<<"fail to generate MD5 from" <<p.path<<endl;
			cout<<"ret = "<<ret<<endl;
			//delete p;
			continue;
		}
		//cout<<"md5 generated is : "<<p->md5<<endl;
		int Valid = OmnRandom::nextInt(0, 2);
		if(Valid == 0)
		{
			sprintf(sql, "insert into cmdtable values(\"%s\",\"%s\");",p.path, "this is a fake MD5 code");
			sqlite3_exec(db, sql,0,0,&zErrMsg);
			setInvalid(p.path);
			
		}
		else
		{
			sprintf(sql, "insert into cmdtable values(\"%s\",\"%s\");",p.path, p.md5);
			sqlite3_exec(db, sql,0,0,&zErrMsg);
			setValid(p.path);
		}
	}
	
	sqlite3_close(db);
	cout<<"succeed to add data into DB !"<<endl;
	cout<<"we have "<<ValidCommandNum<<" valid and "<<InvalidCommandNum<<" invalid commands"<<endl;
	cout<<"valid cmds are :"<<endl;
  	for(int i=0;i<ValidCommandNum;i++)
  		cout<<ValidCommands[i]<<endl;
  	cout<<"invalid cmds are :"<<endl;
  	for(int i=0;i<InvalidCommandNum;i++)
  		cout<<InvalidCommands[i]<<endl;
  	
	// 2.execute the command
	int tries = 505;
	while(tries)		
	{
		
		cout<<"tries = " <<tries<<endl;

		// 2.0 Random choose if the command is avalible
		int valid = OmnRandom::nextInt(0, 2);
		cout<<"valid = "<<valid<<endl;
		OmnString cmdrslt;
		cmdrslt.reset();
		if(valid)
		// 2.1 Random select the command to execute
		{
			if(ValidCommandNum == 0)
			{
				cout<<"No Valid Command!"<<endl;
				tries--;
				continue;
			}
			int OneOfCommands = OmnRandom::nextInt(0, ValidCommandNum-1);
			cout<<"valid command is : "<<ValidCommands[OneOfCommands]<<endl;
			//cmdrslt="";
			//OmnCliSysCmd::doShell(ValidCommands[OneOfCommands], cmdrslt);
			//cout<<"command executed!"<<endl;
//			cmdrslt.reset();
//			OmnCliSysCmd::doShell("kl", cmdrslt);
//			char rst[200];
//			strcpy(rst,cmdrslt.data());
//			cout<<rst<<endl;
			
			//bool executed = Executed(cmdrslt.data());
			unsigned int rslt = system(ValidCommands[OneOfCommands]);
			cout<<"rslt = "<<rslt<<endl;
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt != DB_ERROR)) << endtc;
			//if(rslt == 32256) return false;
		}
		
		else
		{
			//OmnCliSysCmd::doShell("kl", cmdrslt);
			//continue;
			///// do some invalid cmd

			if(InvalidCommandNum == 0)
			{
				cout<<"No Invalid Command!"<<endl;
				tries--;
				continue;
			}
			int OneOfCommands = OmnRandom::nextInt(0, InvalidCommandNum-1);
			cout<<"invalid command is : "<<InvalidCommands[OneOfCommands]<<endl;
			//cmdrslt = "";
			//OmnCliSysCmd::doShell(/*InvalidCommands[OneOfCommands]*/ "ijnviwds", cmdrslt);
			//cout<<"test:"<<cmdrslt<<"\t len:"<<strlen(cmdrslt)<<endl;
			//cout<<"command executed!"<<endl;
			
			unsigned int rslt = system(InvalidCommands[OneOfCommands]);
			cout<<"rslt = "<<rslt<<endl;
			//bool executed = Executed(cmdrslt.data());
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt == DB_ERROR)) << endtc;
			//if(rslt == 32256) return false;
		}
		
		//// 3.result exam
		//////////////////////////////////////////////////////////////////
		
		tries--;
		//if(tries%1 == 0) sleep(1);
	}

	
	//sqlite3_exec(db,"delete from cmdtable",0,0,&zErrMsg);
	cout<<"end of the function"<<endl;
	return true;
}

bool CommandManTorturer::Executed(const char* rslt)
{
	////////////// this function checks if a command has been executed by rslt format of it ////////
	///  if it has been executed, return true ; else return false;
	///
	char* gotit = (char*)rslt;
	cout<<"rslt = "<<rslt<<endl;
	gotit = strstr(gotit,"Cannot allocate memory");
	if(gotit == NULL) return true;
	else return true;
}
	
bool CommandManTorturer::setValid(char* command)
{
	if(command != NULL)
	{
		ValidCommands[ValidCommandNum] = command;
		ValidCommandNum++;
	}
	else return false;
	
	/*
	if(!strcmp(command,"/bin/ls"))
	{
		ValidCommands[ValidCommandNum] = "ls -f";
		ValidCommandNum++;
		return true;
	}

	if(!strcmp(command,"/bin/arch"))
	{
		ValidCommands[ValidCommandNum] = "arch -f";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/bzdiff"))
	{
		ValidCommands[ValidCommandNum] = "bzdiff 1.txt 2.txt -f";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/cp"))
	{
		ValidCommands[ValidCommandNum] = "cp 1.txt cp1.txt -f";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/bzgrep"))
	{
		ValidCommands[ValidCommandNum] = "bzgrep line 1.txt";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/basename"))
	{
		ValidCommands[ValidCommandNum] = "basename 1.txt";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/bzmore"))
	{
		ValidCommands[ValidCommandNum] = "bzmore 1.txt";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/cat"))
	{
		ValidCommands[ValidCommandNum] = "cat 1.txt";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/chmod"))
	{
		ValidCommands[ValidCommandNum] = "chmod cp1.txt -w";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/cksum"))
	{
		ValidCommands[ValidCommandNum] = "cksum 1.txt";
		ValidCommandNum++;
		return true;
	}	
	if(!strcmp(command,"/bin/comm"))
	{
		ValidCommands[ValidCommandNum] = "comm 1.txt 2.txt";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/date"))
	{
		ValidCommands[ValidCommandNum] = "date";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/df"))
	{
		ValidCommands[ValidCommandNum] = "df";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dir"))
	{
		ValidCommands[ValidCommandNum] = "dir";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dircolors"))
	{
		ValidCommands[ValidCommandNum] = "dircolors";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dirname"))
	{
		ValidCommands[ValidCommandNum] = "dirname /bin/dirname";
		ValidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dmesg"))
	{
		ValidCommands[ValidCommandNum] = "dmesg";
		ValidCommandNum++;
		return true;
	}
	return true;*/
}

bool CommandManTorturer::setInvalid(char* command)
{
	if(command != NULL)
	{
		InvalidCommands[InvalidCommandNum] = command;
		InvalidCommandNum++;
	}
	else return false;
	
	/*
	if(!strcmp(command,"/bin/ls"))
	{
		InvalidCommands[InvalidCommandNum] = "ls -f";
		InvalidCommandNum++;
		return true;
	}

	if(!strcmp(command,"/bin/arch"))
	{
		InvalidCommands[InvalidCommandNum] = "arch -f";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/bzdiff"))
	{
		InvalidCommands[InvalidCommandNum] = "bzdiff 1.txt 2.txt -f";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/cp"))
	{
		InvalidCommands[InvalidCommandNum] = "cp 1.txt cp1.txt -f";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/bzgrep"))
	{
		InvalidCommands[InvalidCommandNum] = "bzgrep line 1.txt";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/basename"))
	{
		InvalidCommands[InvalidCommandNum] = "basename 1.txt";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/bzmore"))
	{
		InvalidCommands[InvalidCommandNum] = "bzmore 1.txt";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/cat"))
	{
		InvalidCommands[InvalidCommandNum] = "cat 1.txt";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/chmod"))
	{
		InvalidCommands[InvalidCommandNum] = "chmod cp1.txt -w";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/cksum"))
	{
		InvalidCommands[InvalidCommandNum] = "cksum 1.txt";
		InvalidCommandNum++;
		return true;
	}	
	if(!strcmp(command,"/bin/comm"))
	{
		InvalidCommands[InvalidCommandNum] = "comm 1.txt 2.txt";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/date"))
	{
		InvalidCommands[InvalidCommandNum] = "date";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/df"))
	{
		InvalidCommands[InvalidCommandNum] = "df";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dir"))
	{
		InvalidCommands[InvalidCommandNum] = "dir";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dircolors"))
	{
		InvalidCommands[InvalidCommandNum] = "dircolors";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dirname"))
	{
		InvalidCommands[InvalidCommandNum] = "dirname /bin/dirname";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/dmesg"))
	{
		InvalidCommands[InvalidCommandNum] = "dmesg";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/du"))
	{
		InvalidCommands[InvalidCommandNum] = "du";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/echo"))
	{
		InvalidCommands[InvalidCommandNum] = "echo";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/env"))
	{
		InvalidCommands[InvalidCommandNum] = "env";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/expr"))
	{
		InvalidCommands[InvalidCommandNum] = "expr";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/false"))
	{
		InvalidCommands[InvalidCommandNum] = "false";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/fuser"))
	{
		InvalidCommands[InvalidCommandNum] = "fuser";
		InvalidCommandNum++;
		return true;
	}
	if(!strcmp(command,"/bin/grep"))
	{
		InvalidCommands[InvalidCommandNum] = "grep";
		InvalidCommandNum++;
		return true;
	}
	return true;
	*/
}
