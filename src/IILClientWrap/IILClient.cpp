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
// 2011/03/17	Created by Chen Ding	
////////////////////////////////////////////////////////////////////////////
#include "IILClient/IILClient.h"

#include "AppMgr/App.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILProc.h"
#include "IILClient/IILProcMgr.h"
#include "IILUtil/IILLogType.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"


OmnSingletonImpl(AosIILClientSingleton,
                 AosIILClient,
                 AosIILClientSelf,
                "AosIILClient");

AosIILClient::AosIILClient()
:
mLock(OmnNew OmnMutex())
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
	AosIILMgrSelf->stop();
	return AosIIL::staticStop();
}


bool		
AosIILClient::start(const AosXmlTagPtr &config)
{
	AosIILProcMgr::getSelf()->start(config);
	return true;
}


bool 		
AosIILClient::addDoc(
		const char *word,
		const int len,
		const u64 &docId) 
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->addDoc(word,len,docId);	
	}
	
	if (len > eMaxWordLen) return true;
	char mem[100];
	memset(mem, '\0', 100);
	AosBuff buff(mem, 100, 0);

	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(word, true, eAosIILType_Str);
	aos_assert_r(iilid, false);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eHitAddDoc);

	buff.setU64(docId);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool		
AosIILClient::addValueDoc(
		const OmnString &iilname,
		const OmnString &value,
		const u64 &docid,
		const bool value_unique,
		const bool docid_unique)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->addValueDoc(
			iilname,value,docid,value_unique,docid_unique);
	}

	if (iilname.length() > eMaxNameLength) return true;
	if (value.length() > AosIILUtil::eMaxStrValueLen) return true;
	char mem[AosIILUtil::eMaxStrValueLen + 100];
	AosBuff buff(mem, AosIILUtil::eMaxStrValueLen + 100, 0);

	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_Str);
	aos_assert_r(iilid, false);

	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eStrAddValueDoc);

	buff.setOmnStr(value);
	buff.setU64(docid);
	buff.setU8((u8)value_unique);
	buff.setU8((u8)docid_unique);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool		
AosIILClient::addValueDoc2(
					const OmnString &iilname,
					const u64 &value,
					const u64 &docid,
					const bool value_unique,
					const bool docid_unique)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->addValueDoc2( 
				iilname, value, docid, value_unique, docid_unique);
	}
	
	if (iilname.length() > eMaxNameLength) return true;
	char mem[100];
	AosBuff buff(mem, 100, 0);

	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_r(iilid, false);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64AddValueDoc);

	buff.setU64(value);
	buff.setU64(docid);
	buff.setU8((u8)value_unique);
	buff.setU8((u8)docid_unique);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool		
AosIILClient::modifyValueDoc1(
					const OmnString &iilname,
					const OmnString &oldvalue,
					const OmnString &newvalue,
					const bool value_unique,
					const bool docid_unique,
					const u64 &docid)
{
	//server init
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->modifyValueDoc1( iilname, oldvalue, newvalue,
	 				value_unique, docid_unique, docid);
	}
	
	if (iilname.length() > eMaxNameLength) return true;
	if (oldvalue.length() > AosIILUtil::eMaxStrValueLen) return true;
	if (newvalue.length() > AosIILUtil::eMaxStrValueLen) return true;

	removeValueDoc(iilname, oldvalue, docid);
	addValueDoc(iilname, newvalue, docid, value_unique, docid_unique);

	return true;
}


bool		
AosIILClient::modifyValueDoc1(
					const OmnString &name,
					const u64 &oldvalue,
					const u64 &newvalue,
					const bool value_unique,
					const bool docid_unique,
					const u64 &docid)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->modifyValueDoc1( name, oldvalue, 
				newvalue, value_unique, docid_unique, docid);
	}

	if (name.length() > eMaxNameLength) return true;

	removeValueDoc(name,oldvalue, docid);
	addValueDoc2(name, newvalue, docid, value_unique, docid_unique);

	return true;
}


