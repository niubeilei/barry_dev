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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StreamEngine_Service_h
#define Aos_StreamEngine_Service_h

#include "API/AosApiG.h"
#include "Jimo/Jimo.h"
#include "JimoJob/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/GenericObj.h"
#include "XmlUtil/XmlTag.h"
#include "JSON/JSON.h"
#include "StreamEngine/RDD.h"
#include "StreamEngine/StreamSnapshot.h"
#include <vector>
using namespace std;

class AosStreamData;
class AosService : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	u64											mDocid;
	OmnString									mId;
	OmnString									mName;
	JSONValue									mJsonParams;
	OmnMutexPtr									mLock;
	map<OmnString, JSONValue>					mParamMap;
	map<OmnString, JSONValue>  					mDataProcMap;
	map<OmnString, AosDataProcObjPtr>			mDataProcObjMap;
	map<OmnString, vector<OmnString> >			mDataflowMap;
	map<OmnString, vector<AosDataProcObj*> > 	mInputDataProcMap;

public:
	AosService();
	~AosService();

	bool config(
			const AosRundataPtr &rdata,
			AosXmlTagPtr &def);
	
	bool configParam(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &def);

	bool runService(
			AosRundata *rdata,
			const u64 &serviceDocid);


	//getters and setters
	inline OmnString getName() { return mName; }
	inline OmnString getId() { return mId; }
	inline u64 getDocid() { return mDocid; }
    inline map<OmnString, AosDataProcObjPtr> &getDataProcObjMap(){return mDataProcObjMap;}
 	
	//data methods
	bool serviceDataArrived(const AosRundataPtr &rdata, const OmnString &dsName, const AosRDDPtr &rdd);
	static bool dataArrived(const AosRundataPtr &rdata, const OmnString &dsName, const AosRDDPtr &rdd);
	bool registerListeners(const OmnString &inputName, AosDataProcObj *dp);

	//RDD handling tracking methods
	bool updateRDDStatus(const u64 &rddId, const OmnString &dpName, const int numConsumed, const int numProduced);
	bool findLastRemain(const OmnString &downstream_name, int &remain);

private:
	bool replaceJsonParams(JSONValue &json, JSONValue &jsonNew);

	bool createDataProcs(AosRundata* rdata);
	bool runDataProcs(
			AosXmlTagPtr &serviceConf,
			AosRundata *rdata);

	bool calDatasetRunNumDataProcs();
	bool stopDataProcs(AosRundata *rdata);
};

#endif
