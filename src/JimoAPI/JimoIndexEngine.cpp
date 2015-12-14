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
//
// Modification History:
// 2014/12/23 Created by Yazong Ma
// 2015/03/14 Copied from IILPackage.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoIndexEngine.h"

#include "API/AosApiI.h"
#include "CounterUtil/CounterQuery.h"
#include "JimoAPI/JimoPackage.h"
#include "JimoAPI/JimoIndexFunc.h"
#include "JimoAPI/JimoClusterMgr.h"
#include "JimoAPI/JimoRepPolicyMgr.h"
#include "JimoCall/JimoCall.h"
#include "JimoCall/JimoCallMgr.h"
#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallSyncRead.h"
#include "JimoCall/JimoCallSyncNorm.h"
#include "JimoCall/JimoCallSyncWrite.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEUtil/IILName.h"
#include "Porting/StrUtil.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/DocTypes.h"
#include "Util/StrSplit.h"
#include "Util/String.h"

#include <vector>


static OmnString sgJimoIndexEnginePackageName = "AosIILPackageJimoCalls";
static bool sgShowLog = true;

namespace Jimo
{
AosXmlTagPtr getXmlFromBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, 0);

	u32 len = buff->getU32(0);
	aos_assert_r(len, 0);

	AosBuffPtr b = buff->getBuff(len, false AosMemoryCheckerArgs);
	aos_assert_r(b, 0);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(b->data(), len, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, 0);

	return xml;
}

bool jimoAddStrValueDoc(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(value.length() > 0, false);

	if (sgShowLog) OmnScreen << "Add Value doc, " << iilname
				<< ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();

	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncWrite(rdata,
				sgJimoIndexEnginePackageName, JimoIndexFunc::eAddStrValueDoc,
					distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, value);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoAddStrValueDoc(
        AosRundata *rdata,
        const OmnString &iilname,
        const OmnString &key,
        const u64 &docid,
        const bool value_unique,
        const bool docid_unique,
        const bool ispersis)
{
    aos_assert_rr(docid!= 0, rdata, 0);
    AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
    aos_assert_rr(cluster, rdata, 0);
    u32 distr_id = cluster->getDistrID(rdata, docid);

    AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
        sgJimoIndexEnginePackageName, JimoIndexFunc::eAddStrValueDoc, 
        distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
    jimo_call->arg(AosFN::eIILName, iilname);
    jimo_call->arg(AosFN::eKey, key);
    jimo_call->arg(AosFN::eValue, docid);
    jimo_call->arg(AosFN::eKeyUnique, value_unique);
    jimo_call->arg(AosFN::eValueUnique, docid_unique);
    jimo_call->arg(AosFN::eFlag, ispersis);
    jimo_call->makeCall(rdata);

    bool rslt = jimo_call->isCallSuccess();
    if (!rslt)
    {
        OmnAlarm << "syncCall failed" << enderr;
        return false;
    }

    return true;
}

bool jimoBitmapQueryNewSafe(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eBitmapQueryNewSafe, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);

	bool rslt;

	AosXmlTagPtr xml_rslt;
	rslt = query_rslt->serializeToXml(xml_rslt);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue1, xml_rslt->toString());

	AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
	query_bitmap->saveToBuff(buff);
	aos_assert_r(buff, false);
	jimo_call->arg(AosFN::eValue2, buff);


	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	return true;			
}

bool jimoBitmapRsltQuerySafe(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eBitmapRsltQuerySafe, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);

	bool rslt;

	AosXmlTagPtr xml_rslt;
	rslt = query_rslt->serializeToXml(xml_rslt);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue1, xml_rslt->toString());

	AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
	query_bitmap->saveToBuff(buff);
	aos_assert_r(buff, false);
	jimo_call->arg(AosFN::eValue2, buff);


	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	return true;
}


bool  jimoGetDocid(
		const OmnString &iilname,
		const OmnString &the_value,
		const AosOpr opr,
		const bool reverse,
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(AosOpr_valid(opr), false);

	if (sgShowLog) OmnScreen << "getdocid, " << iilname << ":"
		<< value << ":" << AosOpr_toStr(opr) << endl;


	aos_assert_r(value != "", false);

	u64 snap_id = rdata->getSnapshotId();

	// Create JimoCall
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata.getPtr(), docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata.getPtr(),
			    sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidByIILNameOpr,
				    distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, value);
	jimo_call->arg(AosFN::eOpr, opr);
	jimo_call->arg(AosFN::eReverse, reverse);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata.getPtr(), AosFN::eBuff);
	rslt = resp->getU8(0);
	if (rslt)
	{
		docid = resp->getU64(0);
//aos_assert_r(docid, false);
		isunique = resp->getU8(0);
	}
	else
	{
		docid = 0;
		isunique = false;
	}

	return true;
}



bool jimoGetDocid(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &key,
		const AosOpr opr,
		const bool reverse,
		u64 &docid,
		bool &isunique)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidByIILNameOpr, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eOpr, opr);
	jimo_call->arg(AosFN::eReverse, reverse);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	//xuqi 2105/10/16
	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	aos_assert_r(resp, false); 
	
	rslt = resp->getU8(0);           
	if(rslt)
	{
		docid = resp->getU64(0);
		isunique = resp->getU8(0);
	}
	else
	{
		docid = 0;
		isunique = true;
	}
	return true;
}

bool  jimoModifyStrValueDoc(
		const OmnString &iilname,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	int oldlen = oldvalue.length();
	int newlen = newvalue.length();
	aos_assert_r(oldlen > 0 && newlen > 0, false);

	char *olddata = (char *)oldvalue.data();
	char *newdata = (char *)newvalue.data();
	char old_c = 0, new_c = 0;
	bool too_long = false;
	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		old_c = olddata[AosIILUtil::eMaxStrValueLen];
		olddata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		new_c = newdata[AosIILUtil::eMaxStrValueLen];
		newdata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}

	if (too_long && strcmp(olddata, newdata) == 0)
	{
		// This means that the values are too long and they were
		// chopped. After the chopping, they become the same.
		// There is no need to send the request.                              
		if (oldlen >= AosIILUtil::eMaxStrValueLen)
		{
			olddata[AosIILUtil::eMaxStrValueLen] = old_c;
		}
		if (newlen >= AosIILUtil::eMaxStrValueLen)
		{
			newdata[AosIILUtil::eMaxStrValueLen] = new_c;
		}
		return true;
	}

	if (sgShowLog) OmnScreen << "Modify Value doc, "
		<< iilname << ":" << oldvalue << ":" << newvalue << ":" << docid << endl;

	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		olddata[AosIILUtil::eMaxStrValueLen] = old_c;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		newdata[AosIILUtil::eMaxStrValueLen] = new_c;
	}

	u64 snap_id = rdata->getSnapshotId();
	/*AosTransPtr trans = OmnNew AosIILTransStrModifyValueDoc(
			iilname, oldvalue, newvalue, docid,
			value_unique, docid_unique, override, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << oldvalue;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();                                                                       
	}
*/
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata.getPtr(),
				sgJimoIndexEnginePackageName, JimoIndexFunc::eModifyStrValueDoc, cluster);

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue1, oldvalue);
	jimo_call->arg(AosFN::eValue2, newvalue);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, override);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
	    OmnAlarm << "makeCall failed" << enderr;
	    return false;
	}

	return true;
}

