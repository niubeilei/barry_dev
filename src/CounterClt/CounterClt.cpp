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
// This is a utility to select docs.
//
// Modification History:
// 06/16/2011	Created by Lynch Yang 
////////////////////////////////////////////////////////////////////////////
#include "CounterClt/CounterClt.h"

#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "CounterClt/Ptrs.h"
#include "CounterClt/CounterAssembler.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/CounterName.h"
#include "CounterUtil/CounterOperations.h"
#include "CounterUtil/CounterUtil.h"
#include "CounterUtil/CounterVarType.h"
#include "CounterUtil/CounterQuery.h"
#include "CounterUtil/RecordFormat.h"
#include "CounterUtil/CounterParameter.h"
#include "CounterUtil/Ptrs.h"
#include "QueryUtil/FieldFilter.h"
#include "IILUtil/IILId.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SingletonClass/SingletonImpl.cpp"
//#include "TransClient/TransClient.h"
#include "TransUtil/IdRobin.h"
#include "Util/BuffArray.h"
#include "Util/StrSplit.h"
#include "UtilTime/TimeInfo.h"
#include "UtilTime/TimeGran.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/SeXmlParser.h"


OmnSingletonImpl(AosCounterCltSingleton,
                 AosCounterClt,
                 AosCounterCltSelf,
                 "AosCounterClt");


AosCounterClt::AosCounterClt()
{
	mAssembler = OmnNew AosCounterAssembler();
}


AosCounterClt::~AosCounterClt()
{
}


bool
AosCounterClt::config(const AosXmlTagPtr &conf)
{
	// the config should be in the form:
	// <CounterClt isLocal="true">
	//    <transaction module_id="1" sync="false">
	//       <transfile dirname="/home/AOS/Trans"
	//                      doc_filename="trans"
	//                      resend_time="3"
	//                      csleeptime="10"
	//                      recover_time="7200"/>
	//        <servers>
	//           <server remote_addr="0.0.0.0"
	//                   remote_port="5530"
	//                   num_send_conn="2"
	//                   num_recv_conn="2"/>
	//        </servers>
	//      </transaction>
	//      <serverInfo num_physicals="2" zky_numvirtuals="5" />
	//  </CounterClt>
	
	aos_assert_r(conf, false);

    // gets the CounterClt config.
    AosXmlTagPtr counterCltConfig = conf->getFirstChild(AOSTAG_COUNTERCLT);
	
	// Chen Ding, 06/05/2012
	// Counter Client is not mandatory.
	if (!counterCltConfig) return true;

	aos_assert_r(mAssembler, false);

	//mAssembler->init(mTransClient->getNumPhysicalsSvr(), mNumVirtuals);
	mAssembler->init(-1, AosGetNumCubes());
   	return true;
}


bool
AosCounterClt::start()
{
	return true;
}


bool
AosCounterClt::stop()
{
	return true;
}

/*
bool 
AosCounterClt::procCounter(
		const OmnString &cname, 
		const AosRundataPtr &rdata)
{
	return procCounter(cname, 1, AosTimeGran::eNoTime, "", "", "", "", AOSCOUNTEROPR_ADDCOUNTER, rdata);
}


bool 
AosCounterClt::procCounter(
		const OmnString &cname, 
		const int64_t &value,
		const AosRundataPtr &rdata)
{
	return procCounter(cname, value, AosTimeGran::eNoTime, "", "", "", AOSCOUNTEROPR_ADDCOUNTER, rdata);
}

bool 
AosCounterClt::procCounter(
		const OmnString &cname, 
		const AosTimeGran::E time_gran,
		const AosRundataPtr &rdata)
{
	return procCounter(cname, 1, time_gran, "", "", "", "", AOSCOUNTEROPR_ADDCOUNTER, rdata);
}

bool 
AosCounterClt::procCounter(
		const OmnString &cname, 
		const int64_t &value,
		const AosTimeGran::E time_gran,
		const OmnString &timeStr,
		const OmnString &timeFormat,
		const AosRundataPtr &rdata)
{
	return procCounter(cname, value, time_gran, timeStr, timeFormat, "", "", AOSCOUNTEROPR_ADDCOUNTER, rdata);
}
*/

bool 
AosCounterClt::procCounter(
		const OmnString &cname, 
		const int64_t &value,
		const AosTimeGran::E time_gran,
		const OmnString &timeStr,
		const OmnString &timeFormat,
		const OmnString &statType,
		const OmnString &operationStr,
		const AosRundataPtr &rdata)
{
	if (operationStr == "add2")
	{
		return procCounter2(cname, value, time_gran, timeStr, timeFormat, statType, "", operationStr, rdata);
	}
	return procCounter(cname, value, time_gran, timeStr, timeFormat, statType, "", operationStr, rdata);
}


