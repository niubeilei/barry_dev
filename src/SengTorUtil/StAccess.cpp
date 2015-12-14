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
// 01/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StAccess.h"

#include "Random/RandomUtil.h"
#include "XmlUtil/XmlTag.h"

AosStAccess::Hash_t AosStAccess::smAccessMap;
static vector<AosStAccessPtr> sgAccesses;

AosStAccess::AosStAccess(
		const OmnString &name, 
		const bool flag)
:
mName(name)
{
	if (flag)
	{
		AosStAccessPtr thisptr(this, false);
		registerAccess(thisptr);
	}
}


bool
AosStAccess::registerAccess(const AosStAccessPtr &access)
{
	HashItr_t itr = smAccessMap.find(access->mName);
	aos_assert_r(itr == smAccessMap.end(), false);
	smAccessMap[access->mName] = access;
	sgAccesses.push_back(access);
	return true;
}


bool 
AosStAccess::checkAccessByOprStatic(
		const AosXmlTagPtr &access_tag,
		const AosStDocPtr &local_doc,
		bool &granted, 
		bool &denied,
		const AosSengTestThrdPtr &thread)
{
	aos_assert_r(access_tag, false);
	HashItr_t itr = smAccessMap.find(access_tag->getAttrStr(AOSTAG_TYPE));
	aos_assert_r(itr != smAccessMap.end(), false);
	AosStAccessPtr access = itr->second;
	aos_assert_r(access, false);
	return access->checkAccessByOpr(access_tag, local_doc, granted, denied, thread);
}


OmnString 
AosStAccess::randAccessXmlStr(
		const OmnString &tagname, 
		const AosSengTestThrdPtr &thread)
{
	// This function randomly generates an access. 
	for (u32 i=0; i<sgAccesses.size(); i++)
	{
		int idx = OmnRandom::nextInt1(0, sgAccesses.size()-1);
		if (sgAccesses[idx])
		{
			return sgAccesses[idx]->getXmlStr(tagname, thread);
		}
	}

	return "";
}

