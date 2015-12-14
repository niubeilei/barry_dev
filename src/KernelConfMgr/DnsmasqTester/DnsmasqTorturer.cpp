////////////////////////////////////////////////////////////////////////////
//
//Copyright(C)2005
//PacketEngineering,Inc.Allrightsreserved.
//
//Redistributionanduseinsourceandbinaryforms,withorwithout
//modificationisnotpermittedunlessauthorizedinwritingbyaduly
//appointedofficerofPacketEngineering,Inc.oritsderivatives
//
//FileName:DnsmasqTester.cpp
//Description:
//dnsmasqCli
//
//1.	dnsmasqsaveconfig5%
//2.	dnsmasqshowconfig5%
//3.	dnsmasqsetname10%
//4.	dnsmasqsetip_range10%
//5.	dnsmasqsetlease_time5%
//6.	dnsmasqsetdns5%
//7.	dnsmasqdelhost5%
//8.	dnsmasqaddhost15%
//9.	dnsmasqaddbind15%
//10.	dnsmasqdelbind5%
//11.	dnsmasqsetexcept_dev5%
//12.	dnsmasqstop5%
//13.	dnsmasqstart5%
//14.	dnsmasqclearconfig5%
//
//ModificationHistory:
//
////////////////////////////////////////////////////////////////////////////
#include"SystemCli/DnsmasqTester/DnsmasqTorturer.h"

#include"aosUtil/Alarm.h"
#include"aosUtil/Tracer.h"
#include"aosUtil/Types.h"
#include"KernelSimu/in.h"
#include"Debug/Debug.h"
#include"KernelInterface/CliProc.h"
#include"Porting/Sleep.h"
#include"Random/RandomInteger.h"
#include"Tester/Test.h"
#include"Tester/TestPkg.h"
#include"Tester/TestMgr.h"
#include"Thread/Mutex.h"
#include"Util/OmnNew.h"
#include"Util/IpAddr.h"
#include"Util/Random.h"
#include"UtilComm/Ptrs.h"


static OmnString sgCliSelectorDef=
"<Elements>"
"<Element>1,	5</Element>"		//dnsmasq save config
"<Element>2,	5</Element>"		//dnsmasq show config
"<Element>3,	10</Element>"		//dnsmasq set name
"<Element>4,	10</Element>"		//dnsmasq set ip_range
"<Element>5,	5</Element>"		//dnsmasq set lease_time
"<Element>6,	8</Element>"	//dnsmasq set dns
"<Element>7,	5</Element>"	//dnsmasq del host
"<Element>8,	15</Element>"	//dnsmasq add host
"<Element>9,	15</Element>"	//dnsmasq add bind
"<Element>10,	5</Element>"	//dnsmasq del bind
"<Element>11,	5</Element>"	//dnsmasq set except_dev
"<Element>12,	5</Element>"	//dnsmasq stop
"<Element>13,	5</Element>"	//dnsmasq start
"<Element>14,	2</Element>"	//dnsmasq clear config
"</Elements>";


AosDnsmasqTorturer::AosDnsmasqTorturer()
{
	mHostname = "rhc";
	mExceptDev = "outcard";
	mIp1 = 0;     
	mIp2 = 0;     
	mNetmask = 0; 
	mStatus  = "stop";
	mLeaseTime = "24h";
	mDns1 = 0;
	mDns2 = 0;
}

