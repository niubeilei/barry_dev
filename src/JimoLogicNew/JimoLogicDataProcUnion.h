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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcUnion_h
#define AOS_JimoLogicNew_JimoLogicDataProcUnion_h

//#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcUnion : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

protected:
    OmnString           		mDataProcName;
    vector<OmnString>  			mInputs;
    vector<OmnString>  			mInputNames;
	OmnString 					mOutputName;
	vector<AosExprObjPtr> 		mNameValueList;
	OmnString           		mDataprocIILName ;
	OmnString           		mIndexOutput;
	OmnString					mDataprocIndexName;
	OmnString					mIILName;
	vector<OmnString>			mSubFields;
	OmnString					mIndexKey;
//	map<OmnString, vector<OmnString> > mUnionMap;
//	vector<OmnString>			mUnionList;
//	vector<OmnString>			mLeftSelectList;
//	vector<OmnString>			mRightSelectList;
	OmnString					mSchemaName;

	struct FieldConf
	{
		OmnString 		fname;
		OmnString		type;
		OmnString		alias;
		OmnString		max_length;
	};

	vector<FieldConf> 		mFieldConf;
	// jimodb-753
	vector<OmnString>		mFieldStr;

	bool createIndexDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
	bool createIILBatchoprDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
	bool addSelectTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool addIndexTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool addIILTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool parseRun(AosRundata* rdata, OmnString &stmt, AosJimoProgObj *jimo_prog);
	bool setOutputName(AosRundata*, AosJimoProgObj *, const OmnString&);
	bool setCache(AosRundata*, AosJimoProgObj *, const OmnString&);
	bool getOutputName(AosRundata *rdata, AosJimoProgObj *prog,OmnString &name) {name = mOutputName; return true;}
	bool setOrderFields(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& name);
	bool setOrderType(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& name);
//	bool setOutputNames(AosRundata *rdata, const OmnString &name, const OmnString &output);
	//vector<AosExprObjPtr> getFields();
	vector<AosExprObjPtr> getDistinct();
	OmnString 	mErrmsg;

public:
	AosJimoLogicDataProcUnion(const int version);
	~AosJimoLogicDataProcUnion();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	virtual bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog);

	virtual bool run(
					AosRundata *rdata,
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);

	virtual bool getInputList(vector<OmnString> &inputs);

	virtual OmnString getLogicName() const;
	virtual bool getInputV(vector<OmnString> &inputs);
	
	//felicia,2015/09/21 for streaming dataflow
	virtual AosJimoLogicType::E getJimoLogicType() const {return AosJimoLogicType::eDataProcUnion;}

private:
	inline OmnString getOutputName() const
	{
		OmnString output_name = "_dsdp_";
		output_name << mDataProcName << "_union_outputs";
		return output_name;
	}

	//bool configFields(AosRundata*, vector<AosExprObjPtr>&, OmnString&, bool);
	//bool configFields(AosRundata*, AosJimoProgObj *prog,vector<JSONValue> &fieldsJV);
	bool setFields(AosRundata*, AosJimoProgObj*, const OmnString& field_str);
	OmnString getIILName() const {return mIILName;}
	vector<OmnString> getSubFields() const {return mSubFields;}
	bool setInputNames();
	//bool getFields(std::vector<AosExprObjPtr> &);


	bool getDataFieldsDocs(
			AosRundata 	*rdata,
			const vector<OmnString> &schema_names,
			vector<AosXmlTagPtr> &datafields_docs);

	bool getSchemaFromDoc(
			AosRundata* rdata,
			AosXmlTagPtr& doc);

	bool config(AosRundata*);

	bool
	getUnionFieldConf(
			AosRundata *rdata,
			AosJimoProgObj *prog,
			vector<JSONValue> fieldsJV);


	bool
	getFieldsInfo(
			AosRundata *rdata,
			vector<JSONValue> fieldsJV,
			vector<OmnString> &types,
			vector<OmnString> &names);

	bool
	configFields(
			AosRundata *rdata,
			vector<JSONValue> &fieldsJV);

	bool
	getFieldStr(vector<OmnString> &field_strs);

};
#endif