bool
AosCounterClt::procCounter(
        const OmnString &cname,
        const int64_t &cvalue,
		const AosTimeGran::E time_gran,
        const OmnString &timeStr,
		const OmnString &timeFormat,
		const OmnString &statTypeStr,
		const OmnString &formatStr,
		const OmnString &operationStr,
        const AosRundataPtr &rdata)
{
	// This function processes 'cname'. 
	bool rslt = AosCounterUtil::isValidCounterName(cname);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_counter_name") << ": " << cname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosTime::TimeFormat format = AosTime::convertTimeFormatWithDft(formatStr);
	u64 unitime = AosTime::getUniTime(format, time_gran, timeStr, timeFormat);

	OmnScreen << "timeStr (" << cname << "," << timeStr <<")"<< endl;

	AosStatType::E statType = AosStatType::toEnum(statTypeStr);
	if (!AosStatType::isOrigStat(statType))
	{
		AosSetError(rdata, "invalid_stat_type") << ": " << statType;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosCounterOperation::E operation = AosCounterOperation::toEnum(operationStr);
	if (!AosCounterOperation::isValid(operation))
	{
		OmnAlarm << "Invalid Counter Operation :" << operationStr << enderr;
		operation = AosCounterOperation::eAddCounter;
	}

	// Resolve counters
	OmnString rslvCname;
	rslt = resolveCounter(cname, rslvCname, rdata);
	aos_assert_r(rslt, false);

	// Retrieve the counterID
	// 'cname' should be in the form:
	// 		counter_id|$$|name|$$|name|$$|...
	// 	counter_id :AOSZTG_COUNTER + '_' + counter_id
	OmnString counter_id = AosCounterUtil::composeCounterIILName(cname);
	aos_assert_r(counter_id != "", false);
	OmnScreen << "counterId :" << counter_id << endl;
	
	// Chen Ding, 04/13/2012
	if (rslvCname != "")
	{
		mAssembler->addCounter(counter_id, rslvCname, cvalue, unitime, statType, operation, rdata);
	}
	else
	{
		mAssembler->addCounter(counter_id, cname, cvalue, unitime, statType, operation, rdata);
	}
	return true;
}


// Ketty 2013/03/25
/*
bool
AosCounterClt::sendBuffTrans(
		const int vid, 
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	// This function sends a buff-ed transaction to the corresponding
	// counter server. Its format is:
	// 	<request 
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(vid >= 0 && vid < AosGetNumCubes(), rdata, false);

	//int svr_id = AosGetPhysicalIdByVirtualId(vid);
	//aos_assert_r(svr_id != -1, false);

	//return mTransClient->addTrans(rdata, svr_id, buff->data(),
	//			buff->dataLen(), false, 0);
}
*/

bool
AosCounterClt::retrieveSingleCounter(
		const OmnString &counter_id,
		const OmnString &cname,
		const vector<AosStatType::E> stat_types,
		const u64 &start_time,
		const u64 &end_time,
		const int64_t &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(stat_types.size() > 0, rdata, false);
	buff->reset();
	aos_assert_r(rdata, false);
	bool rslt = AosCounterUtil::isValidCounterName(counter_id);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_counter_name") << ": " << counter_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
      
	u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);

	for (u32 i = 0; i < stat_types.size(); i++)
	{
		if (!AosStatType::isOrigStat(stat_types[i]))
		{
			AosSetError(rdata, "invalid_stat_type") << ": " << stat_types[i];
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		if (!AosTime::isValidTimeRange(start_time, end_time))
		{
			// It is not a time-based query.
			OmnString counter_name = AosCounterUtil::composeCounterKey(cname, stat_types[i]);
			bool rslt = AosCounterUtil::isValidNoTimeCounterName(counter_name);
			aos_assert_r(rslt, false);
			OmnScreen << "cname :" << counter_name << endl;
			bool is_unique = false;
			bool found;
			u64 value = AosGetKeyedStrValue(iilid, counter_name, 0, is_unique, found, rdata);
			if (found && is_unique)
			{
				int64_t vv = AosCounterUtil::convertValueToInt64(value);
				buff->setI64(vv);
			}
			else
			{
				buff->setI64(dft_value);
			}
		}
		else
		{
			OmnString startName = AosCounterUtil::composeTimeCounterName(
					cname, stat_types[i], start_time);
			bool rslt = AosCounterUtil::isValidTimeCounterName(startName);
			aos_assert_r(rslt, false);
			OmnString endName = AosCounterUtil::composeTimeCounterName(
					cname, stat_types[i], end_time);
			rslt = AosCounterUtil::isValidTimeCounterName(endName);
			aos_assert_r(rslt, false);

			bool proc_times = true;
			rslt = queryRange(iilid, startName, endName, buff, proc_times, true, false, rdata);
			aos_assert_r(rslt, false);
		}

	}
	buff->reset();
	return true;
}


bool
AosCounterClt::retrieveMultiCounters(
		const OmnString &counter_id,
		const vector<OmnString> &cnames,
		const int64_t &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	buff->reset();
	aos_assert_r(rdata, false);
	bool rslt = AosCounterUtil::isValidCounterName(counter_id);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_counter_name") << ": " << counter_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
      
	u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);
	return AosGetKeyedStrValues(iilid, cnames, true, dft_value, buff, rdata);
}


