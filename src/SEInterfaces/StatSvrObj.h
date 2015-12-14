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
// 2014/02/21	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_StatSvrObj_h
#define Aos_SEInterfaces_StatSvrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "StatUtil/StatModifyInfo.h"
#include "StatUtil/StatQueryInfo.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/TransId.h"

class AosStatSvrObj : virtual public OmnRCObject
{
	static AosStatSvrObjPtr	smStatSvr;

public:
	static void setStatSvr(const AosStatSvrObjPtr &d) {smStatSvr = d;}
	static AosStatSvrObjPtr getStatSvr() {return smStatSvr;}
	
	virtual bool modify(
				const AosRundataPtr &rdata, 
				const AosXmlTagPtr &stat_cube_conf,
				const AosBuffPtr &buff,
				AosStatModifyInfo &mdf_info) = 0;
	
	virtual u64 createStatMetaFile(
				const AosRundataPtr &rdata,
				const u32 cube_id,
				const OmnString &file_prefix) = 0;
	
	virtual AosBuffPtr retrieve(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &stat_cube_conf,
				vector<u64> &qry_stat_docids,
				const AosXmlTagPtr &stat_qry_conf) = 0;

};

#endif