bool 		
AosIILClient::removeDoc(
					const char *word,
					const int len,
					const u64 &docId)
{
	aos_assert_r(word, false);
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->removeDoc(word, len, docId);
	}
	
	if (len > eMaxWordLen) return true;
	char mem[100];
	AosBuff buff(mem, 100, 0);

	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(word, true, eAosIILType_Hit);
	aos_assert_r(iilid, false);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eHitRemoveDoc);

	buff.setU64(docId);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool		
AosIILClient::removeValueDoc(
					const OmnString &name,
					const OmnString &value,
					const u64 &docid)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->removeValueDoc( name, value, docid);
	}
	
	if (name.length() > eMaxNameLength) return true;
	if (value.length() > AosIILUtil::eMaxStrValueLen) return true;

	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(name, true, eAosIILType_Str);
	aos_assert_r(iilid, false);
	char mem[AosIILUtil::eMaxStrValueLen + 100];
	AosBuff buff(mem, AosIILUtil::eMaxStrValueLen + 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eStrRemoveValueDoc);

	buff.setOmnStr(value);
	buff.setU64(docid);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool		
AosIILClient::removeValueDoc(
					const OmnString &name,
					const u64 &value,
					const u64 &docid)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->removeValueDoc(name, value, docid);
	}
		
	if (name.length() > eMaxNameLength) return true;

	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(name, true, eAosIILType_U64);
	aos_assert_r(iilid, false);
	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64RemoveValueDoc);

	buff.setU64(value);
	buff.setU64(docid);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


AosXmlTagPtr	
AosIILClient::getDoc(const u64 &docid)
{
	return AosIILMgrSelf->getDoc(docid);
}


AosXmlTagPtr	
AosIILClient::getDoc(const OmnString &siteid,
						const OmnString &objid)
{
	return AosIILMgrSelf->getDoc( siteid, objid);
}


bool			
AosIILClient::getDocid(
		const AosRundataPtr &rdata,
		const OmnString &tagname,
		const OmnString &attrname,
		const OmnString &key,
		u64 &docid,
		bool &isunique)
{
	aos_assert_r(key!="", false);
	return AosIILMgrSelf->getDocid(rdata, tagname, attrname, key, docid, isunique, false);
}


bool			
AosIILClient::getDocid(
		const AosRundataPtr &rdata,
		const OmnString &iilname,
		const OmnString &key,
		u64 &docid,
		bool &isunique)
{
	aos_assert_r(key!="", false);
	return AosIILMgrSelf->getDocid(rdata, iilname, key, docid, isunique, false);
}


bool
AosIILClient::getDocid(
		const OmnString &iilname,
		const u64 &key,
		u64 &docid,
		const u64 &dft,
		bool &found,
		bool &is_unique)
{
	return AosIILMgr::getSelf()->getDocid(iilname, key, docid, dft, found, is_unique);
}


bool			
AosIILClient::getDocidByObjid(
		const AosRundataPtr &rdata,
		const OmnString &siteid,
		const OmnString &objid,
		u64 &docid,
		bool &isunique)
{
	//Zky2248,Linda, 01/13/2011 
	aos_assert_r(objid!="", false);
	return AosIILMgrSelf->getDocidByObjid(rdata, siteid, objid, docid, isunique, false);
}


AosXmlTagPtr
AosIILClient::getCtnrMemRcd(
		const OmnString &siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicate,
		OmnString &errmsg)
{
	return AosIILMgrSelf->getCtnrMemRcd(
		siteid, ctnr_objid, keyname, keyvalue, duplicate, errmsg);
}


AosXmlTagPtr
AosIILClient::getCtnrMemRcd(
		const OmnString &siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicate,
		OmnString &errmsg)
{
	return AosIILMgrSelf->getCtnrMemRcd( siteid, ctnr_objid, keyname1, keyvalue1,
				keyname2, keyvalue2, duplicate, errmsg);
}


u64
AosIILClient::getCtnrMemDocid(
		const OmnString &siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicated,
		OmnString &errmsg)
{
	return AosIILMgrSelf->getCtnrMemDocid( siteid, ctnr_objid, keyname1, keyvalue1,
				keyname2, keyvalue2, duplicated, errmsg);
}


