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
#ifndef AOS_JimoLogicNew_JimoLogic_h
#define AOS_JimoLogicNew_JimoLogic_h

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/JimoLogicObjNew.h"
#include "SEInterfaces/JimoParserObj.h"
#include "SEInterfaces/JimoParserObj.h"
#include "SEInterfaces/Ptrs.h"

#include "Ptrs.h"

#include "JQLExpr/Ptrs.h"
#include "XmlUtil/Ptrs.h"


class AosJimoLogicNew : public AosJimoLogicObjNew, 
						public AosJimoParserObj,
						public AosJqlStatement
{
public:
	struct Field
	{
		OmnString	field_name;
		OmnString	field_type;
		int			max_len;
		int         offset;

		Field()
		:
		max_len(0)
		{
		}

		bool isValid(OmnString &errmsg)
		{
			if (field_name == "")
			{
				errmsg = "field_name_empty";
				return false;
			}

			if (field_type == "") field_type = "str";

			return true;
		}
	};

	enum ErrorType
	{
		eGenericError = 0,
		eMissingParm = 1,
		eInvalidParm = 2,
		eNotExist = 3,
		eSyntaxError = 4,
		eFilePathNotExist = 5,
		eUnknowParm = 6,
		eMissingIndex = 7,
		eDuplicatedIndexName = 8,
		eIILegalName = 9
	};

protected:
	OmnString					mJimoName;
	AosJimoProgObjPtr           mJob;
	std::vector<AosExprObjPtr> 	mNameValueList;
	AosExprObjPtr 				mInput;
	vector<OmnString>			mOrderFields;
	vector<OmnString>			mGroupByFields;
	vector<OmnString>			mOrderType;
	vector<OmnString>			mOutputNames;
	map<OmnString, OmnString>   mDsOutputNames;
	vector<AosXmlTagPtr>        mDocConf;
	struct FieldConf
	{
		OmnString       fname;
		OmnString       type;
		OmnString       alias;
		OmnString       max_length;
	};

public:
	struct SplitterConf
	{
		OmnString		mType;
		OmnString		mDistributionMapName;
		OmnString		mSplitField;
		OmnString		mShuffleType;
	};

	SplitterConf		mSplitter;
	OmnString			mCond;
	OmnString			mCache;
	OmnString			mSaveDoc;

	AosJimoLogicNew();
	AosJimoLogicNew(const int version);
	//AosJimoLogicNew(const AosJimoLogicNew &rhs);
	~AosJimoLogicNew();

	virtual bool setUserVarValue(AosRundata *rdata, const AosValueRslt &value);

	virtual AosValueRslt getUserVarValue(AosRundata *rdata);

	virtual bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog); 

	virtual bool setOutputName(
						AosRundata *rdata, 
						AosJimoProgObj *prog, 
						const OmnString &name);

	virtual bool setUnionDatasetName(
			            AosRundata *rdata, 
						AosJimoProgObj *prog,
						const OmnString &name);


	virtual bool setCache(
						AosRundata *rdata, 
						AosJimoProgObj *prog, 
						const OmnString &cache)
	{
		return false;
	}

	virtual bool getOutputName(AosRundata *rdata, AosJimoProgObj *prog, OmnString &name) { return false; }

	virtual OmnString getLogicName() const { return ""; }

	virtual AosJimoLogicType::E getJimoLogicType()const {return AosJimoLogicType::eInvalid;}

	static bool getDatasetOutput(AosRundata *rdata, 
						AosJimoProgObj *prog, 
						const OmnString &input_name,
						vector<OmnString> &outputNames);
/*
	virtual bool setOutputNames (AosRundata *rdata,
						const OmnString &name,
						const OmnString &output_name)
	{
		mDsOutputNames[name] = output_name;
		return true;
	}
*/
	virtual OmnString getOutputByName(
						AosRundata *rdata,
						const OmnString &name)
	{
		map<OmnString, OmnString>::iterator itr = mDsOutputNames.find(name);    
		if (itr == mDsOutputNames.end()) return 0;
		return itr->second;
	}

	virtual OmnString getIILName() const {return "";}

	virtual OmnString getNewDataProcName()const {return "";}
	virtual vector<OmnString> getSubFields() const {vector<OmnString> v; return v;}

	virtual bool semanticsCheck(AosRundata *rdata, AosJimoProgObj *prog);

protected:
	OmnString				        mJobName;
	OmnString				        mOriginJob;
