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
// 2015/12/13 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcOnNetOffNet.h"

#include "API/JimoCreators.h"

#include "boost/date_time/gregorian/gregorian.hpp"   
using namespace boost::gregorian;


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcOnNetOffNet_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcOnNetOffNet(version);
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


AosDataProcOnNetOffNet::AosDataProcOnNetOffNet(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcOnNetOffNet),
mRawUserIdExpr(0),
mRawTimeExpr(0),
mRawOnNetOutputRecord(0),
mRawOffNetOutputRecord(0)
{
}


AosDataProcOnNetOffNet::AosDataProcOnNetOffNet(const AosDataProcOnNetOffNet &proc)
:
AosStreamDataProc(proc),
mRawUserIdExpr(0),
mRawTimeExpr(0),
mRawOnNetOutputRecord(0),
mRawOffNetOutputRecord(0)
{
	if (proc.mUserIdExpr)
	{
		mUserIdExpr = proc.mUserIdExpr->cloneExpr();
		mRawUserIdExpr = mUserIdExpr.getPtr();
	}
	if (proc.mTimeExpr)
	{
		mTimeExpr = proc.mTimeExpr->cloneExpr();
		mRawTimeExpr = mTimeExpr.getPtr();
	}
	mTimeUnit = proc.mTimeUnit;
	mShreshold = proc.mShreshold;
	mFirstDay = proc.mFirstDay;
	mCrtDay = proc.mCrtDay;
	mLastDay = proc.mLastDay;

	if (proc.mOnNetOutputRecord)
	{
		mOnNetOutputRecord = proc.mOnNetOutputRecord->clone(0 AosMemoryCheckerArgs);
		mRawOnNetOutputRecord = mOnNetOutputRecord.getPtr();
	}
	if (proc.mOffNetOutputRecord)
	{
		mOffNetOutputRecord = proc.mOffNetOutputRecord->clone(0 AosMemoryCheckerArgs);
		mRawOffNetOutputRecord = mOffNetOutputRecord.getPtr();
	}
	if (proc.mDatasetConf)
		mDatasetConf = proc.mDatasetConf->clone(AosMemoryCheckerArgsBegin);
}


AosDataProcOnNetOffNet::~AosDataProcOnNetOffNet()
{
}


bool
AosDataProcOnNetOffNet::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	//	"type": "onnetoffnet",
	//	"record_type":"fixbin|buff",
	//	 user_id_field:"xxxx",
	//	 time_field:"xxxx",
	//	 time_unit:"xxxx", //default:"%Y-%m-%d"
	//	 shreshold:90, //defalut:90
	//	 start_day:"2012-07-03",
	//	 end_day:"2012-07-05"
	//}
	//]]></data_proc>
	try
	{
		aos_assert_r(def, false);
		OmnString dp_jsonstr = def->getNodeText();
		mName = def->getAttrStr(AOSTAG_NAME, "");
		aos_assert_r(mName != "", false);

		JSONValue json;
		JSONReader reader;
		bool rslt = reader.parse(dp_jsonstr, json);
		aos_assert_r(rslt, false);

		OmnString user_id_field = json["user_id_field"].asString();
		aos_assert_r(user_id_field != "", false);
		mUserIdExpr = convertToExpr(user_id_field, rdata);
		aos_assert_r(mUserIdExpr, false);
		mRawUserIdExpr = mUserIdExpr.getPtr();

		OmnString time_field = json["time_field"].asString();
		aos_assert_r(time_field != "", false);
		mTimeExpr = convertToExpr(time_field, rdata);
		aos_assert_r(mTimeExpr, false);
		mRawTimeExpr = mTimeExpr.getPtr();

		mTimeUnit = json["time_unit"].asString();
		aos_assert_r(mTimeUnit != "", false);

		mShreshold = json["shreshold"].asUInt();
		aos_assert_r(mShreshold > 0, false);

		OmnString start_day = json["start_day"].asString();
		aos_assert_r(start_day != "", false);
		mFirstDay = AosDateTime(start_day, mTimeUnit);
		aos_assert_r(!(mFirstDay.isNotADateTime()), false);

		mCrtDay = mFirstDay;
	
		OmnString end_day = json["end_day"].asString();
		aos_assert_r(end_day != "", false);
		mLastDay = AosDateTime(end_day, mTimeUnit);
		aos_assert_r(!(mLastDay.isNotADateTime()), false);

		//create output record
		rslt = createOutput(mName, json, rdata);
		aos_assert_r(rslt, false);

		aos_assert_r(mTaskDocid, false);
		AosXmlTagPtr task_doc = AosGetDocByDocid(mTaskDocid, rdata);
		aos_assert_r(task_doc, false);
		AosXmlTagPtr datasets_conf = task_doc->getFirstChild("input_datasets");
		aos_assert_r(datasets_conf, false);
		AosXmlTagPtr dataset_conf = datasets_conf->getFirstChild("dataset");
		aos_assert_r(dataset_conf, false);

		mDatasetConf = dataset_conf->clone(AosMemoryCheckerArgsBegin);

		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}


