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
#ifndef Aos_JimoDataProc_JimoDataProc_h
#define Aos_JimoDataProc_JimoDataProc_h

#include "DataProc/DataProc.h"
#include "Jimo/Jimo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/JimoType.h"
#include "SEInterfaces/GenericObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/String.h"
#include "Util/ValueRslt.h"
#include "Util/HashUtil.h"
#include "XmlUtil/Ptrs.h"
#include "Job/JobMgr.h"
#include <vector>
#include <hash_map>
using namespace std;

#define JIMO_DATAPROC_DEBUG false

#if JIMO_DATAPROC_DEBUG 
#define DumpRecord(rec, msg) dumpRecord((rec), (msg))
#else
#define DumpRecord(rec, msg) 
#endif

class AosJimoDataProc : public AosDataProc, public AosGenericObj
{
protected:
	enum 
	{
		eShowProcNum = 10000 //1w
	};

	vector<AosDataAssemblerObjPtr> 				mDataAssemblers;
	vector<boost::shared_ptr<Output> >			mOutputs;	
	bool 										mPipEnd;
	bool 										mDebug;

	i64											mProcDataCount;
	i64											mOutputCount;

public:
	AosJimoDataProc(const AosJimoDataProc &rhs);
	AosJimoDataProc(
		const int version,
		const AosJimoType::E type);
	~AosJimoDataProc();

	virtual AosDataProcStatus::E procData( 		// will delete later.
						const AosDataRecordObjPtr &record,
						const u64 &docid,
						const AosDataRecordObjPtr &output,
						const AosRundataPtr &rdata) 
	{
		OmnShouldNeverComeHere;           
		return AosDataProcStatus::eError; 
	}

	virtual bool	resolveDataProc(			// will delete later.
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata)
	{
		OmnShouldNeverComeHere;           
		return AosDataProcStatus::eError; 
	}

	virtual bool	finish(const AosRundataPtr &rdata)
	{
		OmnNotImplementedYet;
		return true;
	}
	virtual AosDataProcObjPtr cloneProc() {return 0;}
	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) {return 0;}

	//virtual vector<AosDataAssemblerObjPtr> getOutputs() {return mDataAssemblers;}
	virtual vector<boost::shared_ptr<Output> > getOutputs() {return mOutputs;}

	virtual	void 	setPipEnd(bool flag = true) { mPipEnd = flag; }
	bool 	isPipEnd() { return mPipEnd; }
	bool	flushRecord(AosDataRecordObj **output_records, AosDataRecordObj *record, AosRundata *rdata_raw);

	//arvin 2015/04/20
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft)
	{
		OmnShouldNeverComeHere;  
		return false;
	}
	
	virtual bool setJimoName(const OmnString &name)
	{
		OmnShouldNeverComeHere;  
		return false;	
	}

	virtual OmnString getJimoName() const
	{
		OmnShouldNeverComeHere;  
		return "";
	}

	virtual OmnString getObjType(AosRundata *rdata)
	{
		OmnShouldNeverComeHere;  
		return "";
	}

	virtual AosDataFieldType::E convertToDataFieldType(
					AosRundata* rdata,
					const AosExprObjPtr &expr,
					const AosDataRecordObjPtr &record);

	virtual AosDataFieldType::E convertToDataFieldType(
					AosRundata* rdata,
					const OmnString &str_type);

	virtual AosExprObjPtr 		convertToExpr(
					const OmnString &name,
					const AosRundataPtr &rdata);

	void 			resetDataFieldLen(
					const AosDataFieldType::E type,
					int &len);
					
	virtual OmnString getDataProcName();

	bool	dumpRecord(
			const AosDataRecordObjPtr &rec,
			const OmnString &msg);

	virtual i64  getProcDataCount() { return mProcDataCount; }
	virtual i64	 getProcOutputCount() { return mOutputCount; }
};

#endif
