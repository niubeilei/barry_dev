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
#ifndef Omn_Torturer_VpnPassTorturer_h
#define Omn_Torturer_VpnPassTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class AosVpnPassTorturer : public OmnTestPkg
{
private:
enum
{
	eFirewallL2tpPass    = 1 ,
	eFirewallPptpPass    = 2 ,
	eFirewallIpsecPass   = 3 ,
};  
  

 
struct fwVpnPass
{
    OmnString ipsecStat;
    OmnString pptpStat;
    OmnString l2tpStat;
};

private:

fwVpnPass   VpnPassSave;

public:
	AosVpnPassTorturer();
	~AosVpnPassTorturer() {}

	virtual bool		start();  
	
private:
	    	
bool runVpnL2tpPass();
bool runVpnPptpPass();
bool runVpnIpsecPass();
bool genCom(bool isCorrect, OmnString & command);



};  
 extern int sgCount;
#endif
   
                                       