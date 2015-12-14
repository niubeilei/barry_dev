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
#include "JQLStatement/JqlSingleStatTable.h"

#include "API/AosApi.h"
#include "Util/ValueRslt.h"


AosJqlSingleStatTable::AosJqlSingleStatTable()
{
}


AosJqlSingleStatTable::~AosJqlSingleStatTable()
{
}


bool
AosJqlSingleStatTable::initInputFields(const AosRundataPtr &rdata)
{
	bool rslt = AosJqlSingleTable::initInputFields(rdata);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	if (!mInputAggrFields.empty())
		mInputFields.insert(mInputFields.end(), mInputAggrFields.begin(), mInputAggrFields.end());
	return true;
}


bool
AosJqlSingleStatTable::initOutputFieldList(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = AosJqlSingleTable::initOutputFieldList(rdata, select);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString field_name;
	AosJqlSelectFieldPtr field = NULL; 
	map<OmnString, AosJqlSelectFieldPtr>::iterator itr;

	for (u32 i = 0; i < mOutputFields.size(); i++)
	{
		field = mOutputFields[i];
		if (field->checkIsAgrFunc())
		{
			//field->mOName = field->mCName;
			//or dumpByNoEscape()
			field->mOName = field->dumpByStat();
			field_name = field->getFieldName(rdata);
			itr = mInputFieldsMap.find(field_name);
			if (itr != mInputFieldsMap.end())
				mInputFieldsMap.erase(itr);
		}
	}
	return true;
}

bool
AosJqlSingleStatTable::checkIsStatQuery(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select,
		const AosExprObjPtr &table_expr,
		const AosExprObjPtr &alias)
{
	AosJqlSelectTablePtr base_table = OmnNew AosJqlSelectTable();
	base_table->mTableName = table_expr;
	base_table->mAliasName = alias;
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
AosJqlSingleStatTable::init(
		const AosRundataPtr &rdata,
		const AosJqlSelectPtr &select)
{
	bool rslt = AosJqlSingleTable::init(rdata, select);
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	OmnString stat_objid;
	vector<AosXmlTagPtr> all_stat_docs;
	rslt = initStatDocs(rdata, mTableNameStr, stat_objid, all_stat_docs);
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

	AosJqlLimitPtr limitPtr = select->mLimitPtr;
	AosJqlHavingPtr having_conds = select->mHavingPtr;
	rslt = AosJqlStatQuery::init(rdata, stat_objid, mOutputFields,
			//mGroupByFields, mRollupGrpByFieldsGrp,  mConds, mOrderByFields, all_stat_docs); 
			mGroupByFields,mCubeFields, mRollupLists, mConds, mOrderByFields, having_conds, limitPtr, all_stat_docs); 
	if (!rslt)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	return true;
}


OmnString
AosJqlSingleStatTable::generateTableConf(const AosRundataPtr &rdata)
{
	if (!mIsInited && mTableName)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return "";
	}
	
	// jimodb-671
	AosJqlSelectTablePtr table = dynamic_cast<AosJqlSingleStatTable*>(this);
	return AosJqlStatQuery::generateTableConf(rdata,
			mTableName->getValue(rdata.getPtrNoLock()),table);
}


bool
AosJqlSingleStatTable::initStatDocs(
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
	if(table_defs.size() == 0)	return true;

	if (!table_defs.size() == 1)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	OmnString table_name = table_defs[0]->getAttrStr("zky_tb_r_name", "");
	if (table_name == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString stat_objid;
	return initStatDocs(rdata, table_name, stat_objid, all_stat_docs);
}


bool
AosJqlSingleStatTable::initStatDocs(
		const AosRundataPtr &rdata,
		const OmnString &table_name,
		OmnString &stat_objid,
		vector<AosXmlTagPtr> &all_stat_docs)
{
	AosXmlTagPtr table_doc = AosJqlStmtTable::getTable(table_name, rdata);
	
	if (!table_doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr stat_def = table_doc->getFirstChild("statistic");         
	if(!stat_def)	return true;
	//if (!stat_def )
	//{
	//	AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
	//	OmnAlarm << __func__ << enderr;
	//	return false;
	//}
	
	//OmnString stat_objid = stat_def->getAttrStr("stat_doc_objid", "");       
	stat_objid = stat_def->getAttrStr("stat_doc_objid", "");       
	if (stat_objid == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	AosXmlTagPtr stat_doc = getDocByObjid(rdata, stat_objid); 
	bool rslt = initStatDocsPriv(rdata, stat_doc, all_stat_docs);
	if(!rslt)	return false;
	
	u32 num = 20;
	OmnString new_table_name, new_stat_objid;


	for(u32 i=1; i<=num; i++)
	{
		new_table_name = table_name;
		new_table_name << "_impr_" << i;
		table_doc = AosJqlStmtTable::getTable(new_table_name, rdata);
		if (!table_doc)	continue;

		stat_def = table_doc->getFirstChild("statistic");
		if(!stat_def)	break;
		
		new_stat_objid = stat_def->getAttrStr("stat_doc_objid", "");
		if (new_stat_objid == "")	break;
		
		stat_doc = getDocByObjid(rdata, new_stat_objid); 
		rslt = initStatDocsPriv(rdata, stat_doc, all_stat_docs);
		if(!rslt)	break;

	}
	
	return true;
}


bool
AosJqlSingleStatTable::initStatDocsPriv(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &stat_doc,
		vector<AosXmlTagPtr> &all_stat_docs)
{
	aos_assert_rr(stat_doc, rdata, false);
	
	AosXmlTagPtr internal_stat_confs = stat_doc->getFirstChild("internal_statistics");
	if (!internal_stat_confs)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	
	AosXmlTagPtr each_stat_conf;
	OmnString conf_objid;
	AosXmlTagPtr each_conf = internal_stat_confs->getFirstChild(true);
	while(each_conf)
	{
		conf_objid = each_conf->getAttrStr("zky_stat_conf_objid", "");
		if (conf_objid == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	
		each_stat_conf = getDocByObjid(rdata, conf_objid);
		aos_assert_rr(each_stat_conf, rdata, false);
		all_stat_docs.push_back(each_stat_conf);
		
		each_conf = internal_stat_confs->getNextChild();
	}

	return true;
}


