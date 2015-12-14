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
#include "API/AosApiC.h"

#include "API/AosApi.h"
#include "aosDLL/DllMgr.h"
#include "AppMgr/App.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Security/SessionMgr.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/IndexMgrObj.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/QueryCacherObj.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEInterfaces/SequenceMgrObj.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/AccessRcdMgr.h"


static AosIILClientObjPtr sgIILClient;
static AosDocClientObjPtr sgDocClient;

AosXmlTagPtr AosCreateDoc(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(sgDocClient, 0);
	return sgDocClient->createDoc(doc, rdata);
}


AosXmlTagPtr AosCreateDoc(
		const OmnString &docstr,
		const bool is_public,
		const AosRundataPtr &rdata)
{
	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(sgDocClient, 0);

	return sgDocClient->createDocSafe1(rdata, docstr, rdata->getCid(), 
				"", is_public, false, false, false, false, true);
}


AosXmlTagPtr AosCreateBinaryDoc(
		const int &vid,
		const OmnString &docstr,
		const bool is_public,
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	if (!sgDocClient) sgDocClient = AosDocClientObj::getDocClient();
	aos_assert_r(sgDocClient, 0);

	return sgDocClient->createBinaryDoc(docstr, is_public, buff, vid, rdata);
}


u64 AosCreateIIL( 
		const AosIILType iiltype,
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	if (!sgIILClient) sgIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(sgIILClient, 0);

	bool rslt;
	if (isPersis)
	{
		rslt = sgIILClient->createTablePublic(iilid, iiltype, rdata);
	}
	else
	{
		rslt = sgIILClient->createIILPublic(iilid, iiltype, rdata);
	}
	aos_assert_r(rslt, false);
	return iilid;
}


bool AosCheckIsLogin(const AosRundataPtr &rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkIsLogin(rdata);
}


u64 AosCreateIIL( 
		const OmnString &iilname,
		const AosIILType iiltype,
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	u64 iilid = 0;
	if (!sgIILClient) sgIILClient = AosIILClientObj::getIILClient();
	aos_assert_r(sgIILClient, 0);

	bool rslt;
	if (isPersis)
	{
		rslt = sgIILClient->createTablePublic(iilname, iilid, iiltype, rdata);
	}
	else
	{
		rslt = sgIILClient->createIILPublic(iilname, iilid, iiltype, rdata);
	}
	aos_assert_r(rslt, false);
	return iilid;
}


u64 AosCreateSizeId(
		const int size,
		const AosXmlTagPtr &record,
		const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->createSizeId(size, record, rdata);
}


u64 AosCreateSizeId(
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	AosSizeIdMgrObjPtr obj = AosSizeIdMgrObj::getSizeIdMgr();
	aos_assert_r(obj, 0);
	return obj->createSizeId(record_docid, rdata);
}

