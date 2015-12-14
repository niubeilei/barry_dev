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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcDoc_h
#define AOS_JimoLogicNew_JimoLogicDataProcDoc_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcDoc : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:

	OmnString										mParser;
	OmnString										mRecordName;
	OmnString										mInput;
	OmnString	 									mTable;
	vector<OmnString> 								mFields;
	OmnString										mInputName;
	OmnString										mDocName;
	//bool											mHaveColumn;
	OmnString										mFormat;
    OmnString                                       mDataProcIILName;
    OmnString                                       mDataProcIndexName;
	OmnString 										mPrime;
	vector<OmnString> 								mDocList;
	OmnString	 									mSchemaName;
	OmnString										mSelectName;

public:

	AosJimoLogicDataProcDoc(const int version);
	~AosJimoLogicDataProcDoc();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;


	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	//JimoLogicObjNew interface
	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

	//arvin 2015.08.21
	virtual OmnString getInput()const;
	
	virtual OmnString getTableName() const;

	virtual bool isExist(const OmnString &name,const OmnString &parm);

	virtual AosJimoLogicType::E getJimoLogicType()const {return AosJimoLogicType::eDataProcDoc;}
	
	// AosJqlStatement interface
	virtual bool run(
			AosRundata* rdata,
			AosJimoProgObj *prog,
			OmnString &statement_str,
			bool isparse);


private:

	bool createDocBatachoprDataproc(
			AosRundata *rdata,
			AosJimoProgObj *prog,
			OmnString &statement_str);

	bool addDocTask(
			AosRundata *rdata,
			AosJimoProgObj *prog);

	bool getDataFields(
		vector<OmnString> &fields,
		const AosXmlTagPtr &tag,
		AosRundata * rdata);

    bool createIndexDataproc(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statement_str);

    bool createIILBatchoprDataproc(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statement_str);

	bool addIndexTask(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog);

	bool addIILTask(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog);
			
	bool createSelectDataproc(    
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statements_str);
};
#endif

