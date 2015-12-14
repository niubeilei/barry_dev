////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ApplicationProxyTester.h
// Description:
//   
//
// Modification History:
// 	11/16/2006	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Torturer_PppoeTorturer_h
#define Omn_Torturer_PppoeTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosPppoeTorturer : public OmnTestPkg
{
private:
enum
{
	ePppoePasswordSet = 1 ,
	ePppoeUsernameSet = 2 ,
	ePppoeDnsSet      = 3 ,
	ePppoeStop        = 4 ,
	ePppoeStart       = 5 ,
	ePppoeShow        = 6 ,
};  
    
struct pppoeList
{   
	OmnString mUsername;
	OmnString mPassword;
	OmnString mDns1;
	OmnString mDns2;
	OmnString mStatus;
};

private:
pppoeList pppoeSave;
pppoeList pppoeShow;
public:
	AosPppoeTorturer();
	~AosPppoeTorturer() {}

	virtual bool		start();  
	
private:
	    	
bool  runPppoePasswordSet();
bool  runPppoeUsernameSet();
bool  runPppoeDnsSet();                        
bool  runPppoeStop();                             
bool  runPppoeStart();                            
bool  runPppoeShow();  
bool  genName(OmnString &name, int minLen, int maxLen);
bool  genIpAddr(bool isCorrect, OmnString &ip);     
int   readBlock(const char *buffer, const char *start, const char *end, char *result, int result_len);
};  
 
#endif
#   
    
    
    
    
    
    
    