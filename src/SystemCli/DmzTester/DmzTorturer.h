////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosDmzTorturer.h
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_DmzTorturer_h
#define Omn_Torturer_DmzTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#define Max_Num 1024


class AosDmzTorturer : public OmnTestPkg
{
private:
enum
{
	eDmzSetStatus  = 1 ,
	eDmzAddMachine = 2 ,
	eDmzDelMachine = 3 ,
	eDmzShowConfig = 4 ,
	eDmzClearConfig = 5 ,
	eDmzSaveConfig = 6 ,
   // eDmzLoadConfig = 7 ,
};
    
struct dmzMachineEntry
{
	OmnString mAlias;
	OmnString mPub_ip;
	OmnString mDmz_ip;
	dmzMachineEntry();
    	~dmzMachineEntry();
    	
    	AosDmzTorturer::dmzMachineEntry & operator = (const AosDmzTorturer::dmzMachineEntry & simDmzMachineEntry);
	
};

private:
OmnString mStatus;
dmzMachineEntry dmzMachine[Max_Num];
    	AosDmzTorturer & operator = (const AosDmzTorturer & simDmzTorturer);
int mNumDmz;
int mNumDmzShow;
public:
	AosDmzTorturer();
	~AosDmzTorturer() {}

	virtual bool		start();  
	
private:
	    	
bool  runDmzSetStatus();
bool  runDmzAddMachine();
bool  runDmzDelMachine();    
bool  runDmzShowConfig();       
bool  runDmzClearConfig();       
bool  runDmzSaveConfig();
bool  runDmzLoadConfig();
int   initDmz();
bool  genName(OmnString &name, int minLen, int maxLen);
bool  genIpAddr(bool isCorrect, OmnString &ip);    
bool  addToSave(OmnString alias, OmnString &ip1, OmnString &ip2);
bool  nameExist(OmnString &name);
bool  delDmz(int DmzIndex); 
int   readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len);
};  

	extern int sgCount;
	
#endif
#   
    
    
    
    
    
    
    
