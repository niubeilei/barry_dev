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
// 2013/12/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStructs_Vector2D_h
#define Aos_DataStructs_Vector2D_h

#include "DataStructs/Ptrs.h"
#include "DataStructs/DistBlock.h"
#include "DataStructs/DistBlockMgr.h"
#include "DataStructs/RootStatDoc.h"
#include "DataStructs/StatTimeCond.h"
#include "DataStructs/StatImRslt.h"
#include "StatUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/StructProcObj.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "StatUtil/StatRecord.h"
#include "SEUtil/StrValueInfo.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

#include <map>

#define AOS_VECTOR2D_DOCID 		"docid" 
#define AOS_VECTOR2D_TIME 		"time" 
#define AOS_VECTOR2D_STR 		"str" 

class AosVector2D : public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eExtRcdLen = sizeof(u64) + sizeof(u64) + sizeof(u64) + sizeof(int64_t) + sizeof(u8),
	};
	
	typedef vector<u64> IdVector;

protected:
	int64_t 				mRootStatDocSize;
	int64_t 				mExtensionStatDocSize;
	u64						mStartTimeSlot;

	AosDistBlockMgrPtr		mDistBlockMgr;
	OmnString				mCtrlDocObjid;
	u64						mCtrlDocid;		// Ketty 2014/07/21

	AosRootStatDoc::Parameter	mParm;
	AosStructProcObjPtr 	mProc;
	AosDataType::E			mDataType;
	
	AosAggregationFuncPtr	mAgrFunc;		// Ketty 2014/04/24
	AosAggregationFunc*		mAgrFuncRaw;	// Ketty 2014/04/24
	AosStatTimeUnit::E 		mTimeUnit;		// Ketty 2014/04/24
	AosXmlTagPtr			mQueryMeasureConf;	// Ketty 2014/06/02

	AosStatBatchReaderPtr	mBatchReader;
	
	char *					mInvalidStatValue;	// Ketty 2014/05/23
	u32						mValueSize;			// Ketty 2014/06/11

public:
	class AosProcThrdRunner : public  OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosVector2DPtr					mVt2d;
		AosStructProcObjPtr				mJimoProc;
		OmnSemPtr 						mSem;
		int								mNumThrdsRunning;
		AosBuffPtr						mBuff;
		int								mRecordLen;
		AosStrValueInfo					mStatIdInfo;
		AosStrValueInfo					mTimeInfo;
		AosStrValueInfo					mStrInfo;
		AosStrValueInfo					mValueInfo;
		int64_t							mStartPos;
		int64_t							mEndPos;
		AosRundataPtr 					mRdata;

	public:
		AosProcThrdRunner(
				const AosVector2DPtr &vt2d,
				const AosStructProcObjPtr &proc,
				const OmnSemPtr &sem,
				const int num_thrds_running,
				const AosBuffPtr &buff,
				const int64_t &start_pos,
				const int64_t &end_pos,
				const int record_len,
				AosStrValueInfo	&statid_info,
				AosStrValueInfo	&time_info,
				AosStrValueInfo	&str_info,
				AosStrValueInfo	&value_info,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("modifyvector2d"),
		mVt2d(vt2d),
		mJimoProc(proc),
		mSem(sem),
		mNumThrdsRunning(num_thrds_running),
		mBuff(buff),
		mRecordLen(record_len),
		mStatIdInfo(statid_info),
		mTimeInfo(time_info),
		mStrInfo(str_info),
		mValueInfo(value_info),
		mStartPos(start_pos),
		mEndPos(end_pos),
		mRdata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}

		virtual bool run();
		virtual bool procFinished();

		vector<AosValueRslt> & getNewValues();
		vector<AosValueRslt> & getOldValues();

		void clearValuesRslt(); 
	};


