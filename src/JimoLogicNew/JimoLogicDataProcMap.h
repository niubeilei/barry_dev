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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcMap_h
#define AOS_JimoLogicNew_JimoLogicDataProcMap_h

//#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcMap : virtual public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString			mInput;
	OmnString 			mParser;
	OmnString 			mMapName;
	OmnString           mTableName;
	OmnString  			mDataprocMap;
	OmnString  			mDataprocDoc;
	OmnString			mMapIILName;
	vector<OmnString>	mTasks;
	vector<AosExprObjPtr> mNameValueList;
	//arvin
	OmnString			mValueAggrOpr;
	vector<OmnString> 		    mConds;
	vector<OmnString>			mValues;
	vector<int>					mMaxLens;
	vector<OmnString>			mIILNames;
	vector<vector<OmnString> >	mKeyFields;
	vector<OmnString>			mDataType;
	OmnString			mInputName;
	map<OmnString, OmnString>	mVfFieldNameValueMap;
	vector<OmnString> mMapTypes;

public:
	AosJimoLogicDataProcMap(const int version);
	~AosJimoLogicDataProcMap();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosJimoLogicObjNew interface
	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata,
					AosJimoParserObj *jimo_parser,
					AosJimoProgObj *prog,
					bool &parsed,
					bool dft = false);

	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);

	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);

private:
	bool addIILTask(AosRundata *rdata, AosJimoProgObj *jimo_prog, int opt);
	bool addMapTask(AosRundata *rdata, AosJimoProgObj *jimo_prog, int opt);
	bool createIILDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str,int opt);
	bool createStr2StrMapDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str,int opt);
	bool createStr2NumMapDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str,int opt, const OmnString &datatype);
	//arvin 2015.06.11
	/*bool createDocBatchoprDataproc(
			AosRundata *rdata, 
			AosJimoProgObj *prog, 
			OmnString &statements_str);*/

	/*bool addDocTask(
			AosRundata *rdata, 
			AosJimoProgObj *prog,
			int opt);*/

	bool configKeysValueCond(
			AosRundata *rdata, 
			AosXmlTagPtr &table_doc);

	bool createVfNameValueMap(const AosXmlTagPtr &schema_doc);

};
#endif

