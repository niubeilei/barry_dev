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
// 2013/11/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DllMgrObj_h
#define Aos_SEInterfaces_DllMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosDllMgrObj : virtual public OmnRCObject
{
private:
	static AosDllMgrObjPtr 	smDllMgr;
	static AosDllMgrObj* 	smDllMgrRaw;

public:
	AosDllMgrObj();
	~AosDllMgrObj();

	static AosDllMgrObj * getDllMgr1() {return smDllMgrRaw;}
	static void setDllMgr(const AosDllMgrObjPtr &m) 
	{
		smDllMgr = m;
		smDllMgrRaw = smDllMgr.getPtr();
	}

	virtual AosJimoPtr createJimo(
				AosRundata *rdata,
				const AosXmlTagPtr &worker_doc, 
				const int version) = 0;

	virtual AosJimoPtr createJimo(
				AosRundata *rdata,
				const AosXmlTagPtr &worker,
				const AosXmlTagPtr &jimo_doc, 
				const int version) = 0;

	virtual AosJimoPtr createJimoByClassname(
				AosRundata *rdata,
				const OmnString &classname, 
				const int version) = 0;

	virtual bool callFunc(
				AosRundata *rdata,
				const OmnString &libname,
				const OmnString &method, 
				const int version) = 0;

	virtual AosDLLObjPtr createDLLObj(
				AosRundata *rdata,
				const AosXmlTagPtr &worker,
				const OmnString &libname,
				const OmnString &method,
				const int version) = 0;

	virtual AosJimoPtr createJimoByName(
				AosRundata *rdata, 
				const OmnString &container, 
				const OmnString &key, 
				const OmnString &name, 
				const int version) = 0;

	// Chen Ding, 2015/09/11, JIMODB-741
	virtual AosJimoPtr createJimoByName(
				AosRundata *rdata, 
				const OmnString &jimo_namespace,
				const OmnString &jimo_name, 
				const int version) = 0;

};
#endif
