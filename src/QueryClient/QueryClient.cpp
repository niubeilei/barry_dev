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
//  This class is used to manage queries. Each query that involves
//  multiple queries will create a query session. It is created
//  by this class. When a subsequent query is received, it retrieves
//  the query session from this class. When a query finishes, 
//  the query is removed. 
//
//  We assume all the subsequent queries will hit the same server. 
//  For this reason, there is no need to share the files (that store
//  query sessions with others). 
//
//  Query sessions will expire automatically after a given period of
//  time (configurable). 
//
//  The class maintains a circular queue. When requesting a new query,
//  it checks whether the queue is full. If not, it uses the next slot
//  to store the query. Otherwise, it kicks off the oldest one and 
//  replaces it with a newly created one.
//
//  A map is used to map query IDs (strings) to the iterator pointing 
//  to the query list. When a query is used, it is removed from the list
//  and inserted in front of the query list. 
//
// Modification History:
// 01/02/2011 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryClient/QueryClient.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/DatasetObj.h"
#include "Dataset/Jimos/DatasetByQuery.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/QueryFormatObj.h"
#include "SEInterfaces/QueryIntoObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosQueryClientSingleton,
                 AosQueryClient,
                 AosQueryClientSelf,
                "AosQueryClient");


AosQueryClient::AosQueryClient()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mQueryId(0),
mCurrQueryId(1000)
{
}


AosQueryClient::~AosQueryClient()
{
}


bool
AosQueryClient::start()
{
	return true;
}


bool
AosQueryClient::stop()
{
    if(mThread.isNull())
    {
        return true;
    }
    mThread->stop();
    return true;
}


bool
AosQueryClient::config(const AosXmlTagPtr &def)
{
	return true;
}


bool    
AosQueryClient::signal(const int threadLogicId)
{
	return true;
}


bool    
AosQueryClient::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosQueryClient::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(100);
	}
	return true;
}


AosQueryReqObjPtr
AosQueryClient::createQuery(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	/*
	try
	{
		mLock->lock();
		AosQueryReqPtr trans;
		OmnString queryid = def->getAttrStr(AOSTAG_QUERYID);

		map<OmnString , list<AosQueryReqPtr>::iterator>::iterator itr = mQryReqsMap.find(queryid);
		if (itr != mQryReqsMap.end())
		{
			trans = *itr->second;
			mQryReqs.erase(itr->second);
			mQryReqs.push_front(trans);
			itr->second = mQryReqs.begin();
			mLock->unlock();
			return trans;
		}
		queryid = "";

		if (mQryReqsMap.size() > eMaxQueries)
		{
			trans = mQryReqs.back();
			mQryReqs.pop_back();
			queryid << trans->getQueryId();
			mQryReqsMap.erase(queryid);
			queryid = "";
		}

		trans = OmnNew AosQueryReq(def, rdata);
		trans->setQueryId(mQueryId++);
		if (!trans->isGood()) 
		{
			mLock->unlock();
			return 0;
		}
		queryid << trans->getQueryId();
		mQryReqs.push_front(trans);
		mQryReqsMap.insert(make_pair(queryid, mQryReqs.begin()));
		mLock->unlock();
		return trans;
	}

	catch (exception &e)
	{
		OmnAlarm << "Failed to create query: "
			<< def->toString() << enderr;
		return 0;
	}
	
	OmnShouldNeverComeHere;
	return 0;
	*/
	
	AosQueryReqObjPtr query;
	OmnString queryid = def->getAttrStr(AOSTAG_QUERYID);

	mLock->lock();
	// Chen Ding, 2014/04/10
	// There is no query ID in the query. Need to calculate the
	// query's SH1/MD5 with 'start_idx' and 'pagesize' removed.
	// Use the hash as the key to see whether it is a cached query.
	if (queryid != "" && false)
	{
		mapitr_t itr = mQryReqsMap.find(queryid);
		if (itr != mQryReqsMap.end())
		{
			query = *itr->second;
			mQryReqs.erase(itr->second);
			mQryReqs.push_front(query);
			itr->second = mQryReqs.begin();
			mLock->unlock();
			return query;
		}
	}

	// The query is not cached. 
	queryid = "";

	if (mQryReqsMap.size() > eMaxQueries)
	{
		query = mQryReqs.back();
		mQryReqs.pop_back();
		queryid << query->getQueryId();
		mQryReqsMap.erase(queryid);
		queryid = "";
	}

	mLock->unlock();
		
	query = AosQueryReqObj::createQueryStatic(def, rdata);
	aos_assert_r(query, 0);
	
	mLock->lock();
	query->setQueryId(mQueryId++);
	if (!query->isGood()) 
	{
		mLock->unlock();
		return 0;
	}

	queryid << query->getQueryId();
	mQryReqs.push_front(query);
	mQryReqsMap.insert(make_pair(queryid, mQryReqs.begin()));
	mLock->unlock();

	return query;
}