bool  jimoModifyStrValueDoc(
		const u64 &iilid,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid != 0, false);

	int oldlen = oldvalue.length();
	int newlen = newvalue.length();
	aos_assert_r(oldlen > 0 && newlen > 0, false);

	char *olddata = (char *)oldvalue.data();
	char *newdata = (char *)newvalue.data();
	char old_c = 0, new_c = 0;
	bool too_long = false;
	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		old_c = olddata[AosIILUtil::eMaxStrValueLen];
		olddata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		new_c = newdata[AosIILUtil::eMaxStrValueLen];
		newdata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}

	if (too_long && strcmp(olddata, newdata) == 0)
	{
		// This means that the values are too long and they were
		// chopped. After the chopping, they become the same.
		// There is no need to send the request.                              
		if (oldlen >= AosIILUtil::eMaxStrValueLen)
		{
			olddata[AosIILUtil::eMaxStrValueLen] = old_c;
		}
		if (newlen >= AosIILUtil::eMaxStrValueLen)
		{
			newdata[AosIILUtil::eMaxStrValueLen] = new_c;
		}
		return true;
	}

	if (sgShowLog) OmnScreen << "Modify Value doc, "
		<< iilid << ":" << oldvalue << ":" << newvalue << ":" << docid << endl;

	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		olddata[AosIILUtil::eMaxStrValueLen] = old_c;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		newdata[AosIILUtil::eMaxStrValueLen] = new_c;
	}

	u64 snap_id = rdata->getSnapshotId();
	/*AosTransPtr trans = OmnNew AosIILTransStrModifyValueDoc(
			iilname, oldvalue, newvalue, docid,
			value_unique, docid_unique, override, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << oldvalue;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();                                                                       
	}
*/
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata.getPtr(),
				sgJimoIndexEnginePackageName, JimoIndexFunc::eModifyStrValueDoc, cluster);

	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eValue1, oldvalue);
	jimo_call->arg(AosFN::eValue2, newvalue);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, override);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
	    OmnAlarm << "makeCall failed" << enderr;
	    return false;
	}

	return true;
}

bool  jimoModifyU64ValueDoc(
		const OmnString &iilname,
		const u64 &oldvalue,
		const u64 &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	int oldlen = sizeof(oldvalue);
	int newlen = sizeof(newvalue);
	aos_assert_r(oldlen > 0 && newlen > 0, false);

	char *olddata = (char *)oldvalue;
	char *newdata = (char *)newvalue;
	char old_c = 0, new_c = 0;
	bool too_long = false;
	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		old_c = olddata[AosIILUtil::eMaxStrValueLen];
		olddata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		new_c = newdata[AosIILUtil::eMaxStrValueLen];
		newdata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}

	if (too_long && strcmp(olddata, newdata) == 0)
	{
		// This means that the values are too long and they were
		// chopped. After the chopping, they become the same.
		// There is no need to send the request.                              
		if (oldlen >= AosIILUtil::eMaxStrValueLen)
		{
			olddata[AosIILUtil::eMaxStrValueLen] = old_c;
		}
		if (newlen >= AosIILUtil::eMaxStrValueLen)
		{
			newdata[AosIILUtil::eMaxStrValueLen] = new_c;
		}
		return true;
	}

	if (sgShowLog) OmnScreen << "Modify Value doc, "
		<< iilname << ":" << oldvalue << ":" << newvalue << ":" << docid << endl;

	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		olddata[AosIILUtil::eMaxStrValueLen] = old_c;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		newdata[AosIILUtil::eMaxStrValueLen] = new_c;
	}

	u64 snap_id = rdata->getSnapshotId();
	/*AosTransPtr trans = OmnNew AosIILTransStrModifyValueDoc(
			iilname, oldvalue, newvalue, docid,
			value_unique, docid_unique, override, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << oldvalue;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();                                                                       
	}
*/
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata.getPtr(),
				sgJimoIndexEnginePackageName, JimoIndexFunc::eModifyU64ValueDoc, cluster);

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue1, oldvalue);
	jimo_call->arg(AosFN::eValue2, newvalue);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, override);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
	    OmnAlarm << "makeCall failed" << enderr;
	    return false;
	}

	return true;
}

bool  jimoModifyU64ValueDoc(
		const u64 &iilid,
		const u64 &oldvalue,
		const u64 &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid != 0, false);

	int oldlen = sizeof(oldvalue);
	int newlen = sizeof(newvalue);
	aos_assert_r(oldlen > 0 && newlen > 0, false);

	char *olddata = (char *)oldvalue;
	char *newdata = (char *)newvalue;
	char old_c = 0, new_c = 0;
	bool too_long = false;
	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		old_c = olddata[AosIILUtil::eMaxStrValueLen];
		olddata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		new_c = newdata[AosIILUtil::eMaxStrValueLen];
		newdata[AosIILUtil::eMaxStrValueLen] = 0;
		too_long = true;
	}

	if (too_long && strcmp(olddata, newdata) == 0)
	{
		// This means that the values are too long and they were
		// chopped. After the chopping, they become the same.
		// There is no need to send the request.                              
		if (oldlen >= AosIILUtil::eMaxStrValueLen)
		{
			olddata[AosIILUtil::eMaxStrValueLen] = old_c;
		}
		if (newlen >= AosIILUtil::eMaxStrValueLen)
		{
			newdata[AosIILUtil::eMaxStrValueLen] = new_c;
		}
		return true;
	}

	if (sgShowLog) OmnScreen << "Modify Value doc, "
		<< iilid << ":" << oldvalue << ":" << newvalue << ":" << docid << endl;

	if (oldlen >= AosIILUtil::eMaxStrValueLen)
	{
		olddata[AosIILUtil::eMaxStrValueLen] = old_c;
	}
	if (newlen >= AosIILUtil::eMaxStrValueLen)
	{
		newdata[AosIILUtil::eMaxStrValueLen] = new_c;
	}

	u64 snap_id = rdata->getSnapshotId();
	/*AosTransPtr trans = OmnNew AosIILTransStrModifyValueDoc(
			iilname, oldvalue, newvalue, docid,
			value_unique, docid_unique, override, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << oldvalue;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();                                                                       
	}
*/
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata.getPtr(),
				sgJimoIndexEnginePackageName, JimoIndexFunc::eModifyU64ValueDoc, cluster);

	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eValue1, oldvalue);
	jimo_call->arg(AosFN::eValue2, newvalue);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, override);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
	    OmnAlarm << "makeCall failed" << enderr;
	    return false;
	}

	return true;
}

