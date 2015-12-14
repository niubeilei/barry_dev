////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/04/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Jimo_JimoDocCreator_h
#define Aos_Jimo_JimoDocCreator_h

#include "Jimo/Ptrs.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosJimoDocCreator
{
private:
	typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str>::iterator itr_t;

	static map_t	smMap;

public:
	static AosXmlTagPtr createJimoDoc(const AosRundataPtr &rdata, const OmnString &objid);
	static bool init();
};
#endif