AosIILPtr 	
AosIILClient::getIILPublic(const OmnString &iilname,
							 const bool crt_flag,
							 const AosIILType iiltype)
{
	return AosIILMgrSelf->getIILPublic(iilname, crt_flag, iiltype);
}


u64
AosIILClient::getIILIdPublic(const OmnString &iilname,
							 const bool crt_flag,
							 const AosIILType iiltype)
{
	return AosIILMgrSelf->getIILIdPublic(iilname, crt_flag, iiltype);
}


bool		
AosIILClient::saveAllIILs(const bool reset)
{
	return AosIILMgrSelf->saveAllIILs(reset);
}


bool 		
AosIILClient::returnIILPublic(const AosIILPtr &iil)
{
	return AosIILMgrSelf->returnIILPublic(iil);
}


bool		
AosIILClient::returnIILPublic(const AosIILPtr &iil,
		                       const bool iilmgrLocked,
							   bool &returned)
{
	return AosIILMgrSelf->returnIILPublic(iil, iilmgrLocked, returned);
}


AosIILPtr 	
AosIILClient::createSubiilSafe(const AosIILType iiltype, const bool locked)
{
	return AosIILMgrSelf->createSubiilSafe(iiltype, locked);
}


void 		
AosIILClient::setSanithCheck(const bool s)
{
	return AosIILMgrSelf->setSanithCheck(s);
}


void 		
AosIILClient::setShowLog(const bool s)
{
	return AosIILMgrSelf->setShowLog(s);
}


void 		
AosIILClient::setCheckRef(const bool s)
{
	return AosIILMgrSelf->setCheckRef(s);
}


void 		
AosIILClient::setCheckPtr(const bool s)
{
	return AosIILMgrSelf->setCheckPtr(s);
}


int 		
AosIILClient::getDocids(
					const OmnString &iilname,
					const OmnString &key,
					u64 *docids,
					const int arraysize,
					OmnString &errmsg) 
{
	return AosIILMgrSelf->getDocids(iilname, key, docids, arraysize, errmsg);
}


u64
AosIILClient::getCtnrMemDocid(
		const OmnString &siteid,
		const OmnString &ctnr_objid,
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicated,
		OmnString &errmsg)
{
	return AosIILMgrSelf->getCtnrMemDocid(
		siteid,
		ctnr_objid,
		keyname,
		keyvalue,
		duplicated,
		errmsg);
}


bool 
AosIILClient::saveAllIILsPublic(const bool reset)
{
	return AosIILMgrSelf->saveAllIILsPublic(reset);
}


bool		
AosIILClient::saveAllIILDocids(const bool reset)
{
	return AosIILMgrSelf->saveAllIILDocids(reset);
}


AosIILPtr 	
AosIILClient::createSubiilDocidSafe(const bool locked)
{
	return AosIILMgrSelf->createSubiilDocidSafe(locked);
}


AosIILPtr 	
AosIILClient::retrieveIIL(
		const char *word, 
		const int len, 
		const AosIILType type, 
		const bool createFlag)
{
	return AosIILMgrSelf->retrieveIIL(word, len, type, createFlag);
}


AosIILPtr 
AosIILClient::loadIILByIDPublic(const u64 &iilid, AosIILType &iiltype)
{
	return AosIILMgrSelf->loadIILByIDPublic1(iilid, iiltype);
}


