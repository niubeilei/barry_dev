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
// 2014/04/23	Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtStatistics.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprFieldName.h"
#include "JQLExpr/ExprGenFunc.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLStatement/JqlStmtTable.h"
#include "Query/Ptrs.h"                          
#include "Rundata/Rundata.h"
#include "SEBase/SeUtil.h"                       
#include "SEUtil/Docid.h"                        
#include "SEUtil/DocTags.h"                      
#include "SEUtil/JqlUtil.h"                   
#include "SEUtil/Objid.h"                        
#include "SEUtil/SysLogName.h"                   
#include "SEUtil/UrlMgr.h"                       
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"                        

AosJqlStmtStatistics::AosJqlStmtStatistics(const OmnString &errmsg)
:
mErrmsg(errmsg)
{

}

AosJqlStmtStatistics::AosJqlStmtStatistics()
{
	mTableName = "";
	mStatName = "";
	//mKeyFields = 0; 
	mStatValues = 0;
	mStartIdx = 0;  
	mUpdateDimensions = 0;
	//mDimensions = 0;
	mGroupByFields = 0;
	mTableNames = 0;
	mWhereCond = 0;
	mMeasures = 0;
	mHasInsertDistinct = false;

	// Ketty 2014/11/07
	//mDistKeyFields = 0;
	mDistStatValues = 0;
	mNormStatValues = 0;
	mShuffleFields = 0;
}

AosJqlStmtStatistics::~AosJqlStmtStatistics()
{
	if (mGroupByFields)
	{
		for(size_t i=0; i<mGroupByFields->size(); i++)
		{
			OmnDelete (*mGroupByFields)[i];
		}
		OmnDelete mGroupByFields;
	}
	
	if (mUpdateDimensions)
	{
		for(size_t i=0; i<mUpdateDimensions->size(); i++)
		{
			OmnDelete (*mUpdateDimensions)[i];
		}
		OmnDelete mUpdateDimensions;
	}

	if (mStatValues)
	{
		OmnDelete mStatValues;
	}

	if (mWhereCond)
	{
		delete mWhereCond;
	}

	if(mTableNames)
	{
		OmnDelete mTableNames;
	}

	if(mMeasures)
	{
		OmnDelete mMeasures;
	}
	
	// Ketty 2014/11/07
	//if(mDistKeyFields)
	//{
	//	OmnDelete mDistKeyFields;
	//}

	if(mDistStatValues)
	{
		OmnDelete mDistStatValues;	
	}
	
	if(mNormStatValues)
	{
		OmnDelete mNormStatValues;
	}
	
	if (mShuffleFields)
	{
		OmnDelete mShuffleFields;
		mShuffleFields = NULL;
	}
}


bool
AosJqlStmtStatistics::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	// 2015/7/28 JIMODB-77
	bool rslt = checkDatabase(rdata);
	if (!rslt) return false;

	if (mOp == JQLTypes::eCreate) return createStatisticsNew(rdata);
	if (mOp == JQLTypes::eShow) return showStatistics(rdata);
	if (mOp == JQLTypes::eDescribe) return describeStatistics(rdata);
	if (mOp == JQLTypes::eUpdate) return updateStatistics(rdata);
	if (mOp == JQLTypes::eDrop) return dropStatistics(rdata);

	AosSetEntityError(rdata, "JQL_index_run_err", "JQL Statistics", "")      
		        << "opr is undefined!" << enderr;                               
	return false;                                                           
}

bool 
AosJqlStmtStatistics::checkUpdateConfig(
		const AosXmlTagPtr &internal_statistics, 
		const AosRundataPtr &rdata)
{
	if (!mUpdateDimensions)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString msg;
	mDimensions.clear();
	bool rslt;
	for(size_t i=0; i<mUpdateDimensions->size(); i++)
	{
		AosExprSet group;
		rslt = checkNoRepeatFields((*mUpdateDimensions)[i], group, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		mDimensions.push_back(group);
	}
	
	return true;
}
    
bool
AosJqlStmtStatistics::showStatistics(const AosRundataPtr &rdata)
{
	vector<AosXmlTagPtr> statistics;
	if ( !getDocs(rdata, JQLTypes::eStatisticsDoc, statistics) )
	//if ( !getDocByObjid(mStatName) )
	{
		OmnString error;
		error << "No statistics found";
		rdata->setJqlMsg(error);
		return true;
	}
	 
	int total = statistics.size(); 
	OmnString content;
	content << "<content total=\"" << total << "\">";
	for(u32 i=0; i<statistics.size(); i++)
	{
		OmnString stat_name = statistics[i]->getAttrStr("zky_stat_name", "");
		OmnString table_name = statistics[i]->getAttrStr("zky_table_name", "");
		//content << "<record zky_statname=\"" << stat_name << "\" zky_tablename=\"" << table_name << "\"/>";
		content << "<record zky_statname=\"" << stat_name << "\" zky_tablename=\"" << table_name << "\"/>";
	}

	content << "</content>";
	rdata->setResults(content);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(content.data(), content.length(), "" AosMemoryCheckerArgs);
	if (mContentFormat == "xml")
	{
		rdata->setResults(doc->toString());
		return true;
	}
	vector<OmnString> fields;
	fields.push_back("zky_statname");
	fields.push_back("zky_tablename");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_statname"] = "StatName";
	alias_name["zky_tablename"] = "TableName";

    OmnString msg = printTableInXml(fields, alias_name, doc);
	setPrintData(rdata, msg);
	return true;
}


bool
AosJqlStmtStatistics::describeStatistics(const AosRundataPtr &rdata)
{
	OmnString msg = "";
	if (mStatName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	} 
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eStatisticsDoc, mStatName);
	AosXmlTagPtr doc = getDocByObjid(rdata, objid);
	if (!doc)
	{
		msg << "Failed to display statistics " << mStatName << " : not found.";
		rdata->setJqlMsg(msg);
		return false;
	}

	OmnString content = "<content>";
    OmnString stat_name =doc->getAttrStr("zky_stat_name", "");
	OmnString stat_identify_key =doc->getAttrStr("zky_stat_identify_key", "");
	//AosXmlTagPtr measuresNode = doc->getFirstChild("measures");
    //AosXmlTagPtr measureNode = measuresNode->getFirstChild("measure");
	/*while (measureNode)                                        
	{
		    // get attr
			measureNode = measuresNode->getNextChild("measure");
     }*/
	content << "<record zky_statname=\"" << stat_name << "\" zky_stat_identify_key=\"" <<stat_identify_key << "\"/>";

	vector<OmnString> fields;
	fields.push_back("zky_statname");
	fields.push_back("zky_stat_identify_key");
	//fields.push_back("zky_update_time");
	//fields.push_back("zky_keys");
	//fields.push_back("zky_timefield");
	//fields.push_back("zky_measures");

	map<OmnString, OmnString> alias_name;
	alias_name["zky_statname"] = "StatName";
	alias_name["zky_stat_identify_key"] = "Identify_Key";
	//alias_name["zky_update_time"] = "Update_Time";
	//alias_name["zky_keys"] = "Keys";
	//alias_name["zky_timefield"] = "TimeField";
	//alias_name["zky_measures"] = "Measures";
	content << doc->toString() << "</content>";

	if (mContentFormat == "xml")
	{
		    rdata->setResults(content);
			    return true;
	}

	AosXmlTagPtr def = AosXmlParser::parse(content AosMemoryCheckerArgs);
	msg = printTableInXml(fields, alias_name, def);
	setPrintData(rdata, msg);
	return true;

}