bool	
AosQueryClient::queryFinished(
		const AosQueryReqObjPtr &query,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query, false);
	mLock->lock();
	OmnString queryid;
	queryid <<  query->getQueryId();
	map<OmnString , list<AosQueryReqObjPtr>::iterator>::iterator itr = mQryReqsMap.find(queryid);
	if (itr != mQryReqsMap.end())
	{
		list<AosQueryReqObjPtr>::iterator list_itr = itr->second;
		mQryReqsMap.erase(queryid);
		mQryReqs.erase(list_itr);
	}
	mLock->unlock();
 	return true;
}


bool
AosQueryClient::executeQuery(
		const AosXmlTagPtr &def,
		AosDatasetObjPtr &dataset,
		const AosRundataPtr &rdata)
{
	dataset = 0;
	aos_assert_r(def, false);

	OmnString queryid = def->getAttrStr("queryid");
	aos_assert_r(queryid != "", false);

	AosXmlTagPtr table = def->getFirstChild("table");
	aos_assert_r(table, false);

	AosXmlTagPtr convert_tag = def->getFirstChild("convert");

/*
	//Jozhi JIMODB-688
	AosXmlTagPtr convert_tag = def->getFirstChild("convert");
	if (convert_tag)
	{
		OmnString zky_field_delimiter = ",";
		OmnString zky_text_qualifier = "DQM";
		OmnString zky_row_delimiter = "LF";

		AosXmlTagPtr outputrecord = def->xpathGetChild("table/outputrecord");
		AosXmlTagPtr datarecord = outputrecord->getFirstChild("datarecord");
		datarecord->setAttr("type", "csv");
		datarecord->setAttr("zky_field_delimiter", zky_field_delimiter);
		datarecord->setAttr("zky_text_qualifier", zky_text_qualifier);
		datarecord->setAttr("zky_row_delimiter", zky_row_delimiter);

		AosXmlTagPtr datafields = datarecord->getFirstChild("datafields");
		AosXmlTagPtr datafield = datafields->getFirstChild(true);
		while(datafield)                                       
		{
			datafield->setAttr("type", "str");
			datafield->removeAttr("zky_xpath");
			datafield = datafields->getNextChild();
		}
	}
*/
	dataset = AosCreateDataset(rdata.getPtr(), table);
	aos_assert_r(dataset, false);
	bool rslt = dataset->config(rdata, table);
	aos_assert_r(rslt, false);
	
	rslt = dataset->sendStart(rdata);
	aos_assert_r(rslt, false);

	//Jozhi JIMODB-688
	if (convert_tag)
	{
		AosDatasetObjPtr output_dataset;
		rslt = AosDataset::convert(rdata.getPtr(), 
				output_dataset, convert_tag, dataset);
		aos_assert_r(rslt, false);
		dataset = output_dataset;
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
	}

	return true;
}


bool
AosQueryClient::executeQuery(
		const AosXmlTagPtr &def,
		AosRecordsetObjPtr &recordset,	
		const AosRundataPtr &rdata)
{
	recordset = 0;
	aos_assert_r(def, false);

	AosDatasetObjPtr dataset;
	bool rslt = executeQuery(def, dataset, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(dataset, false);

	rslt = dataset->nextRecordset(rdata, recordset);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosQueryClient::executeQuery(
		const AosXmlTagPtr &def,
		OmnString &contents,
		const AosRundataPtr &rdata)
{
	bool rslt;
	contents = "";
	aos_assert_r(def, false);

	AosDatasetObjPtr dataset;
	rslt = executeQuery(def, dataset, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(dataset, false);

	rslt = dataset->data2Str(rdata,contents);
	aos_assert_r(rslt, true);
	return true;
}

bool
AosQueryClient::executeQuery(
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
	bool rslt = dataset->config(rdata, table);
	aos_assert_r(rslt, false);

	//set input dataset
	AosDatasetByQuery *datasetQuery = (AosDatasetByQuery*)dataset.getPtr();
	datasetQuery->setScanDataset(inputDataset);
	rslt = datasetQuery->sendStart(rdata);
	aos_assert_r(rslt, false);

	return true;
}

u64	
AosQueryClient::getQueryId(
		const OmnString &query,
		const OmnString &session_id,
		const AosRundataPtr &rdata)
{
	u64 query_id = 0;
	OmnString str, md5;
	str << query << session_id;
	md5 = AosMD5Encrypt(str);
	mLock->lock();
	if (mQueryIdMap.count(md5) == 0)
	{
		mQueryIdMap[md5] = ++mCurrQueryId;
	}
	query_id = mQueryIdMap[md5];
	mLock->lock();
	return query_id;
}

