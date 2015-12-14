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
// 2015/09/20 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_SyncherJoin_h
#define AOS_Synchers_SyncherJoin_h

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"
#include "AosConf/DataField.h"
using AosConf::DataField;


class AosSyncherJoin : public AosSyncher
{
private:
	OmnMutexPtr					mLock;
	OmnString 					mJoinType;
	vector<OmnString>			mJoinTables;
	vector<OmnString>			mJoinIndexs;
	AosDataRecordObjPtr 		mOutputRecord;
	AosExprObjPtr       		mCondition;
	AosBuffPtr 					mDeltaBeansBuff;
	//OmnString 					mDBName;
	OmnString 					mCrtTableObjid;

public:
	AosSyncherJoin(const int version);
	virtual ~AosSyncherJoin();

	virtual AosSyncherType::E getType() {return AosSyncherType::eJoin;}
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }

	virtual bool config(AosRundata *rdata, const AosXmlTagPtr &def);
	virtual bool procData(AosRundata *rdata, AosDataRecordObj *record);
	virtual bool flushDeltaBeans(AosRundata *rdata);

public:
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);
	bool 	appendDeltaBean(AosRundata *rdata, AosDataRecordObj *record);

private:
	AosXmlTagPtr generatorDatasetConf(AosRundata *rdata);
	bool 	createOutput(AosRundata *rdata, 
						const AosXmlTagPtr &l_rcddoc, 
						const AosXmlTagPtr &r_rcddoc);
	AosQueryRsltObjPtr getRecordsByQuery(
						AosRundata *rdata,
						const AosValueRslt &v,
						const AosOpr opr,
						const OmnString &iilname);
	bool 	generaterJoinRecord(AosRundata *rdata, 
						AosDeltaBeanOpr::E opr,
						const AosDataRecordObjPtr &l_record,
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt);
	bool	combinRecords(AosRundata *rdata, 
						AosDeltaBeanOpr::E opr,
						const AosDataRecordObjPtr &l_rcd, 
						const AosDataRecordObjPtr &r_rcd);
	vector<boost::shared_ptr<DataField> > getFields(
						AosRundata *rdata, 
						const AosXmlTagPtr &record_doc);
	OmnString getIILName(AosRundata *rdata, const AosExprObjPtr &expr);
	AosXmlTagPtr getRecordIILConf(AosRundata *rdata, 
						const OmnString &iilobjid);
	int 	getNumFields(AosRundata *rdata, 
						const AosDataRecordObjPtr &record);
};

#endif
