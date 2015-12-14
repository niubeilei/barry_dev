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
// 	There are some system defined containers, such as the ones used to
// 	store images, vpds, albums, comments, etc. When adding a doc into 
// 	such containers, some system defined tags should be added into 
// 	the doc. This class is mainly used to manage the system defined 
// 	tags. 
//
// 	There is a system defined XML doc that defines a map between a
// 	system defined container and the tags to be added to docs:
//		container_name:		tags
//		container_name:		tags
//		...
//
//  When this class starts, it reads the doc and create a map.
//
//  There are public and private containers, which are determined
//  based on container objids. For public containers, the following
//  tag should be added, in addition to whatever being defined by 
//  containers:
//  		AOSSYSTAG_PUBDOC
//
//  Similarly, for private containers, the following should be added:
//  		AOSSYSTAG_PRIDOC
//	
// Modification History:
// 09/14/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/TagMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/StrSplit.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosTagMgrSingleton,
                 AosTagMgr,
                 AosTagMgrSelf,
                "AosTagMgr");


AosTagMgr::AosTagMgr()
:
mIsStopping(false)
{
}


AosTagMgr::~AosTagMgr()
{
}


bool      	
AosTagMgr::start()
{
	return true;
}


bool        
AosTagMgr::stop()
{
	mIsStopping = true;
	return true;
}


bool
AosTagMgr::config(const AosXmlTagPtr &def)
{
	return true;
}


bool	
AosTagMgr::setSysTags(const AosXmlTagPtr &doc)
{
	return true;
}

