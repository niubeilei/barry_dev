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
// 2015/04/23 Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicIndex2.h"

#include "JimoAPI/JimoParserAPI.h"
#include "Rundata/Rundata.h"

AosJLIndex2::AosJLIndex2(
		const AosRundataPtr &rdata,
		const OmnString &input_name,
		//const AosXmlTagPtr &def,
		const OmnString &tablename,
		const AosJimoProgObjPtr &jimo_prog,
		const AosXmlTagPtr &table_doc,
		const bool prime)
:
AosJimoLogic(1),
mInput(input_name),
mTableName(tablename),
mTableDoc(table_doc),
mJimoProg(jimo_prog)
{
	//aos_assert(def);
	mTableName = tablename;
	//mIdxName = def->getAttrStr("zky_name");

	mInput = input_name;

	//AosXmlTagPtr conds = def->getFirstChild("conds");

	//if (conds){
	//	mCondsStr = conds->getNodeText();
	//}


//	AosXmlTagPtr fields = def->getFirstChild("columns");
//	if (fields)
//	{
//		AosXmlTagPtr field = fields->getFirstChild(true);
//		while (field)
//		{
//			mKeys.push_back(field->getAttrStr("zky_name"));
//			field = fields->getNextChild();
//		}
//	}

	bool rslt;
	if (checkIsVirtualTable(rdata))
	{
		rslt = createJsonStrForMutilTable(rdata);
	}
	else
	{
		//rslt = createJsonStr(rdata, mKeys, "");
		rslt = createJsonStr(rdata, "");
	}

	if (!rslt)
	{
		OmnAlarm << "error ." << enderr;
	}
}

AosJLIndex2::AosJLIndex2(const AosJLIndex2 &rhs)
:
AosJimoLogic(rhs.mJimoVersion),
//mIdxName(rhs.mIdxName),
mInput(rhs.mInput),
mCondsStr(rhs.mCondsStr),
mTableName(rhs.mTableName),
mKeys(rhs.mKeys),
mTableDoc(rhs.mTableDoc),
mJimoProg(rhs.mJimoProg)
{
}

bool 
AosJLIndex2::createJsonStr(
		const AosRundataPtr &rdata,
		//const vector<OmnString> &keys,
		const OmnString &record_name)
{
	OmnString str;
	OmnString jobName = mJimoProg->getJobname();
	str << "dataproc index " << jobName << "_dp_index_" << mTableName << "(";

	if (mTableName != "") {
		str << "table : \"" << mTableName << "\",";
	}

	//str << " keyfields : [";
	//for (u32 i = 0; i < keys.size(); i++) {
	//	if (i != 0) 
	//		str << ",";

	//	str << " \"" << keys[i] << "\" ";
	//}

	//str << "],";

	//if (keys.size() == 1)
	//{
	//	OmnString type;
	//	AosXmlTagPtr colDoc = 0;
	//	AosXmlTagPtr doc = mTableDoc;
	//	doc = doc->getFirstChild("columns");
	//	aos_assert_r(doc, false);
	//	colDoc = doc->getFirstChild("column");
	//	aos_assert_r(colDoc, false);
	//	while (colDoc)
	//	{ 
	//		if (keys[0] == colDoc->getAttrStr("name"))
	//		{
	//			type = colDoc->getAttrStr("datatype"); 
	//			if (type == AOSDATATYPE_U64)
	//			{
	//				str << "type : " << "\"indexu64\",";
	//			}
	//			break;
	//		}                                                                                 
	//		colDoc = doc->getNextChild();
	//	}
	//}
//	if (mCondsStr != "")
//		str << "condition : " << "\"" << mCondsStr << "\",";
//
	if (record_name != "")
		str << "recordname : " << "\"" << record_name << "\",";

	str << "inputs : \"" << mInput << "\"";
	str << ");";

	AosJimoParserObjPtr jimoParser = Jimo::jimoCreateJimoParser(rdata.getPtr());
	aos_assert_rr(jimoParser, rdata.getPtr(), 0);
	vector<AosJqlStatementPtr> statements;
	AosJimoProgObj *prog = mJimoProg.getPtrNoLock();
	jimoParser->parse(rdata.getPtr(), prog, str, statements);

	OmnString ss;
	for (u32 i=0; i<statements.size(); i++)
	{
		statements[i]->setContentFormat("xml");
		statements[i]->run(rdata.getPtrNoLock(), prog, ss, false);
	}                                                                                      
	return true;
}


AosJimoPtr 
AosJLIndex2::cloneJimo() const
{
	return OmnNew AosJLIndex2(*this);
}

AosJLIndex2::AosJLIndex2(int version)
:AosJimoLogic(version)
{
}


AosJLIndex2::~AosJLIndex2()
{
}

bool 
AosJLIndex2::createJsonStrForMutilTable(const AosRundataPtr &rdata)
{
	OmnString record_name;
	OmnString field_name;
	AosXmlTagPtr value_node;

	// map<virtual_field_name, map<record_name, field_name > >
	map<OmnString, map<OmnString, OmnString> > cols;
	map<OmnString, map<OmnString, OmnString> >::iterator itr;
	AosXmlTagPtr cols_node = mTableDoc->getFirstChild("columns");
	if (cols_node)
	{
		AosXmlTagPtr col_node = cols_node->getFirstChild(true);
		while (col_node)
		{
			// map<record_name, field_name>
			map<OmnString, OmnString> values;
			field_name = col_node->getAttrStr("name");
			cols.insert(make_pair(field_name, values));
			value_node = col_node->getFirstChild(true);
			while (value_node)
			{
				record_name = value_node->getAttrStr("recordname");
				field_name = value_node->getAttrStr("fieldname"); 
				values.insert(make_pair(record_name, field_name));
				value_node = col_node->getNextChild();
			}
			col_node = cols_node->getNextChild();
		}
	}

	/*
	map<OmnString, OmnString> values;
	map<OmnString, OmnString>::iterator itr2;
	// map<record_name, field_names> 
	map<OmnString, vector<OmnString> > index_keys;
	map<OmnString, vector<OmnString> >::iterator itr3;

	for (u32 i = 0; i < mKeys.size(); i++)
	{
		itr = cols.find(mKeys[i]);
		aos_assert_r(itr != cols.end(), false);
		values = itr->second;
		for (itr2 = values.begin(); itr2 != values.end(); itr2++)
		{
			index_keys[itr2->first].push_back(itr2->second);
		}
	}

	for (itr3 = index_keys.begin(); itr3 != index_keys.end(); itr3++)
	{
		createJsonStr(rdata, itr3->second, itr3->first);
	}
	*/
	return true;
}

bool 
AosJLIndex2::checkIsVirtualTable(const AosRundataPtr &rdata)
{
	return false;
	return mTableDoc->getAttrBool("zky_virtual");
}