public:
	AosVector2D();

	AosVector2D(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &conf);

	AosVector2D(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &stat_vt2d_conf,
		const AosXmlTagPtr &stat_measure_conf);

	~AosVector2D();


	bool modifyLocal(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff,
			const int64_t &record_len,
			const int64_t &start_pos,
			const int64_t &end_pos,
			AosStrValueInfo	&statid_info,
			AosStrValueInfo	&time_info,
			AosStrValueInfo	&str_info,
			AosStrValueInfo	&value_info,
			const int num_thrds_running,
			const AosStructProcObjPtr &proc);

	bool modify(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff,
			const int64_t &record_len,
			map<OmnString, AosStrValueInfo> &field_info,
			const OmnString	&value_field_name,
			vector<AosValueRslt> &oldvalue_rslt,
			vector<AosValueRslt> &newvalue_rslt);

	void initDistBlock(
			const AosRundataPtr &rdata,
			map<int, u64> &root_ctrl_doc_snap_ids);

	void initSnapShot(
			map<int, u64> &snap_ids, 
			map<int, u64> &bsnap_ids);

	//GetStatDoc
	bool retrieveStatByDocidLocal(
			const AosRundataPtr &rdata,
			vector<u64> &dist_block_docid,
			vector<u64> &stat_ids,
			vector<AosStatTimeArea> &orig_time_conds,
			const AosStatTimeUnit::E grpby_time_unit,
			const AosVector2DGrpbyRsltPtr &grpby_rslt);

	bool retrieveStatByDocid(
			const AosRundataPtr &rdata,
			const u64 dist_block_docid,
			vector<u64> &stat_ids,
			vector<AosStatTimeCond *> &time_conds,
			const AosStatTimeUnit::E grpby_time_unit,
			const AosVector2DGrpbyRsltPtr &grpby_rslt);

	AosStatTimeUnit::E  getTimeUnit(){ return mTimeUnit; }
	
	AosXmlTagPtr getQueryMeasureConf(){ return mQueryMeasureConf;} 	// Ketty 2014/06/02
	
	bool splitTimeSlots(
			vector<AosStatTimeArea> &orig_time_conds,
			vector<AosStatTimeCond*> &time_conds,
			const AosStatTimeUnit::E grpby_time_unit);

	//AosDataRecordObjPtr operator [](const i64 idx);	
	
private:
	void config(
			const AosRundataPtr &rdata, 
			const AosXmlTagPtr &def);

	AosDistBlockMgrPtr configDistBlockMgr(
			const AosRundataPtr &rdata,
			const u64 &docid,
			const u64 &snap_id,
			const bool need_snap_shot);

	void configRetrieveAndCubSvr(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &stat_vt2d_conf, 
			const AosXmlTagPtr &stat_measure_conf);

	bool addUpdateEntry(
			const AosBuffPtr &entries,
			const u64 &docid,
			const u64 &ext_docid,
			const u64 &time_slot,
			const bool is_new,
			const int64_t &value);

	bool modifyExtensionEntry(
			const AosRundataPtr &rdata,
			const AosBuffPtr &entries,
			const AosStructProcObjPtr &proc);

	bool modifyKickOutStatDoc(
			const AosRundataPtr &rdata,
			map<u64, AosBuffPtr> &kickOutStatDoc);

	bool addKickOutStatDoc(
			map<u64, AosBuffPtr> &kickOutStatDoc,
			const u64 &ext_docid,
			const AosBuffPtr &kick_bucket);

	bool modifyRecord(
			const AosRundataPtr &rdata,
			AosDistBlock &dist_block,
			const u64 &docid,
			const u64 &time_slot,
			const int64_t &value,
			const OmnString &key,
			const AosBuffPtr &entries,
			map<u64, AosBuffPtr> &kickOutStatDoc,
			const AosStructProcObjPtr &proc);

	bool modifyExtStatDocEntry(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff,
			const u64 &docid,
			const u64 &idx, 
			const int64_t &value,
			const AosStructProcObjPtr &proc);

	void calculateRootStatDocSize();

	bool configBasePriv(
			const AosXmlTagPtr &stat_vt2d_conf,
			const AosXmlTagPtr &vt2d_conf,
			int64_t &index_size,
			u64 &time_bucket_weight);

	bool configStructProcPriv(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &stat_vt2d_conf);

	bool processOneGroup(
			const AosRundataPtr &rdata,
			vector<OmnThrdShellProcPtr> &runners,
			const OmnSemPtr &sem,
			const AosBuffPtr &buff,
			const int64_t &start_pos,
			const int64_t &end_pos,
			const int record_len,
			AosStrValueInfo	&statid_info,
			AosStrValueInfo	&time_info,
			AosStrValueInfo	&str_info,
			AosStrValueInfo	&value_info,
			const int num_thrds_running);

	bool getExtStatDoc(
			const AosRundataPtr &rdata,
			const u64 &stat_id,
			const u64 &ext_docid,
			const u64 &time_slot,
			const int64_t &value,
			const bool is_new,
			AosXmlTagPtr &doc,
			AosBuffPtr &buff,
			const AosStructProcObjPtr &proc);

	bool modifyExtStatDoc(
			const AosRundataPtr &rdata,
			const u64 &ext_docid,
			const bool is_new,
			const AosXmlTagPtr &doc,
			const AosBuffPtr &buff);