bool
AosCounterClt::retrieveMultiCounters(
		const OmnString &counter_id,
		const vector<OmnString> &cnames,
		const u64 &start_time,
		const u64 &end_time,
		const int64_t &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	buff->reset();
	aos_assert_r(rdata, false);
	bool rslt = AosCounterUtil::isValidCounterName(counter_id);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_counter_name") << ": " << counter_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
      
	u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);

	OmnString startName, endName;
	for (u32 i = 0; i< cnames.size(); i++)
	{
		startName = AosCounterUtil::composeTimeCounterNamePart(cnames[i], start_time);
		rslt = AosCounterUtil::isValidTimeCounterName(startName);
		aos_assert_r(rslt, false);

		endName = AosCounterUtil::composeTimeCounterNamePart(cnames[i], end_time);
		rslt = AosCounterUtil::isValidTimeCounterName(endName);
		aos_assert_r(rslt, false);

		rslt = queryRange(iilid, startName, endName, buff, true, true, false, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosCounterClt::retrieveCounters(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{
	u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);

	for (u32 i = 0; i < stat_types.size(); i++)
	{
		OmnString name = AosCounterUtil::composeCounterName(cname, stat_types[i], "");
		bool rslt = AosCounterUtil::isValidNoTimeCounterName(name);
		aos_assert_r(rslt, false);

		rslt = queryPrefix(iilid, name, buff, true, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosCounterClt::retrieveCounters(
		const vector<OmnString> &counter_ids,
		const OmnString &cname,
		const vector<AosStatType::E> stat_types,
		AosBuffPtr &buff,
		bool use_iilname,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	//bool is_super_iil = false;
	for (u32 k = 0; k < counter_ids.size(); k++)
	{
		if (use_iilname)
		{
			//is_super_iil = AosIsSuperIIL(counter_ids[k]);
		    iilid = AosIILClientObj::getIILClient()->getIILID(counter_ids[k], rdata);
		}
		else
		{
			iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_ids[k], 0, rdata);
		}
		for (u32 i = 0; i < stat_types.size(); i++)
		{
			OmnString name = AosCounterUtil::composeCounterName(cname, stat_types[i], "");
		//	bool rslt = AosCounterUtil::isValidNoTimeCounterName(name);
		//	aos_assert_r(rslt, false);

		//	rslt = queryPrefix(iilid, name, buff, true, rdata);
		//	aos_assert_r(rslt, false);
			OmnString startName = name;
			startName << (char)32;
			OmnString endName = name;
			endName << (char) 255;                                                
			bool rslt = queryRange(iilid, startName, endName, 
						buff, false, true, false, rdata);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}


bool
AosCounterClt::retrieveCounters(
		const vector<OmnString> &counter_ids,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		AosBuffPtr &buff,
		bool use_iilname,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	//bool is_super_iil = false;
	for (u32 k = 0; k < counter_ids.size(); k++)
	{
		if (use_iilname)
		{
			//is_super_iil = AosIsSuperIIL(counter_ids[k]);
			iilid  = AosIILClientObj::getIILClient()->getIILID(counter_ids[k], rdata);
		}
		else
		{
			iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_ids[k], 0, rdata);
		}

		for (u32 i = 0; i < stat_types.size(); i++)
		{
			for (u32 j = 0; j< cnames.size(); j++)
			{
				OmnString name = AosCounterUtil::composeTimeCounterKey(cnames[j], stat_types[i], "");
				//bool rslt = AosCounterUtil::isValidTimeCounterName(name);
				//aos_assert_r(rslt, false);
				//queryPrefix(iilid, name, buff, false, rdata);
				OmnString startName = name;
				startName << (char)32;
				OmnString endName = name;
				endName << (char) 255;
				bool rslt = queryRange(iilid, startName, endName, 
							buff, false, false, false, rdata);
				aos_assert_r(rslt, false);
			}
			buff->setOmnStr("");
		}
	}
	return true;
}


bool
AosCounterClt::retrieveSubCountersBySingle(
		const vector<OmnString> &counter_ids,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		const OmnString &member,
		AosBuffPtr &buff,
		bool use_iilname,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	bool is_super_iil = false;
	for (u32 k = 0; k < counter_ids.size(); k++)
	{
		if (use_iilname)
		{
			is_super_iil = AosIsSuperIIL(counter_ids[k]);
			//iilid  = AosIILClientObj::getIILClient()->getIILID(counter_ids[k], is_super_iil, rdata);
		}
		else
		{
			iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_ids[k], 0, rdata);
		}
		
		for (u32 i = 0; i < stat_types.size(); i++)
		{
			for (u32 j = 0; j< cnames.size(); j++)
			{
				OmnString name = AosCounterUtil::composeTimeCounterKey(cnames[j], stat_types[i], member);
				bool is_unique = false;
				bool found = false;
				u64 value = 0;
				if (iilid != 0)
				{
					if (is_super_iil)
					{
						bool rslt = AosIILClientObj::getIILClient()->getDocid(counter_ids[k], name, value, rdata);
						aos_assert_r(rslt, false);
					}
					else
					{
						value = AosGetKeyedStrValue(iilid, name, 0, is_unique, found, rdata);
					}
				}
				buff->setOmnStr(name);
				if ((found && is_unique) || is_super_iil)
				{
					int64_t vv = value; 
					if (!is_super_iil) vv = AosCounterUtil::convertValueToInt64(value);
					buff->setI64(vv);
				}
				else
				{
					buff->setI64(0);
				}
			}
			buff->setOmnStr("");
		}
	}
	return true;
}


bool
AosCounterClt::retrieveCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata)
{

	u64 iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);
	for (u32 i = 0; i < stat_types.size(); i++)
	{
		for (u32 j = 0; j< cnames.size(); j++)
		{
			OmnString name = AosCounterUtil::composeTimeCounterKey(cnames[j], stat_types[i], "");
			bool rslt = AosCounterUtil::isValidTimeCounterName(name);
			aos_assert_r(rslt, false);
			queryPrefix(iilid, name, buff, false, rdata);
		}
		buff->setOmnStr("");
	}
	return true;
}


bool 
AosCounterClt::queryRange(
			const u64 &iilid,
			const OmnString &startName,
			const OmnString &endName,
			AosBuffPtr &buff,
			bool proc_times,
			bool use_sep,
			bool is_super_iil,
			const AosRundataPtr &rdata)
{
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	queryRslt->setWithValues(true);
	query_context->setOpr(eAosOpr_range_ge_le);
	query_context->setStrValue(startName);
	query_context->setStrValue2(endName);
	AosQueryColumn(iilid, queryRslt, 0, query_context, rdata); 

	if (!queryRslt) return true;

	u64 docid;
	OmnString key;                                                
	bool finished = false;
	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		// 'buff' should be in the form:
		// 		[name, value]
		// 		[name, value]
		// 		...
		if (finished) break;
		if (key != "")
		{
			OmnString str = key;
			if (proc_times) str = AosCounterUtil::getTimePart(key);

			if (str == "")
			{
				OmnAlarm << "timestr is empty: " << key << enderr;
			}
			else
			{
				buff->setOmnStr(str);
				int64_t vv = docid;
				if (!is_super_iil) vv = AosCounterUtil::convertValueToInt64(docid);
				buff->setI64(vv);
//OmnScreen << "Query result cname:" << key  << " , " << vv << endl;
			}
		}
	}
	if(finished && use_sep) buff->setOmnStr("");
	return true;
}


bool
AosCounterClt::queryPrefix(
			const u64 &iilid,
			const OmnString &prefix,
			AosBuffPtr &buff,
			bool use_sep,
			const AosRundataPtr &rdata)
{
	AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	queryRslt->setWithValues(true);
	query_context->setStrValue(prefix);
	query_context->setOpr(eAosOpr_prefix);
	queryRslt->setWithValues(true);
	AosQueryColumn(iilid, queryRslt, 0, query_context, rdata);
	if (!queryRslt) return true;

	u64 docid;
	OmnString key;                                                
	bool finished = false;
	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		// 'buff' should be in the form:
		// 		[name, value]
		// 		[name, value]
		// 		...
		if (finished) break;
		if (key != "")
		{
			OmnString str = key;
//OmnScreen << "Query result cname:" << key << endl;
			//str = AosCounterUtil::getTimePart(key);
			if (str == "")
			{
				OmnAlarm << "timestr is empty: " << key << enderr;
			}
			else
			{
				buff->setOmnStr(str);
				int64_t vv = AosCounterUtil::convertValueToInt64(docid);
				buff->setI64(vv);
			}
		}
	}
	if(finished && use_sep) buff->setOmnStr("");
	return true;
}


