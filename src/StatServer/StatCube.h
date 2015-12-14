///////////////////////////////////////////////////////////////////////////
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
// 2014/01/22 Created by Ketty
// 
////////////////////////////////////////////////////////////////////////////

#ifndef StatServer_StatCube_h
#define StatServer_StatCube_h

#include "StatServer/Ptrs.h"
#include "StatEngine/StatQryEngine.h"
#include "StatUtil/StatModifyInfo.h"
#include "StatUtil/StatQueryInfo.h"
#include "Vector2DQryRslt/Ptrs.h"
#include "Vector2DUtil/Vt2dInfo.h"
#include "Vector2DUtil/Vt2dRecord.h"
#include "Vector2DUtil/Vt2dModifyInfo.h"
#include "Vector2D/Ptrs.h"

#include <vector>
#include <map>
using namespace std;

#define ISNEW_POS		8
#define DOCID_POS 		9
#define STATKEY_POS		17

class AosStatCube: public OmnRCObject
{
	OmnDefineRCObject;

	typedef map<u32, AosVt2dModifyInfo>::iterator MdfMapItr;
	//typedef map<u32, AosVt2dQueryInfo>::iterator QryMapItr;
	typedef pair<u64, OmnString> StatDocidKeyPair;


	//yang
	//typedef map<OmnString,map<u32,vector<OmnString> > > ktvlist_t;
	typedef map<OmnString,map<u32,map<u8,vector<OmnString> > > > ktvlist_t;
	ktvlist_t mktvlist;


private:
	//AosStatisticPtr		mStatistic;		
	//vector<AosVt2dInfo>	mVt2dInfos;
	u64					mStatId;
	u32					mCubeId;

	// For counter
	u64					mModifyTime;
	u64					mModifyNum;
	
	AosStatKeyConnPtr	mKeyConn;
	vector<AosVector2DPtr>		mVector2Ds;
	//u64					mNextNewStatId;
	
	AosStatQryEngine	*mEngine;

public:
	AosStatCube();
	~AosStatCube();

//yang
	void splitKeyValue(const vector<StatDocidKeyPair>& in_vt_idkey_pair,
			vector<StatDocidKeyPair>& out_vt_idkey_pair);
	void appendValueToKeyTime(OmnString key,u64 timeid,OmnString val);
	void appendTimeToKey(OmnString key,u64 timeid);
	ktvlist_t getKTVList()
	{
		return mktvlist;
	}
	
	void clearKTVList();
	int getDistVt2dIdx();

	bool 	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cube_conf);
	
	u64		getStatId(){ return mStatId; };
	u32		getCubeId(){ return mCubeId; };

	bool 	modify(
				const AosRundataPtr &rdata, 
				const AosBuffPtr &buff,
				AosStatModifyInfo &mdf_info);

	AosBuffPtr retrieve(
				const AosRundataPtr &rdata,
				vector<u64>	&stat_docids,
				AosStatQueryInfo &qry_info);

private:
	//bool 	initNextNewStatId();

	bool 	configKeyConn(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cube_conf);

	bool 	configVector2Ds(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &cube_conf);

private:
	// For Modify.
	bool 	saveNewStatIdKey(
				AosRundata *rdata,
				const AosBuffPtr &buff,
				AosStatModifyInfo &mdf_info);

	bool 	collectNewStatIdKey(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff,
				AosStatModifyInfo &mdf_info,
				vector<StatDocidKeyPair> &vt_idkey_pair);

	bool 	batchModify(
				const AosRundataPtr &rdata,
				const AosBuffPtr &buff,
				vector<AosVt2dModifyInfo> &vt2d_mdf_infos);


	// For Counter
	void	initCounters();
	void	outputCounters();

};

#endif
