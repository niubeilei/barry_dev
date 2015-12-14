////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Torturers/XmlDocTorturer/TesterGetDomain.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/SengTesterThrd.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SengTorUtil/SengTesterFileMgr.h"
#include "SengTorUtil/StOprTranslator.h"
#include "SengTorUtil/StDoc.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/UtUtil.h"
#include <stdlib.h>
#include <regex.h>

static u32 sgErr = 0;
AosGetDomainTester::AosGetDomainTester(const bool regflag)
:
AosSengTester(AosSengTester_GetDomain, AosTesterId::eGetDomain, regflag)
{
}


AosGetDomainTester::AosGetDomainTester()
:
AosSengTester(AosSengTester_GetDomain, "getdomain", AosTesterId::eGetDomain)
{
}

AosGetDomainTester::~AosGetDomainTester()
{
}


bool 
AosGetDomainTester::test()
{
/*
	int tries = rand() % eGetDomainTryWeight + 1;

	while (tries-- > 0)
	{
		OmnString domain;
		aos_assert_r(AosSengAdmin::getSelf()->getDomain(domain), false);
		aos_assert_r(checkDomain(domain), false);
	}

	return true;
*/

	return false;
}


bool
AosGetDomainTester::checkDomain(const OmnString &domain)
{
//OmnScreen << "==========================" << domain << endl;
	const char* s_tmp = domain.data();
	const char* pattern = "(http?)://.*(/images)$";  

	int z=0;  
	regex_t reg;  
	regmatch_t pm[1];                                     

	z = regcomp(&reg, pattern, REG_EXTENDED|REG_ICASE);  
	z = regexec(&reg, s_tmp, 1, pm, 0);  
	
	regfree(&reg);  
	if(z!=0)
	{
		return false; 
		OmnScreen << "**********Error***********" << sgErr++ << endl;
	}

	return true;
}
