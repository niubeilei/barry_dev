//////////////////////////////////////////////////////////////////////////
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
// 01/01/2013: Created by Chen Ding
//////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogUtil_LogEntryCtnr_h
#define AOS_LogUtil_LogEntryCtnr_h

#include "LogUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/String.h"
#include "Util/DoubleList.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosLogEntryCtnr : virtual public AosDoubleList
{
	OmnDefineRCObject;

protected:
	typedef hash_map<const char *, AosIILAssemblerPtr, char_str_hash, compare_charstr> map_t;
	typedef hash_map<const char *, AosIILAssemblerPtr, char_str_hash, compare_charstr>::iterator mapitr_t;
	vector<AosLogEntryPtr>	mEntries;
	map_t					mContainerAttr;
	map_t					mContainerKeyword;
	map_t					mContainerAttrKeyword;
	map_t					mGlobalAttr;
	map_t					mGlobalKeyword;
	map_t					mGlobalAttrKeyword;

public:
	AosLogEntryCtnr();
	~AosLogEntryCtnr();

	bool appendEntry(const AosLogEntryPtr &entry);
	bool serializeTo(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	AosIILAssemblerPtr getContainerAttrAssm(const char *name);
	AosIILAssemblerPtr getContainerKeywordAssm(const char *name);
	AosIILAssemblerPtr getContainerAttrKeywordAssm(const char *name);
	AosIILAssemblerPtr getGlobalAttrAssm(const char *name);
	AosIILAssemblerPtr getGlobalKeywordAssm(const char *name);
	AosIILAssemblerPtr getGlobalAttrKeywordAssm(const char *name);
};
#endif

