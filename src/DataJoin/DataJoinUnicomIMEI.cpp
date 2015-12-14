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
// IIL joins are over-simplified 'table joins', where IILs are special 
// form of 'tables'. There are only two columns in IILs: Key and Value.
// Two IIls can be joined in a number of ways:
// 		eKey
// 		eValue
// 		eFamily
// 		eMember
// 		eSpecificMember
//
// This action supports only equi-joins. Further, this action assumes the
// matching column (either 'key' or 'value' column) is sorted. For instance, 
// if the matching column is the key-column, the normal IIL is used; if
// the matching column is the value-column, the companion IIL is used. 
//
// Modification History:
// 2014/02/27 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "DataJoin/DataJoinUnicomIMEI.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterUtil.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataRecord/DataRecord.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "Query/QueryReq.h"
#include "QueryClient/QueryClient.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"


AosDataJoinUnicomIMEI::AosDataJoinUnicomIMEI()
:
AosDataJoin()
{
}


AosDataJoinUnicomIMEI::AosDataJoinUnicomIMEI(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
AosDataJoin(ctlr, rdata)
{
}


AosDataJoinUnicomIMEI::~AosDataJoinUnicomIMEI()
{
}


bool
AosDataJoinUnicomIMEI::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosTaskObjPtr task = mCtlr->getTask();
	aos_assert_r(task, false);

	mTime = task->getTaskENV("proc_datetime", rdata);
	aos_assert_r(mTime != "", false);

	OmnString timestr(mTime.data(), 4);
	timestr << "-" << mTime[4] << mTime[5] << "-"
			<< mTime[6] << mTime[7] << " 00:00:00";
	u32 time = AosTimeUtil::str2EpochTime(timestr);
	time -= (60 * 60 * 24);
	mLastDay = AosTimeUtil::EpochToTimeNew(time, "%Y%m%d");

	mMonth = def->getAttrStr("sample_month");
	aos_assert_r(mMonth != "", false);

	mIsDBB = def->getAttrBool("is_dbb", false);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	aos_assert_r(scanners, false);
	AosXmlTagPtr scanner = scanners->getFirstChild(AOSTAG_IIL_SCANNER);
	aos_assert_r(scanner, false);
	AosIILScannerListenerPtr thisptr(this, false);	
	mIILScanner = AosIILScannerObj::createIILScannerStatic(thisptr, 0, scanner, rdata);
	aos_assert_r(mIILScanner, false);

	OmnString map_name = "_zt44_unicom_phone_imei_type_";
	map_name << mMonth;
	if (mIsDBB) map_name << "_dbb";
	mStatusMap = AosIILEntryMapMgr::retrieveIILEntryMap(map_name, rdata);
	aos_assert_r(mStatusMap, false);

	map_name = "_zt44_unicom_phone_imei_all_callnum_";
	map_name << mMonth;
	if (mIsDBB) map_name << "_dbb";
	mAllCallMap = AosIILEntryMapMgr::retrieveIILEntryMap(map_name, rdata);
	aos_assert_r(mAllCallMap, false);
	
	map_name = "_zt44_unicom_phone_imei_match_callnum_";
	map_name << mMonth;
	if (mIsDBB) map_name << "_dbb";
	mMatchCallMap = AosIILEntryMapMgr::retrieveIILEntryMap(map_name, rdata);
	aos_assert_r(mMatchCallMap, false);

	map_name = "_zt44_unicom_phone_imei_3g_callnum_";
	map_name << mMonth;
	if (mIsDBB) map_name << "_dbb";
	m3GCallMap = AosIILEntryMapMgr::retrieveIILEntryMap(map_name, rdata);
	aos_assert_r(m3GCallMap, false);

	return true;
}