private:
	// GetStatDoc
	bool getValuesByBucket(
			const AosRundataPtr &rdata,
			AosVector2DRecord * record,
			//const AosBuffPtr &bucket,
			AosBuff *bucket,
			const u64 &start_idx,
			const AosStatTimeCond * time_cond);

	bool retrieveRootBucketPriv(
			const AosRundataPtr &rdata,
			const u64 &dist_block_docid,
			AosDistBlock &dist_block,
			const u64 &stat_id,
			vector<AosStatTimeCond *> &time_conds,
			vector<AosStatImRslt> &stat_im_rslts,
			AosVector2DRecord * record);

	bool retrieveExtBucketPriv(
			const AosRundataPtr &rdata,
			vector<AosStatTimeCond *> &time_conds,
			vector<AosStatImRslt> &stat_im_rslts);

	bool retrieveStatGeneralPriv(
			const AosRundataPtr &rdata,
			const u64 &dist_block_docid,
			AosDistBlock &dist_block,
			const u64 &stat_id,
			AosVector2DRecord * record,
			const AosBuffPtr &stat_value);

	bool retrieveWithEachTimePriv(
			const AosRundataPtr &rdata,
			const u64 &dist_block_docid,
			AosDistBlock &dist_block,
			const u64 &stat_id,
			AosVector2DRecord * record,
			const AosStatTimeUnit::E grpby_time_unit,
			vector<AosStatImRslt> &stat_im_rslts);

	bool retrieveExtWithEachTimePriv(
			const AosRundataPtr &rdata,
			const AosStatTimeUnit::E grpby_time_unit,
			vector<AosStatImRslt> &stat_im_rslts);

	bool splitTimeSlotsPriv(
			AosStatTimeArea &orig_time_cond,
			vector<AosStatTimeCond*> &time_conds,
			map<int64_t, u32> &time_index);

	AosStatTimeCond * findStatTimeCondPriv(
			vector<AosStatTimeCond*> &time_conds,
			map<int64_t, u32> &time_index, 
			const int64_t crt_idx,
			const int64_t crt_slot);

	bool splitTimeAreaByGroupTimeUnit(
			AosStatTimeCond * time_cond,
			AosStatTimeArea &time_area,
			const AosStatTimeUnit::E grpby_time_unit);

	bool retrieveWithTimeConds(
			const AosRundataPtr &rdata,
			const u64 dist_block_docid,
			vector<u64> &stat_ids,
			vector<AosStatTimeCond *> &time_conds,
			const AosStatTimeUnit::E grpby_time_unit,
			AosVector2DGrpbyRslt *grpby_rslt);

	bool retrieveWithEachTime(
			const AosRundataPtr &rdata,
			const u64 dist_block_docid,
			vector<u64> &stat_ids,
			vector<AosStatTimeCond *> &time_conds,
			const AosStatTimeUnit::E grpby_time_unit,
			AosVector2DGrpbyRslt *grpby_rslt);

	bool initTimeCond(
			AosStatTimeCond * stat_cond,
			const u64 start_time_slot, 
			const u64 end_time_slot,
			const AosStatTimeUnit::E grpby_time_unit);

	bool retrieveWithGeneral(
			const AosRundataPtr &rdata,
			const u64 dist_block_docid,
			vector<u64> &stat_ids,
			vector<AosStatTimeCond *> &time_conds,
			AosVector2DGrpbyRslt *grpby_rslt);

	bool getValuesGroupbyTime(
			const AosRundataPtr &rdata,
			AosBuff *bucket,
			const u64 &start_idx,
			const u64 &begin,
			const u64 &end,
			char * &agr_stat_vv);

	bool addStatValuesToRecord(
			const AosRundataPtr &rdata,
			AosVector2DRecord * record,
			vector<u64> &new_time_value,
			AosBuff *new_stat_value);

	bool decomposeStatKey(
			const OmnString &keys,
			AosVector2DRecord * record);