bool jimoAddHitDoc(
		AosRundata *rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &docid,
		int &physical_id)
{
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddHitDocByTrans, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
*/
	physical_id = -1;
	return jimoAddHitDoc(rdata, iilname, docid);
}


bool jimoRemoveHitDoc(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &docid)
{
	//Xuqi 2015/11/6
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveHitDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}



bool jimoAddU64ValueDoc(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &key,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool ispersis)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddU64ValueDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, ispersis);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoAddU64ValueDoc(
		AosRundata *rdata,
		const u64 &iilid,
		const u64 &key,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool ispersis)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddU64ValueDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eKeyUnique, value_unique);
	jimo_call->arg(AosFN::eValueUnique, docid_unique);
	jimo_call->arg(AosFN::eFlag, ispersis);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoRemoveU64ValueDoc(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &key,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveU64ValueDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoRemoveU64ValueDoc(
		AosRundata *rdata,
		const u64 &iilid,
		const u64 &key,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveU64ValueDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoAddHitDoc(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddHitDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}



bool jimoRemoveU64ValueDoc(
		AosRundata *rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		int &physical_id)
{
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveU64ValueDocByNameTrans, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
*/
	physical_id = -1;
	return jimoRemoveU64ValueDoc(rdata, iilname, value, docid);
}


bool jimoAddU64ValueDoc(
		AosRundata *rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		int &physical_id)
{
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddU64ValueDocByTrans, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
*/
	physical_id = 0;
	return jimoAddU64ValueDoc(rdata, iilname, value, docid, value_unique, docid_unique, false);
}


bool jimoRemoveHitDoc(
		AosRundata *rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &docid)
{
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveHitDocByTrans, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
*/
			
	return jimoRemoveHitDoc(rdata, iilname, docid);
}


bool jimoAddStrValueDoc(
		AosRundata *rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const OmnString &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		int &physical_id)
{
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddStrValueDocByTrans, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
*/
	physical_id = -1;
	return jimoAddStrValueDoc(rdata, iilname, value, docid, value_unique, docid_unique, false);
}


bool jimoRemoveStrValueDoc(
		AosRundata *rdata,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const OmnString &value,
		const u64 &docid,
		int &physical_id)
{
/*
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveStrValueDocByNameTrans, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
*/
	physical_id = -1;
	return jimoRemoveStrValueDoc(rdata, iilname, value,docid);
}


bool  jimoRemoveStrValueDoc(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(the_value.length() > 0, false);

	if (docid == 0)
	{
		AosLogError(rdata, true, "docid is 0") << enderr;
		return false;
	}

	if (sgShowLog) OmnScreen << "Remove value, "
		<< iilname << ":" << the_value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();

	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata.getPtr(), docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata.getPtr(),
			    sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveStrValueDoc,
				    distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, the_value);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	/*AosTransPtr trans = OmnNew AosIILTransStrRemoveValueDocByName(
			iilname, the_value, docid, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << the_value;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();
	}
	*/

	return true;
}

bool  jimoRemoveStrValueDoc(
		const u64 &iilid,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid != 0, false);
	aos_assert_r(the_value.length() > 0, false);

	if (docid == 0)
	{
		AosLogError(rdata, true, "docid is 0") << enderr;
		return false;
	}

	if (sgShowLog) OmnScreen << "Remove value, "
		<< iilid << ":" << the_value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();

	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata.getPtr());
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata.getPtr(), docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata.getPtr(),
			    sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveStrValueDoc,
				    distr_id, gRepPolicyMgr.getDocRepPolicy(), cluster);

	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, the_value);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->arg(AosFN::eSnapID, snap_id);

	jimo_call->makeCall(rdata.getPtr());

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "makeCall failed" << enderr;
		return false;
	}

	/*AosTransPtr trans = OmnNew AosIILTransStrRemoveValueDocByName(
			iilname, the_value, docid, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << the_value;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();
	}
	*/

	return true;
}


