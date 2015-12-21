////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2011/07/07	Created by ken Lee	
////////////////////////////////////////////////////////////////////////////
#include "IILClient/IILClient.h"

#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "CounterUtil/CounterQuery.h"
#include "DocUtil/DocUtil.h"
#include "IILTrans/AllIILTrans.h"
#include "IILUtil/IILFuncType.h"
#include "IILUtil/IILId.h"
#include "SEUtil/IILName.h"
#include "IILUtil/IILTypes.h"
#include "SEUtil/SystemIdNames.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/LockMonitor.h"
//#include "TransUtil/AsyncReqTransMgr.h"
#include "TransUtil/IdRobin.h"
#include "UtilData/ModuleId.h"
#include "Tracer/FileLog.h"
#include "Util/Ptrs.h"
#include "Util/BuffArray.h"
#include "Util/OmnNew.h"
#include "Util/DataTable.h"
#include "UtilHash/StrObjHash.h"
#include "UtilHash/HashedObjU64.h"
#include "WordMgr/WordMgr1.h"
#include "WordMgr/WordIdHash.h"
#include "XmlUtil/SeXmlParser.h"


static bool sgShowLog = false;
static bool sgCheckMD5 = false;

//static map<OmnString, u64> sgMap;
//static OmnMutexPtr sgLock = OmnNew OmnMutex();


OmnSingletonImpl(AosIILClientSingleton,
                 AosIILClient,
                 AosIILClientSelf,
                "AosIILClient");

#define AOSIILCLIENT_LIMIT_VALUE_LEN(value)							\
	char *checkvalmaxlendata = (char *)value.data();				\
	char cvmld_c = 0;												\
	int cvmld_len = value.length();								\
	if (cvmld_len > AosIILUtil::eMaxStrValueLen)					\
	{																\
		cvmld_c = checkvalmaxlendata[AosIILUtil::eMaxStrValueLen];	\
		value.setLength(AosIILUtil::eMaxStrValueLen);				\
	}

#define AOSIILCLIENT_RESTORE_VALUE_LEN(value)						\
	if (cvmld_len > AosIILUtil::eMaxStrValueLen)						\
	{																\
		checkvalmaxlendata[AosIILUtil::eMaxStrValueLen] = cvmld_c;	\
		value.setLength(cvmld_len);									\
	}

#define AOSIILCLIENT_IS_VALUE_TOO_LONG								\
	(cvmld_len >= AosIILUtil::eMaxStrValueLen)

#define AOSIILCLIENT_LINENO << "lineno=\"" << __LINE__ << "\" "


AosIILClient::AosIILClient()
{
}


AosIILClient::~AosIILClient()
{
}


bool
AosIILClient::start()
{
	return true;
}


bool
AosIILClient::stop()
{
	return true;
}


bool		
AosIILClient::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);

	AosXmlTagPtr iilClient= config->getFirstChild("iil_client");
	aos_assert_r(iilClient, false);
	
	sgShowLog = iilClient->getAttrBool(AOSCONFIG_SHOWLOG, false);
	return true;
}


