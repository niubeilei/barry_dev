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
// Statement Syntax:
// 		DATASET mydataset
// 		(
// 		    name: value,
// 		    name: value,
// 		    ...
// 		    name: value,
//		);
//
// Modification History:
// 2015/05/23 Create on by Xia Fan
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataset.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


AosJimoLogicDataset::AosJimoLogicDataset(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew, version),
AosJimoLogicNew(version)
{                                             
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}

AosJimoLogicDataset::~ AosJimoLogicDataset()
{
}

/*
bool
AosJimoLogicDataset::configCommonAttrs(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		const vector<AosExprObjPtr> &name_value_list, 
		AosJimoLogicDataset *dataset)
{
	if (!configSplitter(rdata, jimo_parser, name_value_list, dataset)) return false;
	if (!configConditions(rdata, jimo_parser, name_value_list, dataset)) return false;
	if (!configCache(rdata, jimo_parser, name_value_list, dataset)) return false;

	return true;
}


bool
AosJimoLogicDataset::configConditions(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		const vector<AosExprObjPtr> &name_value_list,
		AosJimoLogicDataset *dataset)
{
	// parse condition
    OmnString attrname = "conditions";
    AosExprObjPtr cond= jimo_parser->getParmExpr(rdata, attrname, name_value_list);
    if (cond)
    {
        dataset->mCond = cond->dumpByNoQuote();
	}
	return true;
}


// parse cache
bool
AosJimoLogicDataset::configCache(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		const vector<AosExprObjPtr> &name_value_list,
		AosJimoLogicDataset *dataset)
{
	AosExprObjPtr cache;
	OmnString attrname = "cache";
	cache = jimo_parser->getParmExpr(rdata, attrname, name_value_list);
	if (cache)
	{
		dataset->mCache = cache->dumpByNoQuote();
	}

	return true;
}

bool
AosJimoLogicDataset::configSplitter(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser,
		const vector<AosExprObjPtr> &name_value_list, 
		AosJimoLogicDataset *dataset)
{
	// This function parses the splitter attribute:
	// 	splitter : 
	// 	(
	// 		type: <type>,
	// 		cluster: xxx,
	// 		distribution: xxx,
	// 		timer: xxx,
	// 		min_size: xxx,
	// 		max_size: xxx,
	// 		...				// Addition attributes depending on the type.
	// 	),
	//
	// If 'flag' is true, the splitter is mandatory. Otherwise, it is optional.
	//
	// Retrieve the splitter
	vector<AosExprObjPtr> splitter;
	OmnString attrname  = "splitter";
	bool rslt = jimo_parser->getParmArray(rdata, attrname, name_value_list, splitter);
	if (rslt)
	{
		if(splitter.size() <= 0)
		{
			AosLogError(rdata, true, "missing_splitter") << enderr;
			return false;
		}
	}
	else return true;

	// parse splitter type
	attrname = "type"; 
	dataset->mSplitter.mType = jimo_parser->getParmStr(rdata, attrname, splitter);
	if (dataset->mSplitter.mType == "")
	{
		AosLogError(rdata, true, "missing_key_fields") << enderr;
		return false;
	}

	if (dataset->mSplitter.mType == "distribution_map")
	{
		//parse distribution_map
		attrname = "distribution_map"; 
		dataset->mSplitter.mDistributionMapName = jimo_parser->getParmStr(rdata, attrname, splitter);
		if (dataset->mSplitter.mDistributionMapName == "")
		{
	    	AosLogError(rdata, true, "missing_distribution_map_name") << enderr;
			return false;
		}

		// parse field_values
		attrname = "field_values";
		AosExprObjPtr expr = jimo_parser->getParmExpr(rdata, attrname, splitter);
		if (!expr)
		{
			AosLogError(rdata, true, "missing_field_values") << enderr;
			return false;
		}

		dataset->mSplitter.mSplitField = expr->dumpByNoQuote();
	}
	else
	{
		AosLogError(rdata, true, "type error") << enderr;
		return false;
	}

	// parse distribution(server/cube/..)
	attrname = "distribution"; 
	AosExprObjPtr locality = jimo_parser->getParmExpr(rdata, attrname, splitter);
	if (!locality)
	{
		dataset->mSplitter.mShuffleType = "none";
	}
	else
	{
		dataset->mSplitter.mShuffleType = locality->dumpByNoQuote();
	}

	// timer,
	// min_size,
	// max_size
	
	return true;
}
*/