bool
AosDataJoinUnicomIMEI::run()
{
	mStartTime = OmnGetSecond();
	OmnScreen << "join iil start , time:" << mStartTime << endl;
	
	AosRundataPtr rdata = mRundata;
	mLock->lock();
	bool rslt = runJoin(rdata);
	mLock->unlock();
	mFinished = true;
	mSuccess = rslt;
	
	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, time:" << mEndTime
		<< ", success:" << mSuccess
		<< ", spend:" << (mEndTime - mStartTime) << endl;
	
	AosDataJoinPtr thisptr(this, true);
	mCtlr->joinFinished(thisptr, rdata);

	clear();
	return true;
}


bool
AosDataJoinUnicomIMEI::getNextKey(
		OmnString &phonenum,
		u64 &docid,
		bool &finish,
		const AosRundataPtr &rdata)
{
	phonenum = "";
	docid = 0;
	finish = false;
	bool has_more = true;
	AosDataProcStatus::E status;
	
	while (1)
	{
		status = mIILScanner->moveNext(phonenum, has_more, rdata);
		switch (status)
		{
		case AosDataProcStatus::eOk:
			 aos_assert_r(phonenum != "", false);

			 docid = mIILScanner->getCrtValue();
			 aos_assert_r(docid != 0, false);

			 return true;
		
		case AosDataProcStatus::eRetrievingData:
			 mSem->wait();
			 if (!mDataRetrieveSuccess) return false;
			 break;

		case AosDataProcStatus::eNoMoreData:
			 finish = true;
			 return true;

		default:
			 return false;
		}
	}
	return false;
}


bool
AosDataJoinUnicomIMEI::getLastDatMap(map<OmnString, int> &last_map, const AosRundataPtr &rdata)
{
	OmnString str;
	str << "<cmd psize=\"1000\" order=\"zky_deal_time\" opr=\"retlist\" start_idx=\"0\">"
			<< "<conds><cond type=\"AND\">"
				<< "<term type=\"arith\" order=\"false\" reverse=\"false\">"
					<< "<selector type=\"attr\" aname=\"zky_pctrs\"><![CDATA[]]></selector>"
					<< "<cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[imei_sums]]></cond>"
				<< "</term>"
				<< "<term type=\"arith\" order=\"false\" reverse=\"false\">"
					<< "<selector type=\"attr\" aname=\"zky_mth\"><![CDATA[]]></selector>"
					<< "<cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[" << mMonth << "]]></cond>"
				<< "</term>"
				<< "<term type=\"arith\" order=\"false\" reverse=\"false\">"
					<< "<selector type=\"attr\" aname=\"zky_deal_time\"><![CDATA[]]></selector>"
					<< "<cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[" << mLastDay << "]]></cond>"
				<< "</term>"
				<< "<term type=\"arith\" order=\"false\" reverse=\"false\">"
					<< "<selector type=\"attr\" aname=\"zky_class\"><![CDATA[]]></selector>"
					<< "<cond type=\"arith\" ctype=\"const\" zky_opr=\"eq\"><![CDATA[3]]></cond>"
				<< "</term>"
			<< "</cond><stat/></conds>"
			<< "<fnames><fname type=\"x\" join_idx=\"0\"><oname>xml</oname><cname>xml</cname></fname></fnames>"
		<< "</cmd>";

	AosXmlTagPtr cmd = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmd, false);

	AosQueryReqObjPtr query = AosQueryClient::getSelf()->createQuery(cmd, rdata);
	aos_assert_r(query && query->isGood(), false);

	OmnString contents;
	bool rslt = query->procPublic(cmd, contents, rdata);
	aos_assert_r(rslt, false);

	AosXmlTagPtr data = AosXmlParser::parse(contents AosMemoryCheckerArgs);
	aos_assert_r(data, false);

	last_map.clear();

	AosXmlTagPtr tag = data->getFirstChild(true);
	while (tag)
	{
		OmnString key;
		key << tag->getAttrStr("zky_region_name") << ","
			<< tag->getAttrStr("zky_city_desc") << ","
			<< tag->getAttrStr("zky_type");

		u64 s2 = tag->getAttrInt("zky_s2", 0);
		last_map[key] = s2;
		tag = data->getNextChild();
	}
	return true;
}


