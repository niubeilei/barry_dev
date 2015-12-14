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
// 09/06/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_SengTesterMgr_h
#define AosSengTester_SengTesterMgr_h

#include "SengTorturer/Ptrs.h"
#include "SengTorUtil/StUtil.h"
#include "Thread/Ptrs.h"
#include "Thrift/AosJimoAPI.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/XmlTag.h"



class AosSengTesterMgr : virtual public OmnRCObject
{
private:
	OmnDefineRCObject;

	enum
	{
		eMaxThrds = 10000
	};

	OmnMutexPtr				mLock;
	AosTestFileMgrPtr		mFileMgr;	
	int						mNumThrds;
	AosSengTestThrdPtr *	mThrds;

	OmnString				mRootPasswd;
	OmnString				mRootCtnr;
	OmnString				mRootUsername;
	OmnString				mRootObjid;
	u64						mRootDocid;
	OmnString				mRootSsid;
	OmnString 				mRootCid;
	OmnString				mRootHomeCtnr;
	AosXmlTagPtr			mRootUserdoc;
	u64						mRootUrlDocid;

	int						mNumTries;
	u64						mSpecialDocid;

	vector<AosSengTesterPtr>	mTesters;
	u64						mSiteId;
	static JmoCallData		mCallData;
	OmnString				mAPIKey;
	OmnString				mAppID;
	OmnString				mNormUser;
	OmnString				mNormPasswd;
	static JmoRundata	    smRunData;

	static JmoRundata 		smJmoRundata;

public:
	AosSengTesterMgr(const int num_thrds, const int num_tries);
	~AosSengTesterMgr();

	bool					start();
	bool					startTest();
	AosSengTestThrdPtr 		getThread(int tid) const;
	const AosTestFileMgrPtr	getFileMgr() const;
	OmnString	 			getRootPasswd() const {return mRootPasswd;}
	OmnString				getRootCtnr() const {return mRootCtnr;}
	OmnString				getRootHomeCtnr() const {return mRootHomeCtnr;}
	OmnString				getRootUsername() const {return mRootUsername;}
	OmnString				getRootSsid() const {return mRootSsid;}
	u64						getRootDocid() const {return mRootDocid;}
	u64						getRootUrlDocid() const {return mRootUrlDocid;}
	OmnString				getRootObjid() const {return mRootObjid;}
	void					setRootDocid(const u64 &docid) {mRootDocid = docid;}
	void					setRootSsid(const OmnString &ssid); 
	OmnString  				getRootCid();
	AosXmlTagPtr			getRootUserDoc(){return mRootUserdoc;}	
	bool					createSuperUser();
	bool					createUser();
	bool					config(const AosXmlTagPtr &config);
	bool					loginAsRoot();
	bool					loginAsNormalUser(JmoCallData &call_data);
	bool					canCreateUnderLostFound(const OmnString &cid);
	AosXmlTagPtr 			getAccessRcd(const u32 siteid, const u64 &docid);
	AosXmlTagPtr 			retrieveXmlDoc(const u32 siteid, const OmnString &objid);
	u32 					getDftSiteid();
	u64						getSpecialDocid();
	bool 					addTester(const AosSengTesterPtr &tester);

	bool 	login(	JmoCallData &call_data, 
					const u64 site_id,
					const OmnString &container,
					const OmnString &user_name,
					const OmnString &password);

	bool 	login( const OmnString conf);
	
	bool 	checkAccess(
					const AosXmlTagPtr &doc,
					const OmnString &accesses, 
					const OmnString &cid);


	JmoCallData &getCallData() const {return smJmoRundata.call_data;}
	static OmnString getObjid(OmnString const name);
	JmoCallData &getCallData() {return smRunData.call_data;}
	JmoRundata &getRundata() {return smRunData;} };
#endif

