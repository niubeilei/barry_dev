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
// 2015/05/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JimoLogicNew_JimoLogicDataProcWordCount2_h
#define AOS_JimoLogicNew_JimoLogicDataProcWordCount2_h

#include "JimoLogicNew/JimoLogicDataProcNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcWordCount2 : virtual public AosJimoLogicDataProcNew
{
	OmnDefineRCObject;

protected:
	OmnString					mDpName;
	vector<OmnString>           mWordKeys;
	map<OmnString, int>         mWordMap;
	OmnString 					mStatementStr;
	OmnString 					mErrmsg;

public:

	AosJimoLogicDataProcWordCount2(const int version);
	~AosJimoLogicDataProcWordCount2();
	
	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	bool semanticsCheck(
		AosRundata *rdata,
		AosJimoProgObj *prog);

	// AosGenericObj interface

	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	// AosJqlStatement interface
	/*
	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *prog,
					OmnString &statement_str,
					bool inparser);
	*/
	
	virtual bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

   virtual AosDataProcObjPtr cloneProc() ;
private:

	/*bool createWordCountDataProc(
					AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statement_str);*/
	/*
	bool addWordCountTask(
				AosRundata *rdata, 
				AosJimoProgObj *prog);*/

	bool generateCode(
				AosRundata *rdata,         
				AosJimoProgObj *prog,      
				OmnString &statement_str);

public:
	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	virtual bool    finish(const vector<AosDataProcObjPtr> &prcos, const AosRundataPtr &rdata);

};
#endif