bool jimoRemoveStrValueDoc(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &key,
		const u64 &docid)
{
	//Xuqi 2105/11/6
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveStrValueDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoBindObjid(
		AosRundata *rdata,
		const OmnString &objid, 
		const u64 &docid)
{
	aos_assert_rr(objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eBindObjid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoUnbindObjid(
		AosRundata *rdata,
		const OmnString &objid, 
		const u64 &docid)
{
	aos_assert_rr(objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eUnBindObjid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);
	
	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoRebindObjid(
		AosRundata *rdata,
		const OmnString &old_objid,
		const OmnString &new_objid,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRebindObjid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eObjid, old_objid);
	jimo_call->arg(AosFN::eValue, new_objid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoBindCloudid(
		AosRundata *rdata,
		const OmnString &cid,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eBindCloudid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoUnbindCloudid(
		AosRundata *rdata,
		const OmnString &cid,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eUnBindCloudid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoRebindCloudid(
		AosRundata *rdata,
		const OmnString &old_cid,
		const OmnString &new_cid,
		const u64 &docid)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eReBindCloudid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eCloudid, old_cid);
	jimo_call->arg(AosFN::eValue, new_cid);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoGetDocidByObjid(
		AosRundata *rdata,
		const u32 siteid,
		const OmnString &objid,
		u64 &docid,
		bool &isunique)
{
	aos_assert_rr( objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidBySiteidObjid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	//xuqi 2015/10/16
	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	aos_assert_r(resp, false); 
	
	rslt = resp->getU8(0);           
	if(rslt)
	{
		docid = resp->getU64(0);
		isunique = resp->getU8(0);
	}
	else
	{
		docid = 0;
		isunique = false;
	}
	return true;
}


bool jimoGetDocidByCloudid(
		AosRundata *rdata,
		const u32 siteid,
		const OmnString &cid,
		u64 &docid,
		bool &isunique)
{
	//xuqi 2015/10/23
	aos_assert_rr(cid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, cid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidByCloudid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eCloudid, cid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	rslt = resp->getU8(0);           
	if(rslt)
	{
		docid = resp->getU64(0);
		isunique = resp->getU8(0);
	}
	else
	{
		docid = 0;
		isunique = false;
	}
	return true;
}

bool jimoIncrementDocid(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dft_value)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eIncrementDocidU64ByName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::ePersisFalg, persis);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->arg(AosFN::eValue2, initValue);
	jimo_call->arg(AosFN::eAddFlag, add_flag);
	jimo_call->arg(AosFN::eDft, dft_value);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	value = jimo_call->getU64(rdata, AosFN::eValue);

	return true;
}


bool jimoIncrementDocid(
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		AosRundata *rdata)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);
	u64 snap_id = rdata->getSnapshotId();

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoIndexEnginePackageName, JimoIndexFunc::eIncrementDocidStrByName, 
			distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::ePersisFalg, persis);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->arg(AosFN::eValue2, initValue);
	jimo_call->arg(AosFN::eAddFlag, add_flag);
	jimo_call->arg(AosFN::eSnapID, snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	value = jimo_call->getU64(rdata, AosFN::eValue);

	return true;
}

bool jimoIncrementDocid(
		AosRundata *rdata,
		const u64 &iilid,
		const u64 &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dftValue)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eIncrementDocidU64ByID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::ePersisFalg, persis);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->arg(AosFN::eValue2, initValue);
	jimo_call->arg(AosFN::eAddFlag, add_flag);
	jimo_call->arg(AosFN::eDft, dftValue);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	value = jimo_call->getU64(rdata, AosFN::eValue);

	return true;
}


bool jimoIncrementDocid(
		AosRundata *rdata,
		const u64 &iilid,
		const OmnString &key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eIncrementDocidStrByID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->arg(AosFN::eValue2, initValue);
	jimo_call->arg(AosFN::eAddFlag, add_flag);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	aos_assert_r(resp, false); 
	
	rslt = resp->getU8(0);           
	if(rslt)
	{
		value = resp->getU64(0);
	}
	else
	{
		value = 0;
	}

	return true;
}

bool jimoBatchAdd(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		AosRundata *rdata)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoIndexEnginePackageName, JimoIndexFunc::eBatchAdd, 
			distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eType, (u64)iiltype);
	jimo_call->arg(AosFN::eSize, entry_len);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue, executor_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);  
	rslt = resp->getU8(0);                                      
	if (!rslt) 
	{
		return false;                                    
	}


	return true;
}

bool jimoStrBatchDel(
		AosRundata *rdata,
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eStrBatchDel, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eSize, entry_len);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue, executor_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoBatchDel(                                                              
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		AosRundata *rdata)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
			sgJimoIndexEnginePackageName, JimoIndexFunc::eStrBatchDel,
			distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eType, iiltype);
	jimo_call->arg(AosFN::eSize, entry_len);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue, executor_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoCreateTablePublic(
		AosRundata *rdata,
		u64 &iilid,
		const AosIILType iiltype)
{
	//Xuqi 2015/11/2
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCreateTablePublicByID, cluster);
	
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eType, iiltype);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	aos_assert_r(resp, false); 
	
	rslt = resp->getU8(0);           
	if(rslt)
	{
		iilid = resp->getU64(0);
	}
	else
	{
		iilid = 0;
	}

	return true;
}

bool jimoCreateTablePublic(
		AosRundata *rdata,
		const OmnString &iilname,
		u64 &iilid,
		const AosIILType iiltype)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCreateTablePublicByIDName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eType, iiltype);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}


bool jimoCreateIILPublic(
		AosRundata *rdata,
		u64 &iilid,
		const AosIILType iiltype)
{
	AosClusterObj *cluster = jimoGetDocEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncNorm(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCreateIILPublicByID, cluster);
	jimo_call->arg(AosFN::eType, iiltype);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}


bool jimoCreateIILPublic(
		AosRundata *rdata,
		const OmnString &iilname,
		u64 &iilid,
		const AosIILType iiltype)
{
	// CHENDING0824
	//Step 1: create JimoAPI function.

	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCreateIILPublicByIDName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eType, iiltype);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}

bool jimoQuerySafe(
		AosRundata *rdata,
		const OmnString &iilname,
 		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eQuerySafeByIILName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);

	bool rslt;

	AosXmlTagPtr xml_rslt;
	if (query_rslt)
	{
		AosXmlTagPtr xml_rslt;
		rslt = query_rslt->serializeToXml(xml_rslt);
		aos_assert_r(rslt, false);
		jimo_call->arg(AosFN::eValue1, xml_rslt->toString());
	}

	if (query_bitmap)
	{
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		query_bitmap->saveToBuff(buff);
		aos_assert_r(buff, false);
		jimo_call->arg(AosFN::eValue2, buff);
	}

	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	rslt = resp->getU8(0);                                                            
	if (!rslt) return false;                                                          

	AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);                                   
	aos_assert_r(xml, false);                                                         
	rslt = query_context->serializeFromXml(xml, rdata);                               
	aos_assert_r(rslt, false);                                                        

	if (query_rslt)                                                                   
	{                                                                                 
		xml = getXmlFromBuff(resp, rdata);                                            
		aos_assert_r(xml, false);                                                     
		rslt = query_rslt->serializeFromXml(xml);                                     
		aos_assert_r(rslt, false);                                                    
	}                                                                                 
	                                                                                  
	u32 len = resp->getU32(0);                                                        
	if (len > 0)                                                                      
	{                                                                                 
		aos_assert_r(query_bitmap, false);                                            
		query_bitmap->clean();                                                        
		AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);             
		aos_assert_r(buff, false);                                                    
		rslt = query_bitmap->loadFromBuff(buff);                                      
		aos_assert_r(rslt, false);                                                    
	}                                                                                 

	/*if (query_rslt)
	{
		OmnString str_rslt = buff->getOmnStr(rdata, AosFN::eValue1);
		xml_rslt = AosXmlParser::parse(str_rslt);
		if(xml_rslt&&query_rslt.getPtr())
		{
			rslt = query_rslt->serializeFromXml(xml_rslt);
			aos_assert_r(rslt, false);
		}
	}

	if (query_bitmap)
	{
		AosBuffPtr buf = jimo_call->getBuff(rdata, AosFN::eValue2, 0);
		if(buf&&query_bitmap.getPtr())
		{
			rslt = query_bitmap->loadFromBuff(buf);
			aos_assert_r(rslt, false);
		}
	}

	OmnString str_context = jimo_call->getOmnStr(rdata, AosFN::eValue);
	xml_context = AosXmlParser::parse(str_context);

	query_context->serializeFromXml(xml_context, rdata);*/

	return true;
}