bool AosDnsmasqTorturer::start()
{
	OmnBeginTest<<"TestDnsmasqtest";

	//
	//Create the CLI command selector
	//
	AosRandomInteger cliSelector("CliSelector",sgCliSelectorDef,100);

	int count=0;
	bool rslt;
	while(count < sgCount)
	{
		cout <<"========================== Now run the :"<<count+1<<" time =================================================="<<endl;
		int index = cliSelector.nextInt();

		switch(index)
		{

			case eDnsmasqSaveConfig:
				rslt = runDnsmasqSaveConfig();
				break;
			case eDnsmasqShowConfig:
				rslt = runDnsmasqShowConfig();
				break;
			case eDnsmasqSetName:
				rslt = runDnsmasqSetName();
				break;
			case eDnsmasqSetIp_range:
				rslt = runDnsmasqSetIp_range();
				break;
			case eDnsmasqSetLease_time:
				rslt = runDnsmasqSetLease_time();
				break;
			case eDnsmasqSetDns:
				rslt = runDnsmasqSetDns();
				break;
			case eDnsmasqDelHost:
				rslt = runDnsmasqDelHost();
				break;
			case eDnsmasqAddHost:
				rslt = runDnsmasqAddHost();
				break;
			case eDnsmasqAddBind:
				rslt = runDnsmasqAddBind();
				break;
			case eDnsmasqDelBind:
				rslt = runDnsmasqDelBind();
				break;
			case eDnsmasqSetExcept_dev:
				rslt = runDnsmasqSetExcept_dev();
				break;
			case eDnsmasqStop:
				rslt = runDnsmasqStop();
				break;
			case eDnsmasqStart:
				rslt = runDnsmasqStart();
				break;
			case eDnsmasqClearConfig:
				rslt = runDnsmasqClearConfig();
				break;
			default:
				OmnAlarm<<"Invalid index:" << index << enderr;
				break;
		}
		count++;
	//	updateSeed();
	}

	return true;
}

bool AosDnsmasqTorturer::runDnsmasqShowConfig()
{
//dnsmasq show config

	OmnString rslt;
	OmnString cmd;
	char *p;
	char showBuf[1024];
	char	Hostname[32];
	char	ExceptDev[12];
	char	Ip1[32];
	char	Ip2[32];
	char	Netmask[32];
	char	Status[32];
	char	LeaseTime[32];
	char	Dns1[32];
	char	Dns2[32];
	char *tok = "\n";

	mShowNumBind = 0;
	mShowNumHost = 0;

	cmd<<"dnsmasq show config";
	aosRunCli_getStr(cmd,true,rslt);
	memset(showBuf,0,sizeof(showBuf));
	strncpy(showBuf,rslt.getBuffer(),sizeof(showBuf));
	p=strtok(showBuf,tok);

	if(!strcmp("dnsmasq information",p))
		return false;

	p=strtok(NULL,tok);
	/*
	   while(p)
	   {
	   if(!strstr(p,"--------------------------------"))
	   {
	   cout<<"nothing to do\n"<<endl;		
	   }
	   elseif(!strstr(p,"\t   status : "))
	   {
	   sscanf(p,"%*sbridgename:%s",status);
	   dnsmasqShow[mShowNumBridge].mBridgeName=bridgeName;
	   showBridge[mShowNumBridge].mNumDevice=0;
	   mShowNumBridge++;
	   }
	   elseif(!strstr(p,"ip:"))
	   {
	   sscanf(p,"%*sip:%s",ip);
	   showBridge[mShowNumBridge].mBridgeIp=ip;
	   }
	   elseif(!strstr(p,"status:"))
	   {
	   sscanf(p,"%*sstatus:%s",status);
	   showBridge[mShowNumBridge].mStatus=status;
	   }
	   elseif(!strstr(p,"\t\t\tDev\tIP"))
	   {
	   cout<<"nothingtodo\n"<<endl;
	   }
	   elseif(!strstr(p,"\t\t\t----------------------------"))
	   {
	   cout<<"nothingtodo\n"<<endl;
	   }
	   else
	   {
	   sscanf(p,"%s%s",devName,devIp);
	   showBridge[mShowNumBridge].mDev[showBridge[mShowNumBridge].mNumDevice].mDevName=devName;	
	   showBridge[mShowNumBridge].mDev[showBridge[mShowNumBridge].mNumDevice].mIp=mIp;
	   showBridge[mShowNumBridge].mNumDevice++;
	   }

	   p=strtok(NULL,tok);			
	   }


	   if(mNumBridge!=mShowNumBridge)
	   {
	   cout<<"error\n"<<endl;
	   return false;
	   }

	   for(inti=0;i<mNumBridge-1;i++)
	   {
	   for(intj=0;j<mShowNumBridge;j++)
	   {
	   if(bridge[i].mBridgeName==showBridge[j].mBridgeName)
	   {
	   bridge[i].mBridgeName=showBridge[j].mBridgeName="";
	   }
	   }
	   }


	   for(inti=0;i<mNumBridge-1;i++)
	   {
	   if(bridge[i].mBridgeName!="")
	   {
	   cout<<"error!\n"<<endl;
	   }
	   }

	   for(intj=0;j<mShowNumBridge;j++)
	   {
	if(showBridge[j].mBridgeName!="")
	{
		cout<<"error!\n"<<endl;
	}
}*/		
return true;

}

