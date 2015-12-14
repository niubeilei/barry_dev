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
#include "GenericObjs/Ptrs.h"
#include "GenericObjs/GenericObject.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosGenericObjMgr : public AosGenericObjMgrObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<const OmnString, AosGenericObjPtr, Omn_Str_hash, compare_str> map_t;
	typedef hash_map<const OmnString, AosGenericObjPtr, Omn_Str_hash, compare_str>::iterator itr_t;

	OmnMutexPtr	mLock;
	map_t 		mObjects;

public:
	AosGenericObjMgr(const int version);
	AosGenericObjMgr(const AosGenericObjMgr &rhs);

	~AosGenericObjMgr();

	virtual AosJimoPtr cloneJimo() const;

	virtual AosGenericMethodObjPtr getGenericObj(
				AosRundata *rdata, 
				const OmnString &verb, 
				const OmnString &objname, 
				const int version) {return 0;}

	virtual AosGenericMethodObjPtr createGenericObj(
				AosRundata *rdata, 
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc) {return 0;}

	virtual AosXmlTagPtr getJimoDoc(
				const AosRundataPtr &rdata,
				const OmnString &objname,
				const u32 ver);

	virtual bool  proc( 
			AosRundata *rdata,  
			const AosJimoProgObjPtr &prog,
			const OmnString &verb_name, 
			const OmnString &obj_type, 
			const OmnString &obj_name,  
			const OmnString &json);

private:
	bool init();
	AosGenericObjPtr createGenericObj1(
			AosRundata *rdata, 
			const AosXmlTagPtr &worker_doc,
			const AosXmlTagPtr &jimo_doc,
			const int version);
};
#endif

