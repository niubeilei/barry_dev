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
// 2015/05/18 Created by Chen Ding
// 2015/05/19 Worked on by Rain
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDatasetTable.h"
#include "JQLStatement/JqlStmtTable.h"

#include "API/AosApi.h"
#include "Util/CodeConvertion.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "SEUtil/JqlTypes.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlQueryWhereConds.h"
#include "XmlUtil/XmlTag.h"
#include <dirent.h>

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDatasetTable_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDatasetTable(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosJimoLogicDatasetTable::AosJimoLogicDatasetTable(const int version)
:
AosJimoLogicDataset(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDatasetTable::~AosJimoLogicDatasetTable()
{
}


bool
AosJimoLogicDatasetTable::parseJQL(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser,
		AosJimoProgObj *prog,
		bool &parsed,
		bool dft)
{
	//Dataset dir <dataset_name>
	//	(
	//	     dirs: [(server_id: <ddd>, dir: “<directory>”)...],
	//		 schema: <schema_name> | (<schema_definition>),
	//		 table: <table_name>,
	//		 splitter: (<splitter_spec>)
	//	);

	//we can get the schema whit table; and if table name is not Null, wo
	//would use table-schema and ignore the schema attribute field
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataset", rdata, false);
	aos_assert_rr(keywords[1] == "table", rdata, false);
	mErrmsg = "dataset dir ";
	// parse dataset name
	OmnString dataset_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataset_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataset_name;

	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError, "",mErrmsg);
		return false;
	}

	mDatasetName = dataset_name;
	mErrmsg = mErrmsg;
	mNameValueList = name_value_list;

	//parse table name
	OmnString attrname = "table";
	OmnString tableName = jimo_parser->getParmStr(rdata, attrname, name_value_list);
    if (tableName == "")
    {
        setErrMsg(rdata,eMissingParm,"table",mErrmsg);
        return false;
    }
	mTableName = tableName;

	//parse save doc
	//if(!configSaveDoc(rdata, jimo_parser, name_value_list, stmt.getPtr()))
	//{
	//	AosLogError(rdata, true, "missing_savedoc_attributes");
	//	return false;
	//}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool
AosJimoLogicDatasetTable::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'.
	// 2.
	aos_assert_rr(prog, rdata, false);
   //AosJimoLogicDatasetDirPtr jimologic = dynamic_cast<AosJqlStatementPtr>(this);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDatasetTable*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDatasetName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}
bool
AosJimoLogicDatasetTable::run(
		AosRundata *rdata,
		AosJimoProgObj* prog,
		//const OmnString &verb_name,
		OmnString &statements_str,
		bool isparser)
{
	bool rslt = createDatasetTableDoc(rdata);
	aos_assert_r(rslt, false);

	return rslt;
}


bool
AosJimoLogicDatasetTable::createDatasetTableDoc(
                AosRundata *rdata)
{
    OmnString objid = "_zt4g_dataset_";
    objid << mDatasetName;
    OmnString ds_str = "<dataset></dataset>";
    AosXmlTagPtr ds_xml = AosStr2Xml(rdata, ds_str AosMemoryCheckerArgs);
    aos_assert_r(ds_xml, false);
    ds_xml->setAttr("jimo_objid", "dataset_byreaddoc_jimodoc_v0");
    ds_xml->setAttr("zky_name", mDatasetName);
    ds_xml->setAttr("zky_objid",objid);
    ds_xml->setAttr("zky_otype","dataset");

    // node datascanner
    OmnString dscan_str = "<datascanner></datascanner>";
    AosXmlTagPtr dscan_xml = AosStr2Xml(rdata, dscan_str AosMemoryCheckerArgs);
    aos_assert_r(dscan_xml, false);
    dscan_xml->setAttr("jimo_objid", "datascanner_cube_jimodoc_v0");
    dscan_xml->setAttr("zky_name", mDatasetName);

    //get table doc
	AosRundataPtr rdataPtr(rdata);
	AosXmlTagPtr table_doc = AosJqlStatement::getDoc(rdataPtr,JQLTypes::eTableDoc,mTableName);
	if (!table_doc)
	{
		setErrMsg(rdata,eNotExist,mTableName,mErrmsg);
		return false;
	}
    //OmnString iilname = AosJqlQueryWhereConds::generateDefaultTermConf(rdataPtr,table_doc,mTableName);
    OmnString dbname = table_doc->getAttrStr("zky_database");
    OmnString iilname ;
    iilname << "_zt44_" << dbname << "_" << mTableName;


    OmnString dscon_str = "<dataconnector></dataconnector>";
    AosXmlTagPtr dscon_xml = AosStr2Xml(rdata, dscon_str AosMemoryCheckerArgs);
    aos_assert_r(dscon_xml, false);
    dscon_xml->setAttr("jimo_objid", "dataconnector_iil_jimodoc_v0");
    dscon_xml->setAttr("zky_iilname", iilname);
    dscon_xml->setAttr("zky_with_key","false");

    OmnString query_cond_str = "<query_cond ";
    query_cond_str << "zky_opr =" << "\"an\"" << ">";
    query_cond_str << "</query_cond>";
    AosXmlTagPtr query_cond_xml = AosStr2Xml(rdata, query_cond_str AosMemoryCheckerArgs);

    OmnString value_str = "<zky_value>";
    value_str << "*" << "</zky_value>";
    AosXmlTagPtr value_xml = AosStr2Xml(rdata, value_str AosMemoryCheckerArgs);

    query_cond_xml->addNode(value_xml);
    dscon_xml->addNode(query_cond_xml);

    OmnString split_str = "<split ";
    split_str << "jimo_objid =" << "\"dataspliter_iilbytask_jimodoc_v0\""
          << "/>";
     AosXmlTagPtr split_xml = AosXmlParser::parse(split_str AosMemoryCheckerArgs);

    dscon_xml->addNode(split_xml);
    dscan_xml->addNode(dscon_xml);

    // dataset node
    ds_str = "<dataset></dataset>";
    AosXmlTagPtr ds1_xml = AosStr2Xml(rdata, ds_str AosMemoryCheckerArgs);
    aos_assert_r(ds1_xml, false);
    ds1_xml->setAttr("jimo_objid", "dataset_bydatascanner_jimodoc_v0");
    ds1_xml->setAttr("zky_name", mDatasetName);

    dscan_str = "<datascanner></datascanner>";
    AosXmlTagPtr dscan1_xml = AosStr2Xml(rdata, dscan_str AosMemoryCheckerArgs);
    aos_assert_r(dscan1_xml, false);
    dscan1_xml->setAttr("jimo_objid", "datascanner_cube_jimodoc_v0");
    dscan1_xml->setAttr("zky_name", mDatasetName);

    dscon_str = "<dataconnector></dataconnector>";
    AosXmlTagPtr dscon1_xml = AosStr2Xml(rdata, dscon_str AosMemoryCheckerArgs);
    aos_assert_r(dscon1_xml, false);
    dscon1_xml->setAttr("jimo_objid", "dataconnector_readdoc_bysort_jimodoc_v0");
    dscon1_xml->setAttr("doc_type", "common_doc");

    OmnString  schema_objid = table_doc->getAttrStr("zky_use_schema","");
    aos_assert_rr(schema_objid != "", rdata, false);
    AosXmlTagPtr schema_doc = AosGetDocByObjid(schema_objid, rdataPtr);
    aos_assert_rr(schema_doc, rdata, false);

    dscan1_xml->addNode(dscon1_xml);
    ds1_xml->addNode(dscan1_xml);
    ds1_xml->addNode(schema_doc);

    ds_xml->addNode(dscan_xml);
    ds_xml->addNode(ds1_xml);

    OmnScreen << ds_xml->toString() << endl;
    AosXmlTagPtr new_doc = AosCreateDoc(ds_xml->toString(), true, rdata);
	if(!new_doc)
	{
		OmnString errmsg = "failed create doc";
		rdata->setJqlMsg(errmsg);
		aos_assert_r(new_doc, false);
		return false;
	}
    return true;
}




AosJimoPtr
AosJimoLogicDatasetTable::cloneJimo() const
{
	return OmnNew AosJimoLogicDatasetTable(*this);
}

/*
bool
AosJimoLogicDatasetTable::getOutputName(
		        AosRundata *rdata,
				AosJimoProgObj *prog,
		        OmnString &outputName)
{
	outputName = mDatasetName;
	return true;
}
*/
//this is for JimoLogicNew::getDatasetOutput