bool
AosCounterClt::resolveCounter(
		const OmnString &cname, 
		OmnString &resolved_cname,
		const AosRundataPtr &rdata)
{
	// This function resolves 'counter variables' contained in 
	// 'cname'. 'counter variables' are defined as:
	// 		'AOS_VAR_PREFIX{xxx}'. 
	aos_assert_r(cname != "", false);

	int guard = 1111;
	char *start_ptr = (char *)cname.data();
	resolved_cname = "";
	while (guard-- && start_ptr)
	{
		char *foundptr = strstr(start_ptr, AOS_VAR_PREFIX);
		if (!foundptr) break;

		foundptr += AOS_VAR_PREFIX_LEN;
		char *endptr = strstr(foundptr, "}");
		aos_assert_rr(endptr, rdata, false);

		// Found a variable.
		int var_len = endptr - foundptr;
		AosCounterVarType::E type = AosCounterVarType::toEnum(foundptr, var_len);
		switch (type)
		{
		case AosCounterVarType::eUserCidVar:
			 resolved_cname.assign(start_ptr, foundptr - start_ptr);
			 resolved_cname << rdata->getCid();
			 break;

		case AosCounterVarType::eUserIdVar:
			 resolved_cname.assign(start_ptr, foundptr - start_ptr);
			 resolved_cname << rdata->getUserid();
			 break;

		case AosCounterVarType::eObjIdVar:
			 OmnNotImplementedYet;
			 // cvar = objid;
			 break;

		case AosCounterVarType::eDocIdVar:
			 OmnNotImplementedYet;
			 // cvar = docid;
			 break;

		case AosCounterVarType::eDocValueVar:
			 OmnNotImplementedYet;
			 // if (!AosValueSel::getValueForCounterVarStatic(middleValue, cvar, rdata))
			 // {
			// 	 return false;
			 // }
			 break;

		default:
			 rdata->setError() << "Unrecognized counter var type: " << cname;
			 OmnAlarm << rdata->getErrmsg() << enderr;
			 return false;
		}

		start_ptr = endptr+1;
	}
	rdata->setOk();
	return true;
}












