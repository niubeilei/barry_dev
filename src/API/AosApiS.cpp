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
#include "API/AosApiS.h"

#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/StorageMgrObj.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/QueryEngineObj.h"
#include "SEInterfaces/DataFieldCreatorObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/IpcCltObj.h"
#include "SEInterfaces/SyncEngineClientObj.h"
#include "SEInterfaces/SyncherObj.h"
#include "TransBasic/AppMsg.h"
#include "SysMsg/DiskBadMsg.h"
#include "TransBasic/Trans.h"
#include "Util/StrParser.h"
#include "Util/StrSplit.h"
#include "XmlUtil/SeXmlParser.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


bool AosStrContainWord(					
		const OmnString &words, 
		const OmnString &word)
{
	return OmnStrParser1::containWord(words, word);
}


bool AosStrHasCommonWords(				
		const OmnString &words1, 
		const OmnString &words2)
{
	return words1.hasCommonWords(words2, AOS_STANDARD_WORD_SEP);
}


bool AosSetStrValueDocUnique(
		u64 &iilid, 
		const bool createFlag,
		const OmnString &key, 
		const u64 &docid, 
		const bool must_same,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->setStrValueDocUnique(iilid, createFlag, key, docid, must_same, rdata);
}


bool AosSetU64ValueDocUniqueToTable(
		u64 &iilid,
		const bool createFlag,
		const u64 &key,
		const u64 &value,
		const bool must_same,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->setU64ValueDocUniqueToTable(iilid,
		createFlag, key, value, must_same, rdata);
}



/*
bool AosSendCommand(
		const AosModuleId::E module_id, 
		const u32 server_id,
		const AosBuffPtr &buff, 
		AosRundata *rdata)
{
	// Ketty 2012/05/17
	if(!sgTransClient)
	{
		//sgTransClient = AosGetTransClient();
	}
	aos_assert_r(sgTransClient, -1);

	return sgTransClient->addTrans(rdata, buff, 0, false, server_id, module_id);	
}


// Chen Ding, 07/01/2012
bool AosSendCommandWithResponse(
		const AosModuleId::E module_id, 
		const int machine_id,
		const u32 server_id,
		const AosBuffPtr &buff, 
		AosXmlTagPtr &response,
		bool &timedout,
		AosRundata *rdata)
{
	if(!sgTransClient)
	{
		//sgTransClient = AosGetTransClient();
	}
	aos_assert_r(sgTransClient, -1);

	return sgTransClient->addTrans(buff, 0, false, machine_id, 
			server_id, module_id, response, timedout, rdata);	
}
*/

bool AosSeparateFields(
		const char *data,
		const int data_len,
		const char *separator,
		const int sep_len,
		const bool case_sensitive,
		vector<AosDataFieldObjPtr> &fields,
		AosRundata *rdata)
{
	// This function assumes 'data' is separated by 'separator' into a number
	// of fields. It parses 'data' and separates it into fields that are
	// defined as [start_pos, field_len]. The results are stored in 'start_pos'
	// and 'field_len'.
	//
	// Note that 'data' may not be null terminated. This function will make it
	// null terminated and then restore it upon return.
	OmnAlarm << enderr;
#if 0
	aos_assert_r(data, false);
	int start_idx = 0;
	char *pp = 0;

	char c = data[data_len];
	((char *)data)[data_len] = 0;
	u32 idx = 0;
	AosDataFieldObjPtr field;
	while (start_idx >= data_len)
	{
		if (case_sensitive)
		{
			pp = strcasestr((char*)&data[start_idx], separator);
		}
		else
		{
			pp = strstr((char*)&data[start_idx], separator);
		}

		if (idx >= fields.size())
		{
			field = AosDataFieldObj::createStrFieldStatic(rdata);
			aos_assert_rr(field, rdata, false);
			fields.push_back(field);
		}
		else
		{
			field = fields[idx];
		}

		if (!pp)
		{
			// this is the last one
			field->setStartPos(start_idx);
			field->setFieldLen(data_len - start_idx);
			((char *)data)[data_len] = c;
			return true;
		}
				
		field->setStartPos(start_idx);
		field->setFieldLen(pp - data - start_idx);
		start_idx = pp - data + sep_len;
		idx++;
	}

	((char *)data)[data_len] = c;
#endif
	return true;
}


