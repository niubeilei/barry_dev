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
// 08/08/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JimoLogic/JimoLogicStatistic2.h"

#include "JimoAPI/JimoParserAPI.h"

AosJLStatistic2::AosJLStatistic2(
		const AosRundataPtr &rdata,
		const string &input_name,
//		const AosXmlTagPtr &def,
		const OmnString &table_name,
		const AosJimoProgObjPtr &jimo_prog)
//		const AosXmlTagPtr table_doc)
:
AosJimoLogic(1),
mInput(input_name),
mTableName(table_name),
mJimoProg(jimo_prog)
//mTableDoc(table_doc)
{
/*	
	if (def)
	{

		AosXmlTagPtr conds = def->getFirstChild("cond");

		if (conds){
			mCondsStr = conds->getNodeText();
		}

		mStatName = def->getAttrStr("zky_stat_name");
		mStatName.setLength(mStatName.length()-11);

		OmnString tmp = "";
		tmp << table_name  << "_stat_";
		int idx = mStatName.indexOf(tmp, 0);

		if (idx >= 0)
			mStatName = mStatName.substr(tmp.length()+idx);

		mStatModelName = mStatName;
		AosXmlTagPtr key_fields = def->getFirstChild("key_fields");
		if (key_fields)
		{
			AosXmlTagPtr key_field = key_fields->getFirstChild(true);
			while (key_field)
			{
				mKeys.push_back(key_field->getAttrStr("field_name"));
				key_field = key_fields->getNextChild();
			}
		}

		OmnString str;
		AosXmlTagPtr measures = def->getFirstChild("measures");
		if (measures)
		{
			AosXmlTagPtr measure = measures->getFirstChild(true);
			while (measure)
			{
				str = measure->getAttrStr("zky_name");
				AosJqlUtil::unescape(str);
				mMeasures.push_back(str);
				measure = measures->getNextChild();
			}
		}

		AosXmlTagPtr time_field = def->getFirstChild("time_field");
		if(time_field)
		{
			mTimeField = time_field->getAttrStr("field_name");
			mTimeFormat = time_field->getAttrStr("time_format");
			mTimeUnit = time_field->getAttrStr("grpby_time_unit");
		}
		
		//2015/5/18
		string stat_name = def->getAttrStr("zky_stat_name");
		unsigned found   = stat_name.find_last_of('_');
		string stat_index = stat_name.substr(found+1);
		if(stat_index != "")
			mStatIdx = stat_index;

	}
*/
	bool rslt;
	if (checkIsVirtualTable(rdata))
	{
		rslt = createJsonStrForMutilTable(rdata);
	}
	else
	{
		rslt = createJsonStr(rdata, mKeys, mMeasures, "");
	}
	if (!rslt)
	{
		OmnAlarm << "error ." << enderr;
	}
}

AosJLStatistic2::AosJLStatistic2(const AosJLStatistic2 &rhs)
:
AosJimoLogic(rhs.mJimoVersion),
mStatName(rhs.mStatName),
mInput(rhs.mInput),
mTableName(rhs.mTableName),
mKeys(rhs.mKeys),
mMeasures(rhs.mMeasures),
mJimoProg(rhs.mJimoProg),
mTableDoc(rhs.mTableDoc),
mCondsStr(rhs.mCondsStr)
{
}


AosJLStatistic2::AosJLStatistic2(int version)
:AosJimoLogic(version)
{
}


AosJLStatistic2::~AosJLStatistic2()
{
}


bool
AosJLStatistic2::createJsonStr(
		const AosRundataPtr &rdata,
		const vector<OmnString> &keys,
		const vector<OmnString> &measures,
		const OmnString &record_name)
{
	OmnString str;
	OmnString statname = "mystat";
	str << "dataproc statistics " << statname << "(";

	if (mTableName != "") {
		str << "table : \"" << mTableName << "\",";
	}

	str << "inputs : \"" << mInput << "\"";
/*
	if (mStatModelName != "")
		str << "," << "stat_model :\"" << mStatModelName << "\"";
	if (mStatIdx != "")
		str << "," << "stat_index :\"" << mStatIdx << "\"";
*/
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
		statements[i]->run(rdata.getPtrNoLock(), prog , ss, false);
	}
	return true;
}

bool
AosJLStatistic2::createJsonStrForMutilTable(const AosRundataPtr &rdata)
{
	OmnString record_name;
	OmnString field_name;
	AosXmlTagPtr value_node;
	map<OmnString, vector<OmnString> > stat_measures;

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
				if (stat_measures.count(record_name) == 0)
				{
					stat_measures.insert(make_pair(record_name, mMeasures));
				}
				values.insert(make_pair(record_name, field_name));
				value_node = col_node->getNextChild();
			}
			col_node = cols_node->getNextChild();
		}
	}

	map<OmnString, OmnString> values;
	map<OmnString, OmnString>::iterator itr2;
	// map<record_name, field_names>
	map<OmnString, vector<OmnString> > stat_keys;
	map<OmnString, vector<OmnString> >::iterator itr3;
	map<OmnString, vector<OmnString> >::iterator itr4;

	for (u32 i = 0; i < mKeys.size(); i++)
	{
		itr = cols.find(mKeys[i]);
		aos_assert_r(itr != cols.end(), false);
		values = itr->second;
		for (itr2 = values.begin(); itr2 != values.end(); itr2++)
		{
			stat_keys[itr2->first].push_back(itr2->second);
			itr4 = stat_measures.find(itr2->first);
			aos_assert_r(itr4 != stat_measures.end(), false);
			for (u32 j = 0; j < itr4->second.size(); j++)
			{
				itr4->second[j].replace(mKeys[i], itr2->second, true);
			}
		}
	}

	for (itr3 = stat_keys.begin(); itr3 != stat_keys.end(); itr3++)
	{
		itr4 = stat_measures.find(itr3->first);
		aos_assert_r(itr4 != stat_measures.end(), false);
		createJsonStr(rdata, itr3->second, itr4->second, itr3->first);
	}
	return true;
}

bool
AosJLStatistic2::checkIsVirtualTable(const AosRundataPtr &rdata)
{
	return false;
	return mTableDoc->getAttrBool("zky_virtual");
}