bool jimoQuerySafe(
		AosRundata *rdata,
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eQuerySafeByIILID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);

	bool rslt;
	AosXmlTagPtr xml_rslt;
	if (query_rslt)
	{
		rslt = query_rslt->serializeToXml(xml_rslt);
		aos_assert_r(rslt, false);
		jimo_call->arg(AosFN::eValue1, xml_rslt->toString());
	}

	if (query_bitmap)
	{
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		query_bitmap->saveToBuff(buff);
		aos_assert_r(buff, false);
		jimo_call->arg(AosFN::eValue2, buff);
	}

	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	if (query_rslt)
	{
		OmnString str_rslt = jimo_call->getOmnStr(rdata, AosFN::eValue1);
		xml_rslt = AosXmlParser::parse(str_rslt AosMemoryCheckerArgs);
		if(xml_rslt&&query_rslt.getPtr())
		{
			rslt = query_rslt->serializeFromXml(xml_rslt);
			aos_assert_r(rslt, false);
		}
	}

	if (query_bitmap)
	{
		AosBuffPtr buf = jimo_call->getBuff(rdata, AosFN::eValue2, 0);
		if(buf&&query_bitmap.getPtr())
		{
			rslt = query_bitmap->loadFromBuff(buf);
			aos_assert_r(rslt, false);
		}
	}

	OmnString str_context = jimo_call->getOmnStr(rdata, AosFN::eValue);
	xml_context = AosXmlParser::parse(str_context AosMemoryCheckerArgs);
	query_context->serializeFromXml(xml_context, rdata);

	return true;
}

bool jimoSetU64ValueDocUnique(
		AosRundata *rdata,
		u64 &iilid,
		const bool createFlag,
		const u64 &key,
		const u64 &docid,
		const bool must_same)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eSetU64ValueDocUnique, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eFlag, createFlag);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, must_same);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}

bool jimoSetU64ValueDocUniqueToTable(
		AosRundata *rdata,
		u64 &iilid,
		const bool createFlag,
		const u64 &key,
		const u64 &docid,
		const bool must_same)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eSetU64ValueDocUniqueToTable, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eFlag, createFlag);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, must_same);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}

bool jimoSetStrValueDocUnique(
		AosRundata *rdata,
		u64 &iilid,
		const bool createFlag,
		const OmnString &key,
		const u64 &docid,
		const bool must_same)
{
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eSetStrValueDocUnique,
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eFlag, createFlag);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, must_same);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}


bool jimoSetStrValueDocUniqueToTable(
		AosRundata *rdata,
		u64 &iilid,
		const bool createFlag,
		const OmnString &key,
		const u64 &docid,
		const bool must_same)
{
	//Xuqi 2015/11/2
	aos_assert_rr(docid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, docid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eSetStrValueDocUniqueToTable, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eFlag, createFlag);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->arg(AosFN::eValue, must_same);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	iilid = jimo_call->getU64(rdata, AosFN::eIILID);

	return true;
}

bool jimoDeleteIIL(
		AosRundata *rdata,
		const OmnString &iilname,
		const bool true_delete)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eDeleteIILByName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eFlag, true_delete);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


bool jimoDeleteIIL(
		AosRundata *rdata,
		const u64 &iilid,
		const bool true_delete)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eDeleteIILByID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eFlag, true_delete);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}


u64 jimoGetDocidByObjid(
		AosRundata *rdata, 
		const OmnString &objid)
{
	aos_assert_rr(objid!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidByObjid, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eObjid, objid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eDocid);
}

bool jimoGetDocid(
		AosRundata *rdata,
		const u64 &iilid,
		const u64 &key,
		u64 &docid,
		const u64 &dft,
		bool &found,
		bool &isunique)
{
	//Xuqi 2015/11/3
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidByIILIDDft, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	aos_assert_r(resp, false); 
	
	rslt = resp->getU8(0);           
	if(rslt)
	{
		isunique = resp->getU8(0);
		found = resp->getU8(0);
		docid = found ? resp->getU64(0) : dft; 
	}
	else
	{
		isunique = false;
		found = false;
		docid = dft;
	}

	return true;
}

bool jimoGetDocid(
		AosRundata *rdata,
		const u64 &iilid,
		const OmnString &key,
		const AosOpr opr,
		const bool reverse,
		u64 &docid,
		bool &isunique)
{
	aos_assert_rr(iilid != 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidByIILIDOpr, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);

	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eOpr, opr);
	jimo_call->arg(AosFN::eReverse, reverse);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	docid = jimo_call->getU64(rdata, AosFN::eDocid);
	isunique = jimo_call->getBool(rdata, AosFN::eValueUnique);

	return true;
}

bool jimoGetDocidsByKeys(
		AosRundata *rdata,
		const u64&iilid,
		const vector<OmnString> &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		AosBuffPtr &buff)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidsByKeysIILID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);

	OmnString values = "";
	for(vector<OmnString>::const_iterator itr=keys.begin(); itr!=keys.end(); itr++)
	{
		values << *itr;
		values << ";";
	}
	jimo_call->arg(AosFN::eValue, values);
	jimo_call->arg(AosFN::eFlag, need_dft_value);
	jimo_call->arg(AosFN::eDft, dft_value);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	buff = jimo_call->getBuff(rdata, AosFN::eBuff, 0);
	
	return true;
}



bool jimoGetDocidsByKeys(
		AosRundata *rdata,
		const OmnString &iilname,
		const vector<OmnString> &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		AosBuffPtr &buff)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetDocidsByKeysIILName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);

	OmnString values = "";
	for(vector<OmnString>::const_iterator itr=keys.begin(); itr!=keys.end(); itr++)
	{
		values << *itr;
		values << ";";
	}
	jimo_call->arg(AosFN::eValue, values);
	jimo_call->arg(AosFN::eFlag, need_dft_value);
	jimo_call->arg(AosFN::eDft, dft_value);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	buff = jimo_call->getBuff(rdata, AosFN::eBuff, 0);
	
	return true;	
}

