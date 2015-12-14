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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcSelect_h
#define AOS_JimoLogicNew_JimoLogicDataProcSelect_h

//#include "JimoLogicNew/JimoLogicDataset.h"
#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"

#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosJimoLogicDataProcSelect : virtual public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
	OmnString		    		mType;
    OmnString           		mDataProcName;
	//vector<AosExprObjPtr>		mDistinct;
    OmnString                   mDistinct;
	vector<AosExprObjPtr>		mFields;
    OmnString					mInput;
    OmnString					mOutput;
	OmnString           		mTableName;
	OmnString					mOutputName;
	OmnString  					mDataprocSelectName;
    OmnString           		mDataprocIndexName;
	OmnString					mIILName;
	OmnString					mSelectIILName;
    OmnString           		mDataprocIILName ;
	OmnString					mIndexOutput;
    int                 		mSplitNum;
	OmnString					mShuffleType;
	OmnString					mShuffleField;
	vector<AosExprObjPtr> 		mNameValueList;
	OmnString					mUnionDatasetName;
	OmnString					mInputName;
	vector<OmnString>			mFieldsName;
	OmnString					mOpr;
	OmnString					mSaveOpr;   //it indicate whether to save the select results to file or not
	OmnString					mParser;
	OmnString					mDocBatchOprName;
	vector<OmnString>			mSubFields;
    JSONValue                   mFieldsJV;

    OmnString                   mDataProcDefIILName;
    OmnString                   mDataProcDefIndexName;
	// for service
	OmnString					mDPName;
	OmnString					mSchemaName;
	int							mIdx;
	OmnString					mSelName;
	OmnString					mDB;
	OmnString					mIndexKey;
	OmnString					mLogicName;

	//for set selectname indexname iilname docname
	vector<OmnString>			mSelectList;
	OmnString					mSelectName;
	OmnString					mIndexName;

public:
	AosJimoLogicDataProcSelect(const int version);
	~AosJimoLogicDataProcSelect();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata,
					AosJimoParserObj *jimo_parser,
					AosJimoProgObj *prog,
					bool &parsed,
					bool dft = false);

	bool compileJQL(AosRundata*, AosJimoProgObj *);

	// AosJqlStatement interface
	virtual bool run(
					AosRundata *rdata,
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);

private:
	bool createDataProcIndex(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
	bool createStreamSelectDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
	bool createSelectDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
    bool createStreamIndexDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
    bool createIndexDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
    bool createIILBatchoprDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &statements_str);
    bool addSelectTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
    bool addIndexTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool addIILTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);


    bool createDataProcDoc(AosRundata *rdata, AosJimoProgObj *jimo_prog,OmnString &statements_str);
	bool createDataProcDocBatchOpr(
			AosRundata *rdata,
			AosJimoProgObj *jimo_prog,
			OmnString &statements_str);

	bool addDocTask(
			AosRundata *rdata,
			AosJimoProgObj *prog);

	//arvin 2015.06.19
	/*
	bool getDataFieldsDocs(
			AosRundata *rdata,
			const vector<OmnString> &schema_names,
			vector<AosXmlTagPtr> &datafields_docs);
	*/

	bool parseFields(
		AosRundata *rdata,
		vector<AosExprObjPtr> &name_value_list,
		AosJimoParserObj *jimo_parser,
		const OmnString& errmsg);

	bool checkOrderFieldIsExist(
			AosRundata *rdata,
			const OmnString& errmsg,
			const vector<OmnString>& fields);
	// jimodb-753
	bool configFields(
			AosRundata *rdata,
			vector<OmnString> &field_str);  
	//		const vector<AosXmlTagPtr> &datafields_docs);
	
	bool processOutput(AosRundata *rdata, AosJimoProgObj *prog, const OmnString &output);

	// jimodb-753
	bool getFieldsConf(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		vector<OmnString> &field_str);
		//vector<pair<OmnString, pair<OmnString,OmnString> > >&fieldconfs);
public:
	bool setOutputName(AosRundata *rdata, AosJimoProgObj *prog,const OmnString &name);
	bool setUnionDatasetName( AosRundata *rdata, AosJimoProgObj *prog,const OmnString &name);
	bool getOutputName(AosRundata *rdata, AosJimoProgObj *prog, OmnString &name) {name = mOutputName; return true;}
	bool setCache(AosRundata* rdata, AosJimoProgObj *prog, const OmnString& name) {mCache = name; return true;}
	bool setOrderFields(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& order) {mOrderFields = order; return true;}
	bool setOrderType(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& order) {mOrderType = order; return true;}
	bool setFields(AosRundata*, AosJimoProgObj*, const OmnString& field_str);
	bool getFields(vector<AosExprObjPtr> &fields);
	bool getDistinct(vector<AosExprObjPtr> &distinct);

	OmnString getIILName() const {return mIILName;}
	vector<OmnString> getSubFields() const {return mSubFields;}

	//arvin 2015.06.19
	bool getInputV(vector<OmnString> &inputs);

	bool parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog);

	OmnString getNewDataProcName() const;
	virtual OmnString getLogicName() const;
	
	virtual OmnString getTableName() const {return mTableName;}

	bool getFieldsInfo(
			AosRundata *rdata,
			const vector<AosXmlTagPtr> &datafields_docs,
			u64   &totalLength);

	// jimodb-753
	//bool getFieldStr (OmnString &field_str);
	bool getFieldStr (vector<OmnString> &field_str); 
    bool getFieldsFromSchema(
            AosRundata *rdata,
            const vector<AosXmlTagPtr> &datafields_docs);
};
#endif

