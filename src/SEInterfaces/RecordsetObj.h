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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_RecordsetObj_h
#define Aos_SEInterfaces_RecordsetObj_h

#include "Rundata/Ptrs.h"
#include "MetaData/MetaData.h"
#include "SEInterfaces/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"


class AosRecordsetObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

	static AosRecordsetObjPtr	smObject;

public:
	struct Entry
	{
	//	AosDataRecordObj *	record;
		int					rcd_idx;
		int64_t				offset;
		int					len;
	};
	
	static AosRecordsetObjPtr createRecordsetStatic(
						AosRundata *rdata,
						const AosXmlTagPtr &def);
	
	static AosRecordsetObjPtr createRecordsetStatic(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record);
	
	
	static AosRecordsetObjPtr createStreamRecordsetStatic(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record);

	static void setObject(const AosRecordsetObjPtr &recordset){smObject=recordset;}
	static AosRecordsetObjPtr getObject(){return smObject;}


	static bool checkEmpty(const AosRecordsetObjPtr &recordset);

	virtual bool getIsStreamRecordset() = 0;
	virtual int64_t	size() const = 0;
	virtual bool hasMore() const = 0;
	//virtual int64_t getNumRecordsToAdd() = 0;
	virtual AosRecordsetObjPtr clone(AosRundata *rdata) const = 0;
	virtual bool reset() = 0; 

	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosXmlTagPtr &def) = 0;
	
	virtual AosRecordsetObjPtr createRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record) = 0;
	
	virtual AosRecordsetObjPtr createStreamRecordset(
						AosRundata *rdata,
						const AosDataRecordObjPtr &record) = 0;

	virtual bool nextRecord(
						AosRundata* rdata, 
						AosDataRecordObj *& record) = 0;

	virtual bool setData(
						AosRundata *rdata, 
						const AosBuffPtr &buff) = 0;

	virtual void setMetaData(
						AosRundata *rdata, 
						const AosMetaDataPtr &metadata) = 0;

	virtual char* getData() = 0;

	virtual bool holdRecord(
						AosRundata *rdata, 
						const AosDataRecordObjPtr &record) = 0;

	virtual bool holdRecords(
						AosRundata *rdata, 
						const vector<AosDataRecordObjPtr> &records) = 0;

	virtual AosDataRecordObjPtr getRecord() = 0;
	virtual AosDataRecordObj * getRawRecord(const int index) = 0;

	virtual vector<AosDataRecordObjPtr> getRecords() = 0;

	virtual bool appendRecord(
						AosRundata *rdata, 
						const int index, 
						const int64_t offset, 
						const int len,
						const AosMetaDataPtr &metaData) = 0;
	
	virtual bool appendRecord(
				AosRundata *rdata, 
				AosDataRecordObjPtr &rcd,
				AosBuffDataPtr &metaData) = 0;

	virtual bool swap(
					AosRundata *rdata, 
					const AosDataRecordObjPtr &record) = 0;

	virtual void resetReadIdx() = 0;
	virtual bool resetOffset() = 0;

	virtual bool serializeTo(
				AosRundata *rdata, 
				AosBuff *buff) = 0;

	virtual bool serializeFrom(
				AosRundata *rdata, 
				AosBuff *buff) = 0;

	virtual AosBuffPtr getDataBuff() = 0;
	virtual AosBuffPtr getEntryBuff() = 0;

	virtual bool sort(const AosRundataPtr &rdata, const AosCompareFunPtr &comp) = 0;

	virtual bool replaceRawRecord(AosDataRecordObj* record) = 0;

	// 2015.10.19, xiafan
	virtual bool setSchema(
			AosRundata *rdata,    
			AosSchemaObjPtr schema) = 0;

	virtual bool getBuffData(AosBuffDataPtr &buffdata) {return false;}
	virtual bool getIncomplete() {return false;}

	virtual bool setBuffData(AosRundata *rdata, AosBuffDataPtr buffdata){return false;}

	virtual bool setDataBuff(                
			AosRundata *rdata,       
			AosBuffPtr buff) = 0;

};
#endif



