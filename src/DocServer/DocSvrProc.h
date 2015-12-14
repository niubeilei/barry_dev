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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DocServer_DocSvrProc_h
#define Aos_DocServer_DocSvrProc_h

#include "DocServer/ProcTypes.h"
#include "DocServer/DocSvrProc.h"
#include "DocServer/Ptrs.h"
#include "Rundata/Rundata.h"
#include "TransUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosDocSvrProc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString			 mName;
	AosDocSvrProcId::E	mType;


protected:

public:
	AosDocSvrProc(const OmnString &name, AosDocSvrProcId::E type, const bool regflag);
	~AosDocSvrProc();

	// Ketty 2013/03/22
	//virtual AosDocSvrProcPtr clone() = 0;
	//virtual bool proc(const AosRundataPtr &rdata, 
	//			const AosXmlTagPtr &request,
	//			const AosBigTransPtr &trans) = 0;

	static AosDocSvrProcPtr getProc(const OmnString &id);

private:
	bool registerProc(const AosDocSvrProcPtr &sdoc);
};
#endif

