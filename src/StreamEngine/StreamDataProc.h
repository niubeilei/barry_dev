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
// 2013/07/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StreamDataProc_StreamDataProc_h
#define Aos_StreamDataProc_StreamDataProc_h

#include "DataProc/DataProc.h"
#include "JimoDataProc/JimoDataProc.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JimoType.h"
#include "SEInterfaces/GenericObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "Util/CompareFun.h"
#include "XmlUtil/Ptrs.h"
#include "StreamEngine/StreamData.h"
#include "StreamEngine/Ptrs.h"
#include "StreamEngine/Service.h"
#include <vector>
#include <hash_map>
#include "AosConf/DataRecordFixbin.h"
#include "JSON/JSON.h"
#include <boost/make_shared.hpp>
#include "AosConf/CompFun.h"

using namespace std;

class AosStreamDataProc : public AosJimoDataProc, public OmnThreadedObj
{
private:
	u64									mCount;
	AosRundataPtr 						mRundata;
	AosService*							mService;
	OmnThreadPtr        				mThread;
	OmnCondVarPtr						mCondVar;
	map<OmnString, OmnString>			mInputListenerMap;
	vector<OmnString>					mOutputSpeakerList;
	deque<AosRDDPtr>					mWaitRDDs;
	map<OmnString, AosRDDPtr>			mCurOutputMap;
	map<OmnString, AosCompareFunPtr>	mCompMap;

protected:
	OmnMutexPtr							mLock;
	AosDataRecordObj*					mInputRecordWithDocid;

public:
	AosStreamDataProc(
		const int version,
		const AosJimoType::E type);

	AosStreamDataProc(const AosStreamDataProc &proc);

	~AosStreamDataProc();

	virtual bool init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &dpConf,
		const AosXmlTagPtr &dsConf);

	virtual bool config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) = 0;

	virtual bool run(const AosRundataPtr &rdata);
	virtual bool finish(const AosRundataPtr &rdata);

	virtual bool threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool signal(const int threadLogicId);
	virtual bool procData(AosRundata *rdata, const AosRDDPtr &rdd);
	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	void setService(AosService *service) { mService = service; }

	OmnString getName() { return mName; }
	OmnString getServiceId() { return mService->getId(); }
	u64 getServiceDocid() { return mService->getDocid(); }

	AosRundataPtr getRundata() { return mRundata; }

	bool inputHandler(const AosRDDPtr &rdd);

	u64 getCount() { return mCount; }

	int numOutputs(int &remain);

protected:
	bool addOutput(AosRundata* rdata, const OmnString &outputName, AosDataRecordObjPtr &rcd);
	bool addComp(const OmnString &outputName, const AosXmlTagPtr &compDoc);

private:
	bool addComp(const OmnString &outputName, const boost::shared_ptr<AosConf::CompFun> &confComp);
	bool outputHandler(int &remain, const AosRDDPtr &rdd, AosRundata* rdata);
	bool registerListeners(const JSONValue &jsonConfig);
	bool registerSpeakers(const JSONValue &jsonConfig);
	bool startRDD(const AosRDDPtr &rdd, const AosRundataPtr &rdata);
	bool finishRDD(const AosRDDPtr &rdd, const int remain, const AosRundataPtr &rdata);

};

#endif
