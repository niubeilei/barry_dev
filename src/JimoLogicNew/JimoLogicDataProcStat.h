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
#ifndef AOS_JimoLogicNew_JimoLogicDataProcStat_h
#define AOS_JimoLogicNew_JimoLogicDataProcStat_h

#include "JimoLogicNew/JimoLogicStat.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataProcStat : virtual public AosJimoLogicNew
{
	OmnDefineRCObject;

protected:
	vector<OmnString>					mStatNames;
	AosExprObjPtr						mInput;
	int64_t								mCountValue;
	OmnString           				mDBTableName;
	OmnString 							mOrigStmt;
	OmnString							mOpr;
	

	OmnString 							mDataProcName;
	vector<OmnString>           		mStatModelNames;
	map<OmnString, OmnString>			mStatIdx;
	map<OmnString, OmnString>			mCond;
	map<OmnString, OmnString>  			mKeyIdxOpr;
	map<OmnString, StatTime> 			mTime;
	map<OmnString, vector<OmnString> >  mStatModelKeyFields;
	map<OmnString, vector<OmnString> >  mStatModelMeasures;
	map<OmnString, vector<OmnString> >  mStatModelShuffleFields;
	
	//for stream
	vector<AosExprObjPtr> 				mNameValueList;
public:
	AosJimoLogicDataProcStat(const int version);

	~AosJimoLogicDataProcStat();

	// Jimo Interface
	virtual AosJimoPtr cloneJimo() const;

	// AosGenericObj interface
	virtual bool parseJQL(
			AosRundata *rdata,
			AosJimoParserObj *jimo_parser,
			AosJimoProgObj *prog,
			bool &parsed,
			bool dft = false);

	AosExprObjPtr getInput() {return mInput;}

	bool compileJQL(
			AosRundata *rdata,
			AosJimoProgObj *prog);


	virtual bool run(
			AosRundata *rdata, 
			AosJimoProgObj *job,
			OmnString &statements_str,
			bool isparse);

	virtual OmnString getTableName() const;

private:
	
	void	initParms();
	
	bool syntaxCheck(AosRundata*, AosJimoParserObj*);
	
	bool checkCreateStat(AosRundata*, AosJimoParserObj*);
	
	bool checkRemoveStat(AosRundata*, AosJimoParserObj*);
	
	bool checkDescribeStat(AosRundata*, AosJimoParserObj*);
	
	bool getStatName(
		    AosRundata* rdata, 
			vector<OmnString> &model_name,
			AosXmlTagPtr &statistic_def);

	bool getStatIdx(
		    AosRundata* rdata,	       
			OmnString &stat_model_name,
			OmnString &idx,
			AosXmlTagPtr &statistic_def);

	bool getKeyIndexOpr(
		    AosRundata *rdata,
			vector<OmnString> &keyfields,
			map<OmnString,OmnString> &key_index_info,
			AosXmlTagPtr &tabledoc);

	bool getStatDoc(
			AosRundata *rdata, 
			AosXmlTagPtr &tabledoc,
			AosXmlTagPtr &statistic_def);
};
#endif