bool jimoGetSplitValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryContextObjPtr &context,
		const int size,
		vector<AosQueryContextObjPtr> &contexts)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetSplitValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	
	AosQueryContextObjPtr query_context = context;
	if (!context)
	{
		query_context = AosQueryContextObj::createQueryContextStatic();
		query_context->setOpr(eAosOpr_an);
	}
	aos_assert_r(query_context, false);

	AosXmlTagPtr xml_context;
	bool rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt && xml_context, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());
	jimo_call->arg(AosFN::eSize, size);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	OmnString str = jimo_call->getOmnStr(rdata, AosFN::eData);
	
	contexts.clear();
	AosXmlTagPtr tag;
	AosQueryContextObjPtr tmp = AosQueryContextObj::createQueryContextStatic();
	AosStrSplit split(str, ";");
	vector<OmnString> strV = split.entriesV();
	for(vector<OmnString>::iterator itr=strV.begin(); itr!=strV.end(); itr++)
	{
		tag = AosXmlParser::parse(*itr AosMemoryCheckerArgs);
		aos_assert_r(tag, false);
		tmp->serializeFromXml(tag, rdata);
		aos_assert_r(tmp, false);
		contexts.push_back(tmp);
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	rslt = resp->getU8(0);           
	aos_assert_r(rslt, false);       

	int64_t total_1 = resp->getI64(0); 
	aos_assert_r(total_1 >= 0, false); 

	bool rs1 = rslt;

	bool is_super_iil = AosIsSuperIIL(iilname);      
	if (is_super_iil)                               
	{
		jimo_call->reset();
		OmnString par_iilname = AosIILName::composeParallelIILName(iilname);
		jimo_call->arg(AosFN::eIILName, par_iilname);

		AosQueryContextObjPtr query_context = context;                     
		if (!context)                                                      
		{                                                                  
			query_context = AosQueryContextObj::createQueryContextStatic();
			query_context->setOpr(eAosOpr_an);                             
		}                                                                  
		aos_assert_r(query_context, false);                                

		AosXmlTagPtr xml_context;
		bool rslt = query_context->serializeToXml(xml_context, rdata);
		aos_assert_r(rslt && xml_context, false);
		jimo_call->arg(AosFN::eValue, xml_context->toString());
		jimo_call->arg(AosFN::eSize, size);
		jimo_call->makeCall(rdata);

		rslt = jimo_call->isCallSuccess();
		if (!rslt)
		{
			OmnAlarm << "syncCall failed" << enderr;
			return false;
		}
		AosBuffPtr par_resp = jimo_call->getBuff(rdata, AosFN::eBuff);

		int64_t total_2 = -1;

		bool rs2 = par_resp->getU8(0);
		aos_assert_r(rs1 || rs2, false);

		if (rs2) total_2 = par_resp->getI64(0);

		if (!rs1 && rs2)
		{
			resp = par_resp;
		}
		else if (total_1 < total_2)
		{
			resp = par_resp;
		}
	}

	AosXmlTagPtr xml;
	int len = resp->getInt(0);
	for (int i=0; i<len; i++)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
		rslt = query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);
		contexts.push_back(query_context);
	}


	return true;
}


bool jimoRebuildBitmap(
		AosRundata *rdata,
		const OmnString &iilname)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRebuildBitmap, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoQuerySafe(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eQuerySafeByNameSnap, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);

	/*
	AosAsyncRespCallerPtr
	Need ReWrite
	*/
	bool rslt;

	if (query_rslt)
	{
		AosXmlTagPtr xml_rslt;
		rslt = query_rslt->serializeToXml(xml_rslt);
		aos_assert_r(rslt, false);
		jimo_call->arg(AosFN::eValue1, xml_rslt->toString());
	}

	if (query_bitmap)
	{
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		query_bitmap->saveToBuff(buff);
		aos_assert_r(buff, false);
		jimo_call->arg(AosFN::eValue2, buff);
	}

	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoResetKeyedValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool persis,
		const u64 &incValue)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eResetKeyedValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::ePersisFalg, persis);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoStrBatchAdd(
		AosRundata *rdata,
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eStrBatchAdd, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eSize, entry_len);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue, executor_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);  
	rslt = resp->getU8(0);                                      
	if (!rslt) 
	{
		return false;                                    
	}

	return true;
}

bool jimoStrBatchInc(
		AosRundata *rdata,
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &init_value,
		const AosIILUtil::AosIILIncType incType,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eStrBatchInc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eSize, entry_len);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue, init_value);
	jimo_call->arg(AosFN::eType, incType);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoU64BatchAdd(
		AosRundata *rdata,
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eU64BatchAdd, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, entry_len);
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue1, executor_id);
	jimo_call->arg(AosFN::eValue2, snap_id);
	jimo_call->arg(AosFN::eValue3, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoHitBatchAdd(
		AosRundata *rdata,
		const OmnString &iilname,
		const vector<u64> &docids,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eHitBatchAdd, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	OmnString values = "";
		for(vector<u64>::const_iterator itr=docids.begin(); itr!=docids.end(); itr++)
	{
		values << OmnStrUtil::ltoa(*itr);
		values << ";";
	}
	jimo_call->arg(AosFN::eValue, values);
	jimo_call->arg(AosFN::eValue1, snap_id);
	jimo_call->arg(AosFN::eValue2, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

			
	return false;
}

u64 jimoCreateSnapshot(
		AosRundata *rdata,
		const u32 static_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(static_id!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, static_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCreateSnapshot, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue, static_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eValue1);
}

bool jimoStrBatchAddMerge(
		AosRundata *rdata,
		const OmnString &iilname,
		const int64_t &size,
		const u64 &executor_id,
		const bool true_delete)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eStrBatchAddMerge,
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eSize, size);
	jimo_call->arg(AosFN::eValue, executor_id);
	jimo_call->arg(AosFN::eFlag, true_delete);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoStrBatchIncMerge(
		AosRundata *rdata,
		const OmnString &iilname,
		const int64_t &size,
		const u64 &dftvalue,
		const AosIILUtil::AosIILIncType incType,
		const bool true_delete)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eStrBatchIncMerge, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eSize, size);
	jimo_call->arg(AosFN::eDft, dftvalue);
	jimo_call->arg(AosFN::eValue, incType);
	jimo_call->arg(AosFN::eFlag, true_delete);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoJimoTableBatchAdd(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosXmlTagPtr &cmp_tag,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);
	
	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eJimoTableBatchAdd, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eTag, cmp_tag->toString());
	jimo_call->arg(AosFN::eBuff, buff);
	jimo_call->arg(AosFN::eValue1, executor_id);
	jimo_call->arg(AosFN::eValue2, snap_id);
	jimo_call->arg(AosFN::eValue3, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

u64 jimoGetIILID(
		AosRundata *rdata,
		const OmnString &iilname)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetIILID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eIILID);
}

