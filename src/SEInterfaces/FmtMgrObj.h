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
#ifndef Aos_SEInterfaces_FmtMgrObj_h
#define Aos_SEInterfaces_FmtMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "FmtMgr/Fmt.h"
#include "SEInterfaces/Ptrs.h"
//#include "TransServer/TransProc.h"
#include "UtilData/ModuleId.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class AosFmtMgrObj : virtual public OmnRCObject
{
private:
	//static AosFmtMgrObjPtr		smFmtMgr;

public:
	virtual bool    config(const AosXmlTagPtr &def) = 0;
	virtual bool    start() = 0;
	virtual bool    stop() = 0;
	
	virtual bool 	readFmtBySync(
						const AosRundataPtr &rdata,
						const u32 cube_grp_id,
						const u64 beg_fmt_id,
						const u64 end_fmt_id,
						int &file_seq,
						bool &finish,
						AosBuffPtr &fmt_buff) = 0;

	virtual bool 	sendFmt(
						const AosRundataPtr &rdata,
						const AosFmtPtr	&fmt) = 0;
	virtual bool 	recvFmt(
						const AosRundataPtr &rdata,
						const u32 virtual_id,
						const int from_svrid,
						const AosBuffPtr &fmt_buff) = 0;

	virtual u64		getMaxRecvedFmtId() = 0;
	virtual bool 	recvFinishedFmtId(const u64 max_fmt_id) = 0;

	virtual bool 	getNextSendFmtId(
						const AosRundataPtr &rdata,
						const u32 cube_grp_id,
						const int from_svr_id,
						u64 &next_fmt_id) = 0;

	virtual void    setFmt(const AosFmtPtr &fmt) = 0;

	virtual AosFmtPtr getFmt() = 0;

	virtual bool 	addToGlobalFmt(
						const AosRundataPtr &rdata, 
						const u64 file_id,
						const u64 offset,
						const char *data,
						const int len,
						const bool flushflag) = 0; 

	virtual bool 	addToGlobalFmt(
						const AosRundataPtr &rdata,
						const AosStorageFileInfo &file_info) = 0;

	virtual bool 	addDeleteFileToGlobalFmt(
						const AosRundataPtr &rdata, 
						const u64 &file_id) = 0;

	virtual bool addToGlobalFmt(
						const AosRundataPtr &rdata, 
						const AosFmtPtr &fmt) = 0;

	//static void setFmtMgr(const AosFmtMgrObjPtr &d) {smFmtMgr = d;}
	//static AosFmtMgrObjPtr getFmtMgr() {return smFmtMgr;}
	static AosFmtMgrObjPtr getFmtMgr();

};
#endif
