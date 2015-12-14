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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtUnion.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "JQLExpr/Expr.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

using namespace AosConf;

AosJqlStmtUnion::AosJqlStmtUnion()
{
	mIsUnionAll = false;
}


AosJqlStmtUnion::~AosJqlStmtUnion()
{
}


bool
AosJqlStmtUnion::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	bool rslt = false;
	OmnString str = "<query opr=\"retlist\"  queryid = \"10\"></query>";
	AosXmlTagPtr query_conf = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs); 
	rslt = generateQueryConf(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	query_conf->addNode(mTableDoc);


	OmnCout << "====================== New Query Config" << endl;
	OmnCout << query_conf->toString() << endl;
	OmnCout << "====================== New Query Config" << endl;

	AosXmlTagPtr data = executeQuery(rdata, query_conf);
	if ( !data )
	{                                                      
		OmnScreen << "====================== New Query Error" << endl;
		return false;                                      
	}                                                      

	OmnScreen << "====================== New Query Data" << endl;
	OmnScreen << data->toString() << endl;
	OmnScreen << "====================== New Query Data" << endl;

	rslt = output_data(rdata, data);
	return rslt;
}

bool
AosJqlStmtUnion::generateQueryConf(const AosRundataPtr &rdata)
{
	bool rslt;
	rslt = init(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!mTablesDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if (mUnion)
	{
		rslt = mUnion->generateQueryConf(rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosXmlTagPtr doc = mUnion->getTableDoc();
		if (!doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosXmlTagPtr s_doc = doc->clone(AosMemoryCheckerArgsBegin);
		AosXmlTagPtr tables = s_doc->getFirstChild(true);
		if (!tables)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosXmlTagPtr table = tables->getFirstChild("table");
		while (table)
		{
			mTablesDoc->addNode(table);
			table = tables->getNextChild();
		}
	}

	AosXmlTagPtr tb;
	AosXmlTagPtr table_doc;
	for (u32 i = 0; i < mSelects.size(); i++)
	{
		rslt = mSelects[i]->generateQueryConf(rdata, mTablesDoc);
		if (!rslt )
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		table_doc = mTablesDoc->getLastChild();
		if (!table_doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		table_doc->setAttr("union_all", mIsUnionAll);

		if (!tb)
		{
			tb = mTablesDoc->getFirstChild(true);
			AosXmlTagPtr fields = tb->getFirstChild("fnames");
			if (!fields)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}

			fields = fields->clone(AosMemoryCheckerArgsBegin);

			AosXmlTagPtr fname = fields->getFirstChild(true);
			AosXmlTagPtr cname_tag, oname_tag;
			while (fname)
			{
				cname_tag = fname->getFirstChild("cname");
				oname_tag = fname->getFirstChild("oname");
				aos_assert_r(cname_tag && oname_tag, false);
				// setNodeText(OmnString value, bool isCdata);
				oname_tag->setNodeText(cname_tag->getNodeText(), true);
				fname = fields->getNextChild();
			}
			AosXmlTagPtr output = tb->getFirstChild("outputrecord");
			if (!output)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}

			output = output->clone(AosMemoryCheckerArgsBegin);
			mTableDoc->addNode(fields);
			mTableDoc->addNode(output);
		}
	}

	return true;
}

bool
AosJqlStmtUnion::init(const AosRundataPtr &rdata)
{
	bool rslt;
	mTableDoc = AosStr2Xml(rdata.getPtr(), "<table><tables></tables></table>" AosMemoryCheckerArgs);
	mTableDoc->setAttr("zky_type", "union");
	mTableDoc->setAttr("jimo_name", "jimo_dataset");
	mTableDoc->setAttr("jimo_type", "jimo_dataset");
	mTableDoc->setAttr("current_version", "0");
	mTableDoc->setAttr("zky_otype", "zkyotp_jimo");
	mTableDoc->setAttr("zky_objid", "AosDatasetByQueryUnion");
	mTableDoc->setAttr("zky_classname", "AosDatasetByQueryUnion");
	OmnString ver_str = "<versions><ver_0>libDatasetJimos.so</ver_0></versions>";
	AosXmlTagPtr ver = AosXmlParser::parse(ver_str AosMemoryCheckerArgs); 
	mTableDoc->addNode(ver); 
	mTablesDoc = mTableDoc->getFirstChild("tables");
	if (!mTablesDoc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	for (u32 i = 0; i < mSelects.size(); i++)
	{
		if (!mSelects[i])
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		rslt = mSelects[i]->init(rdata);
		if (!rslt )
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}
	return true;
}

bool
AosJqlStmtUnion::output_data(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &data)
{
	if (!data)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString v, name; 
	vector<OmnString> fieldnames;
	AosJqlSelectFieldPtr field; 
	AosJqlSelectFieldList *fields = mSelects[0]->mFieldListPtr; 
	if (!fields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}


	OmnString names; 
	for(u32 i = 0; i < fields->size(); i++)
	{
		field = (*fields)[i];
		if (!field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		name = field->getFieldEscapeCName(rdata);
		if (name == "")
			name == field->getFieldEscapeOName(rdata);

		i == 0 ? names << name : names << "," << name;
		fieldnames.push_back(name);
	}
	OmnString msg = printTableInXml(fieldnames, mSelects[0]->mNameToAlias, data);  
	if (mContentFormat == "xml")
	{
		data->setNodeName("content");
		data->setAttr("fieldnames", names);
		rdata->setResults(data->toString());
		return true;
	}
	setPrintData(rdata, msg);
	return true;
}


AosJqlStatement *
AosJqlStmtUnion::clone()
{
	return 0;
}

void 
AosJqlStmtUnion::dump()
{
}

