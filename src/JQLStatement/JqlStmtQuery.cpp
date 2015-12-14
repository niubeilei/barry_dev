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
#include "JQLStatement/JqlStmtQuery.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "JQLStatement/JqlStmtTable.h"
#include "JQLExpr/ExprGenFunc.h"
#include "JQLExpr/ExprNameValue.h"
#include "QueryClient/QueryClient.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/JqlUtil.h"
#include "XmlUtil/Ptrs.h"
#include "Util/OmnNew.h"

#include "SEInterfaces/QueryFormatObj.h"  
#include "SEInterfaces/QueryIntoObj.h"    

#include "JQLStatement/JqlTableFactor.h"
#include "Dataset/Jimos/DatasetByQueryNest.h"
#include <fcntl.h>

using namespace AosConf;

AosJqlStmtQuery::AosJqlStmtQuery()
{
	mInputDataset = NULL;
	mChild = NULL;
	mExprs = 0;
}

AosJqlStmtQuery::AosJqlStmtQuery(const OmnString &errmsg)
:
mErrmsg(errmsg)
{
	mExprs = 0;
}

AosJqlStmtQuery::AosJqlStmtQuery(AosJqlSelect *select)
{
	aos_assert(select);
	mSelectPtr = select;
	mExprs = 0;

	mChild = NULL;
	mInputDataset = NULL;
}


AosJqlStmtQuery::~AosJqlStmtQuery()
{
	if (mExprs)
	{
		delete mExprs;
		mExprs = 0;
	}
}


bool
AosJqlStmtQuery::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	//evaluate all the expressions
	bool rslt;
		if (mExprs) return doExpr(rdata);

	if (mErrmsg != "")
	{
		AosSetErrorUser(rdata, __func__) << mErrmsg << enderr;
		OmnAlarm << __func__ << enderr;
		return false;
	}

	OmnString database_name = getCurDatabase(rdata);
	if (database_name == "")
	{
		rdata->setJqlMsg("ERROR: No database selected.");
		return false;
	}

#if 0
	AosXmlTagPtr query_conf = getQueryConf(rdata);
	if (!query_conf)
	{
		AosSetErrorUser(rdata, "AosJqlStmtQuery::run")
			<< "Failed to create query config." << enderr;
		aos_assert_r(NULL, false);
		return false;
	}

	u64 end_time = OmnGetTimestamp() - start_time;
	u64 proc_start = OmnGetTimestamp();
	AosXmlTagPtr data = getQueryRslt(rdata, query_conf);
	if (!data)
	{
		AosSetErrorUser(rdata, "AosJqlStmtQuery::run")
			<< "Failed to run query:"
			<< query_conf->toString() << enderr;
		OmnAlarm << enderr;
		return false;
	}

	u64 end_time = OmnGetTimestamp() - start_time;
	OmnString str = "";
	str << ((double)end_time)/1000;
	data->setAttr("create_query_conf", str);
#endif

	u64 start_time = OmnGetTimestamp();

	//init selectPtr
	//if (!mSelectPtr) return NULL;
	//if(mSelectPtr)
	//	rslt = mSelectPtr->init(rdata);


	AosDatasetObjPtr ds = NULL;
	rslt = runQuery(rdata, ds);
	if (!rslt)
	{
		OmnScreen << "Query Failed!" << endl;
		return false;
	}

	if (mSelectPtr && mSelectPtr->mHackConvertTo)
	{
		setPrintData(rdata, "run successful.");
		return true;
	}
	
	if (mSelectPtr && mSelectPtr->mHackIntoFile)
	{
		setPrintData(rdata, "run successful.");
		return true;
	}

	OmnString contents = "";
	AosXmlTagPtr data = NULL;

    aos_assert_r(ds,false);
	rslt = ds->data2Str(rdata, contents);
	data = AosXmlParser::parse(contents AosMemoryCheckerArgs);
	aos_assert_r(data, false);

	//for debugging
	OmnScreen << "====================== New Query Data" << endl;
	OmnScreen << contents << endl;
	OmnScreen << "====================== New Query Data" << endl;

	//set the query time
	OmnString str = "";
	str << ((double)(OmnGetTimestamp() - start_time))/1000;
	data->setAttr("proc_query", str);

	//use proc_query time as the query time
	data->setAttr("time", str);
	//contents = data->toString();

	//print the result in table format
	vector<OmnString> fields;
	rslt = setOutputFieldLis(rdata, data, fields);
	OmnString msg = printTableInXml(fields, mSelectPtr->mNameToAlias, data);
	if (mContentFormat == "xml")
	{
		data->setNodeName("content");
		//rdata->setResults(contents);
		rdata->setResults(data->toString());
		return true;
	}

	// get Query proc info
	int totalNums = 0;
	int currentNums = 0;

	// JIMODB-889
	// 2015/09/24
	OmnString query_proc_info = "";

	// jimodb-906
	/*
	bool isStat = false;
	if (mSelectPtr)
	{
		AosTableReferences *tableRef = mSelectPtr->mTableReferencesPtr;
		aos_assert_r(tableRef, false);

		AosJqlTableFactorPtr tableFactor = tableRef[0][0]->mTableFactor;
		aos_assert_r(tableFactor, false);
		
		tableFactor->IsStatTable(isStat);
	}
	*/
	string cost_time = data->getAttrStr("time");
	if(data->getFirstChild(true))
	{
		//totalNums = data->getAttrInt("total", 0);
		currentNums = data->getAttrInt("num", 0);
	}
	
	//if (isStat == false)
	//{
	//	query_proc_info << currentNums << " rows in set (" << cost_time << "), total " << totalNums;
	//}

	//else
	//{
		query_proc_info << currentNums << " rows in set (" << cost_time << ")";
	//}


	msg << query_proc_info;
	setPrintData(rdata, msg);
	return rslt;
}

