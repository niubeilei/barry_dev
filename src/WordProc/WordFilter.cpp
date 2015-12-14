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
#include "WordProc/WordFilter.h"

#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"


OmnSingletonImpl(AosWordFilterSingleton,
                 AosWordFilter,
                 AosWordFilterSelf,
                "AosWordFilter");


AosWordFilter::AosWordFilter()
:
mIsStopping(false)
{
}


AosWordFilter::~AosWordFilter()
{
}


bool      	
AosWordFilter::start()
{
	return true;
}


bool        
AosWordFilter::stop()
{
	mIsStopping = true;
	return true;
}


OmnRslt     
AosWordFilter::config(const OmnXmlParserPtr &def)
{
	return true;
}


u64	
AosWordFilter::filter(const char *word, const int len)
{
	return 0;
}