bool	
AosIILClient::incrementDocid(
		const OmnString &iilname, 
		const u64 &key, 
		const u64 &incValue, 
		const u64 &initValue, 
		u64 &new_value)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->incrementDocidSafe(
				iilname, key, incValue, initValue, new_value);
	}

	// This function retrieves the docid identified by 'key'. If it is 
	// there, the docid increments by one. Otherwise, an entry is added into 
	// the IIL. 
	//
	// Since this is the wrapper, we need to check whether an entry 
	// is available in the map. If yes, the docid is incremented. 
	// Otherwise, it is set to 'initValue'.
	if (iilname.length() > eMaxNameLength) return true;

	mLock->lock();
	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_rl(iilid, mLock, false);

	bool flag;
	new_value = getCachedDocid(iilid, key, flag);
	if (!flag)
	{
		// Not in the cache yet. Need to retrieve it from the IIL.
		bool is_unique;
		bool found;
		bool rslt = AosIILMgr::getSelf()->getDocid(
				iilname, key, new_value, initValue, found, is_unique);

		if (!found)
		{
			// The entry is not there yet.
			new_value = initValue;
			rslt = addValueDoc2(iilname, key, new_value, true, false);
			addCachedDocid(iilid, key, new_value);
			mLock->unlock();
			return true;
		}
		
		// The entry exists. 
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(is_unique, mLock, false);
	}
	
	// The entry is already in the system.
	new_value += incValue;
	addCachedDocid(iilid, key, new_value);

	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64IncrementDocid);

	buff.setU64(key);
	buff.setU64(incValue);
	buff.setU64(initValue);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	mLock->unlock();
	return true;
}


bool	
AosIILClient::incrementDocid(
		const u64 &iilid,
		const OmnString &key, 
		u64 &incValue) 
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgr::getSelf()->incrementDocidSafe(iilid, key, incValue);
	}

	bool found;
	mLock->lock();
	u64 new_value = getCachedDocid(iilid, key, found);
	if (!found)
	{
		// Not in the cache yet. Need to retrieve it from the IIL.
		bool is_unique;
		bool found;
		bool rslt = AosIILMgr::getSelf()->getDocid(
				iilid, key, new_value, incValue, found, is_unique);

		if (!found)
		{
			// The entry is not there yet.
			new_value = incValue;
			char mem[AosIILUtil::eMaxStrValueLen + 100];
			AosBuff buff(mem, AosIILUtil::eMaxStrValueLen + 100, 0);
			buff.setU32(AosIILProcMgr::getSelf()->getTransid());
			buff.setU64(iilid);
			buff.setU32((u32)AosIILLogType::eStrAddValueDoc);

			buff.setOmnStr(key);
			buff.setU64(new_value);
			buff.setU8((u8)true);
			buff.setU8((u8)false);
			buff.reset();
			AosIILProcMgr::getSelf()->appendLog(iilid, buff);

			addCachedDocid(iilid, key, new_value);
			mLock->unlock();
			return true;
		}
		
		// The entry exists. 
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(is_unique, mLock, false);
	}
	
	// The entry is already in the system.
	new_value += incValue;
	addCachedDocid(iilid, key, new_value);

	const int len = AosIILUtil::eMaxStrValueLen + 100;
	char mem[len];
	AosBuff buff(mem, len, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eStrIncrementDocid);

	buff.setOmnStr(key);
	buff.setU64(incValue);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	mLock->unlock();
	return true;
}


bool
AosIILClient::updateKeyedValue(
		const OmnString &iilname,
		const u64 &key,
		const bool &flag,
		const u64 &delta,
		u64 &new_value)
{
	// This function increments (flag == true) or decrements (flag = false)
	// the entry:
	// 			[key, value]
	// If the entry is not there, it adds the entry:
	// 			[key, delta]
	// If the entry is there, it modifies the entry:
	// 			[key, value +/- delta]
	
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->updateKeyedValue(iilname, key, flag, delta, new_value);
	}
	
	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_r(iilid, false);
	
	bool exist;
	mLock->lock();
	new_value = getCachedDocid(iilid, key, exist);
	if (!exist)
	{
		// It is not in the cache. Retrieve it from the database.
		bool found, is_unique;
		aos_assert_rl(AosIILMgr::getSelf()->getDocid(
				iilname, key, new_value, delta, found, is_unique), mLock, false);
		if (!found)
		{
			// The entry is not there yet. 
			addValueDoc2(iilname, key, new_value, true, false);
			addCachedDocid(iilid, key, new_value);
			mLock->unlock();
			return true;
		}
	}
	
	if (flag)
	{
		new_value += delta;
	}
	else
	{
		if (new_value < delta)
		{
			new_value = 0;
		}
		else
		{
			new_value -= delta;
		}
	}
	addCachedDocid(iilid, key, new_value);

	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64SetValueDoc);

	buff.setU64(key);
	buff.setU64(new_value);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	mLock->unlock();
	
	return true;
}


