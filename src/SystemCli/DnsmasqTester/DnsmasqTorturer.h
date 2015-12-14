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
#ifndef Omn_Torturer_DnsmasqTorturer_h
#define Omn_Torturer_DnsmasqTorturer_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosDnsmasqTorturer : public OmnTestPkg
{
private:
	enum
	{
		eDnsmasqSaveConfig     =1 ,
		eDnsmasqShowConfig     =2 ,
		eDnsmasqSetName        =3 ,
		eDnsmasqSetIp_range    =4 ,
		eDnsmasqSetLease_time  =5 ,
	    eDnsmasqSetDns         =6 ,
	    eDnsmasqDelHost        =7 ,
	    eDnsmasqAddHost        =8 ,
	    eDnsmasqAddBind        =9 ,
	    eDnsmasqDelBind        =10,
	    eDnsmasqSetExcept_dev  =11,
	    eDnsmasqStop           =12,
	    eDnsmasqStart          =13,
	    eDnsmasqClearConfig    =14,
	
	};    

	enum
	{
		MaxBind = 512,
		MaxHost = 1024,
	};                 
	                       
	struct dnsmasqBindEntry
	{
		OmnString mAlias;
		OmnString mMac;
		OmnString mIp;
		dnsmasqBindEntry();
	    ~dnsmasqBindEntry();
	    	
	    AosDnsmasqTorturer::dnsmasqBindEntry & operator = (const AosDnsmasqTorturer::dnsmasqBindEntry & simdnsmasqBindEntry);
		
	};
	
	struct dnsmasqHostsEntry
	{
		OmnString mAlias;
		OmnString mIp;
		OmnString mDomain;
		dnsmasqHostsEntry();
	    ~dnsmasqHostsEntry();
	    	
	   	AosDnsmasqTorturer::dnsmasqHostsEntry & operator = (const AosDnsmasqTorturer::dnsmasqHostsEntry & simdnsmasqHostsEntry);
		
	};
	
	/*	struct dnsmasqConfigEntry
	{
		OmnString mHostname;
		OmnString mExceptDev;
		OmnString mIp1;
		OmnString mIp2;
		OmnString mNetmask;
		OmnString mStatus;
		OmnString mLeaseTime;
		OmnString mDns1;
		OmnString mDns2;
		//OmnString ip;
		//OmnString mac;
	
		dnsmasqConfigEntry();
	    ~dnsmasqConfigEntry();
	    	
	    	AosDnsmasqTorturer::dnsmasqConfigEntry & operator = (const AosDnsmasqTorturer::dnsmasqConfigEntry & simdnsmasqConfigEntry);
	}; 
*/	
private:
	// dnsmasqConfigEntry dnsmasqSave;
	// dnsmasqConfigEntry dnsmasqShow;
	OmnString mHostname;
	OmnString mExceptDev;
	OmnString mIp1;
	OmnString mIp2;
	OmnString mNetmask;
	OmnString mStatus;
	OmnString mLeaseTime;
	OmnString mDns1;
	OmnString mDns2;
	dnsmasqHostsEntry mDnsmasqHost[MaxHost];
	dnsmasqBindEntry mDnsmasqBind[MaxBind];
	AosDnsmasqTorturer & operator = (const AosDnsmasqTorturer & simdnsmasqTorturer);

	int			mNumBind;
	int         mNumHost;
	int			mShowNumBind;
	int         mShowNumHost;
public:
	AosDnsmasqTorturer();
	~AosDnsmasqTorturer() {}

	virtual bool		start();  
	
private:
	    	
	bool  runDnsmasqSaveConfig(); 
	bool  runDnsmasqShowConfig();
	bool  runDnsmasqSetName();           
	bool  runDnsmasqSetIp_range();              
	bool  runDnsmasqSetLease_time();              
	bool  runDnsmasqSetDns();         
	bool  runDnsmasqDelHost();        
	bool  runDnsmasqAddHost();        
	bool  runDnsmasqAddBind();        
	bool  runDnsmasqDelBind();        
	bool  runDnsmasqSetExcept_dev();  
	bool  runDnsmasqStop();           
	bool  runDnsmasqStart();          
	bool  runDnsmasqClearConfig();  
	bool  genName(OmnString &name, int minLen, int maxLen);
	bool  genIpAddr(bool isCorrect, OmnString &ip);  
	bool  genDevName( bool isCorrect, OmnString &devName);
	bool  genMac( bool isCorrect, OmnString &mac);
	bool  genLeaseTime( bool isCorrect, OmnString &leaseTime);  
	bool  AddToBindArry(OmnString &alias, OmnString &mac, OmnString &ip);
	bool  AddToHostArry(OmnString &alias, OmnString &domain, OmnString &ip);
	bool  DelFromBindArry(int &bindIndex);
	bool  DelFromHostArry(int &hostIndex);
	bool  genNetMask(bool isCorrect, OmnString &netmask);
};
extern int sgCount;
#endif
  
    
    
    
    
    
    
    