/*
	bool setStatName(OmnString &name){mStatName = name; return true;}
	bool setJob(AosJimoProgObj *job){mJob = job; return true;}
	bool setDatasetName(OmnString &name){mDatasetName = name; return true;}
	bool setJobName(OmnString &name){mJobName = name; return true;}
	bool setInput(AosExprObjPtr input) {mInput = input; return true;}
	bool setKeyFields(vector<OmnString> keyfields);
	bool setKeywords(vector<OmnString> keywords);
	bool setmeasure(vector<OmnString> measure);
	bool setNamevalue(vector<AosExprObjPtr> nameValue);
*/
	//vector<OmnString> getKeyfields() {return mFieldKeys;}
	//OmnString getStatname() {return mStatName;}
	//OmnString getDatasetname() {return mDatasetName;}
	//vector<OmnString> getMeasure() {return measures;}
	AosJimoProgObjPtr getjob() {return mJob;}
//	vector<OmnString> getNamevalues();
//	AosExprObjPtr getInput(){return mInput;}

	vector<OmnString> getKeyword() {return mKeywords;}  

	// GenericObj Interface
	virtual OmnString getObjType(AosRundata *rdata)
	{
		return "JimoLogic";
	}

	virtual bool setJimoName(const OmnString &name)
	{
		mJimoName = name;
		return true;
	}

	virtual OmnString getJimoName() const
	{
		return mJimoName;
	}

	bool configStr(AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				const OmnString &attrname,
				const OmnString &dft,
				OmnString &results,
				vector<AosExprObjPtr> &name_value_list);

	bool configInt(AosRundata *rdata,                                       
				AosJimoParserObj *jimo_parser,                      
				const OmnString &attrname,                          
				const int64_t &dft,                                 
				int64_t &results,                                   
				vector<AosExprObjPtr> &name_value_list);      

	bool configStr(AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				const OmnString &attrname,
				OmnString &results,
				vector<AosExprObjPtr> &name_value_list, 
				const OmnString &errmsg);

	bool configExpr(AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				const OmnString &attrname,
				AosExprObjPtr &results,
				vector<AosExprObjPtr> &name_value_list, 
				const bool mandatory, 
				const OmnString &errmsg);

	bool parseFields(AosRundata *rdata, 
				AosJimoParserObj *jimo_parser, 
				const OmnString &attrname,
				vector<Field> &fields,
				vector<AosExprObjPtr> &name_values);

	bool parseField(
				AosRundata *rdata, 
				Field &field,
				const AosExprObjPtr &field_def);

	bool configCommonAttrs(
				AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list,
				OmnString &errmsg);

	bool configSaveDoc(
				AosRundata *rdata,
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list);

	virtual bool createJimoLogicDoc(
				AosRundata *rdata,
				const OmnString &name,
				const OmnString &jql);

	bool configSplitter(
				AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list,
				OmnString &errmsg);
	
	bool configConditions(
				AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list,
				OmnString &errmsg);
	
	bool configCache(
				AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list,
				OmnString &errmsg);

	//bool configOrderBy(
	//				AosRundata *rdata, 
	//				AosJimoParserObj *jimo_parser,
	//				const vector<AosExprObjPtr> &name_value_list, 
	//				AosJimoLogicNew *dataset);	
	
	bool configOrderByGroupBy(
				AosRundata *rdata, 
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list,
				OmnString &errmsg);

	bool configOrderType(
				AosRundata *rdata,
				AosJimoParserObj *jimo_parser,
				vector<AosExprObjPtr> &name_value_list,
				const vector<OmnString> &fields);

	virtual bool parseRun(
				AosRundata* rdata,
				OmnString &stmt,
				AosJimoProgObj *jimo_prog);

	virtual vector<OmnString> getOutputNames();

	virtual bool procInput(
				AosRundata *rdata,
				const OmnString &input,
				AosJimoProgObj *prog,
				OmnString &input_name,
				OmnString &statementts_str);

	bool procOutput(AosRundata *rdata,
				const vector<OmnString> &outputs,
				AosJimoProgObj *prog,
				OmnString &output_name,
				OmnString &iilname);

	bool addJimoLogicToProg(
				AosRundata* rdata,
				OmnString &stmt,
				AosJimoProgObj *jimo_prog);

	// 2015/6/03, Xia Fan
	virtual bool setOrderFields(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& name)
	{
		return false;
	}
	virtual bool setOrderType(AosRundata* rdata, AosJimoProgObj *prog, const vector<OmnString>& name)
	{
		return false;
	}

	virtual bool getFields(std::vector<AosExprObjPtr> &fields);
	virtual bool getDistinct(std::vector<AosExprObjPtr> &distinct);
	virtual bool setFields(AosRundata* rdata, AosJimoProgObj* prog, const OmnString& fields_str);
	virtual bool getOutputsFromDataProc(
						AosRundata* rdata,
						AosJimoProgObj *jimo_prog,
						const vector<OmnString> &procNames,
						vector<OmnString> &outputs);
	virtual bool getSchemaName(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		vector<OmnString> &schema_name);
	//arvin 2015.07.07
	virtual bool setErrMsg(
			AosRundata *rdata, 
			const ErrorType status,
			const OmnString &parm,
			OmnString &errmsg);
	
	virtual bool checkNameValueList(AosRundata *rdata,OmnString &errmsg,vector<AosExprObjPtr> &list);

	virtual bool isLegalName(
			AosRundata *rdata, 
			const OmnString &name,
			//AosJimoProgObj *prog,
			OmnString &errmsg);

	virtual bool setSubFields(
			AosRundata *rdata, 
			AosJimoProgObj* prog, 
			const vector<OmnString> &subfields);
	//xiafan 2015/7/24
	OmnString convertType(AosRundata *rdata, OmnString &type);
	virtual bool getInputList(vector<OmnString> &inputs);
	
	//arvin 2015.08.21
	virtual OmnString getInput()const;
	virtual OmnString getTableName() const;
	virtual bool isExist(const OmnString &name,const OmnString &parm);

	// 2015.09.02
	// jimodb-753 2015.09.18
	virtual bool getFieldStr(vector<OmnString> &);

	// jimodb-753
	// 2015.09.18
	virtual bool getInputSchema(
				AosRundata *rdata,
				AosJimoProgObj *prog,
				vector<OmnString> &field_str);

	bool getDataFieldsDocs(
				AosRundata *rdata,
				const vector<OmnString> &schema_names,
				vector<AosXmlTagPtr> &datafields_docs);

	bool configFields(
			AosRundata *rdata,
			const vector<AosXmlTagPtr> &datafields_docs,
			vector<OmnString> &field_str);
