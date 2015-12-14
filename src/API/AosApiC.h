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
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiC_h
#define AOS_API_ApiC_h

#include "DfmUtil/DfmConfig.h"
#include "Jimo/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "StorageMgrUtil/SignatureAlgo.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageMgrUtil/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "Value/Ptrs.h"
#include "XmlUtil/Ptrs.h"


extern AosXmlTagPtr AosCreateDoc(
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata);

// Chen Ding, 2013/05/13
extern bool AosCreateDoc(
				const AosRundataPtr &rdata,
				const OmnString &docstr,
				const bool is_public);

extern AosXmlTagPtr AosCreateDoc(
				const OmnString &docstr,
				const bool is_public,
				const AosRundataPtr &rdata);

extern u64 AosCreateIIL(
		 		const AosIILType iiltype,
		  		const bool isPersis,
		   		const AosRundataPtr &rdata);

extern u64 AosCreateIIL(
				const OmnString &iilname,
		 		const AosIILType iiltype,
		  		const bool isPersis,
		   		const AosRundataPtr &rdata);

extern u64 AosCreateSizeId(
				const int size,
				const AosXmlTagPtr &record,
				const AosRundataPtr &rdata);

extern u64 AosCreateSizeId(
				const u64 &record_docid,
				const AosRundataPtr &rdata);

extern bool AosCheckSecurityCreateJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata);

extern bool AosCheckSecurityRunJob(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata);

extern bool AosCheckSecurityAbortJob(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata);

extern bool AosCheckSecurityPauseJob(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata);

extern bool AosCheckSecurityResumeJob(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata);

extern bool AosCheckSecurityPauseTask(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata);

extern bool AosCheckSecurityResumeTask(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata);

extern AosXmlTagPtr AosCreateBinaryDoc(
				const int &vid,
				const OmnString &docstr,
				const bool is_public,
				const AosBuffPtr &buff, 
				const AosRundataPtr &rdata);

// Ketty 2012/09/10
extern bool AosCheckCheckSum(const char *data, const int len);

// Ketty 2012/09/10
extern u8 AosCalculateCheckSum(const char *data, const int len);

extern OmnString AosCalculateSign(
		const AosSignatureAlgo::E algo,
		const char *data, 
		const int len); 

// Ketty 2012/10/18
extern bool	AosCheckSvrDeath(const u32 svr_id, bool &svr_death);


extern u64 AosCreateVirtualDir(
				const OmnString &name,
				AosRundata *rdata);

extern bool AosConvertAsciiBinary(OmnString &str);
extern bool AosUnConvertAsciiBinary(OmnString &str);

extern AosDocFileMgrObjPtr AosCreateDocFileMgr(
		const AosRundataPtr &rdata,
		const u32 virtual_id,
		const AosDfmConfig &config);
// Chen Ding, 2013/02/08
extern AosBitmapObjPtr AosCreateBitmap(
				AosRundata *rdata, 
				const AosBuffPtr &buff);

// Chen Ding, 2013/03/01
extern bool AosComputeQueryResults(
		AosRundata *rdata, 
		const u64 &iilid, 
		const AosQueryContextObjPtr &context, 
		const AosBitmapObjPtr &bitmap, 
		const u64 &query_id, 
		const int physical_id);

// Chen Ding, 2013/03/02
extern AosBuffPtr AosCreateMsgHead(
		AosRundata *rdata, 
		const u32 send_cubeid,
		const u32 recv_cubeid,
		const OmnString &msgid);

// Chen Ding, 2013/03/02
extern AosBuffPtr AosCreateMsgHead(
		AosRundata *rdata, 
		const int send_physical_id,
		const int recv_physicalid,
		const OmnString &msgid);

// Chen Ding, 2013/05/04
extern bool AosCallDLLFunc(
		AosRundata *rdata, 
		const OmnString &libname,
		const OmnString &method, 
		const u32 version);

extern AosDLLObjPtr AosCreateDLLObj(
		AosRundata *rdata, 
		const AosXmlTagPtr &def);

extern AosDLLObjPtr AosCreateDLLObj(
		AosRundata *rdata, 
		const AosXmlTagPtr &def, 
		const OmnString &libname, 
		const OmnString &method,
		const u32 version);

extern bool AosCheckIgnoredAlarms();

extern AosXmlTagPtr AosCreateDocByWorker(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker);

// Chen Ding, 2013/07/26
inline OmnString AosConvertToScientific(const u64 &value)
{
	OmnString ss;
	ss << value;
	ss.convertToScientific();
	return ss;
}

inline OmnString AosConvertToScientific(const int64_t &value)
{
	if (value < 0)
	{
		u64 vv = (u64)(value*-1);
		OmnString ss = "-";
		ss << vv;
		ss.convertToScientific();
		return ss;
	}
	OmnString ss;
	ss << value;
	ss.convertToScientific();
	return ss;
}

inline OmnString AosConvertToScientific(const OmnString &value)
{
	OmnString ss = value;
	ss.convertToScientific();
	return ss;
}

// Chen Ding, 2013/11/14
extern AosXmlTagPtr AosCreateUserContainer(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_objid,
		const OmnString &ctnr_name,
		const OmnString &cid);

extern AosDatasetObjPtr AosCreateDataset(
		const AosRundataPtr &rdata, 
		const AosDataScannerObjPtr &data_source);

extern bool AosCreateIndex(
		AosRundata *rdata, 
		const AosIILType type, 
		u64 &iilid);

// extern AosValuePtr AosCreateValue(const u64 value);
// extern AosValuePtr AosCreateValue(const OmnString &value);
// extern AosValuePtr AosCreateValue(const char *data, const int len);
// extern AosValuePtr AosCreateValue( AosRundata *rdata, const AosXmlTagPtr &def);

extern bool AosCheckAccess(
		AosRundata *rdata, 
		const OmnString &opr, 
		const AosXmlTagPtr &accessed_doc);

extern AosAccessRcdPtr AosCreateAccessRecord(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &doc);

extern AosAccessRcdPtr AosCreateAccessRecord(
		const AosRundataPtr &rdata,
		const OmnString &cloudid, 
		const u64 &docid);

extern AosAccessRcdPtr AosConvertToAccessRecord(
		AosRundata *rdata, 
		const AosXmlTagPtr &acd_doc);

extern bool AosCheckCreateDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &container,
		const AosXmlTagPtr &doc);

/*
extern AosDataAssemblerObjPtr AosCreateIILAssembler(
		AosRundata *rdata, 
		const OmnString &dataColId, 
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &def);

extern AosDataAssemblerObjPtr AosCreateDocAssembler(
		AosRundata *rdata, 
		const OmnString &dataColId, 
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &proc_def);
*/

// Chen Ding, 2014/01/03
extern AosQueryCacherObjPtr AosCreateQueryCacher(AosRundata *rdata);
extern bool AosCreateIndexEntry(
		AosRundata *rdata, 
		const AosXmlTagPtr &def);

extern AosXmlTagPtr AosGetIndexDefDoc(
		AosRundata *rdata, 
		const OmnString &table_name, 
		const OmnString &field_name);

extern AosXmlTagPtr AosGetIndexDefDoc(
		AosRundata *rdata, 
		const OmnString &iilname);

extern bool AosCheckIsLogin(const AosRundataPtr &rdata);

// Chen Ding, 2014/07/26
extern AosJimoLogicObjPtr AosCreateJimoLogic(
		const AosXmlTagPtr &def, 
		AosRundata *rdata);

extern bool AosCheckSequenceExist(
		const AosRundataPtr &rdata,
		const OmnString &name,
		const OmnString &func);

#endif

