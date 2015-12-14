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
// 2011/03/17	Created by Chen Ding	
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILClientWrap_IILClient_h
#define AOS_IILClientWrap_IILClient_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

#include "IdGen/IdGen.h"
#include "IILClient/Ptrs.h"
#include "IILClient/IILProcMgr.h"
#include "IILMgr/IILUtil.h"
#include "Rundata/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/LockMonitor.h"
#include "Thread/ThreadedObj.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "UtilComm/Ptrs.h"
#include <map>



OmnDefineSingletonClass(AosIILClientSingleton,
						AosIILClient,
						AosIILClientSelf,
						OmnSingletonObjId::eIILClient,
						"IILClient");


class AosIILClient : virtual public OmnRCObject 
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxWordLen = 20,
		eMaxBuffLen,
		eMaxNameLength,
		eMaxValueLength,

		eAddDoc
	};

	OmnMutexPtr         mLock;
	u64					mValue;
	map<OmnString, u64>	mEntryMap;

	
public:
	AosIILClient();
	~AosIILClient();

    // Singleton class interface
	AosIILPtr 	loadIILByIDPublic(
					const u64 &iilid, 
					const u64 &wordid,
					const AosIILType iiltype, 
					const char *word, 
					const int len);

    static AosIILClient*  	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual OmnString   getSysObjName() const {return "AosIILClient";}
    virtual OmnRslt     config(const OmnXmlParserPtr &def){return true;}
    virtual OmnSingletonObjId::E  getSysObjId() const 
						{
							return OmnSingletonObjId::eIILClient;
						}

	bool		start(const AosXmlTagPtr &config);
	bool		config(const AosXmlTagPtr &config);

	bool	addDoc(
			const char *word,
			const int len,
			const u64 &docId); 
	
	bool 	addDoc(
			const OmnString &name1, 
			const OmnString &name2, 
			const u64 &docid)
	{
		if (name2 == "") return true;
		OmnString tag = name1;
		tag << name2;
		return addDoc(tag.data(), tag.length(), docid);
	}

	bool		addValueDoc(
					const OmnString &iilname, 
					const OmnString &value, 
					const u64 &docid, 
					const bool value_unique,
					const bool docid_unique);
	bool		
	addValueDoc2(
			const OmnString &iilname, 
			const OmnString &value, 
			const OmnString &docid, 
			const bool value_unique,
			const bool docid_unique)
	{
		u64 did = atoll(docid.data());
		return addValueDoc(iilname, value, did, value_unique, docid_unique);
	}

	bool		
	addValueDoc2(
			const OmnString &iilname, 
			const OmnString &attrname, 
			const OmnString &value, 
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique)
	{
		aos_assert_r(attrname != "", false);
		OmnString ww = iilname;
		ww << attrname;
		return addValueDoc(ww, value, docid, value_unique, docid_unique);
	}

	bool		
	addValueDoc2(
			const OmnString &iilname, 
			const OmnString &attrname,
			const u64 &value, 
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique)
	{
		aos_assert_r(attrname != "", false);
		OmnString ww = iilname;
		ww << attrname;
		return addValueDoc2(ww, value, docid, value_unique, docid_unique);
	}

	bool addValueDoc2(
			const OmnString &iilname, 
			const u64 &value, 
			const u64 &docid, 
			const bool value_unique,
			const bool docid_unique);

	bool		
	modifyValueDoc1(
			const OmnString &name, 
			const OmnString &oldvalue,
			const OmnString &newvalue, 
			const bool value_unique,
			const bool docid_unique,
			const OmnString &docid)
	{
		u64 did = atoll(docid.data());
		return modifyValueDoc1(name, oldvalue, newvalue, value_unique, docid_unique, did);
	}

	bool		modifyValueDoc1(
					const OmnString &name, 
					const OmnString &oldvalue, 
					const OmnString &newvalue, 
					const bool value_unique,
					const bool docid_unique,
					const u64 &docid);
	bool		
	modifyValueDoc1(
			const OmnString &name, 
			const u64 &oldvalue, 
			const u64 &newvalue, 
			const bool value_unique,
			const bool docid_unique,
			const OmnString &docid)
	{
		u64 did = atoll(docid.data());
		return modifyValueDoc1(name, oldvalue, newvalue, value_unique, docid_unique, did);
	}

	bool		modifyValueDoc1(
					const OmnString &name, 
					const u64 &oldvalue, 
					const u64 &newvalue, 
					const bool value_unique,
					const bool docid_unique,
					const u64 &docid);
	bool 		removeDoc(
					const char *word, 
					const int len, 
					const u64 &docId); 
	bool		
	removeDoc(const OmnString &name1, 
			const OmnString &name2, 
			const u64 &docid)
	{
		if (name2 == "") return true;
		OmnString nn = name1;
		nn << name2;
		return removeDoc(nn.data(), nn.length(), docid);
	}

	bool 		getIILValue(
					int &idx, 
					const AosOpr opr,
					const bool reverse,
					const OmnString &tag, 
					const OmnString &value, 
					u64 &docid);
	bool		
	removeValueDoc(
			const OmnString &name1, 
			const OmnString &name2,
			const OmnString &value, 
			const u64 &docid)
	{
		aos_assert_r(name2 != "", false);
		OmnString nn = name1;
		nn << name2;
		return removeValueDoc(nn, value, docid);
	}
	bool		removeValueDoc(
					const OmnString &name, 
					const OmnString &value, 
					const u64 &docid);
	bool		removeValueDoc(
					const OmnString &name, 
					const u64 &value, 
					const u64 &docid);

	AosIILDocidPtr 	createDocidIIL(const AosIILPtr &iil);
	AosXmlTagPtr	getDoc(const u64 &docid);
	AosXmlTagPtr	getDoc(
						const OmnString &siteid, 
						const OmnString &objid); 
	AosXmlTagPtr	getDoc(const OmnString &iilname, 
						const OmnString &key, 
						bool &duplicated,
						AosXmlDoc &header, 
						OmnString &errmsg);
	bool			getDocid(
						const AosRundataPtr &rdata,
						const OmnString &tagname, 
						const OmnString &attrname,
						const OmnString &key, 
						u64 &docid,
						bool &isunique);
	bool			getDocid(
						const AosRundataPtr &rdata, 
						const OmnString &iilname, 
						const OmnString &key, 
						u64 &docid,
						bool &isunique); 
	// Tank 12/04/2011
	bool			getDocid(const OmnString &iilname, 
						const u64 &key, 
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &is_unique);
	bool getDocidByObjid(
			const AosRundataPtr &rdata, 
			const OmnString &siteid, 
			const OmnString &objid, 
			u64 &docid, 
			bool &isunique); 
	bool getDocid(const OmnString &iilname, 
				const OmnString &value, 
				u64 &docid, 
				const AosRundataPtr &rdata)
	{
		OmnString errmsg;
		bool isunique;
		aos_assert_r(value!="", false);
		return getDocid(rdata, iilname, value, docid, isunique);
	}

	AosXmlTagPtr	
	getCtnrMemRcd(
		const OmnString &siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicate, 
		OmnString &errmsg);

	AosXmlTagPtr	
	getCtnrMemRcd(
		const OmnString &siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicate, 
		OmnString &errmsg);

	u64
	getCtnrMemDocid(
		const OmnString &siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &keyname1,
		const OmnString &keyvalue1,
		const OmnString &keyname2,
		const OmnString &keyvalue2,
		bool &duplicated, 
		OmnString &errmsg);

	AosIILPtr 	
	getIILPublic(
		const OmnString &iilname,
		const bool crt_flag = false,
		const AosIILType iiltype = eAosIILType_Invalid);

	u64 
	getIILIdPublic(const OmnString &iilname,
		 const bool crt_flag = false,
		 const AosIILType iiltype = eAosIILType_Invalid);

	bool		saveAllIILs(const bool reset);
	bool 		checkIIL(AosIIL *iil);
	bool 		returnIILPublic(const AosIILPtr &iil);
	bool		returnIILPublic(const AosIILPtr &iil,
							   const bool iilmgrLocked,
							   bool &returned);
	OmnString 	getWords(
					const u64 &docid, 
					const u32 &startiil,
					const int psize);
	OmnString	getIILContents(
					const u64 &iilid,
					const int startidx,
					const int psize);
	AosIILPtr 	createSubiilSafe(const AosIILType iiltype, const bool locked);

	void 		setSanithCheck(const bool s);// {mSanityCheck = s;}
	void 		setShowLog(const bool s);// {mShowLog = s;}
	void 		setCheckRef(const bool s);// {mCheckRef = s;}
	void 		setCheckPtr(const bool s);// {mCheckPtr = s;}
	int 		getDocids(
					const OmnString &iilname, 
					const OmnString &key, 
					u64 *docids,
					const int arraysize,
					OmnString &errmsg) ;
	OmnString 	incrementValue(
					const OmnString &tagname, 
					const OmnString &attrname, 
					const OmnString &oldvalue,
					const u64 &docid);

	AosIILPtr	getLogIIL(const OmnString &siteid, const OmnString &logid);

	u64
	getCtnrMemDocid(
		const OmnString &siteid, 
		const OmnString &ctnr_objid, 
		const OmnString &keyname,
		const OmnString &keyvalue,
		bool &duplicated, 
		OmnString &errmsg);
	static bool			staticInit(const AosXmlTagPtr &config);