bool
AosJqlStmtQuery::setOutputFieldLis(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &data,
		vector<OmnString> &fields)
{
	AosJqlSelectFieldPtr field;
	OmnString field_name, names;
	for(u32 i = 0; i < mSelectPtr->mFieldListPtr->size(); i++)
	{
		field = (*mSelectPtr->mFieldListPtr)[i];
		field_name = field->getFieldEscapeCName(rdata);
		fields.push_back(field_name);
		AosJqlUtil::escape(field_name);
		i == 0 ? names << field_name : names << "," << field_name;
	}
	data->setAttr("fieldnames", names);
	return true;
}


bool
AosJqlStmtQuery::procInto(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &data,
		const vector<OmnString> &fields)
{
	return intoOutfile(rdata, data, fields);
}


bool
AosJqlStmtQuery::intoOutfile(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &query,
		const vector<OmnString> &fields)
{
	OmnCout << "Into file \n" << query->toString() << endl;
	OmnString data;
	if (fields.empty()) return true;
	AosXmlTagPtr child_doc = query->getFirstChild(true);
	while (child_doc)
	{
		for (size_t i=0; i<fields.size(); i++)
		{
			OmnString fieldvalue = child_doc->getAttrStr(fields[i]);
			data << fieldvalue << "\t";
		}
		data << "\n";
		child_doc = query->getNextChild();
	}
	OmnCout << ">>>>>>>>>>>>>>>>>>>>>>\n" << data << endl;

	OmnString fname = mSelectPtr->mSelectIntoPtr->getFileName();
	int fd = open(fname.data(), O_WRONLY|O_CREAT, 0666);
	int nums = write(fd, data.data(), data.length());
	if (nums == -1)
	{
		OmnCout << "write file error" << endl;
		rdata->setJqlMsg("Into outfile error: write file error");
		return false;
	}
	close(fd);
	return true;
}


bool
AosJqlStmtQuery::doExpr(const AosRundataPtr &rdata)
{
	if (!mExprs)
	{
		AosSetErrorUser(rdata, "AosJqlStmtQuery::doExpr")
				<< "Expression is null." << enderr;
		aos_assert_r(mExprs, false);
		return false;
	}

	vector<OmnString> fields;
	map<OmnString, OmnString> nametoAlias;
	OmnString str;
	str << "<content total=\"1\" time=\"0.0ms\" >"
		<< "<record>";
	OmnString field_name, as_name, cname, names;
	for (u32 i = 0; i < mExprs->size(); i++)
	{
		AosJqlSelectFieldPtr field = (*mExprs)[i];
		if (!field)
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}
		field_name = field->dumpByNoEscape();
		if (field_name == "")
		{
			AosSetErrorUser(rdata,  __func__) << "internal error!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		AosJqlUtil::escape(field_name);
		cname = field_name;
		as_name = field->getAliasName(rdata);
		if (as_name != "")
		{
			AosJqlUtil::escape(as_name);
			cname = as_name;
			nametoAlias[field_name] = as_name;
		}

		i == 0 ? names << cname : names << "," << cname;
		fields.push_back(cname);

		AosValueRslt vv;
		bool rslt = field->getField()->getValue(rdata.getPtrNoLock(), NULL, vv);
		if (!rslt) 
		{
			AosSetErrorUser(rdata,  __func__) << "failed to get valuerslt!" << enderr;
			OmnAlarm << __func__ << enderr;
			return false;
		}

		OmnString vs;
		if (vv.isNull()) 
		{
			vs = vv.getNullStr();
		}
		else
		{
			vs = vv.getStr();
		}

		str << "<" << cname << ">"
			<< "<![CDATA[" << vs << "]]>"
			<< "</" << cname << ">";
	}
	str << "</record></content>";

	AosXmlTagPtr data = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
	data->setAttr("fieldnames", names);

	OmnString msg = printTableInXml(fields, nametoAlias, data);
	if (mContentFormat == "xml")
	{
		rdata->setJqlData(data->toString());
		return true;
	}
	setPrintData(rdata, msg);
	return true;
}