///////////////////////////////////////// New ///////////////////////////////////////////////
bool
AosCounterClt::retrieveCounters2(
		const OmnString &counter_id,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		const AosCounterQueryPtr &counter_query,
		AosBuffPtr &buff,
		AosQueryRsltObjPtr &queryRslt,
		bool use_iilname,
		const OmnString &query_rslt_opr,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	bool is_super_iil = false;
	if (use_iilname)                                                                
	{
		//is_super_iil = AosIsSuperIIL(counter_id);
		//iilid = AosIILClientObj::getIILClient()->getIILID(counter_id, is_super_iil, rdata);
	}
	else
	{
		iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);
	}
		
	aos_assert_r(cnames.size() == 2, false);
	for (u32 i = 0; i < stat_types.size(); i++)
	{
		OmnString startName = AosCounterUtil::modifyStatType(cnames[0], stat_types[i]);
		OmnString endName = AosCounterUtil::modifyStatType(cnames[1], stat_types[i]);
		endName << (char) 255;                                                
		bool rslt;
		if (use_iilname)
		{
			rslt = counterRange(counter_id, startName, endName, buff, queryRslt, false, 
				false, counter_query, query_rslt_opr, rdata);
		}
		else
		{
			rslt = counterRange(iilid, startName, endName, buff, queryRslt, false, 
				is_super_iil, false, counter_query, query_rslt_opr, rdata);
		}
		aos_assert_r(rslt, false);
		buff->setOmnStr("");
	}
	return true;
}


