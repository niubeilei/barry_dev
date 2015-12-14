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
// 2014/10/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_GenericObjMgr_h
#define Aos_SEInterfaces_GenericObjMgr_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/GenericObjMgrObj.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosGenericObjMgr : public AosGenericObjMgrObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosGenericMethodObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosGenericMethodObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutexPtr	mLock;
	map_t 		mObjects;
	map<OmnString, OmnString> mObjectsJimoObjid;

public:
	AosGenericObjMgr(const int version);
	AosGenericObjMgr(const AosGenericObjMgr &rhs);

	~AosGenericObjMgr();

	virtual AosJimoPtr cloneJimo() const;

	virtual AosGenericMethodObjPtr getGenericObj(
				AosRundata *rdata, 
				const OmnString &verb, 
				const OmnString &objname, 
				const int version);

	virtual AosGenericMethodObjPtr createGenericObj(
				AosRundata *rdata, 
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc);

	virtual AosXmlTagPtr getJimoDoc(
				const AosRundataPtr &rdata,
				const OmnString &verb,
				const OmnString &objname,
				const u32 ver);

private:
	bool init();
};
#endif