u64 AosSendToFrontEndCreateSizeId(
		const int size,
		const AosXmlTagPtr &record,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

u64 AosSendToFrontEndCreateSizeId(
		const int size,
		const u64 &record_docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool AosCheckSecurityCreateJob(
		const AosXmlTagPtr &jobdoc,
		const AosRundataPtr &rdata)
{
	// Ketty 2012/05/12
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkCreateJob(jobdoc, rdata);
}

bool AosCheckSecurityRunJob(
		const AosXmlTagPtr &jobdoc,
		AosRundata *rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkRunJob(jobdoc, rdata);
}

bool AosCheckSecurityAbortJob(
		const AosXmlTagPtr &jobdoc,
		AosRundata *rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkAbortJob(jobdoc, rdata);
}

bool AosCheckSecurityPauseJob(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkPauseJob(jobdoc, rdata);
}

bool AosCheckSecurityResumeJob(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkResumeJob(jobdoc, rdata);
}

bool AosCheckSecurityPauseTask(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkPauseTask(jobdoc, rdata);
}

bool AosCheckSecurityResumeTask(
				const AosXmlTagPtr &jobdoc,
				AosRundata *rdata)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkResumeTask(jobdoc, rdata);
}


u8 AosCalculateCheckSum(const char *data, const int len)
{
	u8 sum = 0;
	for (int i=0; i<len; i++)
	{
		sum += data[i];
	}

	sum = -sum;
	return sum;
}

bool AosCheckCheckSum(const char *data, const int len)
{
	u8 sum = 0;
	for (int i=0; i<len; i++)
	{
		sum += data[i];
	}

	return sum==0;
}

extern OmnString AosCalculateSign(
		const AosSignatureAlgo::E algo,
		const char *data, 
		const int len)
{
	switch (algo)
	{
	case AosSignatureAlgo::eMD5:
		return AosMD5Encrypt(OmnString(data, len)); 

	case AosSignatureAlgo::eSHA1:
		OmnNotImplementedYet;
		return "";

	default:
		break;
	}

	OmnAlarm << "invalid signature algo:" << algo << enderr;
	return "";
}


bool	AosCheckSvrDeath(const u32 svr_id, bool &svr_death)
{
	svr_death = true;
	OmnScreen << "Not Implemented yet." << endl;
	return true;
}

/*
u64 AosCreateVirtualDir(
		        const OmnString &name,
				AosRundata *rdata)
{
	AosStorageMgrObjPtr storagemgr = AosStorageMgrObj::getStorageMgr();
	aos_assert_r(storagemgr, 0);
	return storagemgr->createVirtualDir(name, rdata);
}
*/


bool
AosConvertAsciiBinary(OmnString &str)
{
	// This function checks whether 'str' is in the format:
	// 		0xdd
	// where dd are two hex codes. If yes, it converts str
	// to one character whose ASCII code is 0xdd.
	if (str.length() != 4) return true;

	char *data = (char *)str.data();
	if (!data) return true;
	if (data[0] != '0' || (data[1] != 'x' && data[1] != 'X')) return true;

	char c = data[2];
	u32 code1 = 0;
	if ((c >= '0' && c <= '9')) 
	{
		code1 = c - '0';
	}
	else 
	{
		if (c >= 'a' && c <= 'f') code1 = c - 'a' + 10;
		else return true;
	}

	c = data[3];
	u32 code2 = 0;
	if ((c >= '0' && c <= '9')) 
	{
		code2 = c - '0';
	}
	else 
	{
		if (c >= 'a' && c <= 'f') code2 = c - 'a' + 10;
		else return true;
	}

	str.setLength(1);
	data[0] = (char)((code1 << 4) + code2);
	return true;
}


bool
AosUnConvertAsciiBinary(OmnString &str)
{
	// This function checks whether 'str' is in the format:
	// 		0xdd
	// where dd are two hex codes. If yes, it converts str
	// to one character whose ASCII code is 0xdd.
	if (str.length() != 1) return true;

	char *data = (char *)str.data();
	if (!data) return true;

	char c = data[0];
	str.setLength(4);
	data[0] = '0';
	data[1] = 'x';

	u32 code1 = 0;
	if (c >= 16)
	{
		code1 = c / 16;
	}
	if (code1 >= 10) 
	{
		data[2] = 'a' + code1 - 10;
	}
	else 
	{
		data[2] = '0' + code1;
	}

	u32 code2 = c % 16;
	if (code2 >= 10) 
	{
		data[3] = 'a' + code2 - 10;
	}
	else 
	{
		data[3] = '0' + code2;
	}
	return true;
}


// Chen Ding, 2013/02/11
// AosDocFileMgrObjPtr AosCreateDocFileMgr(
// 			const u32 virtual_id,
// 		    const AosModuleId::E module_id,
// 			const OmnString &prefix,
// 			const AosDevLocationPtr &location,
// 			AosRundata *rdata)
AosDocFileMgrObjPtr AosCreateDocFileMgr(
		const AosRundataPtr &rdata,
		const u32 virtual_id,
		const AosDfmConfig &config)
		//const AosModuleId::E module_id,
		//const AosDfmProcPtr &proc,
		//AosRundata *rdata)
{
	// Chen Ding, 2013/02/11
	// AosStorageMgrObjPtr obj = AosStorageMgrObj::getStorageMgr();
	// aos_assert_rr(obj, rdata, 0);
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);
	//return obj->createDocFileMgr(virtual_id, module_id, proc, rdata);
	return vfsMgr->createDocFileMgr(rdata.getPtr(), virtual_id, config);
}