bool
AosIILClient::createIILPublic(
		u64 &iilid,
		const AosIILType iiltype,
		const AosRundataPtr &rdata)
{
	// This function creates a new IIL with the type 'iiltype'.
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransCreateIIL(iilid, iiltype, false, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	iilid = resp->getU64(0);	
	aos_assert_r(iilid > 0, false);
	if (sgShowLog) OmnScreen << "create iil, iilid:" << iilid << endl;
	return true;
}


bool
AosIILClient::createIILPublic(
		const OmnString &iilname,
		u64 &iilid,
		const AosIILType iiltype,
		const AosRundataPtr &rdata)
{
	iilid = 0;
	aos_assert_r(iilname != "", false);

cout << __FILE__ << ":" << __LINE__ << "Create IIL: " << iilname << ":" << iiltype << endl;
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransCreateIIL(iilname, iiltype, false, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	iilid = resp->getU64(0);
	aos_assert_r(iilid > 0, false);
	if (sgShowLog) OmnScreen << "create iil, iilid:" << iilid << endl;
	return true;
}


bool
AosIILClient::addHitDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &docid,
		int &physical_id,
		const AosRundataPtr &rdata)
{
	if (sgShowLog) OmnScreen << "Add Hit doc, "
		<< iilname << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransHitAddDocByName(
		iilname, false, docid, true, false, snap_id);
	
	physical_id = trans->getToSvrId();
	//physical_id = trans->getMainSvrId();

	return addTransToVector(trans, allTrans, arr_len);
}


bool		
AosIILClient::addHitDoc(
		const OmnString &iilname,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	
	if (sgShowLog) OmnScreen << "Add Hit doc, "
		<< iilname << ":" << docid << endl;
	
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransHitAddDocByName(
		iilname, false, docid, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool		
AosIILClient::addStrValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		int &physical_id,
		const AosRundataPtr &rdata)
{
	physical_id = -1;
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(value.length() > 0, false);
	
	if (sgShowLog) OmnScreen << "Add Value doc, "
		<< iilname << ":" << value << ":" << docid << endl;

	bool vunique = value_unique;
	bool dunique = docid_unique;

	AOSIILCLIENT_LIMIT_VALUE_LEN(value);
	if (AOSIILCLIENT_IS_VALUE_TOO_LONG)
	{
		vunique = false;
		dunique = false;
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrAddValueDoc(
		iilname, false, value, docid, vunique, dunique, true, false, snap_id);
	
	AOSIILCLIENT_RESTORE_VALUE_LEN(value);
	
	// Chen Ding, 2014/10/24
	if (allTrans)
	{
		return addTransToVector(trans, allTrans, arr_len);
	}

	return true;
}


bool		
AosIILClient::addStrValueDoc(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(value.length() > 0, false);
	
	if (sgShowLog) OmnScreen << "Add Value doc, " << iilname
		<< ":" << value << ":" << docid << endl;
	
	bool vunique = value_unique;
	bool dunique = docid_unique;
	AOSIILCLIENT_LIMIT_VALUE_LEN(value);
	if (AOSIILCLIENT_IS_VALUE_TOO_LONG)
	{
		vunique = false;
		dunique = false;
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrAddValueDoc(
		iilname, false, value, docid, vunique, dunique, true, false, snap_id);

	AOSIILCLIENT_RESTORE_VALUE_LEN(value);
	
	return addTrans(trans, rdata);
}


bool
AosIILClient::addStrValueDoc(
		const u64 &iilid,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilid, false);
	aos_assert_r(value.length() > 0, false);
	
	if (sgShowLog) OmnScreen << "Add Value doc, " << iilid
		<< ":" << value << ":" << docid << endl;
	
	bool vunique = value_unique;
	bool dunique = docid_unique;
	AOSIILCLIENT_LIMIT_VALUE_LEN(value);
	if (AOSIILCLIENT_IS_VALUE_TOO_LONG)
	{
		vunique = false;
		dunique = false;
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrAddValueDoc(
		iilid, false, value, docid, vunique, dunique, true, false, snap_id);

	AOSIILCLIENT_RESTORE_VALUE_LEN(value);
	
	return addTrans(trans, rdata);
}


bool
AosIILClient::addU64ValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		int &physical_id,
		const AosRundataPtr &rdata)
{
	physical_id = 0;
	aos_assert_r(iilname != "", false);
	
	if (sgShowLog) OmnScreen << "Add Value doc, "
		<< iilname << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64AddValueDoc(
		iilname, false, value, docid, value_unique, docid_unique, true, false, snap_id);

	// Chen Ding, 2014/10/24
	if (allTrans)
	{
		return addTransToVector(trans, allTrans, arr_len);
	}

	return true;
}


bool
AosIILClient::addU64ValueDoc(
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	if (sgShowLog) OmnScreen << "Add Value doc, "
		<< iilid << ":" << value << ":" << docid << endl;
	
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64AddValueDoc(
		iilid, false, value, docid, value_unique, docid_unique, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::addU64ValueDoc(
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "Add Value doc, "
		<< iilname << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64AddValueDoc(
		iilname, false, value, docid, value_unique, docid_unique, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool 		
AosIILClient::removeHitDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	if (sgShowLog) OmnScreen << "Remove doc, " << iilname << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransHitRemoveDocByName(
		iilname, docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool 		
AosIILClient::removeHitDoc(
		const OmnString &iilname,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "Remove doc, " << iilname << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransHitRemoveDocByName(
		iilname, docid, true, false, snap_id);
	
	return addTrans(trans, rdata);
}


bool		
AosIILClient::removeStrValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		int &physical_id,
		const AosRundataPtr &rdata)
{
	physical_id = -1;
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(value.length() > 0, false);

	if (sgShowLog) OmnScreen << "Remove value, " << iilname
		<< ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrRemoveValueDocByName(
		iilname, the_value, docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool		
AosIILClient::removeStrValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const u64 &iilid,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilid, false);
	aos_assert_r(value.length() > 0, false);

	if (sgShowLog) OmnScreen << "Remove value, " << iilid
		<< ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrRemoveValueDocById(
		iilid, value, docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool		
AosIILClient::removeStrValueDoc(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(value.length() > 0, false);

	if (sgShowLog) OmnScreen << "Remove value, "
		<< iilname << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrRemoveValueDocByName(
		iilname, the_value, docid, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	/*u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << the_value;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();
	}*/

	return rslt;
}


bool		
AosIILClient::removeStrValueDoc(
		const u64 &iilid,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilid, false);
	aos_assert_r(value.length() > 0, false);

	if (sgShowLog) OmnScreen << "Remove value, "
		<< iilid << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrRemoveValueDocById(
		iilid, value, docid, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool			
AosIILClient::removeU64ValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		int &physical_id,
		const AosRundataPtr &rdata)
{
	physical_id = -1;
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "Remove value, " << iilname
		<< ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64RemoveValueDocByName(
		iilname, value, docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool		
AosIILClient::removeU64ValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	if (sgShowLog) OmnScreen << "Remove value, " << iilid
		<< ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64RemoveValueDocById(
		iilid, value, docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool		
AosIILClient::removeU64ValueDoc(
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "Remove value, "
		<< iilname << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64RemoveValueDocByName(
		iilname, value, docid, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool		
AosIILClient::removeU64ValueDoc(
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	if (sgShowLog) OmnScreen << "Remove value, "
		<< iilid << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64RemoveValueDocById(
		iilid, value, docid, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::removeU64DocByValue(
		const OmnString &iilname,
		const u64 &value,
		int &entriesRemoved,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::removeU64DocByValue(
		const u64 &iilid,
		const u64 &value,
		int &entriesRemoved,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool		
AosIILClient::modifyStrValueDoc(
		const u64 &iilid,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);
	
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
	AosTransPtr trans = OmnNew AosIILTransStrModifyValueDoc(
		iilid, oldvalue, newvalue, docid,
		value_unique, docid_unique, override, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool		
AosIILClient::modifyStrValueDoc(
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
	AosTransPtr trans = OmnNew AosIILTransStrModifyValueDoc(
		iilname, oldvalue, newvalue, docid,
		value_unique, docid_unique, override, true, false, snap_id);

	bool rslt = addTrans(trans, rdata);

	// Ken Lee, 2014/03/21
	/*u32 siteid = rdata->getSiteid();
	if (rslt && iilname == AosIILName::composeObjidListingName(siteid))
	{
		OmnString s;
		s << siteid << "_" << oldvalue;

		sgLock->lock();
		sgMap.erase(s);
		sgLock->unlock();
	}*/

	return rslt;
}


bool		
AosIILClient::modifyU64ValueDoc(
		const u64 &iilid,
		const u64 &oldvalue,
		const u64 &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	if (sgShowLog) OmnScreen << "Modify Value doc, " << iilid << ":"
		<< oldvalue << ":" << newvalue << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64ModifyValueDocById(
		iilid, oldvalue, newvalue, value_unique, docid_unique, docid, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool		
AosIILClient::modifyU64ValueDoc(
		const OmnString &iilname,
		const u64 &oldvalue,
		const u64 &newvalue,
		const bool value_unique,
		const bool docid_unique,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "Modify Value doc, " << iilname
		<< ":" << oldvalue << ":" << newvalue << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64ModifyValueDocByName(
		iilname, oldvalue, newvalue, value_unique, docid_unique, docid, true, false, snap_id);

	return addTrans(trans, rdata);
}

	
bool			
AosIILClient::getDocid(
		const u64 &iilid,
		const OmnString &the_value,
		const AosOpr opr,
		const bool reverse,
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilid, false);
	aos_assert_r(AosOpr_valid(opr), false);

	if (sgShowLog) OmnScreen << "getdocid, " << iilid << ":"
		<< value << ":" << AosOpr_toStr(opr) << endl;

	AOSIILCLIENT_LIMIT_VALUE_LEN(value);

	aos_assert_r(value != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrGetDocidById(iilid,
			value, opr, reverse, false, true, snap_id);	

	AOSIILCLIENT_RESTORE_VALUE_LEN(value);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	if (rslt)
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


bool			
AosIILClient::getDocid(
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

	AOSIILCLIENT_LIMIT_VALUE_LEN(value);
	aos_assert_r(value != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrGetDocidByName(
		iilname, value, opr, reverse, false, true, snap_id);

	AOSIILCLIENT_RESTORE_VALUE_LEN(value);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	if (rslt)
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


bool
AosIILClient::getDocid(
		const OmnString &iilname,
		const u64 &key,
		u64 &docid,
		const u64 &dft,
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	if (sgShowLog) OmnScreen << "getdocid, " << iilname << ":" << key << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64GetDocidByName(iilname, key, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt, resp);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	is_unique = resp->getU8(0);
	found = resp->getU8(0);
	docid = found ? resp->getU64(0) : dft;
	return rslt;
}


bool
AosIILClient::getDocid(
		const u64 &iilid,
		const u64 &key,
		u64 &docid,
		const u64 &dft,
		bool &found,
		bool &is_unique,
		const AosRundataPtr &rdata)
{
	if (sgShowLog) OmnScreen << "getdocid, " << iilid << ":" << key << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64GetDocidById(iilid, key, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt, resp);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	is_unique = resp->getU8(0);
	found = resp->getU8(0);
	docid = found ? resp->getU64(0) : dft;
	return rslt;
}


bool
AosIILClient::getDocidByObjid(
		const u32 siteid,
		const OmnString &objid,
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(siteid != 0 && objid != "", false);

	OmnString tmpname = AosIILName::composeObjidListingName(siteid);
	aos_assert_r(tmpname != "", false);

	OmnString iilname = tmpname;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{	
		int vid = AosGetCubeId(objid);
		iilname = AosIILName::composeCubeIILName(vid, tmpname);
		aos_assert_r(iilname != "", false);
	}

	return getDocid(iilname, objid, eAosOpr_eq, false, docid, isunique, rdata);
}
	

u64
AosIILClient::getDocidByObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	OmnString s;
	s << siteid << "_" << objid;

	// Ken Lee, 2014/03/21
	/*sgLock->lock();
	map<OmnString, u64>::iterator itr = sgMap.find(s);
	if (itr != sgMap.end())
	{
		u64 docid = itr->second;
		sgLock->unlock();
		return docid;
	}
	if (sgMap.size() > 100000)
	{
		sgMap.clear();
	}
	sgLock->unlock();
	*/

	u64 docid = 0;
	bool isunique = false;
	bool rslt = getDocidByObjid(siteid, objid, docid, isunique, rdata);
	if (!rslt || !isunique) return 0;

	//sgLock->lock();
	//sgMap[s] = docid;
	//sgLock->unlock();

	return docid;
}







vector<u64>
AosIILClient::getAncestorPriv(
		const OmnString &iilname,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	vector<u64> ancestors;
	aos_assert_r(parent_docid, ancestors);
	aos_assert_r(iilname != "", ancestors);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64GetAncestor(
			iilname, parent_docid, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, ancestors);
	
	u32 p_num = resp->getU32(0);
	aos_assert_r(p_num, ancestors);
	//for(i = 0; i< p_num; i++)
	while(p_num--)
	{
		u64 did = resp->getU64(0);
		ancestors.push_back(did);
	}
	return ancestors;
}


bool
AosIILClient::addAncestor(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(docid || parent_docid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64AddAncestor(
		iilname, docid, parent_docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool
AosIILClient::addDescendant(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const OmnString &ancestoriilname,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(ancestoriilname != "", false);
	aos_assert_r(docid || parent_docid, false);

	vector<u64> ancestors = getAncestorPriv(ancestoriilname, parent_docid, rdata);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64AddDescendant(
		iilname, docid, parent_docid, ancestors, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool
AosIILClient::removeAncestor(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(docid || parent_docid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64RemoveAncestor(
		iilname, docid, parent_docid, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool
AosIILClient::removeDescendant(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const OmnString &ancestoriilname,
		const u64 &docid,
		const u64 &parent_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	aos_assert_r(ancestoriilname != "", false);
	aos_assert_r(docid || parent_docid, false);
	
	vector<u64> ancestors = getAncestorPriv(ancestoriilname, parent_docid, rdata);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64RemoveDescendant(
		iilname, docid, parent_docid, ancestors, true, false, snap_id);

	return addTransToVector(trans, allTrans, arr_len);
}


bool
AosIILClient::bindCloudid(
		const OmnString &cloudid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(cloudid.length() > 0 && cloudid.length() < eAosMaxCloudidLength, false);

	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);
	
	OmnString iilname = AosIILName::composeCloudidListingName(siteid);
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "bindCloudid, "
		<< iilname << ":" << cloudid << ":" << docid << endl;
	
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrBindCloudid(
			iilname, cloudid, docid, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosIILClient::bindObjid(
		const OmnString &objid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(objid.length() > 0 && objid.length() < eAosMaxObjidLen, false);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);
	
	OmnString tmpname = AosIILName::composeObjidListingName(siteid);
	aos_assert_r(tmpname != "", false);

	OmnString iilname = tmpname;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{
		int vid = AosGetCubeId(objid);
		iilname = AosIILName::composeCubeIILName(vid, tmpname);
		aos_assert_r(iilname != "", false);
	}

	if (sgShowLog) OmnScreen << "bindObjid, " << iilname
		<< ":" << objid << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrBindObjid(
		iilname, objid, docid, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosIILClient::unbindCloudid(
		const OmnString &cloudid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(cloudid.length() > 0 && cloudid.length() < eAosMaxCloudidLength, false);

	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);
	
	OmnString iilname = AosIILName::composeCloudidListingName(siteid);
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "unbindCloudid, "
		<< iilname << ":" << cloudid << ":" << docid << endl;
	
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrUnbindCloudid(
		iilname, cloudid, docid, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosIILClient::unbindObjid(
		const OmnString &objid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(objid.length() > 0 && objid.length() < eAosMaxObjidLen, false);
	
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid != 0, false);

	OmnString tmpname = AosIILName::composeObjidListingName(siteid);
	aos_assert_r(tmpname != "", false);
	
	OmnString iilname = tmpname;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{
		int vid = AosGetCubeId(objid);
		iilname = AosIILName::composeCubeIILName(vid, tmpname);
		aos_assert_r(iilname != "", false);
	}

	if (sgShowLog) OmnScreen << "unbindObjid, " << iilname
		<< ":" << objid << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrUnbindObjid(
		iilname, objid, docid, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	return resp->getU8(0);
}


bool
AosIILClient::rebindCloudid(
		const OmnString &old_cloudid,
		const OmnString &new_cloudid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	bool rslt = unbindCloudid(old_cloudid, docid, rdata);
	aos_assert_r(rslt, false);

	rslt = bindCloudid(new_cloudid, docid, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILClient::rebindObjid(
		const OmnString &old_objid,
		const OmnString &new_objid,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	bool rslt = unbindObjid(old_objid, docid, rdata);
	aos_assert_r(rslt, false);

	rslt = bindObjid(new_objid, docid, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILClient::incrementDocid(
		const u64 &iilid,
		const OmnString &the_key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	OmnString &key = (OmnString &)the_key;
	aos_assert_r(iilid, false);
	aos_assert_r(key.length() > 0, false);

	AOSIILCLIENT_LIMIT_VALUE_LEN(key);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrIncrementDocidById(
			iilid, false, key, incValue, initValue, add_flag, true, true, snap_id);

	AOSIILCLIENT_RESTORE_VALUE_LEN(key);
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);	
	value = resp->getU64(0);
	return true;
}


bool			
AosIILClient::incrementDocid(
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool isPersis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);	
	aos_assert_r(key.length() > 0, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrIncrementDocidByName(
			iilname, isPersis, key, incValue, initValue, add_flag, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);	
	value = resp->getU64(0);
	return rslt;
}


bool
AosIILClient::incrementDocid(
		const OmnString &iilname,
		const u64 &key,
		u64 &new_value,
		const bool isPersis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dftvalue,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);	

	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransU64IncrementDocidByName(
			iilname, isPersis, key, incValue, initValue, add_flag, dftvalue, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	new_value = resp->getU64(0);
	return rslt;
}


bool	
AosIILClient::querySafe(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosRundataPtr &rdata)
{
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_an);
	return querySafe(iilname, query_rslt, query_bitmap, query_context, rdata);
}


bool	
AosIILClient::querySafe(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	if (iilid == AOS_INVDID)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	if (query_rslt)
	{
		if (query_bitmap && query_bitmap->isEmpty())
		{
			query_rslt->reset();
			query_context->setFinished(true);
			return true;
		}
	}
	
	bool rslt = querySafeNorm(
		iilid, query_rslt, query_bitmap, query_context, rdata);

	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
	}

	return true;
}


bool
AosIILClient::querySafe(
		const OmnString &iilname,
		const OmnString &docscanner_id,
		const u64 &blocksize,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
#if 0
	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransDistQueryDocByName(iilname,
			docscanner_id, blocksize, query_context, false, false, snap_id);
	
	return addTrans(trans, rdata);
#endif
	return false;
}


bool
AosIILClient::querySafe(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	if (iilname == "")
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	if (query_rslt)
	{
		if (query_bitmap && query_bitmap->isEmpty())
		{
			query_rslt->reset();
			query_context->setFinished(true);
			return true;
		}
	}

	bool rslt = true;
	bool is_super_iil = AosIsSuperIIL(iilname);
	if (!is_super_iil)
	{
		rslt = querySafeNorm(
			iilname, query_rslt, query_bitmap, query_context, rdata);
	}
	else
	{
		rslt = querySafeBig(
			iilname, query_rslt, query_bitmap, query_context, rdata);
	}

	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
	}

	return true;
}


bool
AosIILClient::querySafe(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	if (iilname == "")
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	if (query_rslt)
	{
		if (query_bitmap && query_bitmap->isEmpty())
		{
			query_rslt->reset();
			query_context->setFinished(true);
			return true;
		}
	}

	bool rslt = true;
	bool is_super_iil = AosIsSuperIIL(iilname);
	if (!is_super_iil)
	{
		rslt = querySafeNormAsync(
			iilname, query_rslt, query_bitmap, query_context, resp_caller, reqId, snapId, rdata);
	}
	else
	{
		rslt = querySafeBigAsync(
			iilname, query_rslt, query_bitmap, query_context, resp_caller, reqId, snapId, rdata);
	}

	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
	}

	return true;
}


bool	
AosIILClient::preQuerySafe(
		const u64 &iilid,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	if (iilid == AOS_INVDID)
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}

	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransPreQuery(iilid, query_context, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	if (!rslt)
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}


	AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
	aos_assert_r(xml, false);
	rslt = query_context->serializeFromXml(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILClient::preQuerySafe(
		const OmnString &iilname,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);

	if (iilname == "")
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}

	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransPreQuery(iilname, query_context, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);	
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	if (!rslt)
	{
		query_context->setTotalDocInRslt(0);
		return true;
	}

	AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
	aos_assert_r(xml, false);
	rslt = query_context->serializeFromXml(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosIILClient::addInlineSearchValue(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &seqid,
		const OmnString &splitType,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);	
	aos_assert_r(the_value.length() > 0, false);

	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransStrAddInlineSearchValue(
		iilname, the_value, seqid, splitType, true, false, snap_id);
	
	return addTrans(trans, rdata);
}


bool
AosIILClient::removeInlineSearchValue(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &seqid,
		const OmnString &splitType,
		const AosRundataPtr &rdata)
{
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrRemoveInlineSearchValue(
		iilname, the_value, seqid, splitType, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::modifyInlineSearchValue(
		const OmnString &iilname,
		const OmnString &old_value,
		const OmnString &new_value,
		const u64 &old_seqid,
		const u64 &new_seqid,
		const OmnString &splitType,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);	
	aos_assert_r(old_value.length() > 0 && new_value.length() > 0, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrModifyInlineSearchValue(
		iilname, old_value, new_value, old_seqid, new_seqid, splitType, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::incrementInlineSearchValue(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &seqid,
		const u64 incValue,
		const AosRundataPtr &rdata)
{
	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransStrIncrementInlineSearchValue(
		iilname, the_value, seqid, incValue, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::removeStrFirstValueDoc(
		const OmnString &iilname,
		const OmnString &value,
		u64 &docid,
		const bool reverse,
		const AosRundataPtr &rdata)
{
	aos_assert_r(value.length() > 0, false);
	aos_assert_r(value.length() <= AosIILUtil::eMaxStrValueLen, false);
	
	if (sgShowLog) OmnScreen << "Remove first value doc: " << iilname << ":" << value << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrRemoveFirstValueDocByName(
			iilname, value, reverse, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);	
	docid = resp->getU64(0);
	return true;
}


bool
AosIILClient::setStrValueDocUnique(
		u64 &iilid,
		const bool createFlag,
		const OmnString &the_value,
		const u64 &docid,
		const bool must_same,
		const AosRundataPtr &rdata)
{
	if (iilid == 0)
	{
		if (!createFlag) return true;
		bool rslt = createIILPublic(iilid, eAosIILType_Str, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(iilid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrSetValueDocUniqueById(
		iilid, false, the_value, docid, must_same, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::setU64ValueDocUnique(
		u64 &iilid,
		const bool createFlag,
		const u64 &value,
		const u64 &docid,
		const bool must_same,
		const AosRundataPtr &rdata)
{
	if (iilid == 0)
	{
		if (!createFlag) return true;
		bool rslt = createIILPublic(iilid, eAosIILType_U64, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(iilid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64SetValueDocUniqueById(
		iilid, false, value, docid, must_same, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::appendManualOrder(
		const OmnString &iilname,
		const u64 &docid,
		u64 &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::moveManualOrder(
		const OmnString &iilname,
		u64 &value1,
		const u64 &docid1,
		u64 &value2,
		const u64 &docid2,
		const OmnString flag,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::swapManualOrder(
		const OmnString &iilname,
		const u64 &value1,
		const u64 &docid1,
		const u64 &value2,
		const u64 &docid2,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::removeManualOrder(
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::updateKeyedValue(
		const OmnString &iilname,
		const u64 &key,
		const u64 &value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::updateKeyedValue(
		const OmnString &iilname,
		const u64 &key,
		const bool &flag,
		const u64 &delta,
		u64 &new_value,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::queryValueSafe(
		const u64 &iilid,
		vector<OmnString> &values,
		const AosOpr opr,
		const OmnString &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	values.clear();
	aos_assert_r(iilid, false);
	aos_assert_r(AosOpr_valid(opr), false);

	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::queryU64ValueSafe(
		const u64 &iilid,
		vector<u64> &values,
		const AosOpr opr,
		const u64 &value,
		const bool unique_value,
		const AosRundataPtr &rdata)
{
	values.clear();
	aos_assert_r(iilid, false);
	aos_assert_r(AosOpr_valid(opr), false);

	OmnNotImplementedYet;
	return false;
}


u64
AosIILClient::getCtnrMemDocid(
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicated,
		const AosRundataPtr &rdata)
{
	// This function retrieves the XML document for the object that
	// is in the container 'ctnr_objid', where the container is on
	// the site 'siteid'.

	// 1. Retrieve the container docid
	if (siteid == 0)
	{
		rdata->setError() << "SiteId is missing!";
		return AOS_INVDID;
	}

	if (ctnr_objid == "")
	{
		rdata->setError() << "No container specified!";
		return AOS_INVDID;
	}

	u64 docid = AOS_INVDID;	
	bool isunique;
	bool rslt = getDocidByObjid(siteid, ctnr_objid, docid, isunique, rdata);
	if (!rslt || docid == AOS_INVDID)
	{
		rdata->setError() << "Invalid container: " << ctnr_objid << ". SiteId: " << siteid;
		return AOS_INVDID;
	}
	if (!isunique)
	{
		rdata->setError() << "The container is not unique: " << ctnr_objid;
		return AOS_INVDID;
	}

	// 2. Retrieve the container IIL:
	OmnString ctnr_iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
	
	OmnString iilname = AosIILName::composeAttrIILName(keyname);
	AosBitmapObjPtr query_bitmap = AosBitmapMgrObj::getBitmapStatic();
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(keyvalue);
	query_context->setBlockSize(0);
	
	// SHAWN_WARN, how to set the page size?????
	//rslt = querySafe(iilname, 0, query_bitmap, query_context, rdata);
	rslt = querySafe(ctnr_iilname, 0, query_bitmap, query_context, rdata);
	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}
	
	AosBitmapObjPtr query_bitmap2 = AosBitmapMgrObj::getBitmapStatic();
	// SHAWN_WARN, how to set the page size?????
	AosQueryContextObjPtr query_context2 = AosQueryContextObj::createQueryContextStatic();
	query_context2->setOpr(eAosOpr_an);
	query_context2->setBlockSize(0);

	//rslt = querySafe(iilname, 0, query_bitmap2,query_context2, rdata);
	rslt = querySafe(ctnr_iilname, 0, query_bitmap, query_context, rdata);
	if (!rslt || query_bitmap2->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap2);
		query_bitmap = 0;
		query_bitmap2 = 0;
		return 0;
	}
	
	// Chen Ding, 2013/02/09
	// AosBitmapObj::countRsltAndStatic(query_bitmap, query_bitmap2);
	query_bitmap->andBitmap(query_bitmap2);
	query_bitmap->reset();

	u64 did = 0; 
	if (!query_bitmap->nextDocid(did))
	{
		rdata->setError() << "Object not found (003). Key Name: "
			<< keyname << "; Key Value: " << keyvalue;
	}
	return did;
}


u64
AosIILClient::getCtnrMemDocid(
		const u32 siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicated,
		const AosRundataPtr &rdata)
{
	// This function retrieves the XML document for the object that
	// is in the container 'ctnr_objid', where the container is on
	// the site 'siteid'.

	// 1. Retrieve the container docid
	if (siteid == 0)
	{
		rdata->setError() << "SiteId is missing!";
		return AOS_INVDID;
	}

	if (ctnr_objid == "")
	{
		rdata->setError() << "No container specified!";
		return AOS_INVDID;
	}

	u64 docid = AOS_INVDID;
	bool isunique;
	bool rslt = getDocidByObjid(siteid, ctnr_objid, docid, isunique, rdata);
	if (!rslt || docid == AOS_INVDID)
	{
		rdata->setError() << "Invalid container: " << ctnr_objid << ". SiteId: " << siteid;
		return AOS_INVDID;
	}

	if (!isunique)
	{
		rdata->setError() << "The container is not unique: " << ctnr_objid;
		return AOS_INVDID;
	}

	// 2. Retrieve the container IIL:
	OmnString ctnr_iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
	OmnString iilname1 = AosIILName::composeAttrIILName(keyname1);
	AosBitmapObjPtr query_bitmap = AosBitmapMgrObj::getBitmapStatic();
	// SHAWN_WARN, how to set the page size?????
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(keyvalue1);
	query_context->setBlockSize(0);

	rslt = querySafe(iilname1, 0, query_bitmap, query_context, rdata);
	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}
	
	OmnString iilname2 = AosIILName::composeAttrIILName(keyname2);
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(keyvalue2);
	query_context->setBlockSize(0);
	
	// SHAWN_WARN, how to set the page size?????
	rslt = querySafe(iilname2, 0, query_bitmap, query_context, rdata);

	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}
	
	query_context->setOpr(eAosOpr_an);
	query_context->setBlockSize(0);
	// SHAWN_WARN, how to set the page size?????
	rslt = querySafe(ctnr_iilname, 0, query_bitmap, query_context, rdata);

	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}

	query_bitmap->reset();

	u64 did = 0; 
	if (!query_bitmap->nextDocid(did))
	{
		rdata->setError() << "Object not found (003). Key Name1: "
			<< keyname1 << "; Key Value1: " << keyvalue1
			<< "Key Name2: " << keyname2 << "; Key Value2: " << keyvalue2;
	}
		
	AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
	query_bitmap = 0;
	return did;
}


bool 	
AosIILClient::reCreateIILPublic(
		const OmnString &iilname,
		const AosIILType iiltype,
		const AosRundataPtr &rdata)
{
	return false;
	//u64 wordid = AosWordMgr1::getSelf()->getWordId(
	//	(u8 *)iilname.data(), iilname.length(), false);
	//return AosWordMgr1::getSelf()->setId(wordid, 0);
}


// Chen Ding, 02/10/2012
u64
AosIILClient::nextId1(
		const u32 siteid,
		const u64 &iilid,
		const u64 &init_value,
		const OmnString &id_name,
		const u64 &incValue,
		const AosRundataPtr &rdata)
{
	u64 cid = 0;
	u32 sid = rdata->setSiteid(siteid);
	bool rslt = incrementDocidToTable(iilid, id_name,
		cid, incValue, init_value, true, rdata);
	rdata->setSiteid(sid);
	aos_assert_rr(rslt, rdata, 0);
	return cid;
}


bool
AosIILClient::createTablePublic(
		u64 &iilid,
		const AosIILType iiltype,
		const AosRundataPtr &rdata)
{
	// This function creates a new IIL with the type 'iiltype'.
	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransCreateIIL(iilid, iiltype, true, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);	
	iilid = resp->getU64(0);	
	aos_assert_r(iilid > 0, false);
	if (sgShowLog) OmnScreen << "create iil, iilid:" << iilid << endl;
	return true;
}


bool
AosIILClient::createTablePublic(
		const OmnString &iilname,
		u64 &iilid,
		const AosIILType iiltype,
		const AosRundataPtr &rdata)
{
	iilid = 0;
	aos_assert_r(iilname != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransCreateIIL(iilname, iiltype, true, true, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	iilid = resp->getU64(0);
	aos_assert_r(iilid > 0, false);
	if (sgShowLog) OmnScreen << "create iil, iilid:" << iilid << endl;
	return true;
}


bool		
AosIILClient::addStrValueDocToTable(
		const OmnString &iilname,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	OmnString &value = (OmnString &)the_value;
	aos_assert_r(iilname != "", false);
	aos_assert_r(value.length() > 0, false);
	
	if (sgShowLog) OmnScreen << "Add Value doc, " << iilname
		<< ":" << value << ":" << docid << endl;
	
	bool vunique = value_unique;
	bool dunique = docid_unique;
	AOSIILCLIENT_LIMIT_VALUE_LEN(value);
	if (AOSIILCLIENT_IS_VALUE_TOO_LONG)
	{
		vunique = false;
		dunique = false;
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrAddValueDoc(
		iilname, true, value, docid, vunique, dunique, true, false, snap_id);

	AOSIILCLIENT_RESTORE_VALUE_LEN(value);
	
	return addTrans(trans, rdata);
}


bool
AosIILClient::addU64ValueDocToTable(
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (sgShowLog) OmnScreen << "Add Value doc, "
		<< iilname << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransU64AddValueDoc(
		iilname, true, value, docid, value_unique, docid_unique, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::addU64ValueDocToTable(
		const u64 &iilid,
		const u64 &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	if (sgShowLog) OmnScreen << "Add Value doc, "
		<< iilid << ":" << value << ":" << docid << endl;

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64AddValueDoc(
		iilid, true, value, docid, value_unique, docid_unique, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::incrementDocidToTable(
		const u64 &iilid,
		const OmnString &the_key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	OmnString &key = (OmnString &)the_key;
	aos_assert_r(iilid, false);
	aos_assert_r(key.length() > 0, false);

	AOSIILCLIENT_LIMIT_VALUE_LEN(key);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrIncrementDocidById(
		iilid, true, key, incValue, initValue, add_flag, true, true, snap_id);

	AOSIILCLIENT_RESTORE_VALUE_LEN(key);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);	
	value = resp->getU64(0);
	return true;
}


bool
AosIILClient::setStrValueDocUniqueToTable(
		u64 &iilid,
		const bool createFlag,
		const OmnString &the_value,
		const u64 &docid,
		const bool must_same,
		const AosRundataPtr &rdata)
{
	if (iilid == 0)
	{
		if (!createFlag) return true;
		bool rslt = createTablePublic(iilid, eAosIILType_Str, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(iilid, false);

	u64 snap_id = rdata->getSnapshotId();	
	AosTransPtr trans = OmnNew AosIILTransStrSetValueDocUniqueById(
		iilid, true, the_value, docid, must_same, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::setU64ValueDocUniqueToTable(
		u64 &iilid,
		const bool createFlag,
		const u64 &value,
		const u64 &docid,
		const bool must_same,
		const AosRundataPtr &rdata)
{
	if (iilid == 0)
	{
		if (!createFlag) return true;
		bool rslt = createTablePublic(iilid, eAosIILType_U64, rdata);
		aos_assert_r(rslt, false);
	}
	aos_assert_r(iilid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64SetValueDocUniqueById(
		iilid, true, value, docid, must_same, true, false, snap_id);

	return addTrans(trans, rdata);
}


bool
AosIILClient::incrementDocid(
		const u64 &iilid,
		const u64 &key,
		u64 &value,
		const bool isPersis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dftValue,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransU64IncrementDocidById(
		iilid, isPersis, key, incValue, initValue, add_flag, dftValue, true, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	value = resp->getU64(0);
	return rslt;
}


bool
AosIILClient::sendHitBuff(
		const AosBuffPtr &buff,
		const OmnString &iilname,
		const bool isPersis,
		const AosRundataPtr &rdata)
{
	// 'HitBuff' is an array of u64 for a given IIL whose name is
	// 'iilname'. This function sends the buff to the backend.
	// Ketty 2013/03/20
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::getDocidsByKeys( 	
		const u64 &iilid,
		const vector<OmnString> &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	int num = keys.size();
	aos_assert_r(num > 0, false);

	AosBuffPtr req_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	for(int i=0; i<num; i++)
	{
		req_buff->setOmnStr(keys[i]);
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrGetDocidsByKeys(
		iilid, num, req_buff, need_dft_value, dft_value, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	u32 len = resp->getU32(0);
	buff = resp->getBuff(len, true AosMemoryCheckerArgs);
	aos_assert_r(buff, false);

	return true;	
}


bool
AosIILClient::getDocidsByKeys( 	
		const OmnString &iilname,
		const vector<OmnString> &keys,
		const bool need_dft_value,
		const u64 &dft_value,
		AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	int num = keys.size();
	aos_assert_r(num > 0, false);

	AosBuffPtr req_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	for(int i=0; i<num; i++)
	{
		req_buff->setOmnStr(keys[i]);
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrGetDocidsByKeys(
		iilname, num, req_buff, need_dft_value, dft_value, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	u32 len = resp->getU32(0);
	buff = resp->getBuff(len, true AosMemoryCheckerArgs);
	aos_assert_r(buff, false);

	return true;	
}


bool
AosIILClient::queryData(
		const OmnString &iilname,
		const OmnString &value1,
		const AosOpr opr1,
		const OmnString &value2,
		const AosOpr opr2,
		const int order_by,
		const AosDataTablePtr &table,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(iilname != "", rdata, false);
	aos_assert_rr(table, rdata, false);
	aos_assert_rr(order_by == 0 || order_by == 1, rdata, false);
	table->clear();
	aos_assert_r(AosOpr_valid(opr1), false);
	aos_assert_r(AosOpr_valid(opr2), false);

	int field1_len = table->getFieldLen(0, rdata.getPtr());
	int field2_len = table->getFieldLen(1, rdata.getPtr());
	aos_assert_rr(field1_len > 0 && field2_len > 0, rdata, false);

	OmnNotImplementedYet;
	return false;
}


bool	
AosIILClient::getSplitValue(
			const OmnString &iilname,
			const int &num_blocks,
			const AosQueryRsltObjPtr &query_rslt,
			const AosRundataPtr &rdata)
{
	if (iilname == "" || num_blocks <= 1)
	{
		return false;
	}

	aos_assert_r(query_rslt ,false);

	OmnNotImplementedYet;
	return false;
}


bool		
AosIILClient::deleteIIL(
		const u64 &iilid,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransDeleteIIL(iilid, true_delete, true, false, snap_id);
	
	return addTrans(trans, rdata);
}


bool		
AosIILClient::deleteIIL(
		const OmnString &iilname,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransDeleteIIL(iilname, true_delete, true, false, snap_id);
	bool rslt = addTrans(trans, rdata);
	aos_assert_r(rslt, false);

	bool is_super_iil = AosIsSuperIIL(iilname);
	if (is_super_iil)
	{
		OmnString par_iilname = AosIILName::composeParallelIILName(iilname);	
		trans = OmnNew AosIILTransDeleteIIL(par_iilname, true_delete, true, false, snap_id);
		rslt = addTrans(trans, rdata);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosIILClient::getSplitValue(
		const OmnString &iilname,
		const AosQueryContextObjPtr &context,
		const int size,
		vector<AosQueryContextObjPtr> &contexts,
		const AosRundataPtr &rdata)
{
	contexts.clear();
	aos_assert_r(iilname != "", false);
	
	AosQueryContextObjPtr query_context = context;
	if (!context)
	{
		query_context = AosQueryContextObj::createQueryContextStatic();//liuwei
		query_context->setOpr(eAosOpr_an);
	}
	aos_assert_r(query_context, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransGetSplitValueByName(
		iilname, size, query_context, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	bool is_super_iil = AosIsSuperIIL(iilname);
	if (!is_super_iil)
	{
		rslt = resp->getU8(0);
		aos_assert_r(rslt, false);

		int64_t total = resp->getI64(0);
		aos_assert_r(total >= 0, false);
	}
	else
	{
		OmnString par_iilname = AosIILName::composeParallelIILName(iilname);	
		AosTransPtr par_trans = OmnNew AosIILTransGetSplitValueByName(
			par_iilname, size, query_context, false, true, snap_id);

		AosBuffPtr par_resp;
		bool rslt = addTrans(par_trans, par_resp, rdata);
		aos_assert_r(rslt && resp, false);

		int64_t total_1 = -1;
		int64_t total_2 = -1;

		bool rs1 = resp->getU8(0);
		bool rs2 = par_resp->getU8(0);
		aos_assert_r(rs1 || rs2, false);

		if (rs1) total_1 = resp->getI64(0);
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
		query_context = AosQueryContextObj::createQueryContextStatic();
		rslt = query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);
		contexts.push_back(query_context);
	}

	return true;
}


bool
AosIILClient::counterRange(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	if (iilid == AOS_INVDID)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}
	
	if (query_rslt)
	{
		if (query_bitmap && query_bitmap->isEmpty())
		{
			query_rslt->reset();
			query_context->setFinished(true);
			return true;
		}
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransCounterRange(
		iilid, query_rslt, query_bitmap, query_context, counter_query, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = resp->getU8(0);
	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->clean();
		return true;
	}

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
	
	if (counter_query)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = counter_query->serializeFromXml(xml);
		aos_assert_r(rslt, false);
	}

	u32 len = resp->getU32(0);
	if (len)
	{
		// means has bitmap.
		aos_assert_r(query_bitmap, false);
		query_bitmap->clean();
		AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(buff, false);
		rslt = query_bitmap->loadFromBuff(buff);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosIILClient::counterRange(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);

	if (iilname == "")
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}
	
	aos_assert_r(query_rslt || query_bitmap, false);
	if (query_rslt)
	{
		if (query_bitmap && query_bitmap->isEmpty())
		{
			query_rslt->reset();
			query_context->setFinished(true);
			return true;
		}
	}

	bool rslt = true;
	bool is_super_iil = AosIsSuperIIL(iilname);
	if (!is_super_iil)
	{
		rslt = counterRangeNorm(iilname, query_rslt,
			query_bitmap, query_context, counter_query, rdata);
	}
	else
	{
		rslt = counterRangeBig(iilname, query_rslt,
			query_bitmap, query_context, counter_query, rdata);
	}

	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
	}	

	return true;
}


u64
AosIILClient::getIILID(
		const OmnString &iilname,
		const AosRundataPtr &rdata)
{
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransGetIILID(iilname, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, 0);
	
	u64 iilid = resp->getU64(0);
	if (sgShowLog) OmnScreen << "get iilid, iilid:" << iilid << endl;
	return iilid;
}


bool
AosIILClient::addTransToVector(
		const AosTransPtr &trans,
		vector<AosTransPtr> *allTrans,
		const u32 arr_len)
{
	int svr_id = trans->getToSvrId();
	aos_assert_r(svr_id>= 0 && (u32)svr_id < arr_len, false);

	allTrans[svr_id].push_back(trans);
	return true;
}


bool
AosIILClient::addTrans(
		const AosTransPtr &trans,
		AosBuffPtr &resp,
		const AosRundataPtr &rdata)
{
	bool timeout = false;
	bool rslt = AosSendTrans(rdata, trans, timeout, resp);
	aos_assert_r(rslt, false);	
	if (timeout)
	{
		rdata->setError() << "Failed to add the trans, timeout";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	if (!resp)
	{
		rdata->setError() << "no response";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool
AosIILClient::addTrans(
		const AosTransPtr &trans,
		const AosRundataPtr &rdata)
{
	bool rslt = AosSendTrans(rdata, trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILClient::resetKeyedValue(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool persis,
		const u64 &incValue)
{
	// This function resets the keyed value to 'value' if its current
	// value is greater than 'value'. Otherwise, it increments the
	// current value by 'incValue'. It returns the new value through 'value'.
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::StrBatchAdd(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}

	/*AosTransPtr trans = OmnNew AosIILTransStrBatchAdd(
		iilname, entry_len, buff, executor_id, sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str batch add, " << iilname << endl;
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	return rslt;*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransStrBatchAdd(
	//	iilname, entry_len, buff, executor_id, sgCheckMD5, md5, false, true);
	AosTransPtr trans = OmnNew AosIILTransStrBatchAdd(
		iilname, entry_len, buff, executor_id, snap_id,
		task_docid,sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str batch add, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}


bool
AosIILClient::StrBatchDel(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}

	AosTransPtr trans = OmnNew AosIILTransStrBatchDel(
		iilname, entry_len, buff, executor_id, snap_id,
		task_docid,sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str batch del, " << iilname << endl;

	AosSendTransSyncResp(rdata, trans);	

	return true;
}


bool
AosIILClient::StrBatchInc(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &init_value,
		const AosIILUtil::AosIILIncType incType,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}
	
	/*AosTransPtr trans = OmnNew AosIILTransStrBatchInc(
		iilname, entry_len, buff, init_value, incType, sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str inc block, " << iilname << endl;
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	return rslt;*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransStrBatchInc(
	//	iilname, entry_len, buff, init_value, incType, sgCheckMD5, md5, false, true);
	AosTransPtr trans = OmnNew AosIILTransStrBatchInc(
		iilname, entry_len, buff, init_value, incType,
		snap_id, task_docid, sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str batch inc, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}


bool
AosIILClient::bitmapQueryByPhysicalSafe(
		const int physical_id,
		vector<OmnString> &iilnames,
		vector<AosQueryRsltObjPtr> &bitmap_idlists,
		vector<AosBitmapObjPtr> &partial_bitmaps,
		const AosBitmapObjPtr &bitmap_rslt_phy,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosIILClient::querySafeNorm(
		const u64 &iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilid > 0, false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransQueryDoc(
		iilid, query_rslt, query_bitmap, query_context, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
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

	return true;
}


bool
AosIILClient::querySafeNorm(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(!AosIsSuperIIL(iilname), false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransQueryDoc(
		iilname, query_rslt, query_bitmap, query_context, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
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

	return true;
}

bool
AosIILClient::querySafeNormAsync(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId,
		const AosRundataPtr &rdata)
{
	aos_assert_r(!AosIsSuperIIL(iilname), false);

	AosTransPtr trans = OmnNew AosIILTransQueryDocByNameAsync(
		iilname, query_rslt, query_bitmap, query_context, resp_caller,
		reqId, snapId, false, true);

	return AosSendTransAsyncResp(rdata, trans);
}


bool
AosIILClient::querySafeBigAsync(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosAsyncRespCallerPtr &resp_caller,
		const u64 &reqId,
		const u64 &snapId,
		const AosRundataPtr &rdata)
{
	AosTransPtr trans = OmnNew AosIILTransQueryDocByNameAsync(
		iilname, query_rslt, query_bitmap, query_context, resp_caller,
		reqId, snapId, false, true);

	return AosSendTransAsyncResp(rdata, trans);
}


bool
AosIILClient::querySafeBig(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(AosIsSuperIIL(iilname), false);
	if (query_rslt) query_rslt->setWithValues(true);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransQueryDoc(
		iilname, query_rslt, query_bitmap, query_context, false, true, snap_id);

	OmnString par_iilname = AosIILName::composeParallelIILName(iilname);
	AosTransPtr par_trans = OmnNew AosIILTransQueryDoc(
		par_iilname, query_rslt, query_bitmap, query_context, false, true, snap_id);
	
	AosBuffPtr resp, par_resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = addTrans(par_trans, par_resp, rdata);
	aos_assert_r(rslt && par_resp, false);
	
	rslt = resp->getU8(0);
	bool par_rslt = par_resp->getU8(0);
	
	if (!rslt && !par_rslt) return false;
	if (!rslt && par_rslt)
	{
		resp = par_resp;
		rslt = true;
		par_rslt = false;
	}

	AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
	aos_assert_r(xml, false);
	rslt = query_context->serializeFromXml(xml, rdata);
	aos_assert_r(rslt, false);

	if (par_rslt)
	{
		xml = getXmlFromBuff(par_resp, rdata);
		aos_assert_r(xml, false);
		AosQueryContextObjPtr par_query_context = query_context->clone();
		rslt = par_query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);
		
		rslt = query_context->merge(par_query_context);
		aos_assert_r(rslt, false);
	}

	if (query_rslt)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_rslt->serializeFromXml(xml);
		aos_assert_r(rslt, false);
		
		if (par_rslt)
		{
			xml = getXmlFromBuff(par_resp, rdata);
			aos_assert_r(xml, false);
			AosQueryRsltObjPtr par_query_rslt = AosQueryRsltObj::getQueryRsltStatic();
			rslt = par_query_rslt->serializeFromXml(xml);
			aos_assert_r(rslt, false);

			rslt = query_rslt->merge(par_query_rslt, query_context->finished());
			aos_assert_r(rslt, false);
		}
	}

	u32 len = resp->getU32(0);
	if (len)
	{
		// means has bitmap.
		aos_assert_r(query_bitmap, false);
		query_bitmap->clean();
		AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(buff, false);
		rslt = query_bitmap->loadFromBuff(buff);
		aos_assert_r(rslt, false);
			
		if (par_rslt)
		{
			len = par_resp->getU32(0);
			aos_assert_r(len, false);
			buff = par_resp->getBuff(len, false AosMemoryCheckerArgs);
			aos_assert_r(buff, false);
			AosBitmapObjPtr par_query_bitmap = AosBitmapMgrObj::getBitmapStatic();
			aos_assert_r(par_query_bitmap, false);
			rslt = par_query_bitmap->loadFromBuff(buff);
			aos_assert_r(rslt, false);
		
			rslt = query_bitmap->andBitmap(par_query_bitmap);
			aos_assert_r(rslt, false);
		}
	}

	return true;	
}


bool
AosIILClient::counterRangeNorm(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query,
		const AosRundataPtr &rdata)
{
	aos_assert_r(!AosIsSuperIIL(iilname), false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransCounterRange(
		iilname, query_rslt, query_bitmap, query_context, counter_query, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
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

	if (counter_query)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = counter_query->serializeFromXml(xml);
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

	return true;
}


bool
AosIILClient::counterRangeBig(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosCounterQueryPtr &counter_query,
		const AosRundataPtr &rdata)
{
	aos_assert_r(AosIsSuperIIL(iilname), false);
	if (query_rslt) query_rslt->setWithValues(true);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransCounterRange(
		iilname, query_rslt, query_bitmap, query_context, counter_query, false, true, snap_id);

	OmnString par_iilname = AosIILName::composeParallelIILName(iilname);
	AosTransPtr par_trans = OmnNew AosIILTransCounterRange(
		par_iilname, query_rslt, query_bitmap, query_context, counter_query, false, true, snap_id);
	
	AosBuffPtr resp, par_resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);
	
	rslt = addTrans(par_trans, par_resp, rdata);
	aos_assert_r(rslt && par_resp, false);
		
	rslt = resp->getU8(0);
	bool par_rslt = par_resp->getU8(0);
	
	if (!rslt && !par_rslt) return false;
	if (!rslt && par_rslt)
	{
		resp = par_resp;
		rslt = true;
		par_rslt = false;
	}

	AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
	aos_assert_r(xml, false);
	rslt = query_context->serializeFromXml(xml, rdata);
	aos_assert_r(rslt, false);

	if (par_rslt)
	{
		xml = getXmlFromBuff(par_resp, rdata);
		aos_assert_r(xml, false);
		AosQueryContextObjPtr par_query_context = query_context->clone();
		rslt = par_query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);
		
		rslt = query_context->merge(par_query_context);
		aos_assert_r(rslt, false);
	}

	if (query_rslt)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_rslt->serializeFromXml(xml);
		aos_assert_r(rslt, false);
		
		if (par_rslt)
		{
			xml = getXmlFromBuff(par_resp, rdata);
			aos_assert_r(xml, false);
			AosQueryRsltObjPtr par_query_rslt = AosQueryRsltObj::getQueryRsltStatic();
			rslt = par_query_rslt->serializeFromXml(xml);
			aos_assert_r(rslt, false);

			rslt = query_rslt->merge(par_query_rslt, query_context->finished());
			aos_assert_r(rslt, false);
		}
	}

	if (counter_query)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = counter_query->serializeFromXml(xml);
		aos_assert_r(rslt, false);
	}

	u32 len = resp->getU32(0);
	if (len)
	{
		// means has bitmap.
		aos_assert_r(query_bitmap, false);
		query_bitmap->clean();
		AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);
		aos_assert_r(buff, false);
		rslt = query_bitmap->loadFromBuff(buff);
		aos_assert_r(rslt, false);
			
		if (par_rslt)
		{
			len = par_resp->getU32(0);
			aos_assert_r(len, false);
			buff = par_resp->getBuff(len, false AosMemoryCheckerArgs);
			aos_assert_r(buff, false);
			AosBitmapObjPtr par_query_bitmap = AosBitmapMgrObj::getBitmapStatic();
			aos_assert_r(par_query_bitmap, false);
			rslt = par_query_bitmap->loadFromBuff(buff);
			aos_assert_r(rslt, false);
			rslt = query_bitmap->andBitmap(par_query_bitmap);
			aos_assert_r(rslt, false);
		}
	}

	return true;	
}


bool
AosIILClient::bitmapQuerySafe(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);
	aos_assert_r(iilname != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransBitmapQueryDocByName(
		iilname, query_rslt, query_bitmap, query_context, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}
	
	bool is_super_iil = AosIsSuperIIL(iilname);
	if (!is_super_iil)
	{
		// parse query content
		AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);

		// parse query rslt
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_rslt->serializeFromXml(xml);
		aos_assert_r(rslt, false);

		// parse query_bitmap
		u32 len = resp->getU32(0);
		if (len)
		{
			aos_assert_r(query_bitmap, false);
			query_bitmap->clean();
			AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);
			rslt = query_bitmap->loadFromBuff(buff);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);

		bool has_rslt = resp->getU8(false);
		if (query_rslt && has_rslt)
		{
			xml = getXmlFromBuff(resp, rdata);
			aos_assert_r(xml, false);
			query_rslt->serializeFromXml(xml);
			aos_assert_r(rslt, false);
		}

		bool has_bitmap = resp->getU8(false);
		if(has_bitmap)
		{
			u32 len = resp->getU32(0);
			if (len)
			{
				// means has bitmap.
				aos_assert_r(query_bitmap, false);
				query_bitmap->clean();
				AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);
				aos_assert_r(buff, false);
				rslt = query_bitmap->loadFromBuff(buff);
				aos_assert_r(rslt, false);
			}
		}
	}

	return true;
}


// Chen Ding, 2014/02/05
bool
AosIILClient::bitmapQueryNewSafe(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_r(query_context, false);
	aos_assert_r(query_rslt || query_bitmap, false);
	aos_assert_r(iilname != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransBitmapQueryDocByName(
		iilname, query_rslt, query_bitmap, query_context, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		OmnAlarm << "Failed querying" << enderr;
		return false;
	}

	AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
	aos_assert_r(xml, false);
	rslt = query_context->serializeFromXml(xml, rdata);
	aos_assert_r(rslt, false);

	bool has_rslt = resp->getU8(false);
	if (query_rslt && has_rslt)
	{
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		query_rslt->serializeFromXml(xml);
		aos_assert_r(rslt, false);
	}

	bool has_bitmap = resp->getU8(false);
	if(has_bitmap)
	{
		u32 len = resp->getU32(0);
		if (len)
		{
			// means has bitmap.
			aos_assert_r(query_bitmap, false);
			query_bitmap->clean();
			AosBuffPtr buff = resp->getBuff(len, false AosMemoryCheckerArgs);
			aos_assert_r(buff, false);
			rslt = query_bitmap->loadFromBuff(buff);
			aos_assert_r(rslt, false);
		}
	}
	
	return true;
}


bool
AosIILClient::bitmapRsltQuerySafe(
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosBitmapObjPtr &query_bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(query_context, rdata, false);
	aos_assert_rr(query_rslt && query_bitmap, rdata, false);
	aos_assert_rr(query_bitmap && !query_bitmap->isEmpty(), rdata, false);

// OmnScreen << "Query bitmap" << endl;
// query_bitmap->toString();
	if (iilname == "")
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransBitmapRsltQueryDocByName(
		iilname, query_rslt, query_bitmap, query_context, false, true, snap_id);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	if (!rslt)
	{
		if (query_rslt) query_rslt->reset();
		if (query_bitmap) query_bitmap->clean();
		query_context->setFinished(true);
		return true;
	}

	bool is_super_iil = AosIsSuperIIL(iilname);
	if (!is_super_iil)
	{
		// parse query content
		AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);

		// parse query rslt
		//bool has_rslt = resp->getU8(false);
		resp->getU8(false);
		xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		query_rslt->serializeFromXml(xml);
		aos_assert_r(rslt, false);

	}
	else
	{
		AosXmlTagPtr xml = getXmlFromBuff(resp, rdata);
		aos_assert_r(xml, false);
		rslt = query_context->serializeFromXml(xml, rdata);
		aos_assert_r(rslt, false);

		bool has_rslt = resp->getU8(false);
		if (query_rslt && has_rslt)
		{
			xml = getXmlFromBuff(resp, rdata);
			aos_assert_r(xml, false);
			query_rslt->serializeFromXml(xml);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


u64	
AosIILClient::createSnapshot(
		const u32 virtual_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	if (snap_id == 0) return 0;

	AosTransPtr trans = OmnNew AosIILTransCreateSnapShot(task_docid, virtual_id, snap_id, true, true);
	if (sgShowLog) OmnScreen << "-------------------- create snapshot , " << virtual_id << endl;

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	u64 snap_id1 = resp->getU64(0);
	aos_assert_r(snap_id1, false);
	return snap_id1;
}

bool
AosIILClient::mergeSnapshot(
		const u32 virtual_id,
		const u64 &target_snap_id,
		const u64 &merge_snap_id,
		const AosRundataPtr &rdata)
{
	if (target_snap_id != 0 && merge_snap_id !=0)
	{
		AosTransPtr trans = OmnNew AosIILTransMergeSnapShot(virtual_id, target_snap_id, merge_snap_id, true, true);
		if (sgShowLog) OmnScreen << "-------------------- create snapshot , " << virtual_id << endl;

		AosBuffPtr resp;
		bool rslt = addTrans(trans, resp, rdata);
		aos_assert_r(rslt && resp, false);
		return resp->getU8(0);
	}
	return true;
}


bool	
AosIILClient::commitSnapshot(
		const u32 &virtual_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	if (snap_id == 0) return true;
	AosTransPtr trans = OmnNew AosIILTransCommitSnapShot(task_docid, virtual_id, snap_id, true, false);
	if (sgShowLog) OmnScreen << "-------------------- commit snapshot , " << virtual_id << endl;
	
	bool rslt = addTrans(trans, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIILClient::rollBackSnapshot(
		const u32 virtual_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	if (snap_id == 0) return true;
	AosTransPtr trans = OmnNew AosIILTransRollBackSnapShot(task_docid, virtual_id, snap_id, true, false);
	if (sgShowLog) OmnScreen << "-------------------- rollback snapshot , " << virtual_id << endl;
	
	bool rslt = addTrans(trans, rdata);
	aos_assert_r(rslt, false);
	return true;
}


AosXmlTagPtr
AosIILClient::getXmlFromBuff(
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


int64_t
AosIILClient::getTotalNumDocs(
		const OmnString &iilname,
		const AosRundataPtr &rdata)		
{
	return getTotalNumDocs(iilname, 0, rdata);
}


int64_t
AosIILClient::getTotalNumDocs(
		const OmnString &iilname,
		const u64 &snap_id,
		const AosRundataPtr &rdata)		
{
	aos_assert_r(iilname != "", 0);
	AosTransPtr trans = OmnNew AosIILTransGetTotalNumDocsByName(iilname, snap_id, false, true);
	if (sgShowLog) OmnScreen << "get total num docs: " << iilname << endl;
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	if (!rslt) return 0;

	int64_t total = resp->getI64(-1);
	return total;
}


bool
AosIILClient::StrBatchAddMerge(
		const OmnString &iilname,
		const int64_t &size,
		const u64 &executor_id,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", 0);

	/*AosTransPtr trans = OmnNew AosIILTransStrBatchAddMerge(
		iilname, size, executor_id, true_delete, true, true);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransStrBatchAddMerge(
	//	iilname, size, executor_id, true_delete, true, true);
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrBatchAddMerge(
		iilname, size, executor_id, true_delete, true, true, snap_id);
	if (sgShowLog) OmnScreen << "--------------------str batch add merge, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}


bool
AosIILClient::StrBatchIncMerge(
		const OmnString &iilname,
		const int64_t &size,
		const u64 &dftvalue,
		const AosIILUtil::AosIILIncType incType,
		const bool true_delete,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", 0);

	/*AosTransPtr trans = OmnNew AosIILTransStrBatchIncMerge(
		iilname, size, dftvalue, incType, true_delete, true, true);

	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, 0);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransStrBatchIncMerge(
	//	iilname, size, dftvalue, incType, true_delete, true, true);
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransStrBatchIncMerge(
		iilname, size, dftvalue, incType, true_delete, true, true, snap_id);
	if (sgShowLog) OmnScreen << "--------------------str batch inc merge, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}

	
bool
AosIILClient::HitBatchAdd(
		const OmnString &iilname,
		const vector<u64> &docids,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	/*AosTransPtr trans = OmnNew AosIILTransHitBatchAdd(iilname, docids, false, true);
	if (sgShowLog) OmnScreen << "-------------------- hit batch add , " << iilname << endl;
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransHitBatchAdd(iilname, docids, false, true);
	AosTransPtr trans = OmnNew AosIILTransHitBatchAdd(iilname, docids, snap_id, task_docid, false, true);
	if (sgShowLog) OmnScreen << "--------------------hit batch add, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}


bool
AosIILClient::U64BatchAdd(
		const OmnString &iilname,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}

	/*AosTransPtr trans = OmnNew AosIILTransU64BatchAdd(
		iilname, entry_len, buff, executor_id, sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str add block, " << iilname << endl;
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	return rslt;*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransU64BatchAdd(
	//	iilname, entry_len, buff, executor_id, sgCheckMD5, md5, false, true);
	AosTransPtr trans = OmnNew AosIILTransU64BatchAdd(
		iilname, entry_len, buff, executor_id, snap_id,
		task_docid,sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------u64 batch add, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}


bool
AosIILClient::getBatchDocids(
		const AosRundataPtr &rdata,
		const AosBuffPtr &column,
		const OmnString &iilname)
{
	// It retrieves all the docids for the keys contained in 'keys'.
	OmnString fixstr = rdata->getArg1("field_idx");
	int field_idx = fixstr.toInt();
	AosAsyncRespCallerPtr resp_caller = rdata->getAsyncRespCaller();
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILBatchGetDocidsAsyncTrans(
			column, iilname, resp_caller, field_idx, false, false, false, snap_id);
	AosSendTransAsyncResp(rdata, trans);
	return true;
}


bool
AosIILClient::rebuildBitmap(
				const OmnString &iilname,
				const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);
	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransRebuildBitmap(iilname, false, true, snap_id);

	// Ketty 2014/10/17
	//return addTrans(trans, rdata);
//	AosBuffPtr resp;
//	bool rslt = addTrans(trans, resp, rdata);
//	aos_assert_r(rslt && resp, false);
//	rslt = resp->getU8(0);
	bool timeout = false;
	//AosBuffPtr resp;
	return AosSendTrans(rdata, trans, timeout);
}


bool
AosIILClient::JimoTableBatchAdd(
		const OmnString &iilname,
		const AosXmlTagPtr &cmp_tag,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}

	AosTransPtr trans = OmnNew AosIILTransJimoTableBatchAdd(
		iilname, cmp_tag, buff, executor_id, snap_id,
		task_docid, sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------jimo table batch add, " << iilname << endl;

	AosSendTransSyncResp(rdata, trans);	

	return true;
}


// Chen Ding, 2014/10/25
bool
AosIILClient::querySafe(
		AosRundata *rdata,
		const OmnString &iilname,
		const AosQueryRsltObjPtr &query_rslt,
		const AosValueRslt &start_value,
		const AosValueRslt &end_value,
		AosValueRslt &next_value,
		u64 &next_docid,
		const AosOpr opr,
		const bool reverse,
		const i64 start_pos,
		const int page_size)
{
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 2014/10/25
bool
AosIILClient::querySafe(
		AosRundata *rdata,
		const u64 iilid,
		const AosQueryRsltObjPtr &query_rslt,
		const AosValueRslt &start_value,
		const AosValueRslt &end_value,
		AosValueRslt &next_value,
		u64 &next_docid,
		const AosOpr opr,
		const bool reverse,
		const i64 start_pos,
		const int page_size)
{
	OmnNotImplementedYet;
	return false;
}

bool
AosIILClient::addValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		int &physical_id,
		const AosRundataPtr &rdata
		)
{
	//liuwei
	return true;
}

bool
AosIILClient::addValueDoc(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::addValueDoc(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::addValueDocToTable(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::BatchAdd(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	//liuwei
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}

	/*AosTransPtr trans = OmnNew AosIILTransU64BatchAdd(
		iilname, entry_len, buff, executor_id, sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------str add block, " << iilname << endl;
	
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	return rslt;*/

	// Ketty 2013/07/20
	//AosAsyncReqTransPtr trans = OmnNew AosIILTransU64BatchAdd(
	//	iilname, entry_len, buff, executor_id, sgCheckMD5, md5, false, true);
	AosTransPtr trans = OmnNew AosIILTransBatchAdd(
		iilname, iiltype, entry_len, buff, executor_id, snap_id,
		task_docid,sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "--------------------i64 batch add, " << iilname << endl;

	// Ketty 2013/07/20
	//AosAsyncReqTransMgr::getSelf()->addRequestSync(trans, rdata);
	AosSendTransSyncResp(rdata, trans);	

	return true;
}

bool
AosIILClient::BatchDel(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const int entry_len,
		const AosBuffPtr &buff,
		const u64 &executor_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	if (buff->dataLen() == 0) return true;
	
	OmnString md5;
	if (sgCheckMD5)
	{
		OmnString v(buff->data(), buff->dataLen());
		md5 = AosMD5Encrypt(v);
	}

	AosTransPtr trans = OmnNew AosIILTransBatchDel(
		iilname, iiltype, entry_len, buff, executor_id, snap_id,
		task_docid,sgCheckMD5, md5, false, true);
	if (sgShowLog) OmnScreen << "-------------------- batch del, " << iilname << endl;

	AosSendTransSyncResp(rdata, trans);	

	return true;
}

bool
AosIILClient::getDocid(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const AosOpr opr,
		const bool reverse,
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::getDocid(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const AosOpr opr,
		const bool reverse,
		u64 &docid,
		bool &isunique,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::incrementDocid(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &the_key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::incrementDocid(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &key,
		u64 &value,
		const bool isPersis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::incrementDocidToTable(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &the_key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::modifyValueDoc(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::modifyValueDoc(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &oldvalue,
		const OmnString &newvalue,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique,
		const bool override,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::removeValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		int &physical_id,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::removeValueDoc(
		vector<AosTransPtr> *allTrans,
		const u32 arr_len,
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool
AosIILClient::removeValueDoc(
		const OmnString &iilname,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

bool 
AosIILClient::removeValueDoc(
		const u64 &iilid,
		const AosIILType &iiltype,
		const OmnString &the_value,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	//liuwei
	return true;
}

	
bool
AosIILClient::getMapValues( 
		const OmnString &iilname,
		set<OmnString> &keys,
		vector<u64> &values,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilname != "", false);

	u64 snap_id = rdata->getSnapshotId();
	AosTransPtr trans = OmnNew AosIILTransGetMapValues(iilname, keys, false, true, snap_id);
	AosBuffPtr resp;
	bool rslt = addTrans(trans, resp, rdata);
	aos_assert_r(rslt && resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, 0);
	
	u32 size = resp->getU32(0);
	aos_assert_r(size == keys.size(), false);

	values.clear();
	for (u32 i=0; i<size; i++) 
	{
		u64 value = resp->getU64(0);
		aos_assert_r(value != 0, false);
		values.push_back(value);
	}

	return true;
}