bool
AosCounterClt::retrieveCounters2(
		const vector<OmnString> &counter_ids,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		const AosCounterQueryPtr &counter_query,
		AosBuffPtr &buff,
		bool use_iilname,
		const OmnString &query_rslt_opr,
		const AosRundataPtr &rdata)
{
	for (u32 k = 0; k < counter_ids.size(); k++)
	{
		AosQueryRsltObjPtr queryRslt;
		bool rslt = retrieveCounters2(counter_ids[k], cnames, stat_types,
			counter_query, buff, queryRslt, use_iilname, query_rslt_opr, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosCounterClt::retrieveCountersPrefix(
		const OmnString &counter_id,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		const AosCounterQueryPtr &counter_query,
		AosBuffPtr &buff,
		AosQueryRsltObjPtr &queryRslt,
		bool use_iilname,
		const OmnString &query_rslt_opr,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	bool is_super_iil = false;
	if (use_iilname)                                                                
	{
		//is_super_iil = AosIsSuperIIL(counter_id);
		//iilid = AosIILClientObj::getIILClient()->getIILID(counter_id, is_super_iil, rdata);
	}
	else
	{
		iilid = AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);
	}
		
	for (u32 i = 0; i < stat_types.size(); i++)
	{
		for (u32 j = 0; j < cnames.size(); j++)
		{
			OmnString name = AosCounterUtil::modifyStatType(cnames[j], stat_types[i]);
			bool rslt;
			if (use_iilname)
			{
				rslt = counterPrefix(counter_id, name, buff, queryRslt, false, 
					false, counter_query, query_rslt_opr, rdata);
			}
			else
			{
				rslt = counterPrefix(iilid, name, buff, queryRslt, false, 
					is_super_iil, false, counter_query, query_rslt_opr, rdata);
			}
			aos_assert_r(rslt, false);
		}
		buff->setOmnStr("");
	}

	return true;
}


bool
AosCounterClt::retrieveCountersPrefix(
		const vector<OmnString> &counter_ids,
		const vector<OmnString> &cnames,
		const vector<AosStatType::E> stat_types,
		const AosCounterQueryPtr &counter_query,
		AosBuffPtr &buff,
		bool use_iilname,
		const OmnString &query_rslt_opr,
		const AosRundataPtr &rdata)
{
	for (u32 k = 0; k < counter_ids.size(); k++)
	{
		AosQueryRsltObjPtr queryRslt;
		bool rslt = retrieveCountersPrefix(counter_ids[k], cnames, stat_types,
			counter_query, buff, queryRslt, use_iilname, query_rslt_opr, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool 
AosCounterClt::counterRange(
		const u64 &iilid,
		const OmnString &startName,
		const OmnString &endName,
		AosBuffPtr &buff,
		AosQueryRsltObjPtr &queryRslt,
		bool proc_times,
		bool is_super_iil,
		bool need_convert_int64,
		const AosCounterQueryPtr &counter_query,
		const OmnString &query_rslt_opr,
		const AosRundataPtr &rdata)
{
	if (!queryRslt) queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	queryRslt->setWithValues(true);
	query_context->setOpr(eAosOpr_range_ge_le);
	query_context->setStrValue(startName);
	query_context->setStrValue2(endName);
	query_context->setBlockSize(AosCounterUtil::eMaxPerCounterEntry);
	AosIILClientObj::getIILClient()->counterRange(
			iilid, queryRslt, 0, query_context, counter_query, rdata); 

	if (!queryRslt)return true;
	u64 docid;
	OmnString key;                                                
	bool finished = false;
	aos_assert_r(queryRslt->getNumDocs() <= AosCounterUtil::eMaxPerCounterEntry, false);

	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		// 'buff' should be in the form:
		// 		[name, value]
		// 		[name, value]
		// 		...
		if (finished) break;
		if (key != "")
		{
			OmnString str = key;
			if (proc_times) str = AosCounterUtil::getTimePart(key);
			if (str == "")
			{
				OmnAlarm << "timestr is empty: " << key << enderr;
			}
			else
			{
				buff->setOmnStr(str);
				int64_t vv = docid;
				if (need_convert_int64) vv = AosCounterUtil::convertValueToInt64(docid);
				buff->setI64(vv);
			}
		}
	}
	return true;
}


bool 
AosCounterClt::counterPrefix(
			const u64 &iilid,
			const OmnString &name,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool proc_times,
			bool is_super_iil,
			bool need_convert_int64,
			const AosCounterQueryPtr &counter_query,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata)
{
	if (!queryRslt) queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	queryRslt->setWithValues(true);
	query_context->setOpr(eAosOpr_prefix);
	query_context->setStrValue(name);
	query_context->setBlockSize(AosCounterUtil::eMaxPerCounterEntry);
	AosIILClientObj::getIILClient()->counterRange(
		iilid, queryRslt, 0,  query_context, counter_query, rdata); 

	if (!queryRslt)return true;
	u64 docid;
	OmnString key;                                                
	bool finished = false;
	aos_assert_r(queryRslt->getNumDocs() <= AosCounterUtil::eMaxPerCounterEntry, false);

	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		// 'buff' should be in the form:
		// 		[name, value]
		// 		[name, value]
		// 		...
		if (finished) break;
		if (key != "")
		{
			OmnString str = key;
			if (proc_times) str = AosCounterUtil::getTimePart(key);

			if (str == "")
			{
				OmnAlarm << "timestr is empty: " << key << enderr;
			}
			else
			{
				buff->setOmnStr(str);
				int64_t vv = docid;
				if (need_convert_int64) vv = AosCounterUtil::convertValueToInt64(docid);
				buff->setI64(vv);
			}
		}
	}
	return true;
}


bool
AosCounterClt::retrieveCounters2ByNormal(
			const AosCounterParameterPtr &parm,
			const AosRundataPtr &rdata)
{
	AosRecordFormatPtr record_format = parm->getRecordFormat();
	AosCounterQueryPtr counter_query = parm->counter_query;
	aos_assert_r(record_format, false);
	bool is_super_iil;
	for (u32 k = 0; k < parm->counter_ids.size(); k++)
	{
		OmnString counter_id = parm->counter_ids[k];
		record_format->resetFillingCrtTime();

		u64 iilid = getIILID(counter_id, parm->use_iilname, is_super_iil, rdata);
		for (u32 i = 0; i < parm->stat_types.size(); i++)
		{
			AosStatType::E stat_type = parm->stat_types[i];
			AosQueryContextObjPtr query_context = getQueryContext(parm, stat_type, rdata);
			AosQueryRsltObjPtr queryRslt = AosQueryRsltObj::getQueryRsltStatic();

			bool rslt = runQuery(iilid, is_super_iil, query_context, counter_query, queryRslt, rdata);
			aos_assert_r(rslt, false);

			if (parm->order_by_value && parm->sort_counter_id == counter_id)
			{
				rslt = sortQueryRslt(counter_id, stat_type, parm, queryRslt, rdata);
				aos_assert_r(rslt, false);
			}
			else
			{
				rslt = retrieveQueryRslt(counter_id, stat_type, parm, queryRslt, rdata);
				aos_assert_r(rslt, false);
			}
		}
	}
	return true;
}


bool 
AosCounterClt::runQuery(
			const u64 &iilid,
			bool is_super_iil,
			const AosQueryContextObjPtr &query_context,
			const AosCounterQueryPtr &counter_query,
			const AosQueryRsltObjPtr &queryRslt,
			const AosRundataPtr &rdata)
{
	queryRslt->setWithValues(true);
	query_context->setBlockSize(AosCounterUtil::eMaxPerCounterEntry);

	return AosIILClientObj::getIILClient()->counterRange(
			iilid, queryRslt, 0, query_context, counter_query, rdata); 
}



bool
AosCounterClt::procCounter2(
        const OmnString &cname,
        const int64_t &cvalue,
		const AosTimeGran::E time_gran,
        const OmnString &timeStr,
		const OmnString &timeFormat,
		const OmnString &statTypeStr,
		const OmnString &formatStr,
		const OmnString &operationStr,
        const AosRundataPtr &rdata)
{
	// This function processes 'cname'. 
	bool rslt = AosCounterUtil::isValidCounterName(cname);
	if (!rslt)
	{
		AosSetErrorU(rdata, "invalid_counter_name") << ": " << cname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosTimeFormat::E format = AosTimeFormat::toEnum(timeFormat);
	u64 unitime = AosTime::getUniTime(timeStr, format, time_gran);

	OmnScreen << "timeStr (" << cname << "," << timeStr <<")"<< endl;

	AosStatType::E statType = AosStatType::toEnum(statTypeStr);
	if (!AosStatType::isOrigStat(statType))
	{
		AosSetError(rdata, "invalid_stat_type") << ": " << statType;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosCounterOperation::E operation = AosCounterOperation::toEnum(operationStr);
	if (!AosCounterOperation::isValid(operation))
	{
		OmnAlarm << "Invalid Counter Operation :" << operationStr << enderr;
		operation = AosCounterOperation::eAddCounter;
	}
	// compose entry type
	u16 entry_type = AosCounterUtil::composeTimeEntryType(0, 0, statType, time_gran);

	// Retrieve the counterID
	// 'cname' should be in the form:
	// 		counter_id|$$|name|$$|name|$$|...
	// 	counter_id :AOSZTG_COUNTER + '_' + counter_id
	// 	cname:name|$$|name|$$|...
	OmnString name;
	OmnString counter_id = AosCounterUtil::composeCounterIILName(cname, name);
	aos_assert_r(counter_id != "", false);
	OmnScreen << "counterId :" << counter_id << endl;
	aos_assert_r(name != "", false);
	OmnScreen << "cname :" << name << endl;
	
	mAssembler->addCounter2(operation, counter_id, name, cvalue, unitime, time_gran, entry_type, rdata);
	return true;
}


u64
AosCounterClt::getIILID(
		const OmnString &counter_id,
		const bool &use_iilname,
		bool &is_super_iil,
		const AosRundataPtr &rdata)
{
	is_super_iil = false;
	if (use_iilname)                                                                
	{
		is_super_iil = AosIsSuperIIL(counter_id);
		return AosIILClientObj::getIILClient()->getIILID(counter_id, rdata);
	}
	return AosGetKeyedStrValue(AOSIILID_COUNTER_IILIDS, counter_id, 0, rdata);
}


AosQueryContextObjPtr
AosCounterClt::getQueryContext(
		const AosCounterParameterPtr &parm,
		const AosStatType::E &stat_type,
		const AosRundataPtr &rdata)
{
	vector<OmnString>cnames = parm->cnames;
	aos_assert_r(cnames.size() > 0 ,0);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	OmnString startName = AosCounterUtil::modifyStatType(cnames[0], stat_type);
	if (cnames.size() == 1)
	{
		// prefix(no time) 
		query_context->setOpr(eAosOpr_prefix);
		query_context->setStrValue(startName);
		return query_context;
	}

	//range
	query_context->setOpr(eAosOpr_range_ge_le);
	OmnString endName = AosCounterUtil::modifyStatType(cnames[1], stat_type);
	if (parm->cname_format != "") 
	{
		modifyCname(parm->cname_format, startName, endName, query_context, rdata);
	}
	endName << (char) 255; 
	query_context->setStrValue(startName);
	query_context->setStrValue2(endName);
	OmnScreen << "=============== startName: " << startName << " , endName: " << endName << endl;
	return query_context;
}


void
AosCounterClt::modifyCname(
		const OmnString &cname_format,
		OmnString &startName,
		OmnString &endName,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	//procss cname format:cn.js.10083.sz
	AosCounterUtil::CounterNameFormat format = AosCounterUtil::convertCnameFormat(cname_format);	
	if (format != AosCounterUtil::eAll) return ;

	OmnScreen << "format: " << cname_format << " , startName: " << startName << " , endName: " << endName << endl;
	int stat_field_idx = -1;
	OmnString fieldstr;
	startName = AosCounterUtil::decomposeCname(startName, fieldstr, stat_field_idx);
	endName = AosCounterUtil::decomposeCname(endName, fieldstr, stat_field_idx);

	AosFieldFilterPtr fieldfilter = OmnNew AosFieldFilter("statvalue");
	fieldfilter->setStatValue(stat_field_idx, eAosOpr_eq);
	fieldfilter->addFieldStr(fieldstr);

	query_context->setFieldFilterObj(fieldfilter);
	OmnScreen << "format: " << cname_format << " , startName: " << startName << " , endName: " << endName << endl;
}


bool
AosCounterClt::sortQueryRslt(
			const OmnString &counter_id,
			const AosStatType::E &stat_type,
			const AosCounterParameterPtr &parm,
			const AosQueryRsltObjPtr &queryRslt,
			const AosRundataPtr &rdata)
{
	if (!queryRslt)return true;
	// 1. sort
	// 2. retrieve query rslt
	AosRecordFormatPtr record_format = parm->getRecordFormat();
	aos_assert_r(queryRslt->getNumDocs() <= 100000, false);
	AosBuffArrayPtr buff_array = parm->getBuffArray(rdata); 
	buff_array->appendEntry(queryRslt, rdata.getPtrNoLock());
	buff_array->resetRecordLoop();
	buff_array->sort();
	buff_array->resetRecordLoop();

	char *buff;
	while (buff_array->nextValue(&buff))
	{
		OmnString key(buff);
		int start_pos = buff_array->getRecordLen();
		u64 docid = *(u64 *)&buff[start_pos];
		int64_t vv = docid;
		if (parm->need_convert_int64) vv = AosCounterUtil::convertValueToInt64(docid);
		record_format->addValue(counter_id, stat_type, key, vv, rdata);
	}

	record_format->addValue(counter_id, stat_type, "", 0, rdata);
	return true;
}



bool
AosCounterClt::retrieveQueryRslt(
			const OmnString &counter_id,
			const AosStatType::E &stat_type,
			const AosCounterParameterPtr &parm,
			const AosQueryRsltObjPtr &queryRslt,
			const AosRundataPtr &rdata)
{
	AosRecordFormatPtr record_format = parm->getRecordFormat();
	if (!queryRslt)return true;
	// normal retrieve query rslt 
	u64 docid;
	OmnString key;                                                
	bool finished = false;
	aos_assert_r(queryRslt->getNumDocs() <= AosCounterUtil::eMaxPerCounterEntry, false);

	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		if (finished) break;
		if (key != "")
		{
			int64_t vv = docid;
			if (parm->need_convert_int64) vv = AosCounterUtil::convertValueToInt64(docid);
OmnScreen << "key: " << key << " , vv: " << vv << endl;
			record_format->addValue(counter_id, stat_type, key, vv, rdata);
		}
	}

	if (finished) record_format->addValue(counter_id, stat_type, key, docid, rdata);
	return true;
}


//////////////////////////////////////// End //////////////////////////////////////////

bool 
AosCounterClt::counterRange(
			const OmnString &iilname,
			const OmnString &startName,
			const OmnString &endName,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool proc_times,
			bool need_convert_int64,
			const AosCounterQueryPtr &counter_query,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata)
{
	if (!queryRslt) queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	queryRslt->setWithValues(true);
	queryRslt->setOperator(AosDataColOpr::toEnum(query_rslt_opr));

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_range_ge_le);
	query_context->setStrValue(startName);
	query_context->setStrValue2(endName);
	query_context->setBlockSize(AosCounterUtil::eMaxPerCounterEntry);

	AosIILClientObj::getIILClient()->counterRange(
		iilname, queryRslt, 0, query_context, counter_query, rdata); 

	if (!queryRslt) return true;
	u64 docid;
	OmnString key;                                                
	bool finished = false;
	aos_assert_r(queryRslt->getNumDocs() <= AosCounterUtil::eMaxPerCounterEntry, false);

	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		// 'buff' should be in the form:
		// 		[name, value]
		// 		[name, value]
		// 		...
		if (finished) break;
		if (key != "")
		{
			OmnString str = key;
			if (proc_times) str = AosCounterUtil::getTimePart(key);
			if (str == "")
			{
				OmnAlarm << "timestr is empty: " << key << enderr;
			}
			else
			{
				buff->setOmnStr(str);
				int64_t vv = docid;
				if (need_convert_int64) vv = AosCounterUtil::convertValueToInt64(docid);
				buff->setI64(vv);
			}
		}
	}
	return true;
}


bool 
AosCounterClt::counterPrefix(
			const OmnString &iilname,
			const OmnString &name,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool proc_times,
			bool need_convert_int64,
			const AosCounterQueryPtr &counter_query,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata)
{
	if (!queryRslt) queryRslt = AosQueryRsltObj::getQueryRsltStatic();
	queryRslt->setWithValues(true);
	queryRslt->setOperator(AosDataColOpr::toEnum(query_rslt_opr));

	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_prefix);
	query_context->setStrValue(name);
	query_context->setBlockSize(AosCounterUtil::eMaxPerCounterEntry);

	AosIILClientObj::getIILClient()->counterRange(
		iilname, queryRslt, 0, query_context, counter_query, rdata); 

	if (!queryRslt) return true;
	u64 docid;
	OmnString key;                                                
	bool finished = false;
	aos_assert_r(queryRslt->getNumDocs() <= AosCounterUtil::eMaxPerCounterEntry, false);

	while(queryRslt->nextDocidValue(docid, key, finished, rdata))
	{
		// 'buff' should be in the form:
		// 		[name, value]
		// 		[name, value]
		// 		...
		if (finished) break;
		if (key != "")
		{
			OmnString str = key;
			if (proc_times) str = AosCounterUtil::getTimePart(key);

			if (str == "")
			{
				OmnAlarm << "timestr is empty: " << key << enderr;
			}
			else
			{
				buff->setOmnStr(str);
				int64_t vv = docid;
				if (need_convert_int64) vv = AosCounterUtil::convertValueToInt64(docid);
				buff->setI64(vv);
			}
		}
	}
	return true;
}

/*
bool finished1 = false;
vector<OmnString> str;
int n = AosStrSplit::splitStrBySubstr(startName.data(), ".", str, 100, finished1);
OmnString name;
for (u32 i = 0; i < str.size(); i++)
{
	if (name != "") name << ".";
	if (i == 1) 
	{
		name << "hl";
	}
	else
	{
		name << str[i];
	}
}
finished1 = false;
str.clear();
n = AosStrSplit::splitStrBySubstr(endName.data(), ".", str, 100, finished1);
OmnString name1;
for (u32 i = 0; i < str.size(); i++)
{
	if (name1 != "") name1 << ".";
	if (i == 1) 
	{
		name1 << "hl";
	}
	else
	{
		name1 << str[i];
	}
}

OmnScreen << "name: " << name << " , " << name1 << endl;
query_context->addMultiCond(eAosOpr_range_ge_le, name, 0, name1, 0);
query_context->addMultiCond(eAosOpr_range_ge_le, startName, 0, endName, 0);
*/
