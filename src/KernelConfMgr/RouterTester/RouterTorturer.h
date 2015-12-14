////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosRouterTorturer.h
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_RouterTorturer_h
#define Omn_Torturer_RouterTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#define Max_Num 1024


class AosRouterTorturer : public OmnTestPkg
{
private:
enum
{
	eRouterSetStatus  = 1 ,
	eRouterAddEntry = 2 ,
	eRouterDelEntry = 3 ,
	eRouterShowConfig = 4 ,
	eRouterClearConfig = 5 ,
	eRouterSaveConfig = 6 ,
    eRouterLoadConfig = 7 ,
};

    
struct RouterEntry
{
	OmnString mAlias;
	OmnString mDip;
	OmnString mDmask;
	OmnString mGateWay;
	OmnString mInterface;
	RouterEntry();
    ~RouterEntry();
    	
    AosRouterTorturer::RouterEntry & operator = (const AosRouterTorturer::RouterEntry & simRouterEntry);
	
};

private:
OmnString mStatus;
RouterEntry Router[Max_Num];
    	
AosRouterTorturer & operator = (const AosRouterTorturer & simRouterTorturer);
int mNumRouter;
int mNumRouterShow;
public:
	AosRouterTorturer();
	~AosRouterTorturer() {}

	virtual bool		start();  
	
private:
	    	
bool  runRouterSetStatus();
bool  runRouterAddEntry();
bool  runRouterDelEntry();    
bool  runRouterShowConfig();       
bool  runRouterClearConfig();       
bool  runRouterSaveConfig();
bool  runRouterLoadConfig();
int   initRouter();
bool  genName(OmnString &name, int minLen, int maxLen);
bool  genIpAddr(bool isCorrect, OmnString &ip);    
bool  addToSave(OmnString &alias, OmnString &ip1, OmnString &mask, OmnString &ip2, OmnString &dev);
bool  nameExist(OmnString &name);
bool  delRouter(int RouterIndex); 
bool  genDevName( bool isCorrect, OmnString &devName);
int   readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len);
};  

	
	extern int sgCount;
#endif
#   
    
    
    
    
    
    
    
