////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/01/22 Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StatQueryAnalyzer/StatQueryAnalyzer.h"

#include "API/AosApi.h"
#include "StatUtil/StatDefineDoc.h"
#include "StatQueryAnalyzer/StatQueryPattern1.h"
#include "StatQueryAnalyzer/StatQueryUnit.h"
#include "SEInterfaces/QueryReqObj.h"

AosStatQueryAnalyzer::AosStatQueryAnalyzer()
{
	mPattern1 = OmnNew AosStatQueryPattern1();
}

AosStatQueryAnalyzer::~AosStatQueryAnalyzer()
{
}


bool
AosStatQueryAnalyzer::analyzeReq(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req)
{
	u64 t1 = OmnGetTimestamp(); 
	
	aos_assert_r(mPattern1, false);
	
	AosStatDefineDocPtr stat_def = getStatDefDoc(rdata, req);
	aos_assert_rr(stat_def, rdata, false);

	bool rslt;
	int pattern_num = 1;
	switch (pattern_num)
	{
	case 1:
		 rslt = mPattern1->proc(rdata, req, stat_def);	
		 aos_assert_r(rslt, false);
		 return true;

	case 2:
		 //rslt = procPattern002(rdata, req, stat_def_doc);	
		 //aos_assert_r(rslt, false);
		 //return true;

	default:
		 break;
	}
	
	AosSetErrorUser(rdata, "statqueryanalyzer_unrecog_stat_query")
		<< enderr;

	OmnScreen << "!!!!!!!! run query time:" << OmnGetTimestamp() - t1 << endl;
	return false;
}


AosStatDefineDocPtr
AosStatQueryAnalyzer::getStatDefDoc(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req)
{
	vector<OmnString> tables = req->getTables();
	aos_assert_r(tables.size() == 1, 0);

	OmnString table = tables[0];
	aos_assert_rr(table != "", rdata, 0);

	/*
	AosXmlTagPtr doc = AosGetDocByObjid(table, rdata); 
	aos_assert_r(doc, NULL);                                    
	AosXmlTagPtr tag = doc->getFirstChild("statistics");         
	aos_assert_r(tag, NULL);
	AosXmlTagPtr stat= tag->getFirstChild("statistic");      
	aos_assert_r(stat, NULL);
	OmnString stat_table = stat->getAttrStr("stat_objid", "");       
	aos_assert_r(stat_table != "", NULL);

	AosXmlTagPtr stat_def_xml = AosGetDocByObjid(stat_table, rdata);
	aos_assert_rr(stat_def_xml, rdata, 0);
	*/
	AosXmlTagPtr stat_def_xml = AosGetDocByObjid(table, rdata); 

	AosStatDefineDocPtr stat_doc = OmnNew AosStatDefineDoc();
	bool rslt = stat_doc->config(stat_def_xml);
	aos_assert_r(rslt, 0);
	return stat_doc;
}

	
bool
AosStatQueryAnalyzer::generateRslt(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req,
		OmnString &content)
{
	u64 t1 = OmnGetTimestamp(); 
	
	aos_assert_r(mPattern1, false);
		
	AosStatQueryUnitPtr stat_unit = req->getStatQueryUnit();
	aos_assert_r(stat_unit, false);

	bool rslt = mPattern1->generateRslt(rdata, stat_unit, content);
	aos_assert_r(rslt, false);
	
	OmnScreen << content << endl;
	
	OmnScreen << "!!!!!!!! get query rslt time:" << OmnGetTimestamp() - t1 << endl;
	return true;
}


/*
bool
AosStatQueryAnalyzer::analyzeReq(
		const AosRundataPtr &rdata,
		const AosQueryReqObjPtr &req)
{
	AosXmlTagPtr stat_def_doc = getStatDefDoc(rdata, req);
	aos_assert_rr(stat_def_doc, rdata, false);

	vector<AosQrUtil::FieldDef> & select_fields = req->getSelectedFields();
	aos_assert_r(select_fields.size() != 0, false);

	u32 level_num = 0;
	int pattern_num = findPattern(select_fields, stat_def_doc, level_num);
	aos_assert_r(level_num, false);
	
	switch (pattern_num)
	{
	case 1:
		 rslt = procPattern001(rdata, req, select_fields, stat_def_doc);	
		 aos_assert_r(rslt, false);
		 return true;

	case 2:
		 rslt = procPattern002(rdata, req, stat_def_doc);	
		 aos_assert_r(rslt, false);
		 return true;

	default:
		 break;
	}
	
	AosSetErrorUser(rdata, "statqueryanalyzer_unrecog_stat_query")
		<< enderr;
	return false;
}
	

int
AosStatQueryAnalyzer::findPattern(
		const vector<AosQrUtil::FieldDef> & select_fields,
		const AosXmlTagPtr &stat_def_doc, 
		u32 &level_num)
{
	
	// 1. Determine number of levels
	vector<OmnString> key_fields, value_fields, selected_value_fields;
	vector<int> levels;
	bool rslt = getFieldNamesFromStatDoc(rdata, key_fields, value_fields, levels);

	for (u32 i=0; i<select_fields.size(); i++)
	{
		OmnString fname = select_fields[i].fname;
		if (fname in value_fields)
		{
			selected_value_fields.push_back(fname);
		}
	}

	u32 level_num = selected_value_fields.size();

	if (level_num == 1)
	{
		if (hasSubquery(xxx))
		{
			return 3;
		}

		return 1;
	}

	if (level_num == 2)
	{
		return 2;
	}

	return -1;
}

*/