bool
AosDataProcOnNetOffNet::createOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	bool rslt = createOnNetOutput(dpname, json_conf, rdata);
	aos_assert_r(rslt, false);

	rslt = createOffNetOutput(dpname, json_conf, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDataProcOnNetOffNet::createOnNetOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	OmnString name = "";
	name << dpname << "_onnet_output";

	int len = json_conf["max_keylen"].asInt();
	if (len <= 0) len = 50;

	OmnString type_str = json_conf["record_type"].asString();
	if (type_str == "")
		type_str = AOSRECORDTYPE_FIXBIN;

	OmnString user_id_field = json_conf["user_id_field"].asString();
	OmnString time_field = json_conf["time_field"].asString();

	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	output->setField(time_field, AosDataFieldType::eStr, len);
	output->setField(user_id_field, AosDataFieldType::eStr, len);

	output->init(mTaskDocid, rdata);
	mOnNetOutputRecord = output->getRecord();
	mRawOnNetOutputRecord = mOnNetOutputRecord.getPtr();
	mOutputs.push_back(output);
	return true;
}


bool
AosDataProcOnNetOffNet::createOffNetOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	OmnString name = "";
	name << dpname << "_offnet_output";

	int len = json_conf["max_keylen"].asInt();
	if (len <= 0) len = 50;

	OmnString type_str = json_conf["record_type"].asString();
	if (type_str == "")
		type_str = AOSRECORDTYPE_FIXBIN;

	OmnString user_id_field = json_conf["user_id_field"].asString();
	OmnString time_field = json_conf["time_field"].asString();

	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	output->setField(time_field, AosDataFieldType::eStr, len);
	output->setField(user_id_field, AosDataFieldType::eStr, len);

	output->init(mTaskDocid, rdata);
	mOffNetOutputRecord = output->getRecord();
	mRawOffNetOutputRecord = mOffNetOutputRecord.getPtr();
	mOutputs.push_back(output);
	return true;
}		


AosDataProcStatus::E
AosDataProcOnNetOffNet::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt = runJoin(rdata_raw);
	aos_assert_r(rslt, AosDataProcStatus::eError);
	
	return AosDataProcStatus::eStop;
}


