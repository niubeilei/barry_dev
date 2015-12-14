////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_CounterCltObj_h
#define Aos_SEInterfaces_CounterCltObj_h

#include "alarm_c/alarm.h"
#include "CounterUtil/StatTypes.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "UtilTime/TimeGran.h"
#include "XmlUtil/Ptrs.h"

class AosCounterCltObj : virtual public OmnRCObject
{
private:
	static AosCounterCltObjPtr	smCounterClt;

public:
	virtual bool procCounter(
			const OmnString &cname, 
			const int64_t &value, 
			const AosTimeGran::E time_type,
			const OmnString &timeStr,
			const OmnString &timeFromat,
			const OmnString &statType,
			const OmnString &operation,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveSingleCounter(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			const u64 &start_time,
			const u64 &end_time,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveMultiCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveMultiCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const u64 &start_time,
			const u64 &end_time,
			const int64_t &dft_value,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveCounters(
			const OmnString &counter_id,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveCounters(
			const OmnString &counter_id,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveCounters(
			const vector<OmnString> &counter_ids,
			const OmnString &cname,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata) = 0;

	virtual bool retrieveCounters(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata) = 0;


	virtual bool retrieveSubCountersBySingle(
			const vector<OmnString> &counter_ids,
			const vector<OmnString> &cnames,
			const vector<AosStatType::E> stat_types,
			const OmnString &member,
			AosBuffPtr &buff,
			bool use_iilname,
			const AosRundataPtr &rdata) = 0;


public:
	static void setCounterClt(const AosCounterCltObjPtr &d) {smCounterClt = d;}
	static AosCounterCltObjPtr getCounterClt() {return smCounterClt;}
};
#endif
