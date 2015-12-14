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
// Modification History:
// 08/19/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "WordProc/WordNormal.h"

#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "SearchEngine/DocServer.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlDoc.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/DocServerCb.h"
#include "IILMgr/IILDocid.h"
#include "IILMgr/IILMgr.h"
#include "SEUtil/XmlTag.h"
#include "SEServer/SeReqProc.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"


OmnSingletonImpl(AosWordNormalSingleton,
                 AosWordNormal,
                 AosWordNormalSelf,
                "AosWordNormal");


AosWordNormal::AosWordNormal()
:
mIsStopping(false)
{
}


AosWordNormal::~AosWordNormal()
{
}


bool      	
AosWordNormal::start()
{
	return true;
}


bool        
AosWordNormal::stop()
{
	mIsStopping = true;
	return true;
}


OmnRslt     
AosWordNormal::config(const OmnXmlParserPtr &def)
{
	return true;
}


const char *
AosWordNormal::normalize(
		const char *word, 
		int &len, 
		char *newword, 
		const int newlen)
{
	return word;
}