/*
bool
AosDataProcOnNetOffNet::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	OmnScreen << "mCrtDay:" << mCrtDay << endl;

	OmnString key;
	map<OmnString, int> crt_map;
	map<OmnString, int>::iterator map_itr;
	set<OmnString> begin_set, end_set;
	set<OmnString>::iterator set_itr;

	OmnScreen << "create begin set start:" << (mCrtDay - sgDayInterval) << endl;
	rslt = createSet(mCrtDay - sgDayInterval, begin_set, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create begin set success:" << (mCrtDay - sgDayInterval) << ",set_size:" << (begin_set.size()) << endl;

	OmnScreen << "create crt map start:" << (mCrtDay - sgDayInterval) << endl;
	rslt = createMap(mCrtDay - sgDayInterval, crt_map, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create crt map success:" << (mCrtDay - sgDayInterval) << ",map_size:" << (crt_map.size()) << endl;

	OmnScreen << "create end set start:" << mCrtDay << endl;
	rslt = createSet(mCrtDay, end_set, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create end set success:" << mCrtDay << ",set_size:" << (end_set.size()) << endl;

	while (mCrtDay <= mLastDay)
	{
		for (set_itr = begin_set.begin(); set_itr != begin_set.end(); set_itr++)
		{
			key = *set_itr;
			if (crt_map.count(key) <= 0 && end_set.count(key) <= 0)
			{
				rslt = appendOutNetEntry(key, mCrtDay, rdata);
				aos_assert_r(rslt, false);
			}
		}

		for (set_itr = end_set.begin(); set_itr != end_set.end(); set_itr++)
		{
			key = *set_itr;
			if (crt_map.count(key) <= 0 && begin_set.count(key) <= 0)
			{
				rslt = appendInNetEntry(key, mCrtDay, rdata);
				aos_assert_r(rslt, false);
			}
		}

		mCrtDay++;
		if (mCrtDay > mLastDay)
		{
			break;
		}
		OmnScreen << "mCrtDay:" << mCrtDay << endl;

		OmnScreen << "create begin set start:" << (mCrtDay - sgDayInterval) << endl;
		rslt = createSet(mCrtDay - sgDayInterval, begin_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "create begin set success:" << (mCrtDay - sgDayInterval) << ",set_size:" << (begin_set.size()) << endl;

		OmnScreen << "remove begin set start" << endl;
		rslt = removeSet(crt_map, begin_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "remove begin set success" << endl;

		OmnScreen << "add end set start" << endl;
		rslt = addSet(crt_map, end_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "add end set success" << endl;

		OmnScreen << "create end set start:" << mCrtDay << endl;
		rslt = createSet(mCrtDay, end_set, rdata);
		aos_assert_r(rslt, false);
		OmnScreen << "create end set success:" << mCrtDay << ",set_size:" << (end_set.size()) << endl;
	}

	if (mNeedCity)
	{ 
		AosValueRslt value_rslt;
		map<OmnString, int>::iterator itr = mInNetStMap.begin();
		while (itr != mInNetStMap.end())
		{
			key = itr->first;
			value_rslt.setKeyValue(key.data(), key.length(), true, itr->second);
			rslt = mInNetStIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			itr++;
		}

		itr = mOutNetStMap.begin();
		while (itr != mOutNetStMap.end())
		{
			key = itr->first;
			value_rslt.setKeyValue(key.data(), key.length(), true, itr->second);
			rslt = mOutNetStIILAsm->appendEntry(value_rslt, rdata.getPtrNoLock());
			aos_assert_r(rslt, false);
			itr++;
		}
	}

	return true;
}
*/