AosJqlStatement *
AosJqlStmtQuery::clone()
{
	return 0;
}


void
AosJqlStmtQuery::dump()
{
}


void
AosJqlStmtQuery::setExprs(AosJqlSelectFieldList* exprs)
{
	mExprs = exprs;
}


AosXmlTagPtr
AosJqlStmtQuery::getQueryConf(const AosRundataPtr &rdata)
{
	if (!mSelectPtr) return NULL;

	AosXmlTagPtr query_conf;
	if (mChild && mInputDataset)
	{
		//This query has a subquery
		//set mInputDataset to jqlselectTable to
		//generate the query conf
		AosJqlTableFactorPtr tableFactor = mSelectPtr->mTableReferencesPtr[0][0]->mTableFactor;
		if (!tableFactor->mTable)
		{
			tableFactor->mTable = AosJqlSingleTable::createStatic(rdata, mSelectPtr, tableFactor->mTableName, tableFactor->mAliasName, tableFactor->mIndexHint); 
			aos_assert_r(tableFactor->mTable,NULL);
		}
		//aos_assert_r(tableFactor->mTable, NULL);
		tableFactor->mTable->mInputDataset = mInputDataset;
	}
	bool rslt = mSelectPtr->init(rdata);
	OmnString str;
	str <<"<query opr=\"retlist\"  queryid = \"10\">";
	if (mSelectPtr->mHackConvertTo)
	{
		// 2015/9/16
		mSelectPtr->setSubQuery();

		AosExprObjPtr expr = mSelectPtr->mHackConvertTo;
		aos_assert_r(expr->getType() == AosExprType::eGenFunc, query_conf);
		AosExprGenFuncPtr func_expr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());
		aos_assert_r(func_expr, query_conf);
		vector<AosExprObjPtr> *parms =  func_expr->getParmList();
		str << "<convert name=\"" << func_expr->getFuctName() << "\">";
		for (u32 i = 0; i < parms->size(); i++)
		{
			AosExprNameValuePtr parm = dynamic_cast<AosExprNameValue*>((*parms)[i].getPtr());
			aos_assert_r(parm, query_conf)
			str << "<arg name=\"" << parm->getName() << "\"><![CDATA[" << parm->getValueAsExpr()->getValue(rdata.getPtr()) << "]]></arg>";
		}
		str << "</convert>";
	}
	if (mSelectPtr->mHackFormat)
	{
		// 2015/9/16
		mSelectPtr->setSubQuery();

		AosExprObjPtr expr = mSelectPtr->mHackFormat;
		aos_assert_r(expr->getType() == AosExprType::eGenFunc, query_conf);
		AosExprGenFuncPtr func_expr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());
		aos_assert_r(func_expr, query_conf);
		vector<AosExprObjPtr> *parms =  func_expr->getParmList();
		str << "<format name=\"" << func_expr->getFuctName() << "\">";
		for (u32 i = 0; i < parms->size(); i++)
		{
			AosExprNameValuePtr parm = dynamic_cast<AosExprNameValue*>((*parms)[i].getPtr());
			aos_assert_r(parm, query_conf)
			str << "<arg name=\"" << parm->getName() << "\"><![CDATA[" << parm->getValueAsExpr()->getValue(rdata.getPtr()) << "]]></arg>";
		}
		str << "</format>";
	}
	if (mSelectPtr->mHackIntoFile)
	{
		// 2015/9/16
		mSelectPtr->setSubQuery();

		AosExprObjPtr expr = mSelectPtr->mHackIntoFile;
		aos_assert_r(expr->getType() == AosExprType::eGenFunc, query_conf);
		AosExprGenFuncPtr func_expr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());
		aos_assert_r(func_expr, query_conf);
		vector<AosExprObjPtr> *parms =  func_expr->getParmList();
		str << "<into_file name=\"" << func_expr->getFuctName() << "\">";
		for (u32 i = 0; i < parms->size(); i++)
		{
			AosExprNameValuePtr parm = dynamic_cast<AosExprNameValue*>((*parms)[i].getPtr());
			aos_assert_r(parm, query_conf)
			str << "<arg name=\"" << parm->getName() << "\"><![CDATA[" << parm->getValueAsExpr()->getValue(rdata.getPtr()) << "]]></arg>";
		}
		str << "</into_file>";
	}
	str << "</query>";
	query_conf = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
	rslt = mSelectPtr->generateQueryConf(rdata, query_conf);
	if (!rslt) return NULL;

	OmnScreen << "====================== New Query Config" << endl;
	OmnScreen << query_conf->toString() << endl;
	OmnScreen << "====================== New Query Config" << endl;

	return query_conf;
}