bool AosDnsmasqTorturer::runDnsmasqSetName()
{
	//dnsmasq set name <hostname>
	OmnString	name;
	OmnString	rslt;
	OmnString	cmd;
	
	genName(name, 1, 32);
	cmd<<"dnsmasq set name"<<name;
	aosRunCli_getStr(cmd, true, rslt);
	mHostname=name;

	return true;
}

bool AosDnsmasqTorturer::runDnsmasqSetIp_range()
{
	//dnsmasq set ip_range <ip1> <ip2> <netmask> 

	OmnString   name;
	OmnString	rslt;
	OmnString	cmd;
	OmnString	ip1;
	OmnString	ip2;
	OmnString	netmask;

	cmd << "dnsmasq set ip_range";
	bool ip1Status = RAND_INT(0,100) <  90;
	genIpAddr(ip1Status, ip1);

	bool ip2Status = RAND_INT(0,100) < 90;
	genIpAddr(ip2Status, ip2);

	bool maskStatus = RAND_INT(0, 100) < 90;
	genNetMask(maskStatus, netmask);

	cmd << ip1 << " " << ip2 << " " << netmask;
	aosRunCli_getStr(cmd, ip1Status && ip2Status && maskStatus, rslt);
	//Save the params
	if ( ip1Status && ip2Status && maskStatus )
	{
		mIp1 = ip1;
		mIp2 = ip2;
		mNetmask = netmask;
	}

	return true;
}

bool AosDnsmasqTorturer::runDnsmasqSetLease_time()
{
	//
	// time format 1-59[hHmM]
	// dnsmasq set lease_time <time> 
	OmnString rslt;
	OmnString cmd;
	OmnString leaseTime;
	cmd<<"dnsmasq set lease_time";

	if(	OmnRandom::nextInt(0, 100) > 30)
	{
		//
		//	genrate a correct time
		//
		genLeaseTime(true, leaseTime);
		cmd << " " << leaseTime;
		mLeaseTime = leaseTime;
		aosRunCli_getStr(cmd, true, rslt);

		return true;
	}
	//
	//incorrect time:
	//
	genLeaseTime(false, leaseTime);
	cmd << "" << leaseTime;
	aosRunCli_getStr(cmd, false, rslt);

	return true;	
}

bool AosDnsmasqTorturer::runDnsmasqSetDns()
{
	//dnsmasq dns set(dns1) [<dns2>]

	OmnString	rslt;
	OmnString	cmd;
	OmnString	ip1;
	OmnString	ip2;
	bool isCorrect;

	cmd<<"dnsmasq set dns";

	// gen one dns	
	bool dns1Correct = RAND_INT(0, 100) < 90;
	genIpAddr(dns1Correct, ip1);
	//cout << "dns1Correct is " << dns1Correct << " " << ip1 << endl;

	// gen second dns
	bool dns2Correct = RAND_INT(0, 100) < 70;
	genIpAddr(dns2Correct, ip2);
	//cout << "dns2Correct is " << dns2Correct << " " << ip2 << endl;
	cmd << " " << ip1 << " " << ip2;
	
	aosRunCli_getStr(cmd, dns1Correct && dns2Correct, rslt);

	isCorrect = dns1Correct && dns2Correct;
	
	//cout << "isCorrect is " << isCorrect << endl;
	if (dns1Correct && dns2Correct)
	{
		mDns1 = ip1;
		mDns2 = ip2;
	}
	return true;
}

