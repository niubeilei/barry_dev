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
#ifndef AOS_JimoLogicNew_JimoLogicDataset_h
#define AOS_JimoLogicNew_JimoLogicDataset_h

#include "JimoLogicNew/JimoLogicNew.h"
#include "JimoParser/JimoParser.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include <vector>

class AosJimoLogicDataset : public AosJimoLogicNew
{
public:
/*
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
	//map<OmnString, OmnString>     mDataprocOutputs;

protected:
	bool configCommonAttrs(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser,
					const vector<AosExprObjPtr> &name_value_list, 
					AosJimoLogicDataset *dataset);

	bool configSplitter(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser,
					const vector<AosExprObjPtr> &name_value_list, 
					AosJimoLogicDataset *dataset);
	
	bool configConditions(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser,
					const vector<AosExprObjPtr> &name_value_list, 
					AosJimoLogicDataset *dataset);
	
	bool configCache(
					AosRundata *rdata, 
					AosJimoParserObj *jimo_parser,
					const vector<AosExprObjPtr> &name_value_list, 
					AosJimoLogicDataset *dataset);
*/
protected:
	//OmnString			mUnionDatasetName;

	//vector<OmnString> getKeyword() {return mKeywords;}
public:
	AosJimoLogicDataset(const int version);
	~AosJimoLogicDataset();

	virtual AosJimoLogicType::E getJimoLogicType() const {return AosJimoLogicType::eDataset;}
	
};
#endif

