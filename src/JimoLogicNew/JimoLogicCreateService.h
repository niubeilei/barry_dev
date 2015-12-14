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
#ifndef AOS_JimoLogicNew_JimoLogicService_h
#define AOS_JimoLogicNew_JimoLogicService_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicCreateService : public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	OmnString					mServiceName;
	AosJimoProgObjPtr			mService;
	vector<OmnString>			mParms;
	vector<AosJqlStatementPtr>  mStatements;
	OmnString					mOriginService;
//	bool						mInSelectList;
	vector<OmnString>			mConfList;
/*
	vector<OmnString> 			mNameList;
	vector<OmnString>			mDatasetNameList;
	vector<OmnString>			mSelectList; 
	vector<OmnString> 			mIndexList;
	vector<OmnString> 			mIILBatchOprList;
	vector<OmnString> 			mDocBatchOprList;
	vector<OmnString>			mIILJoinList;
	vector<OmnString> 			mGroupByList;
	vector<OmnString> 			mStatJoinList;
	vector<OmnString> 			mStatDocList;
	vector<OmnString>			mStatKeyList;
	vector<OmnString> 			mIILKeyList;
	vector<OmnString>			mUnionSelectNameList;
	vector<OmnString>			mDatasetList;
	vector<OmnString>			mIsInSelectList;

	map<OmnString, vector<OmnString> > mDatasetMap;
	map<OmnString, vector<OmnString> > mSelectMap;
	map<OmnString, vector<OmnString> > mUnionMap;
	map<OmnString, vector<OmnString> > mJoinMap;
	map<OmnString, vector<OmnString> > mIILJoinMap;
	map<OmnString, vector<OmnString> > mStatMap;
	map<OmnString, vector<OmnString> > mSelectInputMap;
	map<OmnString, vector<OmnString> > mDataProcMap;
	map<OmnString, vector<OmnString> > mDocMap;
	map<OmnString, vector<OmnString> > mIndexMap;
	map<OmnString, vector<OmnString> > mComSelMap;
	map<OmnString, vector<OmnString> > mSelectConfMap;
*/

public:
		AosJimoLogicCreateService(const int version);
		~AosJimoLogicCreateService();

		// Jimo Interface
		virtual AosJimoPtr cloneJimo() const;

		// AosJimoLogicObjNew interface
		virtual bool run(
				AosRundata *rdata, 
				AosJimoProgObj *Service,
				OmnString &statements_str,
				bool isparse);

		virtual bool parseJQL(
				AosRundata *rdata, 
				AosJimoParserObj *jimo_parser, 
				AosJimoProgObj *prog,
				bool &parsed, 
				bool dft = false);

		bool parseRun(
				AosRundata* rdata,
				OmnString &stmt,
				AosJimoProgObj *jimo_prog);

private:

		bool appendStatement(AosRundata *rdata,const AosJqlStatementPtr &stmt);

		bool findSubString(
				AosRundata *rdata,
				OmnString str,
				vector<OmnString> &replace_parms);

	/*
		OmnString sampleFlowConf(OmnString conf_str);
		OmnString dataFlowConf(OmnString name, OmnString conf_str, vector<OmnString> stat_list);
		OmnString selectConfigStr(OmnString conf_str, vector<OmnString> selectLsit, OmnString iilJoinName);
		OmnString getStatFlow(OmnString name, OmnString conf_str, vector<OmnString> iilJoinList);

		OmnString getDataProcMap(OmnString name);
		OmnString getDataProcMapIIL(OmnString name);
		OmnString getDocSelectConf(OmnString name);
		OmnString getDocConf(OmnString name);
		OmnString getIndexConf();
		bool isSameDataset(map<OmnString, vector<OmnString> >::iterator itr, u32 idx);

		bool getDataProcMapConf();
		OmnString getDatasetAndMap(OmnString ds_name);
		OmnString getDataProcMapIILConf();
		bool getDocSelectConf();
		OmnString getDocConf();
		bool insertToDatasetMap(OmnString ds_name, OmnString sel_name);
		bool insertToComSelMap(OmnString sel_name, OmnString idx_name);
		OmnString getCombineMap();
		bool	insertSelectConf();
		*/
		OmnString getDataProcConf();
		OmnString	getAllDataset(AosJimoProgObj *jimo_prog);
};
#endif

