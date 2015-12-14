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
// 06/07/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_aosDLL_DllMgr_h
#define Aos_aosDLL_DllMgr_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Jimo/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DllMgrObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/SPtr.h"
#include "XmlUtil/Ptrs.h"

#include <dlfcn.h>
#include <string>


class AosDllMgr : public AosDllMgrObj
{
	OmnDefineRCObject;	

	typedef bool (*norm_func)(AosRundata *); 
	typedef AosDLLObjPtr (*create_dllobj_func)(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &sdoc,
					const OmnString &libname,
					const int version);
	typedef AosJimoPtr (*create_jimo_func)(const AosRundataPtr &rdata, const int version); 

	typedef hash_map<const OmnString, AosJimoPtr, Omn_Str_hash, compare_str> jmap_t;
	typedef hash_map<const OmnString, AosJimoPtr, Omn_Str_hash, compare_str>::iterator jmapitr_t;
	typedef hash_map<const OmnString, void*, Omn_Str_hash, compare_str> lmap_t;
	typedef hash_map<const OmnString, void*, Omn_Str_hash, compare_str>::iterator lmapitr_t;
	typedef hash_map<const OmnString, bool, Omn_Str_hash, compare_str> bmap_t;
	typedef hash_map<const OmnString, bool, Omn_Str_hash, compare_str>::iterator bmapitr_t;

private:
		OmnMutexPtr		mLock;
		lmap_t			mLib;
		bmap_t			mInvalidLibs;
		jmap_t			mJimoMap;

public:
	AosDllMgr();
	~AosDllMgr() ;

	virtual bool        start();
	virtual bool        stop();                                    
	virtual bool	    config(const AosXmlTagPtr  &config);

	virtual bool callFunc(
				AosRundata *rdata,
				const OmnString &libname,
				const OmnString &method, 
				const int version);

	virtual AosDLLObjPtr createDLLObj(
				AosRundata *rdata,
				const AosXmlTagPtr &worker,
				const OmnString &libname,
				const OmnString &method,
				const int version);

	virtual AosJimoPtr createJimoByClassname(
				AosRundata *rdata,
				const OmnString &classname, 
				const int version);

	AosJimoPtr createJimo(
				AosRundata *rdata,
				const AosXmlTagPtr &worker,
				const OmnString &jimo_objid);

	virtual AosJimoPtr createJimo(
				AosRundata *rdata,
				const AosXmlTagPtr &worker_doc, 
				const int version); 

	virtual AosJimoPtr createJimo(
				AosRundata *rdata,
				const AosXmlTagPtr &worker,
				const AosXmlTagPtr &jimo_doc, 
				const int version = -1);

	AosJimoPtr createJimoByName(
				AosRundata *rdata, 
				const OmnString &container, 
				const OmnString &key,
				const OmnString &name, 
				const int version);

private:
	void *	openLibPriv(AosRundata *rdata, const OmnString &lib);
	void *	findLibPriv(
				AosRundata *rdata,
				const OmnString &lib);
	norm_func findNormFunc(
				AosRundata *rdata,
				const OmnString &libname,
				const OmnString &method, 
				const int version);
	create_dllobj_func findCreateDLLObjFunc(
				AosRundata *rdata,
				const OmnString &libname,
				const OmnString &method,
				const int version);
	create_jimo_func findCreateJimoFuncLocked(
				AosRundata *rdata,
				const OmnString &libname,
				const OmnString &method,
				const int version);
	bool isInvalidLibraryLocked(
				AosRundata *rdata, 
				const OmnString &libname);

	AosJimoPtr findJimoLocked(
				AosRundata *rdata, 
				const OmnString &objid, 
				const int version);

	bool addJimoLocked(
				AosRundata *rdata, 
				const AosJimoPtr &jimo, 
				const OmnString &objid, 
				const int version);

	inline OmnString getKey(
				const OmnString &method, 
				const int version)
	{
		OmnString key = method;
		key << "_" << version;
		return key;
	}

	// Chen Ding, 2015/09/11, JIMODB-741
	virtual AosJimoPtr createJimoByName(
				AosRundata *rdata, 
				const OmnString &jimo_namespace,
				const OmnString &jimo_name, 
				const int version);

private:
	// Chen Ding, 2015/09/11, JIMODB-741
	AosJimoPtr findJimoLocked(
				AosRundata *rdata, 
				const OmnString &jimo_namespace,
				const OmnString &jimo_name, 
				const int version);

	bool addJimoLocked(
				AosRundata *rdata, 
				const AosJimoPtr &jimo, 
				const OmnString &jimo_namespace, 
				const OmnString &jimo_names, 
				const int version);
};
#endif

