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
// Modification History:
// 2015/05/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_DocFileMgrNew_h
#define AOS_SEInterfaces_DocFileMgrNew_h

#include "DfmUtil/Ptrs.h"
#include "JimoCall/JimoCall.h"
#include "UtilData/DfmDocType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "JimoRaft/RaftServer.h"


class AosRundata;
class AosBuff;

class AosDocFileMgrNewObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual bool stop() = 0;
	virtual bool start(AosRundata *rdata) = 0;

	// Doc Operations
	virtual bool createDoc(
					AosRundata* rdata, 
					const u64 aseid,
					const u64 docid, 
					const u64 snap_id,
					const AosBuffPtr &doc_body,
					AosJimoCall &jimo_call,
					AosBuffPtr pHeaderCustomData = NULL) = 0;

	virtual bool modifyDoc(AosRundata* rdata, 
					const u64 docid, 
					const u64 snap_id, 
					const AosBuffPtr &doc_body,
					AosJimoCall &jimo_call,
					AosBuffPtr pHeaderCustomData = NULL) = 0;

	virtual bool deleteDoc(AosRundata *rdata, 
					const u64 aseid,
					const u64 docid,
					const u64 snap_id,
					AosJimoCall &jimo_call,
					AosBuffPtr pHeaderCustomData = NULL) = 0;

	virtual bool getDoc(AosRundata *rdata,
					const u64 aseid,
					const u64 snap_id,
					const u64 docid,
					AosJimoCall &jimo_call) = 0;

	virtual bool removeAllFiles(AosRundata *rdata) = 0;

	virtual AosRaftServer* getRaftServer(u64 cubeId)= 0;

	// Snapshot Operations
	virtual u64 createSnapshot(AosRundata *rdata, const u64 snap_id) = 0;
	virtual bool commitSnapshot(AosRundata *rdata, const u64 snap_id) = 0;
	virtual bool rollbackSnapshot(AosRundata *rdata, const u64 snap_id) = 0;
	virtual bool mergeSnapshot(AosRundata *rdata, 
					const u64 target_snap_id, 
					const u64 merger_snap_id) = 0;
};
#endif
