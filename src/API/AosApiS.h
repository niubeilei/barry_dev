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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiS_h
#define AOS_API_ApiS_h

#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/SyncherType.h"
#include "TransClient/Ptrs.h"
#include "SvrProxyUtil/Ptrs.h"
#include "Util/TransId.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "Util/ValueRslt.h"


extern bool AosStrContainWord(					
 			const OmnString &words, 
 			const OmnString &word);

extern bool AosStrHasCommonWords(
			const OmnString &words1, 
			const OmnString &words2);

// Modified By Ketty 2012/03/28
/*
inline bool AosSendIILTrans(
					const u32 &distid,
					const AosBuffPtr &buff, 
					AosRundata *rdata)
{
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_r(iilclient, false);
	return iilclient->sendTrans(distid, buff, rdata);
}
*/	

extern bool	AosSetStrValueDocUnique(
		u64 &iilid, 
		const bool createFlag,
		const OmnString &key, 
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata); 

extern bool AosSetU64ValueDocUniqueToTable(
		u64 &iilid,
		const bool createFlag,
		const u64 &value,
		const u64 &docid,
		const bool must_same,
		const AosRundataPtr &rdata);

/*
// Chen Ding, 07/01/2012
extern bool AosSendCommandWithResponse(
		const AosModuleId::E module_id, 
		const int machine_id,
		const u32 server_id,
		const AosBuffPtr &buff, 
		AosXmlTagPtr &response,
		bool &timedout,
		AosRundata *rdata);

extern bool AosSendCommand(
		const AosModuleId::E module_id,
		const u32	server_id,
		const AosBuffPtr &buff, 
		AosRundata *rdata);
*/
extern bool AosSeparateFields(
		const char *data,
		const int data_len,
		const char *separator,
		const int sep_len,
		const bool case_sensitive,
		vector<AosDataFieldObjPtr> &fields,
		AosRundata *rdata);

extern OmnFilePtr AosStorageMgrOpenSysFile(
		const OmnString &filekey,
		const bool create_flag,
		AosRundata *rdata);

// Chen Ding, 2013/02/07
extern AosXmlTagPtr AosStr2Xml(
		AosRundata *rdata, 
		const OmnString &str AosMemoryCheckDecl);

// Chen Ding, 2013/08/22
extern AosXmlTagPtr AosStr2Xml(
		const char *str,
		const int len AosMemoryCheckDecl);
// Ketty 2013/03/15
extern bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans);

// Ketty 2013/03/15
extern bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans,
		bool &svr_death);

// Ketty 2013/03/15
extern bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans,
		bool &timeout,
		AosBuffPtr &resp);
	
// Ketty 2013/03/15
extern bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		bool &svr_death);

extern bool AosSendTransAsyncResp(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans);

extern bool AosSendTransSyncResp(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans);

extern bool AosSendTransSyncResp(
		const AosRundataPtr &rdata,
		vector<AosTransPtr> vt_trans);

extern bool AosSendManyTrans(
		const AosRundataPtr &rdata,
		vector<AosTransPtr> &vt_trans); 

// Ketty 2013/07/12
extern bool AosSendMsg(const AosAppMsgPtr &msg);

extern OmnString	AosSizeToHumanRead(const int64_t &size);
extern OmnString	AosTimestampToHumanRead(const int64_t &timestamp);

extern int AosSplitStr(
					const OmnString &str, 
					const char *separators, 
					vector<OmnString> &values, 
					const int max);

inline bool AosStr2U64(
		const char *data, 
		const int data_len, 
		const bool treat_null_as_error,
		u64 &value)
{
	if (!data || data_len <= 0) 
	{
		if (treat_null_as_error) return false;
		value = 0;
		return true;
	}

	char *endptr = 0;
	value = strtoull(data, &endptr, 10);
	if (endptr == data) 
	{
		// No digits found
		value = 0;
		return false;
	}

	return true;
}

inline bool AosStr2U64(
		const char *data, 
		const int data_len, 
		const bool treat_null_as_error,
		AosValueRslt &value)
{
	if (!data || data_len <= 0) 
	{
		if (treat_null_as_error) return false;
		value.setNull();
		return true;
	}

	char *endptr = 0;
	u64 vv = strtoull(data, &endptr, 10);
	if (endptr == data) 
	{
		// No digits found
		value.setNull();
		return false;
	}

	value.setU64(vv);
	return true;
}

inline u64 AosStr2U64(const OmnString &str)
{
	u64 vv;
	bool rslt = AosStr2U64(str.data(), str.length(), false, vv);
	if(!rslt)return 0;
	return vv;
}

inline bool AosStr2Int64(
		const char *data, 
		const int data_len, 
		const bool treat_null_as_error,
		int64_t &value)
{
	if (!data || data_len <= 0) 
	{
		if (treat_null_as_error) return false;
		value = 0;
		return true;
	}

	char *endptr;
	value = strtoll(data, &endptr, 10);
	if (!endptr)
	{
		// The entire string is invalid.
		value = 0;
		return false;
	}

	if (endptr == data) 
	{
		// No digits found
		value = 0;
		return false;
	}

	return true;
}

inline int64_t AosStrToLL(const char *data, const int data_len, const int64_t dft) 
{
	int64_t value;
	if (!AosStr2Int64(data, data_len, true, value)) return dft;
	return value;
}


inline bool AosStr2Double(
		const char *data, 
		const int data_len, 
		const bool treat_null_as_error,
		double &value)
{
	if (!data || data_len <= 0) 
	{
		if (treat_null_as_error) return false;
		value = 0;
		return true;
	}

	char *endptr;
	value = strtod(data, &endptr);
	if (endptr == data) 
	{
		// No digits found
		value = 0;
		return false;
	}

	return true;
}

extern bool AosSendDiskBadMsg();
extern bool	AosServerIsBusy(); 


extern bool AosStr2U64(
		const char* data,
		const int data_len,
		const bool treat_null_as_error,
		u64 &value);

extern int AosSkipWhitespaces(const u8 * const data, const int datalen, int idx);

extern bool	AosStrBatchAdd(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata);

extern bool	AosStrBatchDel(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata);

extern bool	AosStrBatchInc(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &init_value,
		const AosIILUtil::AosIILIncType incType,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata);

extern bool AosSyncEngineDocCreated(
		AosRundata *rdata, 
		const AosXmlTagPtr &doc);

extern bool AosSyncEngineDocCreated(
		AosRundata *rdata, 
		const OmnString &tablename,
		const AosDataRecordObj *record);

extern AosSyncherObjPtr AosSyncEngineGetSyncher(
		AosRundata *rdata, 
		const OmnString &sync_name, 
		const AosSyncherType::E type);

#endif