bool AosDnsmasqTorturer::runDnsmasqDelHost()
{
	//dnsmasq del host <alias>
	OmnString  rslt;
	OmnString  cmd;
	OmnString  alias;

	cmd<<"dnsmasq del host ";

	//remove correct
	if(RAND_INT(0, 100) > 40 && mNumHost > 0)
	{
		//select a exist alias
		int hostIndex = RAND_INT(0,mNumHost-1);
		alias = mDnsmasqHost[hostIndex].mAlias;
		//cout << "Exist hostlist is " << mNumHost << " alias is " << alias << endl;
		cmd << alias;
		aosRunCli_getStr(cmd, true, rslt);
		//del from saved arry
		DelFromHostArry(hostIndex);
	}
	//alias is not exist		
	genName(alias, 1, 32);
	for(int i=0; i < mNumHost; i++)
	{
		if(alias == mDnsmasqHost[i].mAlias)
		return true;
	}

	cmd << alias;
	aosRunCli_getStr(cmd, false, rslt);

	return true;	
}

bool AosDnsmasqTorturer::runDnsmasqAddHost()
{
	//dnsmasq add host <alias> <ip> <domain>
	OmnString rslt;
	OmnString cmd;
	OmnString alias;
	OmnString domain;
	OmnString ip;
	bool isCorrect;
	
	cmd << "dnsmasq add host ";
	genName(alias, 1, 32);
	//judge is add
	for ( int i = 0; i< mNumHost; i++)
	{
		if (alias == mDnsmasqHost[i].mAlias);
		return true;
	}

	genName(domain, 1, 32);
	bool ipCorrect = RAND_INT(0, 100) < 70;
	genIpAddr(ipCorrect, ip);
	
	cmd << alias << " " << ip << " " << domain; 
	aosRunCli_getStr(cmd,isCorrect = (ipCorrect && (mNumHost < MaxHost)), rslt);
	
	if (isCorrect)
	{
	
		//cout << "add success host " << isCorrect << alias
		//	   << " " << ip << " " << domain; 
		AddToHostArry(alias,domain,ip);
	}
	
	return true;
}

bool AosDnsmasqTorturer::runDnsmasqAddBind()
{
	//dnsmasq add bind <alias> <mac> <ip>
	OmnString rslt;
	OmnString cmd;
	OmnString alias;
	OmnString mac;
	OmnString ip;

	cmd << "dnsmasq add bind ";
	genName(alias, 1, 32);

	//judge is add
	for ( int i = 0; i< mNumBind; i++)
	{
		if (alias == mDnsmasqBind[i].mAlias);
		return true;
	}

	//gen ip
	bool ipCorrect = RAND_INT(0, 100) < 90;
	genIpAddr(ipCorrect, ip);
	//gen mac
	bool macCorrect = RAND_INT(0, 100) < 90;
	genMac(macCorrect, mac);
	
	cmd << alias << " " << mac << " " << ip;
	bool isCorrect = (ipCorrect && macCorrect && (mNumBind < MaxBind));
	aosRunCli_getStr(cmd, isCorrect, rslt);
	
	//add to arry
	if (isCorrect)
	{
		AddToBindArry(alias,mac,ip);
	}

	return true;
}

