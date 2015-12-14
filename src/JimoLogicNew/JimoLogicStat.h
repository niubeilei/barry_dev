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
// 2015/03/22 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicStat_h
#define AOS_JimoLogicNew_JimoLogicStat_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/String.h"
#include <vector>



struct StatTime
{
	OmnString           mTimeUnit;
	OmnString           mTimeField;
	OmnString           mFormat;
};

class AosJimoLogicStat :public OmnRCObject
{
	OmnDefineRCObject;

private:
	
	typedef map<OmnString,vector<OmnString> > map_t;
	typedef map<OmnString,vector<OmnString> >::iterator itr_t;

//	OmnString						mStatName;
//	std::vector<OmnString>          mKeyFields;
//	std::vector<OmnString>          mMeasures;
	map<OmnString, OmnString>		mCond;
//	OmnString						mStatIdx;
	AosJimoProgObjPtr               mJob;
	OmnString 						mJobName;
	AosExprObjPtr 					mInput;
	OmnString						mGroupbyDataProcName;
	OmnString						mStatDocName;
	OmnString						mStatKeyTableName;
	OmnString						mKeyTableIILName;
	OmnString						mJoinDataProcName;
	std::vector<OmnString>			mIILDataprocName;
	//std::vector<OmnString> 			mIILNames
	OmnString						mDatasetName;
	OmnString 						mTableName;
//	AosExprObjPtr 					mTimeField;
//	AosExprObjPtr					mFormat;
//	OmnString 						mFormat;
//	OmnString						mTimeField;
//	OmnString						mTimeUnit;
	OmnString						mDbname;
	std::vector<AosExprObjPtr> 		mNameValueList;
	std::vector<OmnString>			mTasks;
	bool							mIsNeedShuffle;
	OmnString						mOpr;
	OmnString						mMaxLen;
	// for streaming
	bool 							mIsService;
	int								mIdx;
	OmnString						mSchemaName;
	OmnString						mTabName;
	OmnString						mDBName;
	OmnString						mInputStat;

	std::vector<OmnString>           mStatModelName;
//	vector<OmnString>       	    mStatList;
	//std::vector<std::vector<OmnString> >  mKeyFields;
	//std::vector<std::vector<OmnString> >  mMeasures;
	map_t							 mKeyFields;
	map_t							 mMeasures;
	map_t							 mShuffleFields;
	map<OmnString, OmnString>  		 mKeyIdxOpr;
	map<OmnString, StatTime>         mTime;
	map<OmnString, OmnString>		 mStatIdx;
	map<OmnString, OmnString>		 mInfoFields;			//map<key,iilmap>
	
	map<OmnString, vector<OmnString> >		 mStatMap;
	int64_t 						mCountValue;

	OmnString						mDataProcName;

public:
	AosJimoLogicStat(
			vector<OmnString> &name,
			OmnString &tablename,
			map<OmnString, vector<OmnString> > &keys,
			map<OmnString, vector<OmnString> >&measures, 
			map<OmnString, vector<OmnString> >&shuffleFields, 
			AosExprObjPtr &input, 
			map<OmnString, OmnString> &cond, 
			map<OmnString, StatTime> &time, 
			OmnString &opr, 
			map<OmnString, OmnString> &idx, 
			map<OmnString, OmnString> &key_idx_opr, 
			const int64_t &countValue, 
			OmnString &dataprocname, 
			bool dft = false);
	
	AosJimoLogicStat(const int version);
	
	~AosJimoLogicStat();

	virtual bool run(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &, bool inparser);
	virtual OmnString getObjType(AosRundata*);

private:
	virtual bool parseRun(AosRundata* rdata,
							OmnString &stmt,
							AosJimoProgObj *jimo_prog);
	
	virtual bool createDataset(AosRundata *rdata,
							AosJimoProgObj *jimo_prog, 
							OmnString &statements_str,
							u32 k);
	virtual bool createGroupbyDataProc(
							AosRundata *rdata,
							AosJimoProgObj *jimo_prog, 
							OmnString &statements_str,
							u32 k);
	virtual bool createStatJoinDataProc(
							AosRundata *rdata,
							AosJimoProgObj *jimo_prog, 
							OmnString &statements_str,
							u32 k);
	virtual bool createStatDocDataProc(
							AosRundata* rdata,
							AosJimoProgObj *jimo_prog, 
							OmnString &statements_str,
							u32 k);
	virtual bool createUpdateStatKeyTableDataProc(
							AosRundata* rdata,
							AosJimoProgObj *jimo_prog, 
							OmnString &statements_str,
							u32 k);
	virtual bool createUpdateIILDataProc(
							AosRundata*,
							AosJimoProgObj *, 
							vector<OmnString> &fields,
							u32 k,
							u32 i, 
							OmnString &statements_str);
	virtual bool addGroupByTask(
							AosRundata* rdata,
							AosJimoProgObj *,
							u32 k);
	virtual bool addJoinTask(
							AosRundata* rdata,
							AosJimoProgObj *,
							u32 k);
	virtual bool addVector2DTask(
							AosRundata* rdata,
							AosJimoProgObj *,
							u32 k);
	virtual bool addKeyTableTask(
							AosRundata* rdata,
							AosJimoProgObj *,
							u32 k);
	virtual bool addIILTask (
							AosRundata* rdata,
							AosJimoProgObj *, 
							vector<OmnString> &fields,
							u32 k,
							u32 i);

	OmnString createIILName();
	OmnString createKeyTableIILName();
	OmnString nextDatasetName(AosRundata*, AosJimoProgObj*);
	OmnString createGroupbyName(AosRundata*);//, AosJimoProgObj*);
	OmnString getStatJoinName(AosRundata*, AosJimoProgObj*);
	OmnString getStatDocName(AosRundata*, AosJimoProgObj*);
	OmnString getDataProcName(AosRundata*, AosJimoProgObj*);
	OmnString getStatTableName(AosRundata*, AosJimoProgObj*);
	OmnString getStatKeyTableName(AosRundata*, AosJimoProgObj*);
	OmnString getGroupbyTaskName(AosRundata*, AosJimoProgObj*);
	OmnString getJoinTaskName(AosRundata*, AosJimoProgObj*);
	//arvin 2015.07.26
	//just for statjoin,convert type
	OmnString convertType(OmnString type);
	virtual bool vFieldCheck(AosRundata *rdata,const vector<OmnString> &keys);
};
#endif