public:
	// test
	bool test1(
			const AosRundataPtr &rdata,
			vector<u64> &dist_block_docid,
			vector<u64> &stat_ids);

	bool test2(
			const AosRundataPtr &rdata,
			vector<u64> &dist_block_docid,
			vector<u64> &stat_ids);

	static void KettyInitPrintTime();
	static void KettyPrintTime();
	static bool test();
	
	bool 	kettyTest();
	bool 	testParserTimeSlots(
				const AosRundataPtr &rdata,
				const AosStatTimeUnit::E to_time_unit,
				vector<u64>	&time_slots);

	bool getStatDocByTester(
			const AosRundataPtr &rdata,
			vector<u64> &dist_block_docids,
			vector<u64> &stat_ids,
			vector<AosVector2DRecord *> &records,
			const int64_t &s_time,
			const int64_t &e_time);
private:
	bool printBucket(const AosBuffPtr &bucket);
	
	bool calculateSum(
			AosVector2DRecord * record,
			const AosBuffPtr &bucket,
			const u64 &start_idx,
			const AosStatTimeCond * time_cond);

	bool calculateSum(
			AosVector2DRecord * record,
			const AosBuffPtr &bucket,
			const u64 &start_time_slot,
			const u64 &end_time_slot);

	bool retrieveExtBucketPrivByTester(
			const AosRundataPtr &rdata,
			vector<AosStatTimeCond *> &time_conds,
			vector<AosStatImRslt> &stat_im_rslts);

	bool retrieveRootBucketPrivByTester(
			const AosRundataPtr &rdata,
			const u64 &dist_block_docid,
			AosDistBlock &dist_block,
			const u64 &stat_id,
			vector<AosStatTimeCond *> &time_conds,
			vector<AosStatImRslt> &stat_im_rslts,
			AosVector2DRecord * record);

	bool saveNewStatBinaryId(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff,
			const int64_t &record_len,
			AosStrValueInfo &statid_info);

	bool collectNeedSaveStatIds(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff,
			const int64_t &record_len,
			AosStrValueInfo	&statid_info,
			IdVector &vt_new_statid);

	bool allocStatBinaryId(
			const AosRundataPtr &rdata,
			IdVector &vt_statid,
			IdVector &vt_binary_docid);

	bool allocStatBinaryId2(
			const AosRundataPtr &rdata,
			IdVector &vt_statid,
			IdVector &vt_binary_docid);

	bool shufferNewStatids(
			IdVector &vt_statid,
			map<u32, IdVector> &statid_grp);

	bool saveStatBinaryIdToRemote(
			const AosRundataPtr &rdata,
			IdVector &vt_statid,
			IdVector &vt_binary_docid,
			AosBuffPtr &new_statid_buff);

};
#endif