bool
AosDataJoinUnicomIMEI::runJoin(const AosRundataPtr &rdata)
{
	bool rslt = mIILScanner->start(rdata);
	aos_assert_r(rslt, false);

	OmnString phonenum, file_context;
	u64 docid = 0;
	bool finish = false;
	AosXmlTagPtr doc;
	map<OmnString, USER> user_map;
	AosIILEntryMapItr itr;
	
	while (!finish)
	{
		rslt = getNextKey(phonenum, docid, finish, rdata);
		if (!rslt)
		{
			OmnAlarm << "error" << enderr;
			continue;
		}

		if (finish)
		{
			break;
		}

		doc = AosGetDocByDocid(docid, rdata);
		if (!doc || doc->getNodeText("PHONE_NO") != phonenum)
		{
			OmnAlarm << "error" << enderr;
			continue;
		}

		int status_code = 4;
		OmnString status;
		mStatusMap->readlock();
		itr = mStatusMap->find(phonenum);
		if (itr != mStatusMap->end())
		{
			status_code = (itr->second).toInt();
		}
		mStatusMap->unlock();
		switch (status_code)
		{
		case 1 : status = "机卡合一"; break;
		case 2 : status = "机卡分离"; break;
		case 3 : status = "无法判定(有话单无有效串号)"; break; 
		case 4 : status = "无法判定(无话单)"; break;
		default : OmnAlarm << "error" << enderr; continue;
		}
		
		OmnString match_call = "0";
		mMatchCallMap->readlock();
		itr = mMatchCallMap->find(phonenum);
		if (itr != mMatchCallMap->end())
		{
			match_call = itr->second;
		}
		mMatchCallMap->unlock();

		OmnString all_call = "0";
		mAllCallMap->readlock();
		itr = mAllCallMap->find(phonenum);
		if (itr != mAllCallMap->end())
		{
			all_call = itr->second;
		}
		mAllCallMap->unlock();
		
		OmnString _3g_call = "0";
		m3GCallMap->readlock();
		itr = m3GCallMap->find(phonenum);
		if (itr != m3GCallMap->end())
		{
			_3g_call = itr->second;
		}
		m3GCallMap->unlock();

		OmnString REGION_NAME = doc->getNodeText("REGION_NAME");
		OmnString CITY_DESC = doc->getNodeText("CITY_DESC"); 
		OmnString USER_ID = doc->getNodeText("USER_ID");
		OmnString IMEI_NUMBER = doc->getNodeText("IMEI_NUMBER");
		OmnString PRODT_CDE = doc->getNodeText("PRODT_CDE");
		OmnString PRODT_DESC = doc->getNodeText("PRODT_DESC");
		OmnString TYPE = doc->getNodeText("TYPE");
		OmnString CHNL_CDE = doc->getNodeText("CHNL_CDE");
		OmnString CHNL_DESC = doc->getNodeText("CHNL_DESC");

		USER user = {
			REGION_NAME, CITY_DESC, USER_ID, phonenum, IMEI_NUMBER,
			PRODT_CDE, PRODT_DESC, TYPE, CHNL_CDE, CHNL_DESC,
			status_code, status, match_call, all_call, _3g_call
		};

		user_map[phonenum] = user;

		OmnString str;
		str	<< "\"" << mTime << "\","
			<< "\"" << REGION_NAME << "\","
			<< "\"" << CITY_DESC << "\","
			<< "=\"" << USER_ID << "\","
			<< "=\"" << phonenum << "\","
			<< "=\"" << IMEI_NUMBER << "\","
			<< "\"" << PRODT_CDE << "\","
			<< "\"" << PRODT_DESC << "\","
			<< "\"" << TYPE << "\","
			<< "\"" << CHNL_CDE << "\","
			<< "\"" << CHNL_DESC << "\","
			<< "\"" << status << "\","
			<< match_call << ","
			<< all_call << ","
			<< _3g_call << "\n";
		file_context << str;
	}

	OmnString file_head = "时间,地市,区县,用户ID,手机号,手机串号,套餐编码,套餐名称,合约类型,代理商编码,代理商,匹配状态,合一话单量,总话单量,_3G话单量\n";

	OmnString filename = "/tmp/";
	filename << mMonth;
	if (mIsDBB) filename << "_dbb";
	filename << "_" << mTime << "all.csv";
	
	OmnFilePtr file = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	file->append(file_head, false);
	file->append(file_context, true);

	map<OmnString, SUM> sum_map;
	map<OmnString, SUM>::iterator sum_itr;
	map<OmnString, USER>::iterator user_itr = user_map.begin();
	while (user_itr != user_map.end())
	{
		USER user = user_itr->second;

		OmnString key;
		key << user.REGION_NAME << "," << user.CITY_DESC << "," << user.TYPE;

		sum_itr = sum_map.find(key);
		if (sum_itr != sum_map.end())
		{
			(sum_itr->second).CNT++;
		}
		else
		{
			SUM sum(user);
			sum_map[key] = sum;
			sum_itr = sum_map.find(key);
			aos_assert_r(sum_itr != sum_map.end(), false);
		}

		if (user.MATCH_CALL == "1")
		{
			(sum_itr->second).MATCH1++;	
		}
		
		switch (user.STATUS_CODE)
		{
		case 1 : (sum_itr->second).S1++; break;
		case 2 : (sum_itr->second).S2++; break;
		case 3 : (sum_itr->second).S3++; break;
		case 4 : (sum_itr->second).S4++; break;
		default : OmnAlarm << "error"; break;
		}

		user_itr++;
	}

	map<OmnString, int> last_map;
	rslt = getLastDatMap(last_map, rdata);
	aos_assert_r(rslt, false);

	sum_itr = sum_map.begin();
	while (sum_itr != sum_map.end())
	{
		OmnString key = sum_itr->first;
		u64 s2 = (sum_itr->second).S2;
		if (last_map.find(key) != last_map.end())
		{
			s2 = last_map[key];
		}
		(sum_itr->second).S2DIFF = (sum_itr->second).S2 - s2;

		sum_itr++;
	}

	map<OmnString, SUM> sum_map_0;
	if (!mIsDBB)
	{
		map<OmnString, SUM>::iterator sum_itr_0;
		sum_itr = sum_map.begin();
		while (sum_itr != sum_map.end())
		{
			SUM sum = sum_itr->second;
			OmnString key;
			key << sum.REGION_NAME;

			sum_itr_0 = sum_map_0.find(key);
			if (sum_itr_0 != sum_map_0.end())
			{
				(sum_itr_0->second) += sum;
			}
			else
			{
				sum.CITY_DESC = "全部";
				sum.TYPE = "全部";
				sum.CLASS = 0;
				sum_map_0[key] = sum;
			}
			sum_itr++;
		}
	}

	map<OmnString, SUM> sum_map_1;
	map<OmnString, SUM>::iterator sum_itr_1;
	sum_itr = sum_map.begin();
	while (sum_itr != sum_map.end())
	{
		SUM sum = sum_itr->second;
		OmnString key;
		key << sum.REGION_NAME << "," << sum.TYPE;

		sum_itr_1 = sum_map_1.find(key);
		if (sum_itr_1 != sum_map_1.end())
		{
			(sum_itr_1->second) += sum;
		}
		else
		{
			sum.CITY_DESC = "全部";
			sum.CLASS = 1;
			sum_map_1[key] = sum;
		}
		sum_itr++;
	}

	map<OmnString, SUM> sum_map_2;
	if (!mIsDBB)
	{
		map<OmnString, SUM>::iterator sum_itr_2;
		sum_itr = sum_map.begin();
		while (sum_itr != sum_map.end())
		{
			SUM sum = sum_itr->second;
			OmnString key;
			key << sum.REGION_NAME << "," << sum.CITY_DESC;

			sum_itr_2 = sum_map_2.find(key);
			if (sum_itr_2 != sum_map_2.end())
			{
				(sum_itr_2->second) += sum;
			}
			else
			{
				sum.TYPE = "全部";
				sum.CLASS = 2;
				sum_map_2[key] = sum;
				sum_itr_2 = sum_map_2.find(key);
				aos_assert_r(sum_itr_2 != sum_map_2.end(), false);
			}
			sum_itr++;
		}
	}

	map<OmnString, SUM> stat_map;
	map<OmnString, SUM>::iterator stat_itr;
	sum_itr = sum_map.begin();
	while (sum_itr != sum_map.end())
	{
		SUM sum = sum_itr->second;
		OmnString key;
		key << sum.REGION_NAME << "," << sum.TYPE;

		stat_itr = stat_map.find(key);
		if (stat_itr != stat_map.end())
		{
			(stat_itr->second) += sum;
		}
		else
		{
			sum.CITY_DESC = "全部";
			sum.CLASS = 3;
			stat_map[key] = sum;
		}
		sum_itr++;
	}

	map<OmnString, SUM> stat_map_0;
	if (!mIsDBB)
	{
		map<OmnString, SUM>::iterator stat_itr_0;
		stat_itr = stat_map.begin();

		SUM sum0 = stat_itr->second;
		sum0.REGION_NAME = "全省";
		sum0.CITY_DESC = "全部";
		sum0.TYPE = "全部";
		sum0.CLASS = 0;

		stat_itr++;
		while (stat_itr != stat_map.end())
		{
			SUM sum = stat_itr->second;
			sum0 += sum;
			stat_itr++;
		}
		stat_map_0["全省"] = sum0;
	}

	map<OmnString, SUM> stat_map_1;
	map<OmnString, SUM>::iterator stat_itr_1;
	stat_itr = stat_map.begin();
	while (stat_itr != stat_map.end())
	{
		SUM sum = stat_itr->second;
		OmnString key;
		key << sum.TYPE;

		stat_itr_1 = stat_map_1.find(key);
		if (stat_itr_1 != stat_map_1.end())
		{
			(stat_itr_1->second) += sum;
		}
		else
		{
			sum.REGION_NAME = "全省";
			sum.CITY_DESC = "全部";
			sum.CLASS = 1;
			stat_map_1[key] = sum;
		}
		stat_itr++;
	}

	map<OmnString, SUM> stat_map_2;
	if (!mIsDBB)
	{
		map<OmnString, SUM>::iterator stat_itr_2;
		stat_itr = stat_map.begin();
		while (stat_itr != stat_map.end())
		{
			SUM sum = stat_itr->second;
			OmnString key;
			key << sum.REGION_NAME;

			stat_itr_2 = stat_map_2.find(key);
			if (stat_itr_2 != stat_map_2.end())
			{
				(stat_itr_2->second) += sum;
			}
			else
			{
				sum.CITY_DESC = "全部";
				sum.TYPE = "全部";
				sum.CLASS = 2;
				stat_map_2[key] = sum;
			}
			stat_itr++;
		}
	}

	if (!mIsDBB) sum_map.insert(sum_map_0.begin(), sum_map_0.end());
	sum_map.insert(sum_map_1.begin(), sum_map_1.end());
	if (!mIsDBB) sum_map.insert(sum_map_2.begin(), sum_map_2.end());

	sum_itr = sum_map.begin();
	while (sum_itr != sum_map.end())
	{
		(sum_itr->second).S1P = 100.0 * (sum_itr->second).S1 / (sum_itr->second).CNT;
		(sum_itr->second).S2P = 100.0 * (sum_itr->second).S2 / (sum_itr->second).CNT;
		(sum_itr->second).S3P = 100.0 * (sum_itr->second).S3 / (sum_itr->second).CNT;
		(sum_itr->second).S4P = 100.0 * (sum_itr->second).S4 / (sum_itr->second).CNT;
		(sum_itr->second).S2DIFFP = 100.0 * (sum_itr->second).S2DIFF / (sum_itr->second).CNT;
		(sum_itr->second).MATCH1P = 100.0 * (sum_itr->second).MATCH1 / (sum_itr->second).CNT;

		sum_itr++;
	}

	if (!mIsDBB) stat_map.insert(stat_map_0.begin(), stat_map_0.end());
	stat_map.insert(stat_map_1.begin(), stat_map_1.end());
	if (!mIsDBB) stat_map.insert(stat_map_2.begin(), stat_map_2.end());

	stat_itr = stat_map.begin();
	while (stat_itr != stat_map.end())
	{
		(stat_itr->second).S1P = 100.0 * (stat_itr->second).S1 / (stat_itr->second).CNT;
		(stat_itr->second).S2P = 100.0 * (stat_itr->second).S2 / (stat_itr->second).CNT;
		(stat_itr->second).S3P = 100.0 * (stat_itr->second).S3 / (stat_itr->second).CNT;
		(stat_itr->second).S4P = 100.0 * (stat_itr->second).S4 / (stat_itr->second).CNT;
		(stat_itr->second).S2DIFFP = 100.0 * (stat_itr->second).S2DIFF / (stat_itr->second).CNT;
		(stat_itr->second).MATCH1P = 100.0 * (stat_itr->second).MATCH1 / (stat_itr->second).CNT;

		stat_itr++;
	}


	OmnString sum_str;
	sum_itr = sum_map.begin();
	while (sum_itr != sum_map.end())
	{
		SUM sum = sum_itr->second;

		char S1P[20], S2P[20], S3P[20], S4P[20], S2DIFFP[20], MATCH1P[20];
		sprintf(S1P, "%.2f", sum.S1P);
		sprintf(S2P, "%.2f", sum.S2P);
		sprintf(S3P, "%.2f", sum.S3P);
		sprintf(S4P, "%.2f", sum.S4P);
		sprintf(S2DIFFP, "%.2f", sum.S2DIFFP);
		sprintf(MATCH1P, "%.2f", sum.MATCH1P);

		OmnString str = "<doc";
		str << " zky_pctrs=\"imei_sums\" zky_deal_time=\"" << mTime << "\" zky_mth=\"" << mMonth << "\"" 
			<< " zky_region_name=\"" << sum.REGION_NAME << "\""
			<< " zky_city_desc=\"" << sum.CITY_DESC << "\""
			<< " zky_class=\"" << sum.CLASS << "\""
			<< " zky_type=\"" << sum.TYPE << "\""
			<< " zky_cnt =\"" << sum.CNT << "\""
			<< " zky_s1=\"" << sum.S1 << "\" zky_s1p=\"" << S1P << "\"" 
			<< " zky_s2=\"" << sum.S2 << "\" zky_s2p=\"" << S2P << "\""
			<< " zky_s3=\"" << sum.S3 << "\" zky_s3p=\"" << S3P << "\""
			<< " zky_s4=\"" << sum.S4 << "\" zky_s4p=\"" << S4P << "\""
			<< " zky_s2diff=\"" << sum.S2DIFF << "\" zky_s2diffp=\"" << S2DIFFP << "\""
			<< " zky_match1=\"" << sum.MATCH1 << "\" zky_match1p=\"" << MATCH1P << "\""
			<< " zky_public_doc=\"true\" zky_public_ctnr=\"true\"/>";

		AosXmlTagPtr xml = AosCreateDoc(str, true, rdata);
		if (!xml)
		{
			OmnAlarm << "failed to create xml:" << str << enderr;
		}

		sum_str << str << "\n";
		sum_itr++;
	}

	OmnString stat_str;
	stat_itr = stat_map.begin();
	while (stat_itr != stat_map.end())
	{
		SUM sum = stat_itr->second;

		char S1P[20], S2P[20], S3P[20], S4P[20], S2DIFFP[20], MATCH1P[20];
		sprintf(S1P, "%.2f", sum.S1P);
		sprintf(S2P, "%.2f", sum.S2P);
		sprintf(S3P, "%.2f", sum.S3P);
		sprintf(S4P, "%.2f", sum.S4P);
		sprintf(S2DIFFP, "%.2f", sum.S2DIFFP);
		sprintf(MATCH1P, "%.2f", sum.MATCH1P);

		OmnString str = "<doc";
		str << " zky_pctrs=\"imei_stats\" zky_deal_time=\"" << mTime << "\" zky_mth=\"" << mMonth << "\"" 
			<< " zky_region_name=\"" << sum.REGION_NAME << "\""
			<< " zky_class=\"" << sum.CLASS << "\""
			<< " zky_type=\"" << sum.TYPE << "\""
			<< " zky_cnt =\"" << sum.CNT << "\""
			<< " zky_s1=\"" << sum.S1 << "\" zky_s1p=\"" << S1P << "\"" 
			<< " zky_s2=\"" << sum.S2 << "\" zky_s2p=\"" << S2P << "\""
			<< " zky_s3=\"" << sum.S3 << "\" zky_s3p=\"" << S3P << "\""
			<< " zky_s4=\"" << sum.S4 << "\" zky_s4p=\"" << S4P << "\""
			<< " zky_s2diff=\"" << sum.S2DIFF << "\" zky_s2diffp=\"" << S2DIFFP << "\""
			<< " zky_match1=\"" << sum.MATCH1 << "\" zky_match1p=\"" << MATCH1P << "\""
			<< " zky_public_doc=\"true\" zky_public_ctnr=\"true\"/>";

		AosXmlTagPtr xml = AosCreateDoc(str, true, rdata);
		if (!xml)
		{
			OmnAlarm << "failed to create xml:" << str << enderr;
		}

		stat_str << str << "\n";
		stat_itr++;
	}

	filename = "/tmp/";
	filename << mMonth;
	if (mIsDBB) filename << "_dbb";
	filename << "_" << mTime << "sums.txt";
	file = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	file->append(sum_str, true);

	filename = "/tmp/";
	filename << mMonth;
	if (mIsDBB) filename << "_dbb";
	filename << "_" << mTime << "stats.txt";
	file = OmnNew OmnFile(filename, OmnFile::eCreate AosMemoryCheckerArgs);
	file->append(stat_str, true);

	return true;
}


bool
AosDataJoinUnicomIMEI::setQueryContext(
		const AosQueryContextObjPtr &context,
		const AosRundataPtr &rdata)
{
	return mIILScanner->setQueryContext(context, rdata);
}


int
AosDataJoinUnicomIMEI::getProgress() 
{
	if (mFinished)
	{
		mProgress = 100;
		return mProgress;
	}
	
	aos_assert_r(mIILScanner, 0);
	mProgress = mIILScanner->getProgress();
	//OmnScreen << "join:[" << this << "], mProgress:" << mProgress;
	return mProgress;
}


void
AosDataJoinUnicomIMEI::clear()
{
	mCtlr = 0;
	mIILScanner->clear();
	mIILScanner = 0;
}


AosDataJoinPtr
AosDataJoinUnicomIMEI::create(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	AosDataJoinUnicomIMEI * join = OmnNew AosDataJoinUnicomIMEI(ctlr, rdata);
	bool rslt = join->config(def, rdata);
	aos_assert_r(rslt, 0);
	return join;
}


bool
AosDataJoinUnicomIMEI::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	return true;
}

