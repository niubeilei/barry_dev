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
#ifndef Omn_Torturer_MacTorturer_h
#define Omn_Torturer_MacTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class AosMacTorturer : public OmnTestPkg
{
private:
enum
{
	eMacSet          = 1 ,
	eMacBackSet   	 = 2 ,
	eMacShowConfig   = 3 ,
};  
  
enum
{
	eMaxDev          =4 ,
};
 
struct macList
{   	             
    OmnString mDev;
    OmnString mMac;
    OmnString mOldMac;
};

private:
int     devNum;
macList macSave[eMaxDev];
macList macShow[eMaxDev];
int     showDevNum;
public:
	AosMacTorturer();
	~AosMacTorturer() {}

	virtual bool		start();  
	
private:
	    	
bool runMacSet();
bool runMacBackSet();
bool runMacShowConfig();

bool genDevName( bool isCorrect, OmnString &devname);
bool genMac( bool isCorrect, OmnString &mac);
bool getCurrentMac(OmnString &devname, OmnString & oldmac);
bool addToArry(OmnString &devname, OmnString &mac, OmnString &oldmac);

};  

	
	extern int sgCount;
#endif
   