AosXmlTagPtr
AosJqlStmtQuery::getQueryRslt(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &query_conf)
{
	if (!query_conf) return NULL;

	return AosJqlStatement::executeQuery(rdata, query_conf);
}

bool
AosJqlStmtQuery::setSubQuery()
{
	if (mSelectPtr)
	{
		AosTableReferences *tableRef = mSelectPtr->mTableReferencesPtr;
		aos_assert_r(tableRef, false);

		AosJqlTableFactorPtr tableFactor = tableRef[0][0]->mTableFactor;
		aos_assert_r(tableFactor, false);

		AosJqlSingleTablePtr table = tableFactor->mTable;
		table->mIsSubQuery = true;
	}

	return true;
}
//
//execuate a query and return the result dataset
//
bool
AosJqlStmtQuery::runQuery(
		const AosRundataPtr &rdata,
		AosDatasetObjPtr &ds)
{
	bool rslt;
/*
	//sanity checking
	if (!mSelectPtr)
	{
		AosSetErrorUser(rdata, "AosJqlStmtQuery::run")
				<< "This is BUG, mSelectPtr is null." << enderr;
		aos_assert_r(NULL, false);
		return false;
	}
*/
	//get mChild if any
	if (mSelectPtr)
	{
		AosTableReferences *tableRef = mSelectPtr->mTableReferencesPtr;
		aos_assert_r(tableRef, false);
		AosJqlTableFactorPtr tableFactor = tableRef[0][0]->mTableFactor;

		//right now, we only support single table, so mTableFactor should not be empty
		aos_assert_r(tableFactor, false);
		mChild = tableFactor->mQuery;
	
		//if query is subQuery,cann't get the table doc,because it doesn't generate that doc	
		if(!mChild)
		{
			//check whether the table exist or not 
			OmnString tableName = tableFactor->mTableName->dumpByNoEscape();
			rslt = checkTableNameExist(rdata, tableName);	
		}
	}
	if (mChild)
	{
		//handle the subquery firstly if any
		//So far, we assume there is only one
		//subquery for a specified primary query
		//init selectPtr
		if (!mChild->mSelectPtr) return NULL;
		bool rslt = mChild->mSelectPtr->init(rdata);

		// 2015/09/15
		//mChild->setSubQuery();
		mChild->mSelectPtr->setSubQuery();
		rslt = mChild->runQuery(rdata, mInputDataset);
		if (!rslt)
		{
			OmnScreen << "Err: failed to run subquery." << endl;
			return false;
		}
	}

	//get the query config xml
	//if(mSelectPtr)
	//	rslt = mSelectPtr->init(rdata);
	
	AosXmlTagPtr query_conf = getQueryConf(rdata);
	if (!query_conf)
	{
		AosSetErrorUser(rdata, "AosJqlStmtQuery::run")
			<< "Failed to create query config." << enderr;
		aos_assert_r(NULL, false);
		return false;
	}

	//get the query result dataset
	AosQueryClient *query_client = AosQueryClient::getSelf();
    aos_assert_r(query_client, false);
	if (!mChild)
	{
		//this is a normal query
		rslt = query_client->executeQuery(query_conf, ds, rdata);
		aos_assert_r(rslt, false);
	}
	else
	{
		//primary query based on the result of
		//the child query, that is mInputDataset
		rslt = executeQuery(query_conf, mInputDataset, ds, rdata);
		aos_assert_r(rslt, false);
	}

	if (!ds)
	{
		AosSetErrorUser(rdata, "AosJqlStmtQuery::run")
			<< "Failed to run query:"
			<< query_conf->toString() << enderr;
		OmnAlarm << enderr;
		return false;
	}

	return true;
}
//
//jimodb-671 Phil
//
bool
AosJqlStmtQuery::executeQuery(
		const AosXmlTagPtr &def,
		const AosDatasetObjPtr &inputDataset,
		AosDatasetObjPtr &dataset,
		const AosRundataPtr &rdata)
{
	dataset = 0;
	aos_assert_r(def, false);

	OmnString queryid = def->getAttrStr("queryid");
	aos_assert_r(queryid != "", false);

	AosXmlTagPtr table = def->getFirstChild("table");
	aos_assert_r(table, false);

	dataset = AosCreateDataset(rdata.getPtr(), table);
	aos_assert_r(dataset, false);

	OmnScreen << "Primary query: started" << endl;

	//set input dataseat
	//AosDatasetByQueryNest *datasetQuery = (AosDatasetByQueryNest*)dataset.getPtr();
	//datasetQuery->setScanDataset(inputDataset);
	AosDatasetByQuery *datasetQuery = (AosDatasetByQuery *)dataset.getPtr();
	datasetQuery->setScanDataset(inputDataset);

	//config the dataset
	bool rslt = dataset->config(rdata, table);
	aos_assert_r(rslt, false);

	//start the dataset
	rslt = datasetQuery->sendStart(rdata);
	aos_assert_r(rslt, false);

	OmnScreen << "Primary query: complete sendStart" << endl;

	// jimodb-858
	AosXmlTagPtr convert_tag = def->getFirstChild("convert");
	//Jozhi JIMODB-688
	if (convert_tag)
	{
		AosDatasetObjPtr output_dataset;
		rslt = AosDataset::convert(rdata.getPtr(), 
				output_dataset, convert_tag, dataset);
		aos_assert_r(rslt, false);
		dataset = output_dataset;

		OmnScreen << "Primary query: complete convert operation" << endl;
	}

	AosXmlTagPtr tag = def->getFirstChild("format");
	if (tag)
	{
		vector<AosDataRecordObjPtr> records;
		rslt = dataset->getRecords(records);
		aos_assert_r(rslt, false);
		aos_assert_r(records.size() > 0 && records[0], false);

		AosXmlTagPtr recordDoc = records[0]->getRecordDoc();
		aos_assert_r(recordDoc, false);

		AosQueryFormatObjPtr queryFormat = AosQueryFormatObj::createQueryFormatStatic(tag, rdata.getPtr());
		aos_assert_r(queryFormat, false);

		rslt = queryFormat->config(tag, recordDoc, rdata.getPtr());
		aos_assert_r(rslt, false);

		tag = def->getFirstChild("into_file");
		if (tag)
		{
			AosQueryIntoObjPtr queryInto = AosQueryIntoObj::createQueryIntoStatic(tag, rdata.getPtr());
			aos_assert_r(queryInto, false);
			
			rslt = queryInto->config(tag, rdata.getPtr());
			if(!rslt) return false;

			queryFormat->setQueryInto(queryInto);

			rslt = queryFormat->proc(dataset, rdata.getPtr());
			aos_assert_r(rslt, false);
		}

		OmnScreen << "Primary query: complete format operation" << endl;
	}

	return true;
}

bool
AosJqlStmtQuery::checkTableNameExist(
	const AosRundataPtr &rdata,
	const OmnString &tableName)
{
	//filt show tables 
	if(tableName == "jobctnr") return true;
	if(tableName == "task_ctnr") return true;
	
	OmnString objName = getObjid(rdata, JQLTypes::eTableDoc, tableName);
//	AosXmlTagPtr tableDoc = AosJqlStatement::getDoc(rdata, JQLTypes::eTableDoc, tableName);
	AosXmlTagPtr tableDoc = AosJqlStatement::getDocByObjid(rdata, objName);

	aos_assert_r(tableDoc, false);	
	if(!tableDoc)
	{
		rdata->setJqlMsg("Table doesn't exist!");
		return false;
	}
	return true;
}
