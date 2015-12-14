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
// 2014/01/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_aosDLL_JimoDocMgr_h
#define Aos_aosDLL_JimoDocMgr_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosJimoDocMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosXmlTagPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosXmlTagPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	map_t		mDocMap;
	OmnMutexPtr	mLock;

public:
	AosJimoDocMgr();

	static AosXmlTagPtr getJimoDocStatic(AosRundata *rdata, const OmnString &classname, const int version);
	static bool init(AosRundata *rdata);

private:
	AosXmlTagPtr getJimoDoc(AosRundata *rdata, const OmnString &classname, const int version);
	bool createJimoDocs(AosRundata *rdata);
	AosXmlTagPtr createJimoDoc(
						AosRundata *rdata, 
						const OmnString &classname,
						const OmnString &libname, 
						const int version, 
						const OmnString &jimo_namespace = "",
						const OmnString &jimo_name = "");
	bool createJimoByName(
						AosRundata *rdata, 
						const OmnString &jimo_namespace,
						const OmnString &jimo_name, 
						const int version);

};
#endif