bool AosDnsmasqTorturer::runDnsmasqDelBind()
{
	OmnString rslt;
	OmnString cmd;
	OmnString alias;

	cmd << "dnsmasq del bind ";
	//remove correct
	if(RAND_INT(0, 100) > 40 && mNumBind > 0)
	{
		//select a exist alias
		//cout << mNumBind << endl;
		int bindIndex = RAND_INT(0,mNumBind-1);
		alias = mDnsmasqBind[bindIndex].mAlias;
		cmd << alias;
		aosRunCli_getStr(cmd, true, rslt);
		//del from savedarry
		DelFromBindArry(bindIndex);
	}

	//alias is not exist		
	genName(alias, 1, 32);
	for(int i=0; i<mNumBind-1; i++)
	{
		if(alias = mDnsmasqBind[i].mAlias)
		return true;
	}

	cmd << alias;
	aosRunCli_getStr(cmd,false,rslt);

	return true;	
}

bool AosDnsmasqTorturer::runDnsmasqSetExcept_dev()
{
	OmnString rslt;
	OmnString cmd;
	OmnString devname;

	cmd << "dnsmasq set except_dev ";
    bool devCorrect = RAND_INT(0, 100) < 80 ;
	genDevName(devCorrect, devname);
	cmd << devname;
	aosRunCli_getStr(cmd, devCorrect, rslt);
	//save the params
	if (devCorrect)
	{
		mExceptDev = devname;
	}
	
	return true;
}

bool AosDnsmasqTorturer::runDnsmasqClearConfig()
{
	OmnString rslt;
	OmnString cmd;

	cmd << "dnsmasq clear config";
	//cout << cmd << endl;
	aosRunCli_getStr(cmd, true, rslt);
	mHostname = "rhc";
	mExceptDev = "outcard";
	mIp1 = 0;     
	mIp2 = 0;     
	mNetmask = 0; 
	mStatus  = "stop";
	mLeaseTime = "24h";
	mDns1 = 0;
	mDns2 = 0;
	
	for(int i=0; i<MaxHost; i++)
	{
		mDnsmasqHost[i].mAlias = 0;
		mDnsmasqHost[i].mDomain = 0;
		mDnsmasqHost[i].mIp = 0;
	}

	for(int i=0; i<MaxBind; i++)
	{
		mDnsmasqBind[i].mAlias = 0;
		mDnsmasqBind[i].mMac = 0;
		mDnsmasqBind[i].mIp = 0;
		
	}
	mNumBind = 0;
	mNumHost = 0;

	return true;
}
	
bool AosDnsmasqTorturer::runDnsmasqSaveConfig()
{
	OmnString rslt;
	OmnString cmd;

	cmd<<"dnsmasq save config";
	aosRunCli_getStr(cmd,true,rslt);
	return true;
}


bool AosDnsmasqTorturer::runDnsmasqStop()
{
	//dnsmasq stop
	OmnString rslt;
	OmnString cmd;

	cmd	<< "dnsmasq stop";
	//cout << "##############################run stop " 
	//     << "status is " << mStatus << endl;
	
	if ( mStatus == "stop")
	{
		aosRunCli_getStr(cmd,false,rslt);
		return true;
	}

	aosRunCli_getStr(cmd,true,rslt);
	mStatus = "stop";	
	return true;

}

bool AosDnsmasqTorturer::runDnsmasqStart()
{
	//dnsmasq start
	OmnString rslt;
	OmnString cmd;
	cmd<<"dnsmasq start";
	
	//cout << "##############################run start" << endl;
	if ( mIp1 == 0
		|| mIp2 == 0 
		|| mNetmask == 0 
		|| mDns1 == 0 
		|| mIp1 == ""
		|| mIp2 == "" 
		|| mNetmask == "" 
		|| mDns1 == "")

	{
		aosRunCli_getStr(cmd, false, rslt);
		mStatus = "stop";
		
		return true;
	}
	
	aosRunCli_getStr(cmd, true, rslt);
	mStatus = "start";
}