// Chen Ding, 2013/02/08
AosBitmapObjPtr AosCreateBitmap(
		AosRundata *rdata, 
		const AosBuffPtr &buff)
{
	AosBitmapObjPtr obj = AosBitmapObj::getObject();
	aos_assert_r(obj, 0);
	return obj->createBitmap(rdata, buff);
}


// Chen Ding, 2013/05/04
bool AosCallDLLFunc(
		AosRundata *rdata, 
		const OmnString &libname, 
		const OmnString &method, 
		const u32 version)
{
	AosDllMgrObj * dllmgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(dllmgr, rdata, false);
	return dllmgr->callFunc(rdata, libname, method, version);
}


// Chen Ding, 2013/05/04
AosDLLObjPtr AosCreateDLLObj(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	aos_assert_rr(def, rdata, 0);
	OmnString libname = def->getAttrStr(AOSTAG_DLL_LIBNAME); 
	aos_assert_rr(libname != "", rdata, 0);
	OmnString method = def->getAttrStr(AOSTAG_DLL_METHOD); 
	aos_assert_rr(method != "", rdata, 0);
	u32 version = def->getAttrInt(AOSTAG_DLL_VERSION, -1);

	AosDllMgrObj * dllmgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(dllmgr, rdata, 0);
	return dllmgr->createDLLObj(rdata, def, libname, method, version);
}


// Chen Ding, 2013/05/04
AosDLLObjPtr AosCreateDLLObj(
		AosRundata *rdata, 
		const AosXmlTagPtr &def, 
		const OmnString &libname, 
		const OmnString &method,
		const u32 version)
{
	AosDllMgrObj * dllmgr = AosDllMgrObj::getDllMgr1();
	aos_assert_rr(dllmgr, rdata, 0);
	return dllmgr->createDLLObj(rdata, def, libname, method, version);
}


bool AosCheckIgnoredAlarms()
{
	return OmnApp::checkIgnoredAlarms();
}


AosXmlTagPtr AosCreateDocByWorker(
		AosRundata *rdata, 
		const AosXmlTagPtr &worker)
{
	bool rslt = AosActionObj::runAction(rdata, AOSACTTYPE_MODIFYATTR, worker);
	aos_assert_r(rslt, 0);
	return rdata->getCreatedDoc();
}


// Chen Ding, 2013/10/02
OmnString AosConvertToScientific(const u64 value)
{
	OmnString ss;
	ss << value;
	ss.convertToScientific();
	return ss;
}


OmnString AosConvertToScientific(const int64_t value)
{
	OmnString ss;
	ss << value;
	ss.convertToScientific();
	return ss;
}



AosXmlTagPtr AosCreateUserContainer(
		const AosRundataPtr &rdata,
		const OmnString &ctnr_objid,
		const OmnString &ctnr_name,
		const OmnString &cid)
{
	AosObjMgrObjPtr mgr = AosObjMgrObj::getObjMgr();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createUserContainer(rdata, ctnr_objid, ctnr_name, cid);
}

bool AosCreateIndex(
		AosRundata *rdata, 
		const AosIILType type, 
		u64 &iilid)
{
	AosIILClientObjPtr client = AosIILClientObj::getIILClient();
	aos_assert_rr(client, rdata, false);
	return client->createIILPublic(iilid, type, rdata);
}


/*
AosValuePtr AosCreateValue(const u64 value)
{
	return OmnNew AosValueU64(value);
}

AosValuePtr AosCreateValue(const OmnString &value)
{
	return OmnNew AosValueStr(value);
}

AosValuePtr AosCreateValue(const char *data, const int len)
{
	return OmnNew AosValueStr(data, len);
}

AosValuePtr AosCreateValue(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return AosValue::createValue(rdata, def);
}

AosValuePtr AosCreateValue(const u32 value)
{
	return OmnNew AosValueU32(value);
}

AosValuePtr AosCreateValue(const u16 value)
{
	return OmnNew AosValueU16(value);
}

AosValuePtr AosCreateValue(const u8 value)
{
	return OmnNew AosValueU8(value);
}

AosValuePtr AosCreateValue(const int64_t value)
{
	return OmnNew AosValueInt64(value);
}

AosValuePtr AosCreateValue(const int32_t value)
{
	return OmnNew AosValueInt64(value);
}

AosValuePtr AosCreateValue(const int16_t value)
{
	return OmnNew AosValueInt16(value);
}

AosValuePtr AosCreateValue(const char value)
{
	return OmnNew AosValueChar(value);
}

AosValuePtr AosCreateValue(const float value)
{
	return OmnNew AosValueFloat(value);
}

AosValuePtr AosCreateValue(const double value)
{
	return OmnNew AosValueDouble(value);
}
*/

