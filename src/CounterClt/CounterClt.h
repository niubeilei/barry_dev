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
// 06/15/2011	Created by Lynch 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_CounterClt_CounterClt_h
#define AOS_CounterClt_CounterClt_h

#include "CounterClt/Ptrs.h"
#include "CounterUtil/StatTypes.h"
#include "CounterUtil/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "SEInterfaces/CounterCltObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "UtilTime/TimeGran.h"
#include "QueryUtil/Ptrs.h"
#include <vector>
using namespace std;

OmnDefineSingletonClass(AosCounterCltSingleton,
                        AosCounterClt,
                        AosCounterCltSelf,
                        OmnSingletonObjId::eCounterClt,
                        "CounterClt");

class AosCounterClt : public AosCounterCltObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxCnameLen = 1000
	};

	OmnMutexPtr				mLock;
	AosCounterAssemblerPtr	mAssembler;

public:
	AosCounterClt();
	~AosCounterClt();

	//
    // Singleton class interface
    //
    static AosCounterClt*    getSelf();
    virtual bool        start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);
	
	bool start(const AosXmlTagPtr &config);

//	bool procCounter(
//			const OmnString &cname, 
//			const AosRundataPtr &rdata);

//	bool procCounter(
//			const OmnString &cname, 
//			const int64_t &value,
//			const AosRundataPtr &rdata);

//	bool procCounter(
//			const OmnString &cname, 
//			const AosTimeGran::E time_type,
//			const AosRundataPtr &rdata);

//	bool procCounter(
//			const OmnString &cname, 
//			const int64_t &value, 
//			const AosTimeGran::E time_type,
//			const OmnString &timeStr,
//			const OmnString &timeFormat,
//			const AosRundataPtr &rdata);

	virtual bool procCounter(
			const OmnString &cname, 
			const int64_t &value, 
			const AosTimeGran::E time_type,
			const OmnString &timeStr,
			const OmnString &timeFormat,
			const OmnString &statType,
			const OmnString &operationStr,
			const AosRundataPtr &rdata);

	bool procCounter(
        	const OmnString &cname,
        	const int64_t &cvalue,
			const AosTimeGran::E time_type,
        	const OmnString &timeStr,
			const OmnString &timeFormat,
			const OmnString &statType,
			const OmnString &formatStr,
			const OmnString &operationStr,
        	const AosRundataPtr &rdata);

/*
	inline bool getCounter(
			const OmnString &cname, 
			int64_t &value,
			const AosRundataPtr &rdata)
	{
		return getCounter(cname, AosTimeGran::eNoTime, "", "", value, rdata);
	}

	bool getCounter(
			const OmnString &cname, 
			const AosTimeGran::E time_gran,
			const OmnString &timeStr,
			const OmnString &statType,
			int64_t &value,
			const AosRundataPtr &rdata);

	bool getCounterSet(
			const AosXmlTagPtr &request,
			AosXmlTagPtr &data,
			const AosRundataPtr &rdata);

	bool getSingleCountersByQuery(
			const AosXmlTagPtr &request,
			AosXmlTagPtr &data,
			const AosRundataPtr &rdata);

	bool getMultiCountersByQuery(
			const AosXmlTagPtr &request,
			AosXmlTagPtr &data,
			const AosRundataPtr &rdata);

	bool getSubCountersByQuery(
			const AosXmlTagPtr &request,
			AosXmlTagPtr &data,
			const AosRundataPtr &rdata);
			*/

	// Ketty 2013/03/25
	//bool sendBuffTrans(
	//		const int vid, 
	//		const AosBuffPtr &buff, 
	//		const AosRundataPtr &rdata);