bool AosSendQueryEngineMsg(
		const int physical_id, 
		const OmnString &msg, 
		AosRundata *rdata)
{
	AosQueryEngineObjPtr object = AosQueryEngineObj::getQueryEngine();
	aos_assert_rr(object, rdata, false);

	return object->sendMsg(physical_id, msg, rdata);
}


// Chen Ding, 2013/02/08
AosXmlTagPtr AosStr2Xml(
		AosRundata *rdata, 
		const OmnString &str AosMemoryCheckDecl)
{
	return AosXmlParser::parse(str AosMemoryCheckerFileLine);
}

// Chen Ding, 2013/08/22
AosXmlTagPtr AosStr2Xml(
		const char *str, 
		const int len AosMemoryCheckDecl)
{
	return AosXmlParser::parse(str, len AosMemoryCheckerFileLine);
}

// Ketty 2013/03/15
bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans)
{
	// sync call. untill ack is recved.
	
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);

	bool svr_death = false;
	return trans_clt->sendTrans(rdata, trans, svr_death);
}


extern bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans,
		bool &svr_death)
{
	// sync call. untill ack is recved.
	
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);

	return trans_clt->sendTrans(rdata, trans, svr_death);
}


// Ketty 2013/03/15
bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans,
		bool &timeout,
		AosBuffPtr &resp)
{
	// sync call. untill resp is recved.

	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);

	timeout = false;
	bool svr_death = false;
	return trans_clt->sendTrans(rdata, trans, resp, svr_death);
}


// Ketty 2013/03/15
bool AosSendTrans(
		const AosRundataPtr &rdata, 
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		bool &svr_death)
{
	// sync call. untill resp is recved.
	
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	return trans_clt->sendTrans(rdata, trans, resp, svr_death);
}


bool AosSendTransAsyncResp(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans)
{
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	return trans_clt->sendTransAsyncResp(rdata, trans);
}


bool AosSendTransSyncResp(
		const AosRundataPtr &rdata,
		const AosTransPtr &trans)
{
	// sync call. untill resp is recved.
	
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	AosBuffPtr resp;
	bool svr_death;
	return trans_clt->sendTrans(rdata, trans, resp, svr_death);
}

	
bool AosSendTransSyncResp(
		const AosRundataPtr &rdata,
		vector<AosTransPtr> vt_trans)
{
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	return trans_clt->sendTransSyncResp(rdata, vt_trans);
}


// Ketty 2013/03/15
bool AosSendManyTrans(
		const AosRundataPtr &rdata,
		vector<AosTransPtr> &vt_trans)
{
	// Ketty 2013/05/09
	//AosTransClientObjPtr trans_clt = AosTransClientObj::getTransClient();
	//aos_assert_r(trans_clt, false);

	//return trans_clt->addTrans(rdata, vt_trans);
	
	AosIpcCltObjPtr ipc_clt = AosIpcCltObj::getIpcClt();
	aos_assert_r(ipc_clt, false);
	
	for(u32 i=0; i<vt_trans.size(); i++)
	{
		//ipc_clt->sendTrans(rdata, vt_trans[i]);
		AosSendTrans(rdata, vt_trans[i]);
	}
	return true;
}


// Ketty 2013/07/12
bool AosSendMsg(const AosAppMsgPtr &msg)
{
	aos_assert_r(msg, false);
	AosIpcCltObjPtr ipc_clt = AosIpcCltObj::getIpcClt();
	aos_assert_r(ipc_clt, false);

	return ipc_clt->sendMsg(msg);
}


// Ken Lee 2013/04/18
OmnString
AosSizeToHumanRead(const int64_t &size)
{
	//this function using 1000 as the conversion unit
	OmnString str;
	if (size >> 30)
	{
		str << (size >> 30) << "G ";
	}
	if (size >> 20)
	{
		str << ((size >> 20) & 1023) << "M ";
	}
	if (size >> 10)
	{
		str << ((size >> 10) & 1023) << "K ";
	}
	str << (size & 1023);
	return str;
}