// Chen Ding, 2013/12/07
bool AosCheckAccess(
		AosRundata *rdata, 
		const OmnString &opr, 
		const AosXmlTagPtr &accessed_doc)
{
	AosSecurityMgrObjPtr security = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(security, rdata, false);
	return security->checkAccess(opr, accessed_doc, rdata);
}


AosAccessRcdPtr AosCreateAccessRecord(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &doc)
{
	AosObjMgrObjPtr mgr = AosObjMgrObj::getObjMgr();
	aos_assert_rr(mgr, rdata, 0);
	AosXmlTagPtr dd = mgr->createAccessRecord(rdata, doc);
	aos_assert_rr(dd, rdata, 0);
	return AosAccessRcdMgr::getSelf()->convertToAccessRecord(rdata, dd);
}


AosAccessRcdPtr AosCreateAccessRecord(
		const AosRundataPtr &rdata,
		const OmnString &cloudid, 
		const u64 &docid)
{
	AosObjMgrObjPtr mgr = AosObjMgrObj::getObjMgr();
	aos_assert_rr(mgr, rdata, 0);
	return mgr->createAccessRecord1(rdata, cloudid, docid);
}


AosAccessRcdPtr AosConvertToAccessRecord(
		AosRundata *rdata, 
		const AosXmlTagPtr &acd_doc)
{
	return AosAccessRcdMgr::getSelf()->convertToAccessRecord(rdata, acd_doc);
}


bool AosCheckCreateDoc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &container,
		const AosXmlTagPtr &doc)
{
	AosSecurityMgrObjPtr theobj = AosSecurityMgrObj::getSecurityMgr();
	aos_assert_rr(theobj, rdata, false);
	return theobj->checkCreateDoc(rdata, container, doc);
}


/*
AosDataAssemblerObjPtr AosCreateIILAssembler(
		AosRundata *rdata, 
		const OmnString &dataColId, 
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &def)
{
	AosDataAssemblerObjPtr obj = AosDataAssemblerObj::getDataAssembler();
	aos_assert_rr(obj, rdata, 0);
	return obj->createIILAssembler(rdata, dataColId, task, def);
}


AosDataAssemblerObjPtr AosCreateDocAssembler(
		AosRundata *rdata, 
		const OmnString &dataColId, 
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &proc_def)
{
	AosDataAssemblerObjPtr obj = AosDataAssemblerObj::getDataAssembler();
	aos_assert_rr(obj, rdata, 0);
	return obj->createDocAssembler(rdata, dataColId, task, proc_def);
}
*/


AosQueryCacherObjPtr AosCreateQueryCacher(AosRundata *rdata)
{
	return AosQueryCacherObj::createQueryCacherStatic(rdata);
}


bool AosCreateIndexEntry(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	return AosIndexMgrObj::createIndexEntryStatic(rdata, def);
}


// Chen Ding, 2014/07/26
AosJimoLogicObjPtr AosCreateJimoLogic(
		const AosXmlTagPtr &def, 
		AosRundata *rdata)
{
	return AosJimoLogicObj::createJimoLogicStatic(def, rdata);
}


bool AosCheckSequenceExist(
		const AosRundataPtr &rdata,
		const OmnString &name,
		const OmnString &func)
{
	AosSequenceMgrObjPtr seqMgr = AosSequenceMgrObj::getSequenceMgr();
	aos_assert_r(seqMgr, false);
	return seqMgr->checkSequenceExist(rdata, name, func);
}