public:
	//JimoLogicNew interface,JimoLogicDataProc must be realized
	virtual bool getInputV(vector<OmnString> &inputs);
private:
	bool createDatasetIIL(
					AosRundata *rdata,
					const OmnString &input_name,
					AosJimoLogicObjNew *logic,
					AosJimoProgObj *prog,
					OmnString &statements_str);

	// 2015.20.09
	// jimodb-753
	bool getFieldsConf(
			AosRundata *rdata,
			AosJimoProgObj *prog,
			vector<OmnString> &field_strs);


	virtual AosJimoParserObjPtr createJimoParser(AosRundata *rdata) 
	{
		return 0;
	}

	virtual bool parse(
				AosRundata *rdata, 
				AosJimoProgObj* prog ,
				const OmnString &stmt, 
				vector<AosJqlStatementPtr> &statements,
				bool dft = false)  {return false;}

	virtual bool getNameValueList(AosRundata *rdata, vector<AosExprObjPtr> &name_values) {return false;}
	virtual bool parseProcedureParms(AosRundata *rdata,vector<OmnString> &parms) {return false;}
	virtual OmnString getParmStr(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values) {return "";}
	virtual int		  getParmInt(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values) {return 0;}

	virtual AosExprObjPtr getParmExpr(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values) {return 0;}

	virtual bool parmExist(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values) {return false;}

	virtual bool getParmArray(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values, 
				vector<AosExprObjPtr> &keyname) {return false;}

	virtual bool getParmArrayStr(
				AosRundata *rdata, 
				const OmnString &parm, 
				vector<AosExprObjPtr> &name_values, 
				vector<OmnString> &measure) {return false;}

	virtual bool nextSemiColon(AosRundata *rdata) {return false;}

	virtual OmnString nextObjName(AosRundata *rdata)  {return "";}

	virtual bool peekNextChar(const char character) {return false;}

	virtual bool nextStatement(
			AosRundata *rdata,
			AosJimoProgObj *prog,			// Xia Fan, 2015/12/05
			OmnString &expr_str, 
			AosJqlStatementPtr &statement,
			bool dft = false) {return false;}

	virtual OmnString nextKeyword(AosRundata *rdata){return "";}
	
	virtual OmnString nextExpr(AosRundata *rdata) {return "";}

	virtual OmnString getOrigStmt() {return "";}
	
	virtual void setOrigStmt(const OmnString stmt) {}


};
#endif