bool 
AosIILClient::setValueDoc(
		const OmnString &iilname, 
		const u64 &key, 
		const u64 &new_value) 
{
	// This function sets the entry [key, docid]. If it is not there, 
	// the entry is added. Otherwise, it is set. 
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->setValueDoc(iilname, key, new_value);
	}
	
	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_r(iilid, false);
	
	mLock->lock();
	addCachedDocid(iilid, key, new_value);

	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64SetValueDoc);

	buff.setU64(key);
	buff.setU64(new_value);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	mLock->unlock();
	return true;
}


bool
AosIILClient::staticInit(const AosXmlTagPtr &config)
{
	return AosIIL::staticInit(config);
}


u64 
AosIILClient::createIILPublic2(const AosIILType iiltype)
{
	return AosIILMgrSelf->createIILPublic2Safe(iiltype);
}


bool		
AosIILClient::updateDoc4(const OmnString &iilname, 
						 const bool createIIL,
						 const u64 value1,
						 const u64 docid1,
						 const u64 value2,
						 const u64 docid2,
						 u64 &iilid)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->updateDoc4Safe(iilname, 
									 createIIL,
									 value1,
									 docid1,
									 value2,
									 docid2,
									 iilid); 
	}
	
	return setValueDoc(iilname, value1, docid1) &&
		   setValueDoc(iilname, value2, docid2);
}


bool 
AosIILClient::appendManualOrder(
		const OmnString &iilname,
		const u64 &key,
		u64 &value)
{
	// Manual Order is a special attribute for docs in a container. When appending
	// a manual order, it will set the attribute value to the value of the last
	// doc + 4G (i << 32). 
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->appendManualOrder(iilname, key, value);
	}

	if (iilname.length() > eMaxNameLength) return true;
	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_r(iilid, false);

	mLock->lock();
	bool flag;
	u64 vv = getCachedDocid(iilid, key, flag);
	if (!flag)
	{
		// The entry is not in the cache yet. Need to get retrieve the last
		// value. 
		aos_assert_rl(AosIILMgr::getSelf()->getLastValue(iilname, vv), mLock, false);
	}
	
	value = vv + AosIILUtil::getManualOrderIncValue();
	addCachedDocid(iilid, key, value);

	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64AppendManualOrder);

	buff.setU64(key);
	buff.setU64(value);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool 
AosIILClient::moveManualOrder(
		const OmnString &iilname,
		const u64 &v1, 
		const u64 &d1,
		const u64 &v2,
		const u64 &d2)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->moveManualOrder(iilname, v1, d1, v2, d2);
	}
	
	if (iilname.length() > eMaxNameLength) return true;

	// Retrieve the counter
	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_r(iilid, false);

	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64MoveManualOrder);
	
	buff.setU64(v1);
	buff.setU64(d1);
	buff.setU64(v2);
	buff.setU64(d2);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	return true;
}


bool 
AosIILClient::swapManualOrder(const OmnString &iilname, 
				const u64 &value1,
				const u64 &docid1, 
				const u64 &value2,
				const u64 &docid2)
{
	// Swapping manual order assumes there are two entries:
	// 		[value1, docid1]
	// 		[value2, docid2]
	// After swapping, it should be:
	// 		[value1, docid2]
	// 		[value2, docid1]
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->swapManualOrder(iilname, value1, docid1, value2, docid2);
	}

	if (iilname.length() > eMaxNameLength) return true;

	// Retrieve the counter
	u64 iilid = AosIILMgr::getSelf()->getIILIdPublic(iilname, true, eAosIILType_U64);
	aos_assert_r(iilid, false);

	char mem[100];
	AosBuff buff(mem, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff.setU64(iilid);
	buff.setU32((u32)AosIILLogType::eU64SetDocid);

	buff.setU64(value1);
	buff.setU64(docid2);
	buff.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff);
	
	char mem1[100];
	AosBuff buff1(mem1, 100, 0);
	buff.setU32(AosIILProcMgr::getSelf()->getTransid());
	buff1.setU64(iilid);
	buff1.setU32((u32)AosIILLogType::eU64SetDocid);

	buff1.setU64(value2);
	buff1.setU64(docid1);
	buff1.reset();
	AosIILProcMgr::getSelf()->appendLog(iilid, buff1);
	
	return true;
}