bool AosDnsmasqTorturer::genIpAddr(bool isCorrect, OmnString & ip)
{
	if(isCorrect)
	{
		ip = OmnRandom::nextStrIP();
	}
	else
	{
		ip = OmnRandom::nextNoSpaceStr(1, 32);
	}
	return true;	
}

bool AosDnsmasqTorturer::genDevName(bool isCorrect, OmnString &devName)
{
	char *dev[2];
	dev[0] = "outcard";
	dev[1] = "incard";
	int index;

	if(isCorrect)
	{
		index = RAND_INT(0, 1);
		devName = dev[index];
	}
	else
	{
	
		if (RAND_INT(0, 100) < 20)
		{
			devName = "";	
		}
		while(1)
		{
			int j = 0;
			devName = OmnRandom::nextNoSpaceStr(1,32);

			for (int i=0;i<2;i++)
			{
				if(devName == dev[i])
				j++;
			}

			if (j == 0)
			break;
		}
	}
	return true;
}

bool AosDnsmasqTorturer::genLeaseTime(bool isCorrect, OmnString &leaseTime)
{

	int time;
	OmnString timeTmp;
	char units[4]={'m','M','H','h'};
	int unIndex;
	char t[3];

	if(isCorrect)
	{
		time = RAND_INT(1,59);
		unIndex = RAND_INT(0,3);
		leaseTime << time << units[unIndex];
	}
	else
	{	
		// input null
		if (RAND_INT(0, 100) < 20)
		{
			leaseTime = "";	
		}

		//incorrect time
		if (RAND_INT(1,100) >40)
		{
			time = RAND_INT(60, 1000);
			unIndex = RAND_INT(0, 3);
			leaseTime << time << units[unIndex];
		}

		//incorrect unit
		if (RAND_INT(1,100) > 60)
		{
			while(1)
			{
				time = RAND_INT(1, 59);
				char tmpunit[1];
				bool isEqual = false; 
				OmnString timeTmp = OmnRandom::nextNoSpaceStr(1,1);
				strncpy(tmpunit,timeTmp.data(),1);
				//incorrect unit
				if((tmpunit[0] =='m')||(tmpunit[0] =='M')
				||(tmpunit[0] =='h')||(tmpunit[0] =='H'))
				{
					isEqual = true;
				}
				
				if (!isEqual)
				leaseTime << time << tmpunit;
				return true;
			}
		}
		//both incorrect
		time = RAND_INT(60,1000);
		while(1)
		{
			char tmpunit[1];
			bool isEqual = false;
			OmnString timeTmp = OmnRandom::nextNoSpaceStr(1,1);
			strncpy(tmpunit,timeTmp.data(),1);
			//incorrect unit
			if((tmpunit[0] =='m')||(tmpunit[0] =='M')
				||(tmpunit[0] =='h')||(tmpunit[0] =='H'))
			{
				isEqual = true;
			}

			if (!isEqual)
			leaseTime << time << tmpunit;
			return true;
		}
	}

	return true;
}

bool AosDnsmasqTorturer::genMac( bool isCorrect, OmnString &mac)
{
	if(isCorrect)
	{
		mac = OmnRandom::nextNetMac();
	   //cout << "MAC:" << mac << endl;
	}
	else
	{
		// input null
		if (RAND_INT(0, 100) < 20)
		{
			mac	= "";	
		}

		mac = OmnRandom::nextNoSpaceStr(3,20);
	}
	return true;
}

bool AosDnsmasqTorturer::genName(OmnString &name,int minLen,int maxLen)
{
	name = OmnRandom::nextLetterStr(minLen,maxLen);
	return true;
}


bool AosDnsmasqTorturer::AddToBindArry(OmnString &alias, OmnString &mac, OmnString &ip)
{
	if (mNumBind >= MaxBind)
	{
		return false;
	}

	mDnsmasqBind[mNumBind].mAlias = alias;
	mDnsmasqBind[mNumBind].mMac = mac;
	mDnsmasqBind[mNumBind].mIp = ip;
	mNumBind++;
	return true;
}

