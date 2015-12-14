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
// 2015/03/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_CubeMgr_ClusterIDO_h
#define Aos_CubeMgr_ClusterIDO_h

#include "ClusterMgr/Ptrs.h"
#include "CubeComm/EndPointInfo.h"
#include "JimoCall/Ptrs.h"
#include "SEInterfaces/ClusterObj.h"
#include "Thread/Ptrs.h"
#include "UtilData/RepPolicy.h"

//using namespace Jimo;

class OmnCondVar;
class OmnMutex;
class AosJimoCall;
class AosCubeMap;

class AosCluster : public AosClusterObj
{
	OmnDefineRCObject;

private:
	OmnString				mClusterName;
	AosJimoCallDialerPtr	mDialer;
	bool					mIsRetrieving;
	bool					mIsValid;
	OmnCondVar *			mCondVar;
	OmnMutex *				mLock;
	AosCubeMap *			mCubeMap;
	int						mNumCubes;
	vector<AosEndPointInfo>	mEndpoints;

public:
	AosCluster(const OmnString &cluster_name);
	AosCluster(AosRundata *rdata, const AosXmlTagPtr &conf);
	~AosCluster();


	// JimoCaller Interface
	virtual bool callFinished(AosRundata *rdata, AosJimoCall &jimo_call){ return true; }

	// IDO interface
	//virtual bool serializeFrom(AosRundata *rdata, const AosBuffPtr &buff){ return false; }
	//virtual bool serializeTo(AosRundata *rdata, const AosBuffPtr &buff) const { return false; }

	virtual bool getTargetCubes(AosRundata *rdata, 
							const i64 cube_id, 
							vector<AosCubeMapObj::CubeInfo> &cube_infos) const;

	virtual bool getTargetCubes(AosRundata *rdata, 
							vector<AosCubeMapObj::CubeInfo> &cube_infos) const;

	virtual OmnString toString() const { return ""; }

	virtual OmnString getClusterName() const {return mClusterName;}
	virtual u64 nextJimoCallID(AosRundata *rdata);
	virtual bool eraseJimoCall(AosRundata *rdata, const u64 call_id);
	virtual bool makeJimoCall(AosRundata *rdata, 
							//const AosCubeMapObj::CubeInfo endpoint_id,
							const int endpoint_id,
							const AosJimoCallPtr &jimo_call, 
							AosBuff *buff);
	virtual bool modifyCubeAllocation(
								AosRundata *rdata, 
								const int cube_id, 
								const vector<AosCubeMapObj::CubeInfo> &endpoint_ids);

	virtual bool moveCube(AosRundata *rdata, 
								const int cube_id,
								const int from_endpoint_id,
								const int to_endpoint_id) { return false; }

	virtual bool isRetrieving() const {return mIsRetrieving;}
	virtual bool isValid() const {return mIsValid;}
	virtual void setRetrieving() {mIsRetrieving = true;}
	virtual void setInvalid() {mIsValid = false;}
	virtual void setValid() {mIsValid = true;}
	virtual bool retrieveFromAdmin(AosRundata *rdata);
	static AosClusterPtr retrieveCluster(AosRundata *rdata, const OmnString &cluster_name);
	virtual int getNumCubes() const;

	// Chen Ding, 2015/07/16
	virtual bool getHostedCubes(AosRundata *rdata, 
					vector<AosCubeMapObj::CubeInfo> &cubes);
	virtual bool getCubeGroup(AosRundata *rdata, 
					const u32 cube_id,
					vector<AosCubeMapObj::CubeInfo> &cubes);

	//by White, 2015-08-28 10:54:41
	virtual bool updateLeadership(
			AosRundata						*rdata,
			const u32						cube_id,
			const i32						leader_id,	//wish i32 shall be not enough for our system, ^_^
			vector<AosCubeMapObj::CubeInfo> &cube_infos);

	//by White, 2015-09-10 16:41:58
	virtual bool putTimeoutEndpointToEnd(
			AosRundata						*rdata,
			const u32						cube_id,
			const i32						endpoint_id,	//I wish i32 shall be not enough for our system, ^_^
			vector<AosCubeMapObj::CubeInfo>	&cube_infos);

private:
	bool config(AosRundata *rdata, const AosXmlTagPtr &conf);
	int  calculateCubeID(const u64 docid) const;
	int  calculateCubeID(const OmnString &objid) const;
};
#endif