bool jimoCounterRange(
		AosRundata *rdata,
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCounterRangeByID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
			
	bool rslt;

	AosXmlTagPtr xml_rslt;
	if (query_rslt)
	{
		rslt = query_rslt->serializeToXml(xml_rslt);
		aos_assert_r(rslt, false);
		jimo_call->arg(AosFN::eValue1, xml_rslt->toString());
	}

	if (query_bitmap)
	{
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		query_bitmap->saveToBuff(buff);
		aos_assert_r(buff, false);
		jimo_call->arg(AosFN::eValue2, buff);
	}

	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	AosXmlTagPtr xml_counter;
	rslt = counter_query->serializeToXml(xml_counter);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue3, xml_counter->toString());

	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	if (query_rslt)
	{
		OmnString str_rslt = jimo_call->getOmnStr(rdata, AosFN::eValue1);
		xml_rslt = AosXmlParser::parse(str_rslt AosMemoryCheckerArgs);
		if(xml_rslt&&query_rslt.getPtr())
		{
			rslt = query_rslt->serializeFromXml(xml_rslt);
			aos_assert_r(rslt, false);
		}
	}

	if (query_bitmap)
	{
		AosBuffPtr buf = jimo_call->getBuff(rdata, AosFN::eValue2, 0);
		if(buf&&query_bitmap.getPtr())
		{
			rslt = query_bitmap->loadFromBuff(buf);
			aos_assert_r(rslt, false);
		}
	}

	OmnString str_context = jimo_call->getOmnStr(rdata, AosFN::eValue);
	xml_context = AosXmlParser::parse(str_context AosMemoryCheckerArgs);
	query_context->serializeFromXml(xml_context, rdata);

	OmnString str_counter = jimo_call->getOmnStr(rdata, AosFN::eValue3);
	xml_counter = AosXmlParser::parse(str_counter AosMemoryCheckerArgs);
	query_rslt->serializeFromXml(xml_counter);

	return true;
}


bool jimoCounterRange(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCounterRangeByName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
			
	bool rslt;

	AosXmlTagPtr xml_rslt;
	if (query_rslt)
	{
		rslt = query_rslt->serializeToXml(xml_rslt);
		aos_assert_r(rslt, false);
		jimo_call->arg(AosFN::eValue1, xml_rslt->toString());
	}

	if (query_bitmap)
	{
		AosBuffPtr buff = OmnNew AosBuff(2048 AosMemoryCheckerArgs);
		query_bitmap->saveToBuff(buff);
		aos_assert_r(buff, false);
		jimo_call->arg(AosFN::eValue2, buff);
	}

	AosXmlTagPtr xml_context;
	rslt = query_context->serializeToXml(xml_context, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue, xml_context->toString());

	AosXmlTagPtr xml_counter;
	rslt = counter_query->serializeToXml(xml_counter);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eValue3, xml_counter->toString());

	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	if (query_rslt)
	{
		OmnString str_rslt = jimo_call->getOmnStr(rdata, AosFN::eValue1);
		xml_rslt = AosXmlParser::parse(str_rslt AosMemoryCheckerArgs);
		if(xml_rslt&&query_rslt.getPtr())
		{
			rslt = query_rslt->serializeFromXml(xml_rslt);
			aos_assert_r(rslt, false);
		}
	}

	if (query_bitmap)
	{
		AosBuffPtr buf = jimo_call->getBuff(rdata, AosFN::eValue2, 0);
		if(buf&&query_bitmap.getPtr())
		{
			rslt = query_bitmap->loadFromBuff(buf);
			aos_assert_r(rslt, false);
		}
	}

	OmnString str_context = jimo_call->getOmnStr(rdata, AosFN::eValue);
	xml_context = AosXmlParser::parse(str_context AosMemoryCheckerArgs);
	query_context->serializeFromXml(xml_context, rdata);

	OmnString str_counter = jimo_call->getOmnStr(rdata, AosFN::eValue3);
	xml_counter = AosXmlParser::parse(str_counter AosMemoryCheckerArgs);
	query_rslt->serializeFromXml(xml_counter);

	return true;
}

bool jimoCommitSnapshot(
		AosRundata *rdata,
		const u32 &static_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(static_id!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, static_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eCommitSnapshot,
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue, static_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoRollBackSnapshot(
		AosRundata *rdata,
		const u32 static_id,
		const u64 &snap_id,
		const u64 &task_docid)
{
	aos_assert_rr(static_id!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, static_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRollBackSnapshot, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue, static_id);
	jimo_call->arg(AosFN::eSnapshot, snap_id);
	jimo_call->arg(AosFN::eDocid, task_docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

u64 jimoNextId1(
		AosRundata *rdata,
		const u32 siteid,
		const u64 &iilid,
		const u64 &init_value,
		const OmnString &id_name,
		const u64 &incValue)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eNextId1, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eValue, init_value);
	jimo_call->arg(AosFN::eKey, id_name);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return 0;
	}

	return jimo_call->getU64(rdata, AosFN::eStartid);
}

bool jimoMergeSnapshot(
		AosRundata *rdata,
		const u32 static_id,
		const u64 &target_snap_id,
		const u64 &merge_snap_id)
{
	aos_assert_rr(static_id!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, static_id);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eMergeSnapshot, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue, static_id);
	jimo_call->arg(AosFN::eValue1, target_snap_id);
	jimo_call->arg(AosFN::eValue2, merge_snap_id);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

u64 jimoGetCtnrMemDocid(
		AosRundata *rdata,
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicated)
{
	aos_assert_rr(ctnr_objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, ctnr_objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetCtnrMemDocidByOneKey, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eObjid, ctnr_objid);
	jimo_call->arg(AosFN::eKey, keyname);
	jimo_call->arg(AosFN::eValue, keyvalue);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return 0;
	}

	duplicated = jimo_call->getBool(rdata, AosFN::eValueUnique);
	return jimo_call->getU64(rdata, AosFN::eDocid);
}


u64 jimoGetCtnrMemDocid(
		AosRundata *rdata,
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicated)
{
	//Xuqi 2015/11/4
	aos_assert_rr(ctnr_objid != "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, ctnr_objid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetCtnrMemDocidByTwoKey, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eSiteid, siteid);
	jimo_call->arg(AosFN::eObjid, ctnr_objid);
	jimo_call->arg(AosFN::eKey, keyname1);
	jimo_call->arg(AosFN::eValue, keyvalue1);
	jimo_call->arg(AosFN::eValue1, keyname2);
	jimo_call->arg(AosFN::eValue2, keyvalue2);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return 0;
	}

	duplicated = jimo_call->getBool(rdata, AosFN::eValueUnique);
	return jimo_call->getU64(rdata, AosFN::eDocid);
}

