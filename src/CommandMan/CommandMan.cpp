////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommandMan.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CommandMan/CommandMan.h"

#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "keyman.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/md5.h>

int aos_Generate_MD5(const char* path, char* md5)
{
	/////////////// Function Description /////////////////
	// this function generates the MD5 code of a file.
	/////////////////////////////////////////////////////
	
	// 1. set system key
	// if none
	char* SystemKey = "12345678";
	//char* SystemKey = new char[33];
	//if(aos_keymagmt_get_key("SystemKey",SystemKey,33))
	//	return -2;   // fail to get system key
	
	// 2. open file
	//cout<<"1"<<endl;
	struct stat buff_stat;
	//cout<<"2"<<endl;
	//cout<<"path is :"<<path<<endl;
	if(stat(path,&buff_stat) < 0)
	{
		return -1;   // fail to get file
	}
	//cout<<"get stat"<<endl;
	if(!S_ISREG(buff_stat.st_mode)) 
	{
		return -1;   // path is not a file name
	}
		
	FILE* file = 0;
	file = fopen(path, "r");
	if(file == NULL) return -1; // error! fail to open the file

	// 3. call md5 function
	MD5_CTX c_md5;
	unsigned char md_md5[MD5_DIGEST_LENGTH];

	MD5_Init(&c_md5);
	while (!feof(file))
	{
		#define bsize 1024
		char buff[bsize];
		unsigned long len;
		len = fread(buff, 1, bsize, file);		
		MD5_Update(&c_md5, buff, len);
	}
	
	MD5_Update(&c_md5, SystemKey, strlen(SystemKey));
	MD5_Final(md_md5, &c_md5);
	
	// 4. result conversion
	
	for (int i=0; i<MD5_DIGEST_LENGTH; i++)
	sprintf(md5+2*i,"%02x", md_md5[i]);
	md5[32]=0;	
			  
  	//cout<<"md5 code is : "<<md5<<endl;
  fclose(file);
	return 0;
}

void CompareWithDataBase()
{
}



extern int lookupCmdTable(const char* ,char*);
extern char* lookupPolicyTable(const char*);


int aos_comman_Authenticate(const char* path)
{
	///////////   Function Description     //////////////////////
	// this function generates the MAC code of a file, then compares it with the 
	// Mac list in the database, and return a command by the result of the 
	// comparation.
	//
	
	// 1. open DB and compare with the list
	char md5_DB[33];
	int retCmd = 0;//lookupCmdTable(path,md5_DB);
	if(retCmd == 0)  // if the file is stored in DB , variable md5_DB got its MAC 
	// 2. MAC comparation
	{
		char md5[33];
	  if(aos_Generate_MD5(path,md5))
	  {
			return -2; // fail to generate MD5
		}
		if(!strcmp(md5,md5_DB))
		{
			return 0;			// md5 matches!   accept!
		}
		else return -1;  // md5 doesn't match!   deny!
	}
	else  	// can't find the path in DB, so make advanced decision 
	{
		char* retPolicy = 0;//lookupPolicyTable(path);
		if(!strcmp(retPolicy,"accept")) return 0;  // although md5 doesn't match, accept 
		else	return -1;   // deny!
	}
}

