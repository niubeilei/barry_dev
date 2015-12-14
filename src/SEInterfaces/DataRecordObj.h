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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataRecordObj_h
#define Aos_SEInterfaces_DataRecordObj_h

#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BuffData.h"
#include "SEInterfaces/DataRecordType.h"
#include "SEInterfaces/DataFieldObj.h"
#include "Util/MemoryChecker.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "SEUtil/DeltaBeanOpr.h"
#include "Value/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosValueRslt;
class AosRundata;

class AosDataRecordObj : public AosJimo
{
	OmnDefineRCObject;

private:
	static AosDataRecordObjPtr	smCreator;

public:
	AosDataRecordObj(const int version);

	virtual void				setOperator(AosDeltaBeanOpr::E type) = 0;
	virtual AosDeltaBeanOpr::E 	getOperator() = 0;
	virtual AosDataRecordType::E getType() const = 0;
	virtual bool				isFixed() const = 0;
	virtual AosXmlTagPtr		getRecordDoc() const = 0;
	virtual int					getRecordLen() = 0;
	virtual int					getEstimateRecordLen() = 0; 
	virtual int					getMemoryLen() = 0;
	virtual char *				getData(AosRundata *rdata) = 0;
	virtual bool				setData(
									char *data, 
									const int len, 
									AosMetaData	*metaData, 
									int &status) = 0;
									//const int64_t offset) = 0;
	virtual void				setMemory(char* data, const int64_t &len) = 0;
	virtual bool				determineRecordLen(
									char* data, 
									const int64_t &len, 
									int &record_len,
									int &status) = 0;

	virtual void 				clear() = 0;
	virtual OmnString 			getFieldDelimiter() const = 0;
	virtual OmnString			getTextQualifier() const = 0;
	virtual char				getTextQualifierChar() const = 0;		// Chen Ding, 2014/12/04
	virtual AosMetaDataPtr		getMetaData() = 0;
	virtual	u64 				getDocid() const = 0;
	virtual	void 				setDocid(const u64 &docid) = 0;
	virtual u64					getDocidByIdx(const int idx) const = 0;
	virtual bool 				setDocidByIdx(const int idx, const u64 &docid) = 0;
	
	virtual int					getNumFields() const = 0;
	virtual u64					getTaskDocid() const = 0;	// Ketty 2013/12/23
	virtual void				setTaskDocid(const u64 task_docid) = 0;
	virtual OmnString			getRecordName() const = 0;
	virtual void 				setRecordName(const OmnString &)= 0;
	virtual OmnString			getRecordObjid() const = 0;

	virtual int64_t				getOffset() const = 0;
	virtual bool				isValid(AosRundata *rdata);
	virtual bool				isDirty() = 0;
	virtual void				flush(const bool clean_memory = false) = 0;
	virtual void				reset() = 0;
	virtual int					getNumRecord() = 0;
	virtual bool				getRecordLens(vector<int> &rcd_lens) = 0;

	virtual AosDataRecordObjPtr	clone(AosRundata *rdata AosMemoryCheckDecl) const = 0;

	virtual AosDataRecordObjPtr create(
	 								const AosXmlTagPtr &def,
	 								const u64 task_docid,
	 								AosRundata *rdata AosMemoryCheckDecl) const = 0;

	// AosDataRecord interface
	virtual AosDataRecordObjPtr	createDataRecord(
									const AosXmlTagPtr &def, 
									const u64 task_docid,
									AosRundata *rdata AosMemoryCheckDecl) = 0;

	virtual bool				getFieldValue(
									const int idx, 
									AosValueRslt &value, 
									const bool copy_flag,
									AosRundata* rdata) = 0;

	virtual bool				getFieldValue(
									const OmnString &field_name,
									AosValueRslt &value, 
									const bool copy_flag,
									AosRundata* rdata) = 0;

	//felicia, 2014/04/24 for record container
	virtual bool				setFieldValue(
									const int idx, 
									AosValueRslt &value, 
									bool &outofmem,
									AosRundata* rdata) = 0;

	virtual bool				setFieldValue(
									const OmnString &field_name, 
									AosValueRslt &value, 
									bool &outofmem,
									AosRundata* rdata) = 0;