bool jimoPreQuerySafe(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryContextObjPtr &query_context)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::ePreQuerySafeByIILName, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	AosXmlTagPtr xml;
	bool rslt = query_context->serializeToXml(xml, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, xml->toString());
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	OmnString str_context = jimo_call->getOmnStr(rdata, AosFN::eValue);
	AosXmlTagPtr xml_context = AosXmlParser::parse(str_context AosMemoryCheckerArgs);

	query_context->serializeFromXml(xml_context, rdata);
	
	return true;
}


bool jimoPreQuerySafe(
		AosRundata *rdata,
		const u64 &iilid,
		const AosQueryContextObjPtr &query_context)
{
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::ePreQuerySafeByIILID, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	AosXmlTagPtr xml;
	bool rslt = query_context->serializeToXml(xml, rdata);
	aos_assert_r(rslt, false);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eValue, xml->toString());
	jimo_call->makeCall(rdata);

	rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	OmnString str_context = jimo_call->getOmnStr(rdata, AosFN::eValue);
	AosXmlTagPtr xml_context = AosXmlParser::parse(str_context AosMemoryCheckerArgs);

	query_context->serializeFromXml(xml_context, rdata);
	return true;
}

bool jimoRemoveStrFirstValueDoc(
			AosRundata *rdata,
			const OmnString &iilname,
			const OmnString &value,
			u64 &docid,
			const bool reverse)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveStrFirstValueDoc, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, value);
	jimo_call->arg(AosFN::eReverse, reverse);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	docid = jimo_call->getU64(rdata, AosFN::eDocid);
	
	return true;
}

bool jimoAppendManualOrder(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &docid,
		u64 &value)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAppendManualOrder, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	value = jimo_call->getU64(rdata, AosFN::eValue);
	
	return true;
}

bool jimoIncrementInlineSearchValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &value,
		const u64 &seqid,
		const u64 incValue)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eIncrementInlineSearchValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, value);
	jimo_call->arg(AosFN::eSeqid, seqid);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoMoveManualOrder(
		AosRundata *rdata,
		const OmnString &iilname,
		u64 &value1,
		const u64 &docid1,
		u64 &value2,
		const u64 &docid2,
		const OmnString flag)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eMoveManualOrder, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eDocid, docid1);
	jimo_call->arg(AosFN::eValue2, docid2);
	jimo_call->arg(AosFN::eFlag, flag);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	value1 = jimo_call->getU64(rdata, AosFN::eValue);

	value2 = jimo_call->getU64(rdata, AosFN::eValue1);
		
	return true;
}

bool jimoRemoveManualOrder(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveManualOrder, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, value);
	jimo_call->arg(AosFN::eDocid, docid);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoSwapManualOrder(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &value1,
		const u64 &docid1,
		const u64 &value2,
		const u64 &docid2)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eSwapManualOrder, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, value1);
	jimo_call->arg(AosFN::eDocid, docid1);
	jimo_call->arg(AosFN::eValue1, value2);
	jimo_call->arg(AosFN::eValue2, docid2);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoGetBatchDocids(
		AosRundata *rdata,
		const AosBuffPtr &column,
		const OmnString &iilname)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eGetBatchDocids, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eValue, column);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoGetDocids(
		AosRundata *rdata,
		const OmnString &iilname,
		int &iilidx,
		int &idx,
		u64 *docids,
		const int array_size)
{
	OmnNotImplementedYet;
	return false;
}


bool jimoAddInlineSearchValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &value,
		const u64 &seqid,
		const OmnString &splitType)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eAddInlineSearchValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, value);
	jimo_call->arg(AosFN::eSeqid, seqid);
	jimo_call->arg(AosFN::eType, splitType);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoUpdateKeyedValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const u64 &key,
		const bool &flag,
		const u64 &delta,
		u64 &new_value)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eUpdateKeyedValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eFlag, flag);
	jimo_call->arg(AosFN::eDelta, delta);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}
	
	new_value = jimo_call->getU64(rdata, AosFN::eValue);
	
	return true;
}

bool jimoModifyInlineSearchValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &old_value,
		const OmnString &new_value,
		const u64 &old_seqid,
		const u64 &new_seqid,
		const OmnString &splitType)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eModifyInlineSearchValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, old_value);
	jimo_call->arg(AosFN::eValue1, new_value);
	jimo_call->arg(AosFN::eValue2, old_seqid);
	jimo_call->arg(AosFN::eValue3, new_seqid);
	jimo_call->arg(AosFN::eType, splitType);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoRemoveInlineSearchValue(
		AosRundata *rdata,
		const OmnString &iilname,
		const OmnString &value,
		const u64 &seqid,
		const OmnString &splitType)
{
	aos_assert_rr(iilname!= "", rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilname);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eRemoveInlineSearchValue, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILName, iilname);
	jimo_call->arg(AosFN::eValue, value);
	jimo_call->arg(AosFN::eSeqid, seqid);
	jimo_call->arg(AosFN::eType, splitType);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	return true;
}

bool jimoIncrementDocidToTable(
		AosRundata *rdata,
		const u64 &iilid,
		const OmnString &key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag)
{
	//Xuqi 2015/11/2
	aos_assert_rr(iilid!= 0, rdata, 0);
	AosClusterObj *cluster = jimoGetIndexEngineCluster(rdata);
	aos_assert_rr(cluster, rdata, 0);
	u32 distr_id = cluster->getDistrID(rdata, iilid);

	AosJimoCallPtr jimo_call = OmnNew AosJimoCallSyncRead(rdata,
		sgJimoIndexEnginePackageName, JimoIndexFunc::eIncrementDocidToTable, 
		distr_id, gRepPolicyMgr.getIndexRepPolicy(), cluster);
	jimo_call->arg(AosFN::eIILID, iilid);
	jimo_call->arg(AosFN::eKey, key);
	jimo_call->arg(AosFN::eValue1, incValue);
	jimo_call->arg(AosFN::eValue2, initValue);
	jimo_call->arg(AosFN::eAddFlag, add_flag);
	jimo_call->makeCall(rdata);

	bool rslt = jimo_call->isCallSuccess();
	if (!rslt)
	{
		OmnAlarm << "syncCall failed" << enderr;
		return false;
	}

	AosBuffPtr resp = jimo_call->getBuff(rdata, AosFN::eBuff);
	aos_assert_r(resp, false); 
	
	rslt = resp->getU8(0);           
	if(rslt)
	{
		value = resp->getU64(0);
	}
	else
	{
		value = 0;
	}

	return true;
}



}