OmnString
AosTimestampToHumanRead(const int64_t &timestamp)
{
	int64_t usec_f = timestamp % 10000;
	int64_t usec = timestamp % 1000000 / 10000;
	int64_t sec = timestamp / 1000000;
	if (usec_f >= 5000 || (sec == 0 && usec == 0)) usec += 1;
	if (usec >= 100) {
		usec -= 100;
		sec += 1;
	}
	int64_t min = sec / 60; sec = sec % 60;
	int64_t hour = min / 60; min = min % 60;
	int64_t day = hour / 24; hour = hour % 24;
	
	OmnString str;
	if (day) str << day << "d "; 
	if (hour) str << hour << "h ";
	if (min) str << min << "m";
	str << sec;
	if (usec)
	{
		str << ".";
		if (usec < 10) str << "0";
		str << usec;
	}
	str << "s ";

	return str;
}


int AosSplitStr(
		const OmnString &str, 
		const char *separators, 
		vector<OmnString> &values, 
		const int max)
{
	return AosStrSplit::splitStrByChar(str, separators, values, max);
}


bool AosSendDiskBadMsg()
{
	// Chen Ding, 2013/12/01
	// Ketty: please move this code to somewhere else.
	// int self_svr_id = AosGetSelfServerId();
	// AosAppMsgPtr msg = OmnNew AosDiskBadMsg(self_svr_id, AOSTAG_SVRPROXY_PID); 
	// return  AosSendMsg(msg);
	OmnNotImplementedYet;
	return false;
}


bool
AosServerIsBusy()                   
{
	int fd = open("/proc/loadavg", O_RDONLY);
	if (fd == -1) return true;
	char buff[128];
	int num = read(fd, buff, 128);
	if (num == -1) return true;
	close(fd);
	float loadavg = atof(buff);
	return loadavg > 1;
}


/*
bool AosStr2U64(
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

	char *endptr;
	value = strtoull(data, &endptr, 10);
	if (endptr == data)
	{
		value = 0;
		return false;
	}

	return true;
}
*/


int AosSkipWhitespaces(const u8 * const data, const int datalen, int idx)
{
	// Chen Ding, 04/22/2010. Characters whose values are 
	// larger than 127 are Unicodes.
	// while (idx < datalen && (data[idx] <= 32 || data[idx] >= 127)) idx++;
	while (idx < datalen && data[idx] <= 32) idx++;
	return idx;
}

bool AosStrBatchAdd(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->StrBatchAdd(iilname, entry_len, buff,
			executor_id, snap_id, task_docid, rdata);
}

bool AosStrBatchDel(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->StrBatchDel(iilname, entry_len, buff,
			executor_id, snap_id, task_docid, rdata);
}

bool AosStrBatchInc(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &init_value,
		const AosIILUtil::AosIILIncType incType,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosIILClientObjPtr obj = AosIILClientObj::getIILClient();
	aos_assert_r(obj, false);
	return obj->StrBatchInc(iilname, entry_len, buff,
			init_value, incType, snap_id, task_docid, rdata);
}


// Young, 2015/10/15
bool AosSyncEngineDocCreated(
		AosRundata *rdata, 
		const AosXmlTagPtr &doc)
{
	OmnNotImplementedYet;
	return true;
	//AosSyncEngineClientObj *se_client = AosSyncEngineClientObj::getSyncEngineClient();
	//if (!se_client) return true;
	//return se_client->docCreated(rdata, doc);
}

// Young, 2015/10/15
bool AosSyncEngineDocCreated(
		AosRundata *rdata, 
		const OmnString &tablename,
		const AosDataRecordObj *record)
{
	//AosSyncEngineClientObj *se_client = AosSyncEngineClientObj::getSyncEngineClient();
	//if (!se_client) return true;
	//return se_client->docCreated(rdata, tablename, record);
	OmnNotImplementedYet;
	return true;
}

AosSyncherObjPtr
AosSyncEngineGetSyncher(
		AosRundata *rdata, 
		const OmnString &sync_name, 
		const AosSyncherType::E type)
{
	AosSyncEngineClientObjPtr se_client = AosSyncEngineClientObj::getSyncEngineClient();
	if (!se_client) return NULL;

	return se_client->getSyncher(rdata, sync_name, type);
}