bool AosIILClient::removeManualOrder(
		const OmnString &iilname,
		const u64 &value,
		const u64 &docid)
{
	if (!OmnApp::isRunning())
	{
		return AosIILMgrSelf->removeManualOrder(iilname, value, docid);
	}
	
	removeValueDoc(iilname, value, docid);

	return true;
}


bool AosIILClient::getDocidByPrefix(
		int &idx,
		int &iilidx,
		const bool reverse,
		const OmnString &iilname,
		const OmnString &prefix,
		u64 &docid)
{
	return AosIILMgrSelf->getDocidByPrefix(idx, iilidx, reverse, iilname, prefix, docid);

}


bool AosIILClient::getDocidByValue(
		int &idx,
		int &iilidx,
		const bool reverse,
		const OmnString &iilname,
		const OmnString &value,
		u64 &docid)
{
	return AosIILMgrSelf->getDocidByValue(idx, iilidx, reverse, iilname, value, docid);

}


void
AosIILClient::printHitStat()
{
	AosIILMgr::printHitStat();
}


// Chen Ding, 2011/02/24
bool 
AosIILClient::pickNextDoc(
		const OmnString &iilname,
		int &idx, 
		int &iilidx,
		const bool reverse, 
		OmnString &value, 
		u64 &docid)
{
	return AosIILMgrSelf->pickNextDoc(iilname, idx, iilidx, reverse, value, docid);
}


// Chen Ding, 03/27/2011
u64 
AosIILClient::getCachedDocid(const u64 &iilid, const u64 &key, bool &found)
{
	// The class maintains a map that maps [iilname + value] to docid. 
	// Or in other word, it is a collection of IIL entries from different
	// IILs. It is used for entries that are used as counters. 
	// Changes to the entries are cached in this map. Entries in this map
	// may be flushed out based on the time. When an entry is not used for
	// a given number of seconds, it may be flushed out. If an entry occupies
	// 150 bytes and the map maintains up to 100K entries, the map needs about
	// 15M bytes. 
	//
	// Entries are identified by iilname concatenated by the value.
	OmnString id;
	id << iilid << "_" << key;

	map<OmnString, u64>::iterator iter = mEntryMap.find(id);
	if (iter != mEntryMap.end())
	{
	 	// Found it. 
		found = true;
	 	return iter->second;
	}
	
	found = false;
	return 0;
}


u64 
AosIILClient::getCachedDocid(const u64 &iilid, const OmnString &key, bool &found)
{
	// The class maintains a map that maps [iilname + value] to docid. 
	// Or in other word, it is a collection of IIL entries from different
	// IILs. It is used for entries that are used as counters. 
	// Changes to the entries are cached in this map. Entries in this map
	// may be flushed out based on the time. When an entry is not used for
	// a given number of seconds, it may be flushed out. If an entry occupies
	// 150 bytes and the map maintains up to 100K entries, the map needs about
	// 15M bytes. 
	//
	// Entries are identified by iilname concatenated by the value.
	OmnString id;
	id << iilid << "_" << key;

	map<OmnString, u64>::iterator iter = mEntryMap.find(id);
	if (iter != mEntryMap.end())
	{
	 	// Found it. 
		found = true;
	 	return iter->second;
	}
	
	found = false;
	return 0;
}


bool
AosIILClient::addCachedDocid(
		const u64 &iilid, 
		const u64 &key, 
		const u64 &docid)
{
	OmnString id;
	id << iilid << "_" << key;

	mEntryMap[id] = docid;
	return true;
}


bool
AosIILClient::addCachedDocid(
		const u64 &iilid, 
		const OmnString &key, 
		const u64 &docid)
{
	OmnString id;
	id << iilid << "_" << key;

	mEntryMap[id] = docid;
	return true;
}