bool AosDnsmasqTorturer::AddToHostArry(OmnString &alias, OmnString &domain, OmnString &ip)
{
	if(mNumHost >= MaxHost)
	{
		return false;
	}

	mDnsmasqHost[mNumHost].mAlias = alias;
	mDnsmasqHost[mNumHost].mDomain = domain;
	mDnsmasqHost[mNumHost].mIp = ip;
	mNumHost++;
	return true;
}

bool AosDnsmasqTorturer::DelFromBindArry(int &BindIndex)
{
	//cout << mNumBind << endl;
	for(int i=BindIndex; i<(mNumBind-1); i++)
	{
		mDnsmasqBind[i] = mDnsmasqBind[i+1];
	
	}
	mNumBind--;
}

bool AosDnsmasqTorturer::DelFromHostArry(int &hostIndex)
{
	//cout << mNumHost << endl;
	for(int i = hostIndex; i < (mNumHost-1); i++)
	{
		mDnsmasqHost[i] = mDnsmasqHost[i+1];
	}
	mNumHost--;	
}


AosDnsmasqTorturer::dnsmasqBindEntry::dnsmasqBindEntry()
{
}

AosDnsmasqTorturer::dnsmasqBindEntry::~dnsmasqBindEntry()
{
}


AosDnsmasqTorturer::dnsmasqHostsEntry::dnsmasqHostsEntry()
{
}

AosDnsmasqTorturer::dnsmasqHostsEntry::~dnsmasqHostsEntry()
{
}

AosDnsmasqTorturer::dnsmasqBindEntry & AosDnsmasqTorturer::dnsmasqBindEntry::operator = (const AosDnsmasqTorturer::dnsmasqBindEntry & simdnsmasqBindEntry)
{
	mAlias = simdnsmasqBindEntry.mAlias;
	mMac   = simdnsmasqBindEntry.mMac;
	mIp    = simdnsmasqBindEntry.mIp;
}

AosDnsmasqTorturer::dnsmasqHostsEntry & AosDnsmasqTorturer::dnsmasqHostsEntry::operator = (const AosDnsmasqTorturer::dnsmasqHostsEntry & simdnsmasqHostsEntry)
{
	mAlias = simdnsmasqHostsEntry.mAlias;
	mIp    = simdnsmasqHostsEntry.mIp;
	mDomain= simdnsmasqHostsEntry.mDomain;
}

AosDnsmasqTorturer & AosDnsmasqTorturer::operator = (const AosDnsmasqTorturer & simdnsmasqTorturer)
{
	mHostname  = simdnsmasqTorturer.mHostname;
	mExceptDev = simdnsmasqTorturer.mExceptDev;
	mIp1       = simdnsmasqTorturer.mIp1;
	mIp2       = simdnsmasqTorturer.mIp2;
	mNetmask   = simdnsmasqTorturer.mNetmask;
	mStatus    = simdnsmasqTorturer.mStatus;
	mLeaseTime = simdnsmasqTorturer.mLeaseTime;
	mDns1 = simdnsmasqTorturer.mDns1;
	mDns2 = simdnsmasqTorturer.mDns2;

	for(int i=0; i<MaxHost-1; i++)
	{
		mDnsmasqHost[i] = simdnsmasqTorturer.mDnsmasqHost[i];
	}

	for(int i=0; i<MaxBind-1; i++)
	{
		mDnsmasqBind[i] = simdnsmasqTorturer.mDnsmasqBind[i];
	}

}

bool AosDnsmasqTorturer::genNetMask(bool isCorrect, OmnString &netmask)
{
	if(isCorrect)
	{
		netmask = OmnRandom::nextStrIP();
	}
	else
	{
		netmask = OmnRandom::nextNoSpaceStr(1,20);
	}
	return true;	

}
