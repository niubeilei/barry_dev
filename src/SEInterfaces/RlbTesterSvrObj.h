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
// 2013/07/16 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_RlbTesterSvrObj_h
#define Aos_SEInterfaces_RlbTesterSvrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "TransServer/TransProc.h"
#include "UtilData/ModuleId.h"
#include "Util/String.h"
#include "Util/TransId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosRlbTesterSvrObj : virtual public OmnRCObject
{
private:
	static AosRlbTesterSvrObjPtr		smSvr;

public:
	virtual bool start() = 0;
	
	virtual bool createFile(
					const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const u32 cube_id,
					const u32 site_id, 
					const u64 requested_size,
					u64 &file_id,
					OmnString &fname) = 0;

	virtual bool modifyFile(
					const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const u64 file_id,
					const u64 offset,
					const AosBuffPtr &data) = 0;

	virtual bool deleteFile(
					const AosRundataPtr &rdata,
					const AosTransId &trans_id,
					const u64 file_id) = 0;

	virtual bool readData(
					const AosRundataPtr &rdata,
					const u64 file_id,
					const u64 offset,
					const u32 data_len,
					AosBuffPtr &data) = 0;

	static void setSvr(const AosRlbTesterSvrObjPtr &d) {smSvr = d;}
	static AosRlbTesterSvrObjPtr getSvr() {return smSvr;}
};
#endif