bool
AosDataProcOnNetOffNet::runJoin(
		AosRundata *rdata)
{
	//OmnScreen << "mCrtDay:" << mCrtDay.toString() << endl;

	map<OmnString, int> crt_map;
	set<OmnString> begin_set, end_set;

	days d(mShreshold);
	ptime pt = mCrtDay.getPtime() - d;
	AosDateTime begin_day(pt, mTimeUnit);
	OmnString day = begin_day.toString();

	OmnScreen << "create begin set start:" << day << endl;
	bool rslt = createSet(day, begin_set, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create begin set success:" << day << ",set_size:" << (begin_set.size()) << endl;

	OmnScreen << "create crt map start:" << day << endl;
	rslt = createMap(crt_map, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create crt map success:" << day << ",map_size:" << (crt_map.size()) << endl;

	day = mCrtDay.toString();
	OmnScreen << "create end set start:" << day << endl;
	rslt = createSet(day, end_set, rdata);
	aos_assert_r(rslt, false);
	OmnScreen << "create end set success:" << day << ",set_size:" << (end_set.size()) << endl; 
	OmnString key;
	set<OmnString>::iterator set_itr;
	while (mCrtDay <= mLastDay)
	{
		for (set_itr = begin_set.begin(); set_itr != begin_set.end(); set_itr++)
		{
			key = *set_itr;
			if (crt_map.count(key) <= 0 && end_set.count(key) <= 0)
			{
				rslt = appendOffNetEntry(key, mCrtDay.toString(), rdata);
				aos_assert_r(rslt, false);
			}
		}

		for (set_itr = end_set.begin(); set_itr != end_set.end(); set_itr++)
		{
			key = *set_itr;
			if (crt_map.count(key) <= 0 && begin_set.count(key) <= 0)
			{
				rslt = appendOnNetEntry(key, mCrtDay.toString(), rdata);
				aos_assert_r(rslt, false);
			}
		}

		days d(1);
		mCrtDay.getPtime() += d;
		if (mCrtDay > mLastDay)
		{
			break;
		}
		//OmnScreen << "mCrtDay:" << mCrtDay.toString() << endl;

		begin_day.getPtime() += d;
		day = begin_day.toString();
		//OmnScreen << "create begin set start:" << day << endl;
		rslt = createSet(day, begin_set, rdata);
		aos_assert_r(rslt, false);
		//OmnScreen << "create begin set success:" << day << ",set_size:" << (begin_set.size()) << endl;

		//OmnScreen << "remove begin set start" << endl;
		rslt = removeSet(crt_map, begin_set, rdata);
		aos_assert_r(rslt, false);
		//OmnScreen << "remove begin set success" << endl;

		//OmnScreen << "add end set start" << endl;
		rslt = addSet(crt_map, end_set, rdata);
		aos_assert_r(rslt, false);
		//OmnScreen << "add end set success" << endl;

		//OmnScreen << "create end set start:" << mCrtDay.toString() << endl;
		rslt = createSet(mCrtDay.toString(), end_set, rdata);
		aos_assert_r(rslt, false);
		//OmnScreen << "create end set success:" << mCrtDay.toString() << ",set_size:" << (end_set.size()) << endl;
	}
	return true;
}


bool
AosDataProcOnNetOffNet::getNextKey(
		OmnString &key,
		OmnString &day,
		AosDataRecordObj *input_record,
		const AosRundataPtr &rdata)
{
	key = "";
	day = "";

	AosValueRslt value_rslt;
	bool rslt = mTimeExpr->getValue(rdata.getPtr(), input_record, value_rslt);
	aos_assert_r(rslt, false);
	day = value_rslt.getStr();

	rslt = mUserIdExpr->getValue(rdata.getPtr(), input_record, value_rslt);
	aos_assert_r(rslt, false);
	key = value_rslt.getStr();
	return true;
}


bool
AosDataProcOnNetOffNet::createMap(
		map<OmnString, int> &crt_map,
		const AosRundataPtr &rdata)
{
	crt_map.clear();

	days d(mShreshold - 1);
	ptime pt = mCrtDay.getPtime() - d;
	AosDateTime begin_day(pt, mTimeUnit);
	OmnString value1 = begin_day.toString();
	OmnString value2 = mCrtDay.toString();

	aos_assert_r(mDatasetConf, false);
	AosXmlTagPtr datasetconf = mDatasetConf->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(datasetconf, false);
	AosXmlTagPtr scanner = datasetconf->getFirstChild("datascanner");
	aos_assert_r(scanner, false);
	AosXmlTagPtr connector = scanner->getFirstChild("dataconnector");
	aos_assert_r(connector, false);
	AosXmlTagPtr query_conf = connector->getFirstChild("query_cond");
	aos_assert_r(query_conf, false);
	query_conf->setAttr(AOSTAG_OPR, AosOpr_toStr(eAosOpr_range_ge_lt));
	AosXmlTagPtr value_conf = query_conf->getFirstChild("zky_value");
	aos_assert_r(value_conf, false);
	value_conf->setNodeText(value1, true);
	AosXmlTagPtr value2_conf = query_conf->getFirstChild("zky_value2");
	if (!value2_conf)
	{
		query_conf->addNode1("zky_value2");
		value2_conf = query_conf->getFirstChild("zky_value2");
		aos_assert_r(value2_conf, false);
	}
	value2_conf->setNodeText(value2, true);

	AosDatasetObjPtr dataset = AosCreateDataset(rdata.getPtr(), datasetconf);
	dataset->config(rdata, datasetconf);
	dataset->sendStart(rdata);

	AosRecordsetObjPtr record_set = NULL;
	AosRecordsetObj *record_set_raw = NULL;
	AosDataRecordObj * input_record = NULL;

	//OmnString dd;
	OmnString day;
	OmnString key;
	bool rslt = false;
	map<OmnString, int>::iterator itr;
	while (1)
	{
		rslt = dataset->nextRecordset(rdata, record_set);
		aos_assert_r(rslt, false);

		if (AosRecordsetObj::checkEmpty(record_set))
			return scanFinished(dataset, rdata);

		record_set_raw = record_set.getPtr();
		while(1)
		{
			rslt = record_set_raw->nextRecord(rdata.getPtr(), input_record);
			aos_assert_r(rslt, false);
			if (!input_record)
				break;
			rslt = getNextKey(key, day, input_record, rdata);
			if (!rslt || day < value1 || day >= value2)
			{
				OmnAlarm << "error" << enderr;
				continue;
			}
			/*
			if (day != dd)
			{
				OmnScreen << "map crt day:" << day << endl;
				dd = day;
			}
			*/

			itr = crt_map.find(key);
			if (itr != crt_map.end())
			{
				itr->second++;
			}
			else
			{
				crt_map[key] = 1;
			}
		}
	}
	return false;
}


bool
AosDataProcOnNetOffNet::scanFinished(
		const AosDatasetObjPtr &dataset,
		const AosRundataPtr &rdata)
{
	dataset->sendFinish(rdata);
	return true;
}


bool
AosDataProcOnNetOffNet::createSet(
		const OmnString &crt_day,
		set<OmnString> &crt_set,
		const AosRundataPtr &rdata)
{
	crt_set.clear();

	aos_assert_r(mDatasetConf, false);
	AosXmlTagPtr datasetconf = mDatasetConf->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(datasetconf, false);

	AosXmlTagPtr scanner = datasetconf->getFirstChild("datascanner");
	aos_assert_r(scanner, false);
	AosXmlTagPtr connector = scanner->getFirstChild("dataconnector");
	aos_assert_r(connector, false);
	AosXmlTagPtr query_conf = connector->getFirstChild("query_cond");
	aos_assert_r(query_conf, false);
	query_conf->setAttr(AOSTAG_OPR, AosOpr_toStr(eAosOpr_prefix));
	AosXmlTagPtr value_conf = query_conf->getFirstChild("zky_value");
	aos_assert_r(value_conf, false);
	value_conf->setNodeText(crt_day, true);

	AosDatasetObjPtr dataset = AosCreateDataset(rdata.getPtr(), datasetconf);
	dataset->config(rdata, datasetconf);
	dataset->sendStart(rdata);

	AosRecordsetObjPtr record_set = NULL;
	AosRecordsetObj* record_set_raw = NULL;
	AosDataRecordObj * input_record = NULL;

	OmnString day;
	OmnString key;
	bool rslt = false;
	while (1)
	{
		rslt = dataset->nextRecordset(rdata, record_set);
		aos_assert_r(rslt, false);
		
		if (AosRecordsetObj::checkEmpty(record_set))
			return scanFinished(dataset, rdata);

		record_set_raw = record_set.getPtr();
		while(1)
		{
			rslt = record_set_raw->nextRecord(rdata.getPtr(), input_record);
			aos_assert_r(rslt, false);
			if (!input_record)
				break;
			rslt = getNextKey(key, day, input_record, rdata);
			if (!rslt || day != crt_day)
			{
				OmnAlarm << "error" << enderr;
				continue;
			}
			crt_set.insert(key);
		}
	}
	return false;
}


bool
AosDataProcOnNetOffNet::addSet(
		map<OmnString, int> &crt_map,
		set<OmnString> &crt_set,
		const AosRundataPtr &rdata)
{
	OmnString key;
	set<OmnString>::iterator set_itr;
	map<OmnString, int>::iterator map_itr;
	for (set_itr = crt_set.begin(); set_itr != crt_set.end(); set_itr++)
	{
		key = *set_itr;

		map_itr = crt_map.find(key);
		if (map_itr != crt_map.end())
		{
			map_itr->second++;
		}
		else
		{
			crt_map[key] = 1;
		}
	}
	
	return true;
}


bool
AosDataProcOnNetOffNet::removeSet(
		map<OmnString, int> &crt_map,
		set<OmnString> &crt_set,
		const AosRundataPtr &rdata)
{
	OmnString key;
	set<OmnString>::iterator set_itr;
	map<OmnString, int>::iterator map_itr;
	for (set_itr = crt_set.begin(); set_itr != crt_set.end(); set_itr++)
	{
		key = *set_itr;

		map_itr = crt_map.find(key);
		if (map_itr == crt_map.end())
		{
			OmnAlarm << "missing find key:" << key << ", crt_day" << mCrtDay.toString() << enderr;
			return false;
		}
		
		map_itr->second--;
		if (map_itr->second <= 0)
		{
			crt_map.erase(map_itr);
		}
	}
	
	return true;
}


bool
AosDataProcOnNetOffNet::appendOnNetEntry(
		const OmnString &key,
		const OmnString &day,
		const AosRundataPtr &rdata)
{
	AosValueRslt value_rslt;
	bool outofmem = false;
	mRawOnNetOutputRecord->clear();
	value_rslt.setStr(day);
	bool rslt = mRawOnNetOutputRecord->setFieldValue(0, value_rslt, outofmem, rdata.getPtr());
	aos_assert_r(rslt, false);
	value_rslt.setStr(key);
	rslt = mRawOnNetOutputRecord->setFieldValue(1, value_rslt, outofmem, rdata.getPtr());
	mRawOnNetOutputRecord->flushRecord(rdata.getPtr());
	return true;
}


bool
AosDataProcOnNetOffNet::appendOffNetEntry(
		const OmnString &key,
		const OmnString &day,
		const AosRundataPtr &rdata)
{
	if (day > mLastDay.toString())
	{
		OmnAlarm << "time is error, mEpochDay:" << mLastDay.toString() << ", day:" << day << enderr;
		return true;
	}
	AosValueRslt value_rslt;
	bool outofmem = false;
	mRawOffNetOutputRecord->clear();
	value_rslt.setStr(day);
	bool rslt = mRawOffNetOutputRecord->setFieldValue(0, value_rslt, outofmem, rdata.getPtr());
	aos_assert_r(rslt, false);
	value_rslt.setStr(key);
	rslt = mRawOffNetOutputRecord->setFieldValue(1, value_rslt, outofmem, rdata.getPtr());
	mRawOffNetOutputRecord->flushRecord(rdata.getPtr());
	return true;
}


AosJimoPtr 
AosDataProcOnNetOffNet::cloneJimo() const
{
	return OmnNew AosDataProcOnNetOffNet(*this);
}


AosDataProcObjPtr
AosDataProcOnNetOffNet::cloneProc() 
{
	return OmnNew AosDataProcOnNetOffNet(*this);
}


bool 
AosDataProcOnNetOffNet::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr, 
		const AosJimoProgObjPtr &prog)
{
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataproconnetoffnet_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}
