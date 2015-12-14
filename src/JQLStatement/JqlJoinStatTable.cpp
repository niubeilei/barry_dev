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
// 2014/05/08	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlJoinStatTable.h"

#include "API/AosApi.h"
#include "SEInterfaces/QueryType.h"
#include "SEUtil/Objid.h"

#if 0
AosJqlJoinStatTable::AosJqlJoinStatTable()
{
}


AosJqlJoinStatTable::~AosJqlJoinStatTable()
{
}


bool
AosJqlJoinStatTable::initInputFields(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return true;
}


bool
AosJqlJoinStatTable::checkIsStatQuery(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	AosJqlSelectTablePtr base_table = OmnNew AosJqlSelectTable();
	bool rslt = base_table->init(rdata, select);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	vector<AosXmlTagPtr> all_stat_docs;
	rslt = initStatDocs(rdata, select, all_stat_docs);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	if(all_stat_docs.size() == 0)	return false;
	return AosJqlStatQuery::checkIsStatQuery(rdata, base_table, all_stat_docs);
}


bool
AosJqlJoinStatTable::init(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = AosJqlJoinTable::init(rdata, select);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	OmnNotImplementedYet;
	OmnString join_stat_objid;// = AosObjid::composeJoinStatDocObjid(mAllTableNames);

	vector<AosXmlTagPtr> all_stat_docs;
	rslt = initStatDocs(rdata, join_stat_objid, all_stat_docs);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!all_stat_docs.size())
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	rslt = AosJqlStatQuery::init(rdata, join_stat_objid, mInputAggrFields,
			mGroupByFields, mConds, mOrderByFields, all_stat_docs); 
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	return true;
}


OmnString
AosJqlJoinStatTable::generateWhereConf(const AosRundataPtr &rdata)
{
	return "";
	//return AosJqlStatQuery::generateWhereConf(rdata, mConds);
}	


OmnString
AosJqlJoinStatTable::generateDefaultIndexConf(const AosRundataPtr &rdata)
{
	// must has one join cond at list.
	OmnShouldNeverComeHere;
	return "";
	//return AosJqlStatQuery::generateDefaultWhereConf(rdata);	
}


OmnString
AosJqlJoinStatTable::generateGroupByConf(const AosRundataPtr &rdata)
{
	return "";
	//return AosJqlStatQuery::generateGroupByConf(rdata);
}


OmnString
AosJqlJoinStatTable::generateInputDatasetConf(const AosRundataPtr &rdata)
{
	return "";
	//return AosJqlStatQuery::generateInputDatasetConf(rdata, mInputFields);
}


bool
AosJqlJoinStatTable::initStatDocs(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		vector<AosXmlTagPtr> &all_stat_docs)
{
	vector<AosXmlTagPtr> table_defs;
	bool rslt = select->getAllTableDef(rdata, table_defs);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString table_name;
	vector<OmnString> all_table_names;
	for(u32 i=0; i<table_defs.size(); i++)
	{
		table_name = table_defs[i]->getAttrStr(AOSTAG_TABLENAME, "");
		if (!table_name != "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		all_table_names.push_back(table_name);
	}
	
	OmnString join_stat_objid = AosObjid::composeJoinStatDocObjid(all_table_names);

	rslt = initStatDocs(rdata, join_stat_objid, all_stat_docs);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	return true;
}


bool
AosJqlJoinStatTable::initStatDocs(
		const AosRundataPtr &rdata,
		const OmnString &join_stat_objid,
		vector<AosXmlTagPtr> &all_stat_docs)
{
	if (!join_stat_objid != "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr stat_doc = getDocByObjid(join_stat_objid);
	aos_assert_rr(stat_doc, rdata, false);
	
	AosXmlTagPtr internal_stat_confs = stat_doc->getFirstChild("internal_statistics");
	if (!internal_stat_confs)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr each_conf = internal_stat_confs->getFirstChild(true);
	while(each_conf)
	{
		all_stat_docs.push_back(each_conf);
		each_conf = internal_stat_confs->getNextChild();
	}
	
	return true;	
}
#endif
