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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcJoin_h
#define AOS_JimoLogicNew_JimoLogicDataProcJoin_h

#include "JimoLogicNew/JimoLogicDataProc.h"
#include "JimoParser/JimoParser.h"

#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosJimoLogicDataProcJoin : public AosJimoLogicDataProc
{
	OmnDefineRCObject;

private:
	OmnString		    		mType;
    OmnString           		mDataProcName;
    vector<OmnString> 			mInputs;               
	OmnString					mOutput;
	
    //OmnString		 			mLeftInput;
    //OmnString		 			mRightInput;
	//
	//OmnString	        		mCond;
	OmnString  					mDataprocJoinName;
	vector<OmnString>			mInputNames;
	OmnString					mOutputName;
	//OmnString					mDataprocOutput;
	vector<OmnString> 			mLeftJoinKeys;
	vector<OmnString> 			mRightJoinKeys;
	vector<OmnString> 			mLeftIILJoinKeys;
	vector<OmnString> 			mRightIILJoinKeys;

    int                 		mSplit;
	vector<AosExprObjPtr> 		mNameValueList;
	OmnString					mCache;
	OmnString 					mErrmsg;
	OmnString					mReverse;
	vector<OmnString>      	    mAlias;
	vector<OmnString>			mLeftFieldsList;
	vector<OmnString>			mRightFieldsList;

	vector<AosExprObjPtr>		mLeftList;
	vector<AosExprObjPtr>		mRightList;

	OmnString					mLeftTable;
	OmnString					mRightTable;
	OmnString					mDb;
	OmnString					mTableName;
	OmnString					mSchemaName;
	vector<OmnString>			mSelectNameList;

	// for streaming
	map<OmnString, vector<OmnString> > mJoinMap;
	map<OmnString, vector<OmnString> > mIILJoinMap;
	map<OmnString, u32> mInputMap;
	map<OmnString, vector<OmnString> > mSelectMap;

	//for iiljoinleft and iiljoinright
	vector<OmnString>			mIILJoinList;
	OmnString					mLeftInput;
	OmnString					mRightInput;
	OmnString					mLeftIILJoin;
	OmnString					mRightIILJoin;

	// jimodb-753
	// get fields info from inputs
	vector<OmnString>  mFieldStr;

public:
	AosJimoLogicDataProcJoin(const int version);
	~AosJimoLogicDataProcJoin();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser, 
					AosJimoProgObj *prog,
					bool &parsed, 
					bool dft = false);

	// AosJimoLogicObjNew interface
	virtual bool compileJQL(                       
			        AosRundata *rdata,
					AosJimoProgObj *prog);

	bool getOutputName(AosRundata *rdata, AosJimoProgObj *prog, OmnString &name);

	virtual bool run(
					AosRundata *rdata, 
					AosJimoProgObj *job,
					OmnString &statements_str,
					bool isparse);
	//arvin 2015.08.18
	//JINSHANG-139
	virtual bool getInputV(vector<OmnString> &inputs);

	// 2015/9/20
	bool getFieldStr(vector<OmnString> &field_strs);

private:
	bool createJoinDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);
	
	bool createIILJoinDataproc(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);

	bool createDataprocIILJoin(AosRundata *rdata, AosJimoProgObj *jimo_prog, OmnString &);

	bool parseRun(AosRundata* rdata, OmnString &stmt, AosJimoProgObj *jimo_prog);
	bool addJoinTask(AosRundata *rdata, AosJimoProgObj *jimo_prog);
	bool processInput(
					AosRundata *rdata,
					AosJimoProgObj *prog,
					OmnString &statements_str);
	bool confCond(
                	AosRundata *rdata,
                	AosJimoProgObj* prog,
                	AosJimoLogicObjNewPtr jimologic,
                	OmnString &statement_str);

	bool getInputs(
                	AosRundata *rdata,
                	OmnString &str,
                	vector<OmnString> &inputs);

	bool parseInputFields(AosRundata *rdata,
					AosJimoProgObj *jimo_prog);

	bool isSameInput(AosRundata *rdata, 
					AosJimoProgObj *jimo_prog, 
					OmnString input);

	bool getSameSelect(AosRundata *rdata, 
					AosJimoProgObj *jimo_prog, 
					OmnString input, 
					vector<OmnString> select_list, 
					u32 idx);
	
	bool checkJoinInput(
			AosRundata *rdata,
			OmnString &inputName,
			OmnString &statements_str,
			JSONValue &root,
			int &orderSize,
			int &groupSize,
			int &distinctSize,
			int &orderTypeSize);

    bool checkJoinSort(
			AosRundata *rdata,
			OmnString &joinKey,
			JSONValue &root,
			int &orderSize,     
			int &groupSize,     
			int &distinctSize,  
			int &orderTypeSize);
	
	OmnString subDpSelectName(OmnString &inputName);
};
#endif

