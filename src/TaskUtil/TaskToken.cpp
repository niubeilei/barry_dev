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
// A sync token depends on a number of 'Resources'. Initially all
// resources are 'Not Ready'. When a resource becomes ready, it informs
// this class. When all resources are ready, this token is ready to 
// kick off the 'resources' it manages. 
//
// Modification History:
// 2013/04/27 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskUtil/TaskToken.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosTaskToken::AosTaskToken()
:
mLock(OmnNew OmnMutex())
{
}


AosTaskToken::~AosTaskToken()
{
}


bool
AosTaskToken::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Chen Ding, 07/30/2012
	// Tokens are defined as:
	// 	<token AOSTAG_TOKENID="xxx">
	// 		<AOSTAG_RESOURCES>
	// 			<entry>xxx</entry>
	// 			<entry>xxx</entry>
	// 			...
	// 		</AOSTAG_RESOURCES>
	// 		<AOSTAG_CONTROLLED>
	// 			<entry>xxx</entry>
	// 			<entry>xxx</entry>
	// 			...
	// 		</AOSTAG_CONTROLLED>
	// 	</token>
	aos_assert_r(def, false);

	mTokenId  = def->getAttrStr(AOSTAG_TOKENID);
	if (mTokenId == "")
	{
		AosSetErrorU(rdata, "invalid_token_id:") << def->toString();
		return false;
	}

	AosXmlTagPtr entries = def->getFirstChild("zky_resource");
	if (!entries)
	{
		AosSetErrorU(rdata, "missing_resources:") << def->toString();
		return false;
	}
	
	// Retrieve resources
	OmnString id;
	AosXmlTagPtr entry = entries->getFirstChild(true);
	while (entry)
	{
		id = entry->getNodeText();
		if (id == "")
		{
			AosSetErrorU(rdata, "entry_is_empty:") << def->toString();
			return false;
		}
		mResources.push_back(id);
		mStatus[id] = false;
		entry = entries->getNextChild();
	}
	if (mResources.size() <= 0)
	{
		AosSetErrorU(rdata, "missing_resources:") << def->toString();
		return false;
	}

	// Retrieve controlled
	entries = def->getFirstChild("zky_controlled");
	if (!entries)
	{
		AosSetErrorU(rdata, "missing_controlled:") << def->toString();
		return false;
	}

	entry = entries->getFirstChild(true);
	while (entry)
	{
		id = entry->getNodeText();
		if (id == "")
		{
			AosSetErrorU(rdata, "entry_is_empty:") << def->toString();
			return false;
		}
		mControlled.push_back(id);
		entry = entries->getNextChild();
	}

	if (mControlled.size() <= 0)
	{
		AosSetErrorU(rdata, "missing_controlled:") << def->toString();
		return false;
	}

	return true;
}


bool
AosTaskToken::resourceReady(const OmnString &id, const AosRundataPtr &rdata)
{
	mLock->lock();
	map<OmnString, bool>::iterator itr = mStatus.find(id);
	if (itr != mStatus.end())
	{
		itr->second = true;
	}
	mLock->unlock();
	return true;
}


bool
AosTaskToken::isTokenReady()
{
	mLock->lock();
	bool rslt = true;
	map<OmnString, bool>::iterator itr = mStatus.begin();
	while (itr != mStatus.end())
	{
		rslt = itr->second;
		if (!rslt) break;
		itr++;
	}
	mLock->unlock();
	return rslt;
}

	
AosTaskTokenPtr
AosTaskToken::createToken(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);
	AosTaskTokenPtr token = OmnNew AosTaskToken();
	bool rslt = token->config(def, rdata);
	aos_assert_r(rslt, 0);
	return token;
}

#endif
