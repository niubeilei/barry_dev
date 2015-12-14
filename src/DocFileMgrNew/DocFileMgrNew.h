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
// 2015/05/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgrNew_DocFileMgrNew_h
#define AOS_DocFileMgrNew_DocFileMgrNew_h

#include "SEInterfaces/DocFileMgrNewObj.h"
#include "Util/String.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/Ptrs.h"
#include <vector>
#include "DfmUtil/DfmConfig.h"


class AosRundata;
class AosRaftServer;
class AosJimoCall;

class AosDocFileMgrNew : public AosDocFileMgrNewObj
{
private:
	u64			  				mAseId;
	u32							mHeaderCustomSize;
	std::vector<AosRaftServer*>	mRafts;
	u32							mCubeNum;
	u32							mLocalCubeNum;
	AosDfmConfig				mDfmLog;

public:
	AosDocFileMgrNew(
		AosRundata	*rdata,
		const u64	aseid,
		const u32	headerCustomSize);

	AosDocFileMgrNew(
		AosRundata *rdata,
		const AosDfmConfig &config);

	~AosDocFileMgrNew();
	
	virtual bool start(AosRundata *rdata);
	virtual bool stop();

	// Doc Operations
	virtual bool createDoc(
			AosRundata* rdata,
			const u64 aseid,
			const u64 docid,
			const u64 snap_id,
			const AosBuffPtr &doc_body,
			AosJimoCall &jimo_call,
			AosBuffPtr pHeaderCustomData = NULL);

	virtual bool modifyDoc(AosRundata* rdata, 
					const u64 docid, 
					const u64 snap_id, 
					const AosBuffPtr &doc_body,
					AosJimoCall &jimo_call,
					AosBuffPtr pHeaderCustomData = NULL);

	virtual bool deleteDoc(AosRundata *rdata, 
					const u64 aseid,
					const u64 docid,
					const u64 snap_id,
					AosJimoCall &jimo_call,
					AosBuffPtr pHeaderCustomData = NULL);

	virtual bool getDoc(AosRundata *rdata,
					const u64 aseid,
					const u64 snap_id,
					const u64 docid,
					AosJimoCall &jimo_call);

	virtual bool removeAllFiles(AosRundata *rdata);

	virtual AosRaftServer* getRaftServer(u64 cubeId);

	// Snapshot Operations
	virtual u64 createSnapshot(AosRundata *rdata, const u64 snap_id);	
	virtual bool commitSnapshot(AosRundata *rdata, const u64 snap_id);
	virtual bool rollbackSnapshot(AosRundata *rdata, const u64 snap_id);
	virtual bool mergeSnapshot(AosRundata *rdata, 
					const u64 target_snap_id, 
					const u64 merger_snap_id);
	virtual bool checkLeader(AosRundata *rdata, u64 docid, AosJimoCall &jimo_call);
	virtual bool checkId(u64 docid, AosRundata *rdata, AosJimoCall &jimo_call);

	//helper method
	int getCubeId(u64 docid);

private:
	bool init(AosRundata *rdata);
};
#endif