/*
	bool getSubCountersByQuery(
            const OmnString &counter_id,
			const OmnString &key,
			const OmnString &member,
			const AosTimeGran::E time_gran,
			const OmnString &startime,
			const OmnString &endtime,
			const OmnString &formatStr,
			const OmnString &statTypeStr,
			const AosRundataPtr &rdata);
*/
	virtual bool retrieveMultiCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const u64 &start_time,
			const u64 &end_time,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata);

	virtual bool retrieveMultiCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata);

	virtual bool retrieveSingleCounter(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			const u64 &start_time,
			const u64 &end_time,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata);

	virtual bool retrieveCounters(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata);

	virtual	bool retrieveCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata);

	virtual bool retrieveCounters(
			const vector<OmnString> &counter_ids,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata);

	virtual bool retrieveCounters(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata);

	virtual bool retrieveSubCountersBySingle(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			const OmnString &member,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata);

	bool	retrieveCounters2(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			const AosCounterQueryPtr &counter_query,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool use_iilname,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	bool	retrieveCounters2(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			const AosCounterQueryPtr &counter_query,
			AosBuffPtr &buff,
			bool use_iilname,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	bool	retrieveCountersPrefix(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			const AosCounterQueryPtr &counter_query,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool use_iilname,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	bool	retrieveCountersPrefix(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			const AosCounterQueryPtr &counter_query,
			AosBuffPtr &buff,
			bool use_iilname,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	bool	retrieveCounters2ByNormal(
			const AosCounterParameterPtr &parm,
			const AosRundataPtr &rdata);

	bool	procCounter2(
			const OmnString &cname,
			const int64_t &cvalue,
			const AosTimeGran::E time_gran,
			const OmnString &timeStr,
			const OmnString &timeFormat,
			const OmnString &statTypeStr,
			const OmnString &formatStr,
			const OmnString &operationStr,
			const AosRundataPtr &rdata);

private:
	bool resolveCounter(
			const OmnString &cname,
		    OmnString &resolved_cname,
         	const AosRundataPtr &rdata);

	bool queryRange(
			const u64 &iilid,
			const OmnString &startName,
			const OmnString &endName,
			AosBuffPtr &buff,
			bool proc_times,
			bool use_sep,
			bool is_super_iil,
			const AosRundataPtr &rdata);

	bool queryPrefix(
			const u64 &iilid,
			const OmnString &prefix,
			AosBuffPtr &buff,
			bool use_sep,
			const AosRundataPtr &rdata);

	bool counterRange(
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
			const AosRundataPtr &rdata);

	bool counterPrefix(
			const u64 &iilid,
			const OmnString &name,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool proc_times,
			bool is_super_iil,
			bool need_convert_int64,
			const AosCounterQueryPtr &counter_query,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	bool counterRange(
			const OmnString &iilname,
			const OmnString &startName,
			const OmnString &endName,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool proc_times,
			bool need_convert_int64,
			const AosCounterQueryPtr &counter_query,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	bool counterPrefix(
			const OmnString &iilname,
			const OmnString &name,
			AosBuffPtr &buff,
			AosQueryRsltObjPtr &queryRslt,
			bool proc_times,
			bool need_convert_int64,
			const AosCounterQueryPtr &counter_query,
			const OmnString &query_rslt_opr,
			const AosRundataPtr &rdata);

	//bool queryRangePrefix(
	//		const OmnString &iilname,
	//		const OmnString &startName,
	//		const OmnString &endName,
	//		AosBuffPtr &buff,
	//		const AosRundataPtr &rdata);

/*
	bool dealResponse(
			AosXmlTagPtr &data,
		    const AosXmlTagPtr &resp,
		    const bool timeout,
		    const AosRundataPtr &rdata);
			*/
	
	void	modifyCname(
			const OmnString &cname_format,
			OmnString &startName,
			OmnString &endName,
			const AosQueryContextObjPtr &query_context,
			const AosRundataPtr &rdata);

	bool 	runQuery(
			const u64 &iilid,
			bool is_super_iil,
			const AosQueryContextObjPtr &query_context,
			const AosCounterQueryPtr &counter_query,
			const AosQueryRsltObjPtr &queryRslt,
			const AosRundataPtr &rdata);

	u64		getIILID(
			const OmnString &counter_id,
			const bool &use_iilname,
			bool &is_super_iil,
			const AosRundataPtr &rdata);

	AosQueryContextObjPtr	getQueryContext(
			const AosCounterParameterPtr &parm,
			const AosStatType::E &stat_type,
			const AosRundataPtr &rdata);

	bool	retrieveQueryRslt(
			const OmnString &counter_id,
			const AosStatType::E &stat_type,
			const AosCounterParameterPtr &parm,
			const AosQueryRsltObjPtr &queryRslt,
			const AosRundataPtr &rdata);

	bool	sortQueryRslt(
			const OmnString &counter_id,
			const AosStatType::E &stat_type,
			const AosCounterParameterPtr &parm,
			const AosQueryRsltObjPtr &queryRslt,
			const AosRundataPtr &rdata);
};
#endif

