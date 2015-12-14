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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcWordParser_h
#define Aos_DataProc_DataProcWordParser_h

#include "DataProc/DataProc.h"
#include "DocUtil/DocProcUtil.h"
#include "WordParser/WordNorm.h"     
#include "WordParser/WordParser.h"
#include "SEUtil/SeTypes.h"

class AosDataProcWordParser : virtual public AosDataProc
{
	enum
	{
		eMaxThrdIds = 15
	};

	struct WordInfo
	{
		RecordFieldInfo 			mKeyOutput;
		RecordFieldInfo 			mDocidOutput;
	};
	
	RecordFieldInfo				mKeyInput;
	RecordFieldInfo				mDocidInput;
	vector<WordInfo>			mInfo;
	
	struct ParserUtil {
		AosWordParserPtr            mWordParser;
		AosWordNormPtr              mWordNorm;
		AosDocProcUtil::AosSeWordHash   mWords;

		ParserUtil()
		{
			mWordParser = OmnNew AosWordParser();
			mWordNorm = OmnNew AosWordNorm("", "wordNorm");
		}

		~ParserUtil(){}
	};

	static __thread ParserUtil* stParserUtil;

public:
	AosDataProcWordParser(const bool flag);
	AosDataProcWordParser(const AosDataProcWordParser &proc);
	~AosDataProcWordParser();

	virtual AosDataProcStatus::E procData( 
						const AosDataRecordObjPtr &record,
						const u64 &docid,
						const AosDataRecordObjPtr &output,
						const AosRundataPtr &rdata);
	
	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	virtual bool	resolveDataProc(
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);

	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);
	
	virtual void onThreadInit(const AosRundataPtr &, void **data);
	virtual void onThreadExit(const AosRundataPtr &, void **data);
	
	virtual bool finish(
			const vector<AosDataProcObjPtr> &procs,
			const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};

#endif