bool
AosJqlStmtStatistics::updateStatistics(const AosRundataPtr &rdata)
{
	if (!mUpdateDimensions)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString msg;
	AosXmlTagPtr stat_doc = getDocByObjid(rdata, mStatName);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

    AosXmlTagPtr internal_statistics = stat_doc->getFirstChild("internal_statistics");	
	if (!internal_statistics)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	bool rslt = checkUpdateConfig(internal_statistics, rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if (!mDimensions.size() > 0)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	mStartIdx = internal_statistics->getNumSubtags();	

	AosXmlTagPtr xml = internal_statistics->getFirstChild("statistic");
	if (!xml)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr measures = xml->getFirstChild("measures");
	if (!measures)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	xml = measures->getFirstChild("measure");
	if (!xml)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if (!!mStatValues)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	mStatValues = new AosExprList;

	AosExprObjPtr expr;
	while(xml)
	{
		AosXmlTagPtr tag = xml->getFirstChild("measure_field");
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		OmnString field_name = tag->getAttrStr("field_name"); 

		tag = xml->getFirstChild("agr_type");
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		OmnString type = tag->getAttrStr("type");
		if (type == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		type << "(" << field_name << ");";
		// Chen Ding, 2015/01/31
		// expr = AosParseJQL(type, true, rdata.getPtr());                         
		OmnString errmsg;
		expr = AosParseExpr(type, errmsg, rdata.getPtr());                         
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "Errmsg: " << errmsg
				<< "CondExpr AosParseJQL ERROR:: " << type << enderr;
			return false;
		} 
		mStatValues->push_back(expr->cloneExpr());
		xml = measures->getNextChild();
	}

	OmnString tag_str;
	AosXmlTagPtr tag;
	for(size_t i=0; i<mDimensions.size(); i++)
	{
		tag_str = createStat(mDimensions[i], rdata);
		if (tag_str == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		tag = AosXmlParser::parse(tag_str AosMemoryCheckerArgs);	
		if (!tag)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		internal_statistics->addNode(tag);
	}

	if ( !modifyDoc(rdata, stat_doc) )
	{
		msg << "Failed to modify Statistucs doc. "; 
		rdata->setJqlMsg(msg);
		return false;
	}

cout << " ===================stat_doc============================= " << endl;
cout << stat_doc->toString() << endl;
cout << " ======================================================== " << endl;
	msg << "Statistics " << mStatName << " modified successfully.";
	rdata->setJqlMsg(msg);
	
	return true;
}


bool
AosJqlStmtStatistics::dropStatistics(const AosRundataPtr &rdata)
{
	//JIMODB-163 drop stat by Levi
	vector<AosXmlTagPtr> statistics;
	if ( !getDocs(rdata, JQLTypes::eStatisticsDoc, statistics) )
	{
		OmnString error;
		error << "No statistics found";
		rdata->setJqlMsg(error);
		return true;
	}
	 
	int total = statistics.size(); 

	string objid = "";
	vector<OmnString> valList;
	for (int i=0; i<total; i++)
	{
		OmnString name = statistics[i]->getAttrStr("zky_stat_name");
		int num = AosSplitStr(name, "_", valList, 10);
		aos_assert_r(num > 0, false);

		if(valList[3] == mStatName)
		{
			objid = statistics[i]->getAttrStr("zky_objid");
			break;
		}
	}
	
	OmnString msg;
	bool rslt = deleteDocByObjid(rdata, objid);
	if (!rslt)
	{                                                       
		msg << "Failed to delete statistics doc!";
		rdata->setJqlMsg(msg);
		return false;                                       
	}

	msg << "Drop statistics " << mStatName << " scessfullly.";      
	rdata->setJqlMsg(msg);                                  

	return true;  
}


bool
AosJqlStmtStatistics::createStatistics(const AosRundataPtr &rdata)
{
	bool rslt = checkConfig(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if ( getDocByObjid(rdata, mStatName) )
	{
		OmnString error;
		error << mStatName << " doc already exists!";
		rdata->setJqlMsg(error);
		return true;
	}
	rslt = addNodeInTable(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr doc = createStatDoc(rdata);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	doc->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");

	OmnString msg = "";
	if ( !createDocByObjid(rdata, doc, mStatName) )
	{
		msg << "Failed to create Statistics doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
cout << " ===================stat_doc============================= " << endl;
cout << doc->toString() << endl;
cout << " ======================================================== " << endl;
	msg << "Statistics model" << mStatName << " created successfully.";
	rdata->setJqlMsg(msg);

	OmnShouldNeverComeHere;
	//if ( !refactorInternalStatConf(mStatisticDoc, mStatIdentifyKey) )
	//{
	//	msg << "Failed to create internal doc.";
	//	rdata->setJqlMsg(msg);
	//	return false;
	//}

	return rslt;
}


AosXmlTagPtr
AosJqlStmtStatistics::createStatDoc(const AosRundataPtr &rdata)
{
	OmnString name, from_name;
	OmnString msg;

	if(mTimeUnit == "") mTimeUnit = "_day";
	OmnString target_format = "";
	if(mTimeFieldName != "")
	{
		if (mTimeOriginalFormat == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		if(mTimeUnit == "_day")
		{
			target_format = "epochday";
		}
		else if(mTimeUnit == "_month")
		{
			target_format = "epochmonth";
		}
		else if(mTimeUnit == "_year")
		{
			target_format = "epochyear";
		}
		else
		{
			msg << "Invalid time unit : " << mTimeUnit;
			rdata->setJqlMsg(msg);
			return 0;
		}
	}
	
	if(mTimeFieldName == "")
	{
		name = "zky_time_field";
	}
	else
	{
		name = mTimeFieldName;
	}

	from_name = mTimeFieldName;
	AosJqlUtil::escape(name);
	AosJqlUtil::escape(from_name);
	
	OmnString distinct_field = "";
	OmnString has_distinct = "false";
	bool is_distinct = isDistinct(mKeyFields, distinct_field, rdata);
	if (is_distinct)
	{
		if (distinct_field == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		has_distinct = "true";
	}

	OmnString str;
	str << "<statistic internal_id=\"" << mStartIdx++ << "\" "
		<< 		"has_distinct=\"" << has_distinct << "\" "
		<<		"time_unit=\"" << mTimeUnit << "\" "
		<< 		"time_field_name=\"" << name << "\" "
		<< 		"zky_time_from_field=\"" << from_name << "\" "
		<< 		"zky_targettmfmt=\"" << target_format << "\" "
		<< 		"zky_origtmfmt=\"" << mTimeOriginalFormat << "\">"
		<<	"<key_fields>"
		<< 		createKeyFieldsStr(rdata, mKeyFields, mStatIdentifyKey)
		<<  "</key_fields>"
		<<  "<measures>"
		<< 		createStatValueStr(rdata, mStatValues)
		<<  "</measures>"
		//felicia, 2014/10/09, for ketty
		<< "<time_field time_unit=\"" << mTimeUnit << "\" "
		//<< "<time_field time_unit=\"" << mTimeOriginalFormat << "\" "
		//<< 	    "grpby_time_unit=\"" << mTimeUnit << "\" "
		<< 		"time_field_name=\"" << from_name << "\" />"
		<< "<stat_key_conn />"
		<< "<vt2d_conn time_bucket_weight=\"1000\" start_time_slot=\"14610\" />"
		<< "</statistic>";
	
	AosXmlTagPtr stat_doc = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	stat_doc->setAttr("zky_stat_identify_key",mStatIdentifyKey);

	return stat_doc;
	/*
	doc << "<sdd time_unit=\"" << mTimeUnit << "\" "
		<< "time_field_name=\"" << name << "\" "
		<< "zky_time_from_field=\"" << from_name << "\" "
		<< "zky_targettmfmt=\"" << target_format << "\" "
		<< "zky_origtmfmt=\"" << mTimeOriginalFormat << "\">"
		<< 	"<vector2d "
		<<		" root_stat_doc_size=\"9596\" "
		<<		" index_size=\"584\" "
		<<		" time_bucket_weight=\"1000\" "
		<<		" start_time_slot=\"14610\" "
		<<		" extension_doc_size=\"8000\"/>"
		<<	"<internal_statistics>"
		<<		createStat(mKeyFields, rdata);

	if(mDimensions.size() > 0)
	{
		for(size_t i=0; i<mDimensions.size(); i++)
		{
			doc << createStat(mDimensions[i], rdata);
		}
	}
	doc	<<	"</internal_statistics>"
		<< "</sdd>";
	AosXmlTagPtr stat_doc = AosStr2Xml(rdata.getPtr(), doc AosMemoryCheckerArgs);
	return stat_doc;
	*/
}


OmnString
AosJqlStmtStatistics::createStat(
		AosExprSet  &key_fields,
		const AosRundataPtr &rdata)
{
	OmnString distinct_field = "";
	OmnString has_distinct = "false";
	bool is_distinct = isDistinct(key_fields, distinct_field, rdata);
	if (is_distinct)
	{
		if (distinct_field == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		has_distinct = "true";
	}

	OmnString str;
	str << "<statistic internal_id=\"" << mStartIdx++ << "\" has_distinct=\"" << has_distinct << "\">"
		<<	"<key_fields>"
		<< 		createKeyFieldsStr(rdata, key_fields, mStatIdentifyKey)
		<<  "</key_fields>"
		<<  "<measures>"
		<< 		createStatValueStr(rdata, mStatValues);
//	if(is_distinct)
//	{
//	 	str	<< 	createDistinctStr(distinct_field, rdata);
//	}
	str	<<  "</measures>"
		<< "</statistic>";
/*	
	if (is_distinct)
	{
		str << "<statistic internal_id=\"" << mStartIdx++ << "\">"
			<<	"<key_fields>"
			<< 		createKeyFieldsStr(key_fields, false, rdata)
			<<  "</key_fields>"
			<<  "<measures>"
			<< 		createStatValueStr(rdata)
		 	<< 		createDistinctStr(distinct_field, rdata)
			<<  "</measures>"
			<< "</statistic>";
	}
*/	
	return str;
}

OmnString
AosJqlStmtStatistics::createDistinctStr(
		const OmnString &distinct_field,
		const AosRundataPtr &rdata)
{
	OmnString str = "";
	OmnString root_control_objid = "stat_root_";
	//OmnString extension_control_objid = "stat_ext_";
	root_control_objid << mStatName << "_distinct_count_0x2a_" << mStartIdx;
	//extension_control_objid << mStatName << "_distinct_count_0x2a_" << mStartIdx;

	bool rslt = createControlDoc(root_control_objid, rdata);	
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	OmnString output_fname = "dist_count";
	output_fname << "(" << distinct_field << ")";

	AosJqlUtil::escape(output_fname); 

	OmnString data_type = "int64";
	str << "<measure zky_isdistinct=\"true\">"
		<<	"<measure_field field_name=\"0x2a\" output_field_name=\"" << output_fname << "\"/>"
		<<		"<agr_type type=\"dist_count_new\" distinct_field=\"" << distinct_field << "\" />"
		<<		"<vector2d_control "
		<<			"root_control_objid =\"" << root_control_objid << "\" "
		//<<			"extension_control_objid=\"" << extension_control_objid << "\" "
		<<			"zky_data_type=\"" << data_type <<"\"/>"
		<< "</measure>";
	return str;
}

bool
AosJqlStmtStatistics::isDistinct(
		AosExprSet &key_fields,
		OmnString &distinct_field,
		const AosRundataPtr &rdata)
{
	AosExprSet::iterator itr = key_fields.begin();
	AosExprObjPtr tmp;
	for(; itr != key_fields.end(); ++itr)
	{
		tmp = *itr;
		OmnString name = tmp->getValue(rdata.getPtrNoLock());

		if(!name.hasPrefix("distinct:"))
		{
			continue;
		}

		AosStrSplit split(name, ":");          
		vector<OmnString> strs= split.entriesV();
		if (!strs.size() == 2)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		distinct_field = strs[1];

		OmnScreen << "########## distinct_field : " << strs[1] << endl;
		//OmnString exprstr = strs[1];
		//exprstr << ";";
		//AosExprObjPtr expr = AosRunJQL(exprstr, true, rdata);
		//if (!expr)
		//{
		//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		//	OmnAlarm << __func__ << enderr;
		//	return false;
		//}
		key_fields.erase(itr);
		//key_fields->push_back(expr->cloneExpr());
		return true;
	}
	return false;
}

OmnString 
AosJqlStmtStatistics::createKeyFieldsStr(
		const AosRundataPtr &rdata,
		AosExprSet  &key_fields,
		OmnString &identify_key)
{
	//felicia, 2014/05/15
	//if (!key_fields)
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return "";
	//}

	OmnString str = "";
	set<OmnString> key_fnames;
	set<OmnString>::iterator s_itr;
	
	identify_key = "";
	u32 idx = 0;

	AosExprSet::iterator itr = key_fields.begin();
	for(; itr != key_fields.end(); ++itr)
	{
		OmnString name = (*itr)->getValue(rdata.getPtrNoLock());
		//arvin 2015.08.01
		//JIMODB-198:check field is exist?
		AosJqlColumnPtr column = getTableField(rdata,mTableName,name);
		if(column->name != name)
		{
			OmnString msg = "[ERR] : Table \"" ;
			msg << mTableName << "\" have no field \"" << name << "\"!";
			rdata->setJqlMsg(msg);
			return "";
		}

		AosJqlUtil::escape(name); // Young 2014/06/06

		s_itr = key_fnames.find(name);
		if(s_itr != key_fnames.end())	continue;

		key_fnames.insert(name);
		str << "<field field_name=\"" << name << "\" ";
		if ((*itr)->getType() == AosExprType::eArith)
		{
			str << "zky_type=\"expr\"";
		}
		str << " ></field>";

		if(idx != 0)	identify_key << "_";
		identify_key << name;
		idx++;	
	}

	// Ketty TTTT 2014/11/20
	if(identify_key == "")
	{
		aos_assert_r(key_fields.size() == 0, "");
		identify_key = "__NOKEY";
	}
	return str;
}

/*
OmnString 
AosJqlStmtStatistics::createKeyFieldsStr(const AosRundataPtr &rdata)
{
	if (!mKeyFields)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}

	AosValueRslt vv;
	bool rslt;
	OmnString str, v;
	str << "<field field_name=\"";
	for(u32 i = 0; i < mKeyFields->size(); i++)
	{
		rslt = (*mKeyFields)[i]->getValue(rdata.getPtr(), 0, vv);	
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		v = vv.getStr();
		if (!v != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		if ( i != 0)
			str << "," << v;
		else
			str << v;
	}
	str << "\" />";
	return str;
}
*/

OmnString
AosJqlStmtStatistics::createStatValueStr(
		const AosRundataPtr &rdata,
		AosExprList* stat_values)
{
	if (!stat_values)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	AosValueRslt vv;
	vector<AosExprObjPtr> parms;
	AosExprGenFunc* expr_func;
	OmnString field_name, func_name, output_fname;
	OmnString str = "";
	OmnString data_type;
	OmnString msg;
	for(u32 i = 0; i < stat_values->size(); i++)
	{
		if (((*stat_values)[i])->getType() != AosExprType::eGenFunc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}

		expr_func = dynamic_cast<AosExprGenFunc*>((*stat_values)[i].getPtr());
		if (!expr_func)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		func_name = expr_func->getFuctName();

		u32 num = expr_func->getParms(rdata.getPtr(), parms);	
		if (!num == 1)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		field_name = parms[0]->getValue(rdata.getPtrNoLock());
		//arvin 2015.08.01
		//JIMODB-198:check field is exist?
		AosJqlColumnPtr column = getTableField(rdata,mTableName,field_name);
		if(column->name != field_name && field_name != "*")
		{
			msg = "[ERR] : Table \"" ;
			msg << mTableName << "\" have no field \"" << field_name << "\"!";
			rdata->setJqlMsg(msg);
			return "";
		}


		data_type = getType(rdata, field_name);
		//arvin 2015.07.29
		//JIMODB-114:add actual_datatype into stat_def_doc,then stat_internal_doc will get it from stat_def_doc
		OmnString type = data_type;

		if (data_type != "number" && data_type != "double" )
		{
			data_type = "int64"; 
		}
		if (field_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}

		// Ketty 2014/10/22
		if(func_name == "count") 
		{
/*			//arvin 2015.08.03
			//JIMODB-191
			if(flag)
			{
				msg << "[ERR] : Statistics Model " << mStatName 
					<< " doesn't support multiple count!";
				rdata->setJqlMsg(msg);
				return "";
			}
			//arvin 2015.08.03
*/			//JIMODB-203
			data_type = "u64";
//			field_name = "*";
//			flag = true;
		}
		// Ketty 2014/06/05
		AosJqlUtil::escape(field_name);	// to repliac *. 
		output_fname = func_name;
		output_fname << "(" << field_name << ")";

		AosJqlUtil::escape(output_fname); 
		str << "<measure field_name=\"" << field_name << "\" "
			<< "agr_type=\"" << func_name << "\" "
			<< "zky_name=\"" << output_fname << "\" "
			<< "zky_data_type=\"" << data_type << "\" "
			<< "type=\"" << type << "\" />";
	}
	return str;

}
		
bool 
AosJqlStmtStatistics::createControlDoc(
		const OmnString &root_control_objid, 
		//const OmnString &extension_control_objid,
		const AosRundataPtr &rdata)
{
	//OmnString msg;
	//OmnString str_doc = "<extension_control_doc ";
	//str_doc << "zky_doc_per_distblock=\"1250\" "
	//	   << "zky_otype=\"zky_binarydoc\" "
	//	   << "zky_objid=\"" << extension_control_objid << "\" " 
	//	   << "zky_public_doc=\"true\" "
	//	   << "zky_public_ctnr=\"true\" />";
	//
	//if ( !createDocByObjid(str_doc, extension_control_objid) )
	//{
	//	msg << "Failed to create extension control doc .";
	//	rdata->setJqlMsg(msg);
	//	return false;
	//}                                                                                               
	
	OmnString msg;
	OmnString str_doc = "<root_control_doc ";
	str_doc << "zky_otype=\"zky_binarydoc\" "
		    << "zky_doc_per_distblock=\"1042\" "
			<< "zky_objid=\"" << root_control_objid << "\" "
			<< "zky_public_doc=\"true\" "
			<< "zky_public_ctnr=\"true\" />";

	AosXmlTagPtr doc = AosXmlParser::parse(str_doc AosMemoryCheckerArgs);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if ( !createDocByObjid(rdata, doc, root_control_objid) )
	{
		msg << "Failed to create root control doc .";
		rdata->setJqlMsg(msg);
		return false;
	}
cout << " ===================root_control_doc===================== " << endl;
cout << str_doc << endl;
cout << " ======================================================== " << endl;

	return true;
}


bool
AosJqlStmtStatistics::addNodeInTable(const AosRundataPtr &rdata)
{
	OmnString msg;
	if(mTableName == "")
	{
		if (!mTableNames->size() > 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosExprObjPtr expr = mTableNames->front();
		mTableName = expr->getValue(rdata.getPtrNoLock());
	}
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eTableDoc, mTableName);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString node_str;
	node_str << "<statistic zky_stat_conf_objid=\"" << mStatName << "\" />";
	AosXmlTagPtr node = AosXmlParser::parse(node_str AosMemoryCheckerArgs); 
	if (!node)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr nodes = doc->getFirstChild("statistic");

	if(!nodes)
	{
		OmnString idxs;
		idxs << "<statistic time_field_name=\"" << mTimeFieldName << "\"><statistic_defs>" << node_str << "</statistic_defs></statistic>";
		nodes = AosXmlParser::parse(idxs AosMemoryCheckerArgs); 
		if (!nodes)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		OmnString database_name = getCurDatabase(rdata);
		OmnString node_objid = getObjid(mTableName, database_name);
		
		// Ketty 2014/10/11
		//OmnString stat_name = database_name;
		//stat_name << "_" << mTableName << "_stat";

		//nodes->setAttr("zky_stat_name", stat_name);
		nodes->setAttr("zky_stat_name", mInteStatName);
		nodes->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");
		//doc->addNode(nodes);
		
		if ( !createDocByObjid(rdata, nodes, node_objid) )
		{
			msg << "Failed to create Statistics doc."; 
			rdata->setJqlMsg(msg);
			return false;
		}
cout << " ===================statistics doc===================== " << endl;
cout << nodes->toString() << endl;
cout << " ======================================================== " << endl;
		mStatisticDoc = nodes;

		AosXmlTagPtr stat_doc = doc->addNode1("statistic");
		if (!stat_doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		stat_doc->setAttr("stat_doc_objid", node_objid);
		
		if ( !modifyDoc(rdata, doc) )
		{
			msg << "Failed to create defined statistics doc. : modify table doc  msg."; 
			rdata->setJqlMsg(msg);
			return false;
		}
		return true;
	}

	OmnString stat_doc_objid = nodes->getAttrStr("stat_doc_objid", "");
	if (stat_doc_objid == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr stat_doc = getDocByObjid(rdata, stat_doc_objid);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr def_nodes = stat_doc->getFirstChild("statistic_defs");
	if (!def_nodes)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	def_nodes->addNode(node);
	
	if ( !modifyDoc(rdata, stat_doc) )
	{
		msg << "Failed to create Statistics doc."; 
		rdata->setJqlMsg(msg);
		return false;
	}
cout << " ===================statistics doc===================== " << endl;
cout << stat_doc->toString() << endl;
cout << " ======================================================== " << endl;
		
	mStatisticDoc = stat_doc;		

	/*
	if (!nodes)
	{
		OmnString idxs;
		idxs << "<statistics time_field_name=\"" << mTimeFieldName << "\"><statistic_defs>" << node_str << "</statistic_defs></statistics>";
		nodes = AosXmlParser::parse(idxs AosMemoryCheckerArgs); 
		if (!nodes)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		OmnString node_objid = getObjid(mTableName, database_name);
		nodes->setAttr(AOSTAG_PUBLIC_DOC, "true");               
		nodes->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		nodes->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");
		nodes->setAttr(AOSTAG_OBJID, node_objid);
		doc->addNode(nodes);
		// then createDoc
cout << " ===================statistics doc===================== " << endl;
cout << nodes->toString() << endl;
cout << " ======================================================== " << endl;
	}
	else
	{
		AosXmlTagPtr def_nodes = nodes->getFirstChild("statistic_defs");
		if(!def_nodes)
		{
			def_nodes = nodes->addNode1("statistic_defs");
		}

		def_nodes->addNode(node);
		if ( !modifyDoc(nodes) )
		{
			msg << "Failed to create Statistics doc."; 
			rdata->setJqlMsg(msg);
			return false;
		}
	}

	if ( !modifyDoc(doc) )
	{
		msg << "Failed to create defined statistics doc. : modify table doc  msg."; 
		rdata->setJqlMsg(msg);
		return false;
	}
	*/
	return true;
}

OmnString
AosJqlStmtStatistics::getObjid(
		const OmnString &tname,
		const OmnString &dname)
{
	OmnString objid = "";
	objid << AOSZTG_STATISTICS << "_" << tname << "_" << dname; 
	return objid;
}

bool
AosJqlStmtStatistics::checkStatValues(const AosRundataPtr &rdata)
{
	AosExprList*  stat_values = new AosExprList;	
	set<OmnString> funcs;
	AosMeasure * measure;
	AosExprObj *expr;
	//AosExprList *exprList;
	vector<AosExprObjPtr> parms;
	AosExprGenFunc* expr_func;
	OmnString field_name, func_name, error_msg, key;
	u32 num;
	//bool has_distinct = false;
	for(size_t i=0; i<mMeasures->size(); i++)
	{
		measure = (*mMeasures)[i];
		expr = measure->mExpr;	
		if (expr->getType() != AosExprType::eGenFunc)
			if (!false)
			{
				//Gavin 2015/07/21 JIMODB-170 
				//AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				//OmnAlarm << __func__ << enderr;
				error_msg << "[ERR] : the No." << i+1 << " measure is not a measure function!";
				rdata->setJqlMsg(error_msg);
				return false;
			}

		expr_func = dynamic_cast<AosExprGenFunc*>(expr);
		if (!expr_func)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		func_name = expr_func->getFuctName();
		//Gavin 2015/07/21 JIMODB-170
		if(!(func_name == "sum" || func_name == "count" || func_name == "max" || func_name == "min" || func_name == "dist_count"))
		{
			error_msg << "[ERR] : '" << func_name << "' is not a supported measure function in statistics model.";  
			rdata->setJqlMsg(error_msg);                                   
			return false;                                                  
		}

		num = expr_func->getParms(rdata.getPtr(), parms);	
		if (!num == 1)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		field_name = parms[0]->getValue(rdata.getPtrNoLock());
		if (field_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		
		key = func_name;
		key << "(" << field_name << ")";
		if(funcs.find(key) != funcs.end())
		{
			error_msg << "command error : measure " << key << " repeated!";
			rdata->setJqlMsg(error_msg);
			return false;
		}
		funcs.insert(key);
			
		//if(func_name == "dist_count")
		//{
			// Ketty 2014/11/06
			/*
			if(has_distinct)
			{
				error_msg << "command error : Can't support multi distinct count!";
				rdata->setJqlMsg(error_msg);
				return false;
			}

			mDistinct.clear();
			mDistinctGroup.clear();
			exprList = measure->mExprList;
			rslt = checkNoRepeatFields(exprList, mDistinct, rdata);
			if (!rslt)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
			
			mDistinctGroup = mDistinct;

			OmnString distName = "distinct:";                  
			distName << field_name;                                    
			AosExprObj *distinct_expr = new AosExprFieldName(distName.getBuffer()); 
			mDistinct.insert(distinct_expr);

			has_distinct = true;
			*/
			
		//	if(!mDistStatValues)	mDistStatValues = new AosExprList;
		
		//	mDistStatValues->push_back(expr);
		//	mDistKeyFields.insert(new AosExprFieldName(field_name.getBuffer()));
		//}
		//else
		//{
			if(!mNormStatValues)	mNormStatValues = new AosExprList;
			mNormStatValues->push_back(expr);
		//}
		
		stat_values->push_back(expr);
	}

	setStatValues(stat_values);
	return true;
}

bool
AosJqlStmtStatistics::checkConfig(const AosRundataPtr &rdata)
{
	//check0 : check statvalues;
	//check1 : there is one max group;
	//check2 : there is no the same group;
	//check3 : the small groups must be the max group's child;
	//check4 : there is no the same field in one group;
	//check5 : if distinct group in the small groups, egnore the small group;
	
	// check time field Ketty 2014/11/07
	if(mTimeFieldName != "")
	{
	//	Pay		2015-12-03		JIMODB-1329
	//	if (mTimeOriginalFormat == "")
	//	{
	//		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//		OmnAlarm << __func__ << enderr;
	//		return false;
	//	}
		if (mTimeUnit == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	bool rslt = checkStatValues(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	// Ketty TTTT 2014/11/20
	if(!mGroupByFields)	return true;

	OmnString error_msg;
	map<u32, vector<AosExprSet> > tmp_fields;
	map<u32, vector<AosExprSet> >::iterator itr;
	map<u32, vector<AosExprSet> >::reverse_iterator ritr;
	u32 size;

	for(size_t i=0; i<mGroupByFields->size(); i++)
	{
		size = (*mGroupByFields)[i]->size();
		if(size == 0)
		{
			error_msg << "command error : There is one empty group !";
			rdata->setJqlMsg(error_msg);
			return false;
		}
		itr = tmp_fields.find(size);
		AosExprSet group;
		//tmp_vec.push_back((*GroupByFields)[i]);
		rslt = checkNoRepeatFields((*mGroupByFields)[i], group, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		if(itr == tmp_fields.end())
		{
			vector<AosExprSet> tmp_vec;
			tmp_vec.push_back(group);
			tmp_fields.insert(make_pair(size, tmp_vec));
		}
		else
		{
			itr->second.push_back(group);
		}
	}

	ritr = tmp_fields.rbegin();
	if(ritr == tmp_fields.rend())
	{
		error_msg << "command error : Can't find max size group !";
		rdata->setJqlMsg(error_msg);
		return false;
	}

	vector<AosExprSet> vec = ritr->second;
	if(vec.size() != 1)
	{
		error_msg << "command error : There are " << vec.size() << " max size group !";
		rdata->setJqlMsg(error_msg);
		return false;
	}

	setKeyFields(vec[0]);
	
	vector<AosExprSet> dimension_vec;
	size = ritr->first;
	u32 dfield_num = mDistinctGroup.size();
	if(dfield_num >= size)
	{
		error_msg << "command error : distinct fields are too much!";
		rdata->setJqlMsg(error_msg);
		return false;
	}
	++ritr;
	
	if(ritr == tmp_fields.rend())
	{
		if(!mHasInsertDistinct && dfield_num > 0)
		{
			dimension_vec.push_back(mDistinct);
			mHasInsertDistinct = true;	
		}
	}

	for(;ritr != tmp_fields.rend(); ++ritr)
	{
		if(!mHasInsertDistinct && ritr->first <= dfield_num && dfield_num <= size)
		{
			dimension_vec.push_back(mDistinct);
			mHasInsertDistinct = true;	
		}
		vec = ritr->second;
		rslt = checkSameLevel(vec, dimension_vec, rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	if(dimension_vec.size() > 0)
	{
		setDimensions(dimension_vec);
	}
	return true;
}

bool
AosJqlStmtStatistics::checkSameLevel(
		vector<AosExprSet> &vec,
		vector<AosExprSet> &dimension_vec,
		const AosRundataPtr &rdata)
{
	bool rslt;
	for(size_t i=0; i<vec.size(); i++)
	{
		rslt = checkIsChildLevel(vec[i], rdata);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		for(size_t k=i+1; k<vec.size(); k++)
		{
			rslt = checkNoRepeatGroups(vec[i], vec[k], rdata);
			if (!rslt)
			{
				AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
				OmnAlarm << __func__ << enderr;
				return false;
			}
		}

		if(mDistinctGroup.size() > 0 && mDistinctGroup.size() == vec[i].size())
		{
			rslt = checkNoRepeatGroups(vec[i], mDistinctGroup, rdata);
			if(!rslt)
			{
				continue;
			}
		}
		dimension_vec.push_back(vec[i]);
	}

	return true;
}

bool
AosJqlStmtStatistics::checkNoRepeatFields(
		AosExprList* fields,
		AosExprSet &group,
		const AosRundataPtr &rdata)
{
	OmnString error_msg;
	AosValueRslt vv;
	set<OmnString> group_fields;
	for(u32 i = 0; i < fields->size(); i++)
	{
		OmnString name = (*fields)[i]->getValue(rdata.getPtrNoLock());
		if(name == "")
		{
			error_msg << "command error : There is an empty field in group! ";
			rdata->setJqlMsg(error_msg);
			return false;
		}

		if(group_fields.find(name) != group_fields.end())
		{
			error_msg << "command error : " << name << " already exist in group! " ;
			rdata->setJqlMsg(error_msg);
			return false;
		}
		
		group_fields.insert(name);
		group.insert((*fields)[i]);
	}

	//OmnDelete(fields);
	return true;
}

bool
AosJqlStmtStatistics::checkIsChildLevel(
		AosExprSet &fields,
		const AosRundataPtr &rdata)
{
	OmnString error_msg;
	AosExprSet::iterator itr1 = fields.begin();
	AosExprSet::iterator itr2;
	OmnString name1, name2, str = "(";
	bool find, error = false;
	for(; itr1 != fields.end(); ++itr1)
	{
		if(str != "(")
		{
			str << ", ";
		}

		find = false;
		name1 = (*itr1)->getValue(rdata.getPtrNoLock());
		//if(mKeyFields.find(name) == mKeyFields.end()) error = true;
		for(itr2 = mKeyFields.begin(); itr2 != mKeyFields.end(); ++itr2)
		{
			name2 = (*itr2)->getValue(rdata.getPtrNoLock());
			if(name1 == name2)
			{
				find = true;
				break;
			}
		}

		if(!find) error = true;

		str << name1;
	}
	str << ")";

	if(error)
	{
		error_msg << "command error : " << str << " is not level0's children!";
		rdata->setJqlMsg(error_msg);
		return false;
	}

	return true;
}

bool
AosJqlStmtStatistics::checkNoRepeatGroups(
		AosExprSet &fields1,
		AosExprSet &fields2,
		const AosRundataPtr &rdata)
{
	//if (!fields1.size() == fields2.size())
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return false;
	//}
	if(fields1.size() != fields2.size())
	{
		return true;
	}

	AosExprSet::iterator itr1 = fields1.begin();
	AosExprSet::iterator itr2 = fields2.begin();
	OmnString name1, name2, str = "(";
	for(; itr1 != fields1.end(); ++itr1, ++itr2)
	{
		if(str != "(")
		{
			str << ", ";
		}
		name1 = (*itr1)->getValue(rdata.getPtrNoLock());
		name2 = (*itr2)->getValue(rdata.getPtrNoLock());
		if(name1 != name2)
		{
			return true;
		}
		str << name1;
	}

	str << ")";
	OmnString error_msg;
	error_msg << "command error : " << str << " group repeated! ";
	rdata->setJqlMsg(error_msg);
	return false;
}

AosJqlStatement *
AosJqlStmtStatistics::clone()
{
	return OmnNew AosJqlStmtStatistics(*this);
}


void 
AosJqlStmtStatistics::dump()
{
}


/***************************
 * Getter/setter
 * **************************/
void 
AosJqlStmtStatistics::setTimeFieldName(AosExprObj *name)
{
	mTimeFieldName = name->getValue(0);
}

void 
AosJqlStmtStatistics::setTableName(OmnString name)
{
	mTableName = name;
}

void
AosJqlStmtStatistics::setTableNames(AosExprList *names)
{
	mTableNames = names;
}

void 
AosJqlStmtStatistics::setStatName(OmnString name)
{
	mStatName = name;
}

void 
AosJqlStmtStatistics::setGroupByFields(vector<AosExprList*> *fields)
{
	mGroupByFields = fields;	
}

void
AosJqlStmtStatistics::setKeyFields(AosExprSet &key_fields)
{
	mKeyFields = key_fields;
}


void
AosJqlStmtStatistics::setStatValues(AosExprList* stat_values)
{
	mStatValues = stat_values;
}


void
AosJqlStmtStatistics::setTimeUnit(OmnString time_unit)
{
	mTimeUnit = time_unit;
}

void 
AosJqlStmtStatistics::setTimeOriginalFormat(OmnString format)
{
	mTimeOriginalFormat = format;
}

void
AosJqlStmtStatistics::setDimensions(vector<AosExprSet> dimensions)
{
	mDimensions = dimensions;
}

void
AosJqlStmtStatistics::setDimensions(vector<AosExprList*> *dimensions)
{
	//mDimensions = dimensions;
	mUpdateDimensions = dimensions;
}


void 
AosJqlStmtStatistics::setWhereCond(AosExprObj *cond)
{
	mWhereCond = cond;
}

void 
AosJqlStmtStatistics::setShuffleField(AosExprList *shuffleFields)
{
	mShuffleFields = shuffleFields;
}

void 
AosJqlStmtStatistics::setMeasures(vector<AosJqlStmtStatistics::AosMeasure*> *measures)
{
	mMeasures = measures;
}


//===================================================================================
bool
AosJqlStmtStatistics::createStatisticsNew(const AosRundataPtr &rdata)
{
	bool rslt = checkConfig(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	if(mTableName == "")
	{
		if (!mTableNames->size() > 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosExprObjPtr expr = mTableNames->front();
		mTableName = expr->getValue(rdata.getPtrNoLock());
		//arvin 2015.08.01
		AosXmlTagPtr table_doc = getDoc(rdata,JQLTypes::eTableDoc,mTableName);
		if(!table_doc)
		{
			OmnString msg = "[ERR] : Table \"";
			msg << mTableName << "\" doesn't exist!";
			rdata->setJqlMsg(msg);
			return false;
		}
	}
	//arvin 2015.11.14
	//JIMODB-1118
	if ( getDoc(rdata,JQLTypes::eStatisticsDoc,mStatName) )
	{
		OmnString error = "[ERR] : ";
		error << mStatName << " doc already exists!";
		rdata->setJqlMsg(error);
		return true;
	}

	// mStatName is the user config's stat_name.
	OmnString database_name = getCurDatabase(rdata);
	mInteStatName = database_name;
	mInteStatName << "_" << mTableName << "_stat_" << mStatName;

	AosXmlTagPtr stat_def_doc = createStatDefDoc(rdata);
	if (!stat_def_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	rslt = addStatDefToTable(rdata, stat_def_doc);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	if(mDistStatValues)
	{
		rslt = refactorInternalDistStat(rdata);
	}
	else
	{
		rslt = refactorInternalStat(rdata, mKeyFields, mStatValues);
	}
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	return true;
}


AosXmlTagPtr
AosJqlStmtStatistics::createStatDefDoc(const AosRundataPtr &rdata)
{
	OmnString msg = "";
	AosXmlTagPtr stat_def_doc = createStatDefDocConf(rdata, mKeyFields, mStatValues);
	if (!stat_def_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	//stat_def_doc->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");
	stat_def_doc->setAttr("zky_table_name", mTableName);
	stat_def_doc->setAttr(AOSTAG_PUB_CONTAINER, AOSCTNR_STAT_CONTAINER);
	
	//OmnString identify_key = stat_def_doc->getAttrStr("zky_stat_identify_key", "");
	//OmnString stat_def_objid = mStatName;
	//stat_def_objid << "_" << identify_key;
	
	// Modified by Young, 2014/12/23
	OmnString stat_def_objid = AosObjid::getObjidByJQLDocName(JQLTypes::eStatisticsDoc, mStatName);
	if ( !createDocByObjid(rdata, stat_def_doc, stat_def_objid) )
	{
		msg << "Failed to create Statistics doc.";
		rdata->setJqlMsg(msg);
		return 0;
	}
	
	msg << "Statistics model " << mStatName << " created successfully.";
	rdata->setJqlMsg(msg);
	
	cout << " ===================stat_def_doc============================= " << endl;
	cout << stat_def_doc->toString() << endl;
	cout << " ======================================================== " << endl;
	
	return stat_def_doc;
}


bool
AosJqlStmtStatistics::addStatDefToTable(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_def_doc)
{
	OmnString msg;
	if(mTableName == "")
	{
		if (!mTableNames->size() > 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		AosExprObjPtr expr = mTableNames->front();
		mTableName = expr->getValue(rdata.getPtrNoLock());
	}
	AosXmlTagPtr doc = getDoc(rdata, JQLTypes::eTableDoc, mTableName);
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosXmlTagPtr stat_doc = retrieveStatDoc(rdata, doc);
	if (!stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	AosXmlTagPtr def_nodes = stat_doc->getFirstChild("statistic_defs");
	if (!def_nodes)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString stat_name = stat_def_doc->getAttrStr(AOSTAG_OBJID, "");
	if(stat_name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString def_conf_str;
	def_conf_str << "<statistic zky_stat_conf_objid=\"" << stat_name << "\" />";
	AosXmlTagPtr def_doc_conf = AosXmlParser::parse(def_conf_str AosMemoryCheckerArgs); 
	if (!def_doc_conf)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	def_nodes->addNode(def_doc_conf);
	
	if ( !modifyDoc(rdata, stat_doc) )
	{
		msg << "Failed to create Statistics doc."; 
		rdata->setJqlMsg(msg);
		return false;
	}

	cout << " ===================statistics doc===================== " << endl;
	cout << stat_doc->toString() << endl;
	cout << " ======================================================== " << endl;

	mStatisticDoc = stat_doc;
	return true;
}


AosXmlTagPtr
AosJqlStmtStatistics::retrieveStatDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &table_doc)
{
	OmnString stat_doc_objid;
	AosXmlTagPtr stat_doc;
	AosXmlTagPtr stat_conf_doc = table_doc->getFirstChild("statistic");
	if(stat_conf_doc)
	{
		stat_doc_objid = stat_conf_doc->getAttrStr("stat_doc_objid", ""); 
		if (stat_doc_objid == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		
		stat_doc = getDocByObjid(rdata, stat_doc_objid);
		if (!stat_doc)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return 0;
		}
		
		return stat_doc;
	}
		
	bool rslt = createStatDoc(rdata, stat_doc, stat_doc_objid);
	if(!rslt || !stat_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	stat_conf_doc = table_doc->addNode1("statistic");
	if (!stat_conf_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	stat_conf_doc->setAttr("stat_doc_objid", stat_doc_objid);
	
	if ( !modifyDoc(rdata, table_doc) )
	{
		OmnString msg;
		msg << "Failed to create defined statistics doc: modify table doc msg."; 
		rdata->setJqlMsg(msg);
		return 0;
	}
	return stat_doc;
}


bool
AosJqlStmtStatistics::createStatDoc(
		const AosRundataPtr &rdata,
		AosXmlTagPtr &stat_doc,
		OmnString &stat_doc_objid)
{
	OmnString database_name = getCurDatabase(rdata);
	//OmnString stat_name = database_name;
	//stat_name << "_" << mTableName << "_stat";
	stat_doc_objid = getObjid(mTableName, database_name); 
		
	OmnString stat_doc_str;
	stat_doc_str << "<statistic>" 
				<< "<statistic_defs></statistic_defs>"
				<< "</statistic>";
	stat_doc = AosXmlParser::parse(stat_doc_str AosMemoryCheckerArgs); 

	stat_doc->setAttr("time_field_name", mTimeFieldName);
	//stat_doc->setAttr("zky_stat_name", stat_name);
	stat_doc->setAttr("zky_stat_name", mInteStatName);
	stat_doc->setAttr(AOSTAG_PUB_CONTAINER, "_zt4g_stat_ctnr");
		
	if ( !createDocByObjid(rdata, stat_doc, stat_doc_objid) )
	{
		OmnString msg;
		msg << "Failed to create Statistics doc."; 
		rdata->setJqlMsg(msg);
		return false;
	}

	return true;
}


bool
AosJqlStmtStatistics::refactorInternalStat(
		const AosRundataPtr &rdata,
		AosExprSet	&key_fields,
		AosExprList* stat_values)
{
	bool rslt;
	AosXmlTagPtr stat_def_doc;
	
	stat_def_doc = createStatDefDocConf(rdata, key_fields, stat_values);
	if (!stat_def_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}

	OmnString stat_doc_objid = mStatisticDoc->getAttrStr(AOSTAG_OBJID, "");
	if (stat_doc_objid == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	rslt = refactorInternalStatConf(rdata, stat_doc_objid, stat_def_doc);	// parent func.
	if(!rslt)
	{
		OmnString msg;
		msg << "Failed to create internal doc.";
		rdata->setJqlMsg(msg);
		return false;
	}
	if(!mDistStatValues)	return true;
	

	return true;
}


bool
AosJqlStmtStatistics::refactorInternalDistStat(const AosRundataPtr &rdata)
{
	if(!mDistStatValues)	return true;
	bool rslt;
	
	rslt = refactorInternalStat(rdata, mKeyFields, mNormStatValues);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	AosExprSet::iterator itr = mDistKeyFields.begin();
	u32 idx = 0;
	for(; itr != mDistKeyFields.end(); itr++, idx++)
	{
		AosExprSet key_fields = mKeyFields;
		key_fields.insert(*itr);

		aos_assert_r(idx < mDistStatValues->size(), false);	
		AosExprList*  stat_values = new AosExprList;	
		stat_values->push_back((*mDistStatValues)[idx]);
	
		rslt = refactorInternalStat(rdata, key_fields, stat_values);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}
	
	return true;
}



AosXmlTagPtr
AosJqlStmtStatistics::createStatDefDocConf(
		const AosRundataPtr &rdata,
		AosExprSet	&key_fields,
		AosExprList* stat_values)
{
	OmnString time_name = mTimeFieldName;
	AosJqlUtil::escape(time_name);
	OmnString identify_key;
	
	OmnString str;
	str << "<statistic>";
	OmnString key_str = createKeyFieldsStr(rdata,key_fields,identify_key);
	str <<	"<key_fields>" << key_str << "</key_fields>";
	OmnString measure_str = createStatValueStr(rdata,stat_values);
	if(measure_str == "")
	{	
		//arvin 2015.08.14
		//JIMODB-444
		OmnString msg = rdata->getJqlMsg();
		if(msg == "")
		{
			msg << "[ERR] : Create Statistic Model need measure_field!";
		}
		rdata->setJqlMsg(msg);
		return 0;
	}
	str	<<  "<measures>" << measure_str <<  "</measures>";
	
	//proc time fields
	if(mTimeFieldName != "" )
	{
		str << "<time_field time_format =\"" << mTimeOriginalFormat << "\" "
			<< 	    "grpby_time_unit=\"" << mTimeUnit << "\" "
			<< 		"time_field_name=\"" << time_name << "\" />";
	}
	
	//proc where condition
	if (mWhereCond)
	{
	    str <<  "<cond><![CDATA[" << mWhereCond->dump() << "]]></cond>";
	}

	//proc shuffle field
	if(mShuffleFields)
	{
		str << "<shuffle_fields>";
		for(size_t i = 0;i < mShuffleFields->size(); i++)
		{
			if(!syntaxCheck(rdata,(*mShuffleFields)[i],key_fields))
				return 0;
			str << "<field field_name=\"" <<(*mShuffleFields)[i]->dumpByNoEscape()<<"\"/>";
		}
		str << "</shuffle_fields>";
	}

	str << "<stat_key_conn />"
		<< "<vt2d_conn time_bucket_weight=\"1000\" start_time_slot=\"14610\" />"
		<< "</statistic>";
	
	AosXmlTagPtr stat_def_doc = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
	if (!stat_def_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	
	stat_def_doc->setAttr("zky_stat_identify_key", identify_key);
	stat_def_doc->setAttr("zky_stat_name", mInteStatName);
	
	return stat_def_doc;
}
	
OmnString 
AosJqlStmtStatistics::getType(
		const AosRundataPtr &rdata,
		const OmnString field_name)
{
	if(mTableName == "")
	{
		if (!mTableNames->size() > 0)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return "";
		}
		AosExprObjPtr expr = mTableNames->front();
		mTableName = expr->getValue(rdata.getPtrNoLock());
	}
	AosJqlColumnPtr column = AosJqlStatement::getTableField(rdata, mTableName, field_name);
	if (column)
	{
		if (column->sdatatype != "")
			return AosJqlStatement::AosJType2DType(column->sdatatype);
		return AosJqlStatement::AosJType2DType(AosDataType::getTypeStr(column->type));
	}
	return "";
}
#if 0
bool
AosJqlStmtStatistics::checkTimeField(const AosRundataPtr &rdata)
{
	OmnString fieldName = mTimeFieldName;
	OmnString format = mTimeOriginalFormat;
	AosXmlTagPtr table_doc = getDoc(rdata,JQLTypes::eTableDoc,mTableName);
	aos_assert_r(table_doc, false);
	OmnString schemaObjid = table_doc->getAttrStr("zky_use_schema");
	AosXmlTagPtr schemaDoc = getDocByObjid(rdata, schemaObjid);
	aos_assert_r(schemaDoc, false);
	AosXmlTagPtr recordNode = schemaDoc->getFirstChild("datarecord");
	aos_assert_r(recordNode, NULL);
	AosXmlTagPtr fieldsNode = recordNode->getFirstChild("datafields");
	aos_assert_r(fieldsNode, NULL);
	AosXmlTagPtr fieldNode = fieldsNode->getFirstChild("datafield");
	bool flag = false;
	while(fieldNode)
	{
		OmnString name = fieldNode->getAttrStr("zky_name","");
		if(name == fieldName)
		{
			OmnString tfmt = fieldNode->getAttrStr("format");
			if(tfmt == "")
			{
				if(fieldNode->getAttrStr("type") != "str")
				{
					mErrmsg = "[ERR] : please check time format in the schema!";
					rdata->setJqlMsg(mErrmsg);
					return false;
				}
			}
			OmnString format = convertTimeFormat(rdata,flag,mTimeOriginalFormat);
			//arvin 2015.08.19
			//JIMODB-507
			if(flag && tfmt != "")
			{
				mTimeOriginalFormat = reverseConvertTimeFormat(rdata,tfmt);
			}
			if(tfmt == format)
			{
				return true;	
			}
			else
			{
				mErrmsg = "[ERR] : please check the time format!";
				rdata->setJqlMsg(mErrmsg);
				return false;
			}
		}
		fieldNode = fieldsNode->getNextChild();
	}
	OmnShouldNeverComeHere;
	return false;
}

OmnString
AosJqlStmtStatistics::convertTimeFormat(
		const AosRundataPtr &rdata,
		bool &flag,
		const OmnString &format)
{
	if(format == "yyyymmdd")
	{
		return "%Y%m%d";	
	}
	if(format == "yymmdd")
	{
		return "%y%m%d";	
	}
	if(format == "yymmddhhmmss")
	{
		return "%y%m%d%H%M%S";	
	}
	if(format == "yyyymmddhhmmss")
	{
		return "%Y%m%d%H%M%S";
	}
	if(format == "yymmddhhmmss")
	{
		return "%y%m%d%H%M%S";
	}
	if(format == "yyyy-mm-dd hh:mm:ss")
	{
		return  "%Y-%m-%d %H:%M:%S";
	}
	flag = true;
	return format;
}

OmnString
AosJqlStmtStatistics::reverseConvertTimeFormat(
		const AosRundataPtr &rdata,
		const OmnString &format)
{
	if(format == "%Y%m%d")
	{
		return "yyyymmdd";
	}
	if(format == "%y%m%d")
	{
		return "yymmdd";	
	}
	if(format == "%y%m%d%H%M%S")
	{
		return "yymmddhhmmss";
	}
	if(format == "%Y%m%d%H%M%S")
	{
		return "yyyymmddhhmmss";
	}
	if(format == "%y%m%d%H%M%S")
	{
		return "yymmddhhmmss";
	}
	if(format == "%Y-%m-%d %H:%M:%S")
	{
		return "yyyy-mm-dd hh:mm:ss";
	}
	OmnShouldNeverComeHere;
	return "";
}
#endif
bool 
AosJqlStmtStatistics::syntaxCheck(
		const AosRundataPtr &rdata,
		const AosExprObjPtr &shuffleField,
		const AosExprSet	&keyFields)
{
	OmnString shuffleFieldStr = shuffleField->dumpByNoEscape();
	AosExprSet::iterator itr = keyFields.begin();
	for(; itr != keyFields.end(); ++itr)
	{
		OmnString name = (*itr)->getValue(rdata.getPtrNoLock());
		if(name == shuffleFieldStr)
			return true;
	}
	OmnString msg = "[ERR] : ";
	msg	<< "shuffle field \"" << shuffleFieldStr << "\" must appear in group by fields!";
	rdata->setJqlMsg(msg);
	
 	return false;
}