	/*
	//yang
	virtual bool 				replaceField(
									const int idx,
									const AosDataFieldObjPtr& newfield) = 0;
*/

	virtual AosXmlTagPtr		serializeToXmlDoc(
			               			const char *data,
									const int data_len,
									AosRundata* rdata) = 0;

	virtual AosBuffPtr			serializeToBuff(
			               			const AosXmlTagPtr &doc,
									AosRundata *rdata) = 0;
	
	virtual int					getFieldIdx(
									const OmnString &name,
									AosRundata *rdata) = 0;

	virtual AosDataFieldObj*	getDataField(
									AosRundata *rdata, 
									const OmnString &name) = 0;

	// Chen Ding, 2014/07/24
	// virtual AosDataFieldObjPtr	getFieldByIdx(const u32 idx) = 0;		// Ketty 2014/05/07
	virtual AosDataFieldObj*	getFieldByIdx1(const u32 idx) = 0;		// Ketty 2014/05/07

	virtual AosXmlTagPtr		getDataFieldConfig(
									const OmnString &name,
									AosRundata *rdata) = 0;
	
	virtual bool				createRandomDoc(
									const AosBuffPtr &buff,
									AosRundata *rdata) = 0;

	virtual bool				getRecordENV(
									const OmnString &name,
									OmnString &record_env,
									AosRundata *rdata) = 0;

	static AosDataRecordObjPtr	createDataRecordStatic(
									const AosXmlTagPtr &def, 
									const u64 task_docid,
									AosRundata *rdata AosMemoryCheckDecl);

	static void setCreator(const AosDataRecordObjPtr &creator) {smCreator = creator;}
	static AosDataRecordObjPtr getCreator() {return smCreator;}

	// Chen Ding, 2014/11/12
	//virtual AosDataRecordObjPtr createDataRecord(
	//								AosRundata *rdata, 
	//								const vector<AosGenericValueObjPtr> &parms) = 0;

	virtual OmnString getFieldValue(AosRundata *rdata, const OmnString &field_name) = 0;

	//virtual bool                getFieldValue(const OmnString &field_name,
	//		                                  vector<AosGenericValueObjPtr> &values,
	//		                                  AosRundata* rdata)=0;//2014 11 20
	virtual void				flushRecord(AosRundata *rdata_raw) = 0;
	virtual void				setDataAssembler(AosDataAssemblerObj	*assemble) = 0;
	virtual void				bindDataProcs(vector<AosDataProcObj*> &procs) = 0;

	virtual vector<AosDataProcObj*>& getBindDataProcs() = 0;
	
	/*virtual bool                getFieldValue(
				         const OmnString &field_name,
				         vector<AosGenericValueObjPtr> &values,
				         AosRundata* rdata)=0;*/
	// Chen Ding, 2015/01/24
	virtual bool appendField(AosRundata *rdata, 
							const OmnString &name, 
							const AosDataType::E type,
							const AosStrValueInfo &info) = 0;

	virtual bool appendField(AosRundata *rdata, 
						const AosDataFieldObjPtr &field) = 0;

	virtual bool removeFields() = 0;

	static AosDataRecordObjPtr createBuffRecordStatic(AosRundata *rdata AosMemoryCheckDecl);
	virtual AosDataRecordObjPtr createBuffRecord(AosRundata *rdata AosMemoryCheckDecl) = 0;
	virtual vector<AosDataFieldObjPtr> getFields() const = 0;
	virtual bool dumpData(bool flag, OmnString title) = 0;
	virtual bool isValid() = 0;
	virtual u64 getSchemaDocid() = 0;



	//yang,2015/08/13
	virtual bool determineBitmapOffset(
			AosRundata* rdata,
			int &offset) = 0;

	//yang,2015/08/13
	virtual bool buildRecordBitmap(
	 				AosRundata *rdata) = 0;

	/*Phil,  2015/09/03
	virtual bool compareRecord(
			AosRundata *rdata, 
			AosDataRecordObj* rcd,
			int &cmpRslt) = 0;
	*/


};
#endif

