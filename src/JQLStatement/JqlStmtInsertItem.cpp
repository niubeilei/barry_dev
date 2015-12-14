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
#include "JQLStatement/JqlStmtInsertItem.h"

#include "JQLStatement/JqlStmtTable.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLExpr/Expr.h"

#include "SEInterfaces/SysInfo.h"

#include "XmlUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "API/AosApi.h"

#include "Rundata/Rundata.h"
#include "Debug/Debug.h"

AosJqlStmtInsertItem::AosJqlStmtInsertItem(const OmnString &errmsg)
:
mPInsert(0),
mPNames(0),
mPValues(0),
mErrmsg(errmsg)
{
}


AosJqlStmtInsertItem::AosJqlStmtInsertItem(AosJqlStmtInsert *insert)
{
	aos_assert(insert);
	mPInsert = insert;
	mPNames = insert->opt_col_names;
	aos_assert(mPNames);
	mPValues = insert->insert_vals_list;
	mTableName = insert->table_name;
}


AosJqlStmtInsertItem::AosJqlStmtInsertItem(const AosJqlStmtInsertItem &rhs)
:
mPInsert(0),
mPNames(0),
mPValues(0)
{
}


AosJqlStmtInsertItem::~AosJqlStmtInsertItem()
{
	delete mPInsert;
}


bool
AosJqlStmtInsertItem::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString database_name = getCurDatabase(rdata);
	if (database_name == "") return false;

	if (!mPInsert)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!mPNames)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (!mPValues)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}
	if (mTableName == "")
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return  false;
	}	


	// Modify by Young for Jozhi
	// Table data format has 4 types: (CSV/Fixbin/Virtual/XML)
	// If table doc's attribute zky_otype="zky_table", do CSV/Fixbin
	// Otherwise do insert into table as XML
	AosXmlTagPtr tableDoc = getDoc(rdata, JQLTypes::eTableDoc, mTableName);
	if ( !tableDoc )
	{
		OmnAlarm << __func__ << enderr;
		return false;
	}

	//add by Barry 2015/07/14
	if (mPNames->size() == 0)
	{
		AosXmlTagPtr columns = tableDoc->getFirstChild("columns");
		if (columns)
		{
			AosXmlTagPtr column = columns->getNextChild("column");
			while(column)
			{
				OmnString name = column->getAttrStr("name");
				name << ";";
				OmnString errmsg;
				AosExprObjPtr expr = AosParseExpr(name, errmsg, rdata.getPtr());
				aos_assert_r(expr, false);
				mPNames->push_back(expr);
				column = columns->getNextChild("column");
			}
		}
	}

	OmnString type = tableDoc->getAttrStr("zky_data_format");
	if (type == "csv" || type == "fixbin")
	{
		OmnString tableObjid = getObjid(rdata, JQLTypes::eTableDoc, mTableName);                 

		OmnString conf = "";
		conf << "<data zky_name=\"" << tableObjid << "\">";
		for (u32 i = 0; i < mPNames->size(); i++)
		{
			OmnString name, value;
			if (i < mPValues->size())
			{
				value = ((*mPValues)[i])->getValue(rdata.getPtrNoLock());
			}
			name = ((*mPNames)[i])->getValue(rdata.getPtrNoLock());
			conf << "<datafield zky_name=\"" << name << "\">"
				<< "<![CDATA[" << value << "]]>"
				<< "</datafield>";
		}
		conf << "</data>";

		// Call Thrift function
		if (doInsertInto(rdata, conf))
		{
			rdata->setJqlMsg("OK");
			return true;
		}
		else
		{
			rdata->setJqlMsg("Insert failed");
			return true;
		}
	}


	OmnString msg;                                                         
	AosValueRslt value_rslt;
	AosXmlTagPtr record = AosXmlParser::parse("<record></record>" AosMemoryCheckerArgs); 
	record->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	record->setAttr(AOSTAG_PUBLIC_DOC, "true");
	record->setAttr(AOSTAG_HPCONTAINER, mTableName); 
	OmnString name, value;
	for (u32 i = 0; i < mPNames->size(); i++)
	{
		name = "", value = "";
		if (i < mPValues->size()) {
			value = ((*mPValues)[i])->getValue(rdata.getPtrNoLock());
			if(!(rdata->getJqlMsg().isNull()))
			{
				rdata->setJqlMsg(OmnString("Err:sequence is not exist,insert failed."));
				return false;
			}
		}
		name = ((*mPNames)[i])->getValue(rdata.getPtrNoLock());
		bool rslt = record->xpathSetAttr(name, value, true);
		if (!rslt)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
	}

	bool rslt = createDocByObjid(rdata, record, "");
	if (!rslt)
	{
		rdata->setJqlUpdateCount(0);
		msg << "Failed to Insert record .";
		rdata->setJqlMsg(msg);
		return false;
	}
	msg << "Insert successful.";
	rdata->setJqlMsg(msg);
	rdata->setJqlUpdateCount(1);
	return true;
}


AosJqlStatement *
AosJqlStmtInsertItem::clone()
{
	return OmnNew AosJqlStmtInsertItem(*this);
}


void 
AosJqlStmtInsertItem::dump()
{
}


AosXmlTagPtr
AosJqlStmtInsertItem::insertData(
		const AosRundataPtr &rdata,
		const OmnString dbName,
		const OmnString tableName,
		map<OmnString, OmnString> *fieldValueMap )
{
	AosXmlTagPtr record = AosXmlParser::parse("<record></record>" AosMemoryCheckerArgs); 
	record->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	record->setAttr(AOSTAG_PUBLIC_DOC, "true");
	record->setAttr(AOSTAG_HPCONTAINER, tableName); 
	record->setAttr(AOSTAG_OBJID, "");  

	map<OmnString, OmnString>::iterator itr = fieldValueMap->begin();
	while (itr != fieldValueMap->end())
	{
		record->setAttr(itr->first, itr->second);
		itr++;
	}

	OmnString str_doc = record->toString();
	AosXmlTagPtr doc = AosCreateDoc(str_doc, true, rdata);    
	if (!doc)
	{
		AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
		OmnAlarm << __func__ << enderr;
		return 0;
	}
	OmnTagFuncInfo << "Insert a new doc. Doc is: " << str_doc << endl;
	return doc;
}


AosXmlTagPtr
AosJqlStmtInsertItem::insertJobStat(
		const AosRundataPtr &rdata,
		map<OmnString, OmnString> *fieldValueMap)
{
	return insertData(rdata, SYSDB, SYSTABLE_JOBSTAT, fieldValueMap); 
}


AosXmlTagPtr
AosJqlStmtInsertItem::insertJobData(
		const AosRundataPtr &rdata,
		map<OmnString, OmnString> *fieldValueMap )
{
	return insertData(rdata, SYSDB, SYSTABLE_JOBDATA, fieldValueMap); 
}


AosXmlTagPtr
AosJqlStmtInsertItem::insertJobDataStat(
		const AosRundataPtr &rdata,
		map<OmnString, OmnString> *fieldValueMap )
{
	return insertData(rdata, SYSDB, SYSTABLE_JOBDATASTAT, fieldValueMap); 
}