public:
	bool saveAllIILsPublic(const bool reset);
		
	bool		saveAllIILDocids(const bool reset);
	bool 		returnIILDocidPublic(const AosIILDocidPtr &iil,
									 const bool iilmgrLocked,
									 bool &returned);
	bool		returnIILDocidPublicWithoutLock(const AosIILDocidPtr &iil,
												const bool iilmgrLocked,
												bool &returned);

	AosIILPtr 	createSubiilDocidSafe(const bool locked);

	bool		incrementCounter(
					const OmnString &iilname, 
					const u64 &counter_id,
					const bool &flag,
					const u64 &value, 
					u64 &counter);

	bool        setValueDoc(const OmnString &iilname, const u64 &key, const u64 &docid);

	bool		updateDoc4(const OmnString &iilname, 
										 const bool createIIL,
										 const u64 value1,
										 const u64 docid1, 
										 const u64 value2,
										 const u64 docid2,
										 u64 &iilid); 
		
public:
	bool	checkIILLocks();
	AosIILPtr 	retrieveIIL(const char *word, const int len, const AosIILType, const bool);
	AosIILPtr 	loadIILByIDPublic(const u64 &iilid, AosIILType &iiltype);
	AosIILPtr 	createIILPublic(const AosIILType iiltype);
	u64 		createIILPublic2(const AosIILType iiltype);

	bool updateKeyedValue(
			const OmnString &iilname, 
			const u64 &key, 
			const bool &flag,
			const u64 &delta,
			u64 &new_value);
	
	
	bool incrementDocid(
			const u64 &iilid, 
			const OmnString &key,
			u64 &incValue);
	
	bool incrementDocid(                
			const OmnString &iilname, 
			const u64 &key,
			const u64 &incValue,
			const u64 &initValue,
			u64 &new_value);

	bool appendManualOrder(
			const OmnString &iilname, 
			const u64 &docid, 
			u64 &value);

	bool moveManualOrder(
			const OmnString &iilname, 
			const u64 &v1, 
			const u64 &d1, 
			const u64 &v2, 
			const u64 &d2);

	bool swapManualOrder(
			const OmnString &iilname, 
			const u64 &value1, 
			const u64 &docid1, 
			const u64 &value2, 
			const u64 &docid2);

	bool removeManualOrder(
			const OmnString &iilname, 
			const u64 &value, 
			const u64 &docid);

	bool 	addU64Doc1(const u64 &iilid,
				const u64 &value,
				const u64 &name,
				u64 &docid,
				const u64 &Max_num,
				bool &needKickout);

	bool	getDocidByValue(
			int &idx,
			int &iilidx,
			const bool reverse,
			const OmnString &iilname,
			const OmnString &value,
			u64 &docid);

	bool	getDocidByPrefix(
			int &idx,
			int &iilidx,
			const bool reverse,
			const OmnString &iilname,
			const OmnString &prefix,
			u64 &docid);

	bool	removeDocByPrefix(
			const OmnString &iilname,
			const OmnString &prefix);

	static void printHitStat();

	bool pickNextDoc(
			const OmnString &iilname,
			int &idx, 
			int &iilidx,
			const bool reverse, 
			OmnString &value, 
			u64 &docid);
	u64 getCachedDocid(
			const u64 &iilid, 
			const u64 &key, 
			bool &found);
	u64 getCachedDocid(
			const u64 &iilid, 
			const OmnString &key, 
			bool &found);
	bool addCachedDocid(
			const u64 &iilid,
			const u64 &value,
			const u64 &docid);
	bool addCachedDocid(
			const u64 &iilid,
			const OmnString &value,
			const u64 &docid);
};

#endif

