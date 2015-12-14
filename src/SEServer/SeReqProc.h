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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SESrver_SeReqProc_h
#define Aos_SESrver_SeReqProc_h

#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEServer/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Security/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/MetaExtension.h"
#include "Thread/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "Util/UtUtil.h"
#include "XmlUtil/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlInterface/Server/Ptrs.h"

#include <vector> 
#include <map> 
using namespace std;



class AosSeReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxPagesize = 100,
		AOS_ATTR_FLAG = '1',
		AOS_CDATA_FLAG = '2',
		AOS_TEXT_FLAG = '3',
		eMaxRequesters = 1000,
		eTimeoutSec = 600,
		sgMaxAccounts = 500,
		eMaxArgs = 8,
		eMaxFields = 50,
		eMaxConns = 100,

		eHealthCheckFreq = 5,
		eHealthCheckMaxAges = 200,
		eHealthCheckMaxEntries = 800000
	};

private:
	u64					mUserid;
	OmnString			mAppid;
	u32					mSiteid;
	bool				mIsAscending;
	int					mDftPagesize;
	int					mNumProcessed;
	OmnString       	mReqNames[eMaxRequesters];
	int             	mNumReqs;
	AosWebRequestPtr    mConns[eMaxRequesters];
	int         		mTime[eMaxRequesters];
	OmnMutexPtr			mLock;
	OmnString			mOperation;
	AosSessionObjPtr	mSession;
	OmnString			mSsid;
	OmnString 			mRObjid;
	OmnString			mTransId;
	OmnString			mClientSsid;
	map<OmnString, OmnString> mRecvCookie;
	AosRundataPtr		mRundata;
	u64					mUrldocDocid;
	bool				mNeedLoginObj;

	static AosImgProcPtr	mImgProc;
	static bool				mIsStopping;
	static OmnString		mDomainAddr;
	static vector<OmnString>	mMsgSvrUrl;

	u64					mReqid;			// Chen Ding, 2013/03/06
	bool				mFinished;		// Chen Ding, 2013/03/06
	OmnConnBuffPtr		mConnBuff;		// Chen Ding, 2013/03/07
	AosWebRequestPtr	mWebRequest;	// Chen Ding, 2013/03/07

public:
	AosSeReqProc();
	AosSeReqProc(const OmnConnBuffPtr &req);		// Chen Ding, 2013/03/07
	~AosSeReqProc();

	static bool		config(const AosXmlTagPtr &config);
	static bool		stop();
	virtual bool	procRequest(const OmnConnBuffPtr &req);
	virtual AosNetReqProcPtr	clone();

	bool 	procReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);

	void 	sendResp(const AosWebRequestPtr &req, 
	 			const OmnString &errmsg, 
	 			const OmnString &contents,
				const OmnString &jql_msg); 

	static bool retrieveObjPublic(
			const AosWebRequestPtr &req, 
			const AosXmlTagPtr &root, 
			const AosXmlTagPtr &docRetrieved, 
			const bool isVersionObj, 
			const AosXmlTagPtr &obj,
			const AosXmlTagPtr &urldoc,
			const AosRundataPtr &rdata); 

	static bool postReadProc(
			const AosWebRequestPtr &req, 
			const AosXmlTagPtr &doc, 
			bool &resp_sent, 
			const AosRundataPtr &rdata);
	void setNeedLoginObj(const bool b) {mNeedLoginObj = b;}

private:
	bool 	procServerCmdReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);
	bool 	deleteObjReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);
	bool 	deleteObjReqs(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);
	
	bool 	deleteObj(
				const AosWebRequestPtr &req, 
				const OmnString &docid, 
				const OmnString &dname);

	//Zky2782 Ketty 2011/02/14
	bool	rebuildDelObjReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);
	
	bool 	retrieveObjReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);
	bool 	modifyObjReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &child, 
				const AosXmlTagPtr &root); 
	bool 	getRecords(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);
	bool 	getRecordsByCmd(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &childelem); 
	bool 	createContainer(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &childelem);
	bool 	sendObject(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &xml);
	bool 	procSql(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root,
				const AosXmlTagPtr &child);
	bool 	sendmsg(
	 			const AosWebRequestPtr &req, 
	 			const AosXmlTagPtr &root);
	bool 	regServer(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);
	bool 	procServerReq(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);
	bool 	notifyUsers(const OmnString &username);
	
	
	bool 	userlogout(
				const AosWebRequestPtr &req, 
				const AosXmlTagPtr &root);

	AosSessionPtr getSession();

	bool
	retrieveVersionObj(const AosRundataPtr &rdata);

	bool isValidVoteType(const OmnString &type);
	

	bool pullMsg(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root);

	bool sendMsg(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root);

	bool addMember(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root);

	bool removeMember(
		const AosWebRequestPtr &req, 
		const AosXmlTagPtr &root);

	bool alipay(
			const AosWebRequestPtr &req, 
			const AosXmlTagPtr &root, 
			const AosXmlTagPtr &rootchild, 
			const AosXmlTagPtr &cmd); 

private:
	AosRundataPtr
	getRundata(const AosWebRequestPtr &req, const AosXmlTagPtr &doc);
	void sendResp(const AosRundataPtr &rdata);

public:
	u32 getSiteid() const {return mSiteid;}
	static bool getIsStopping() {return mIsStopping;}
	static OmnString getDomainAddr() {return mDomainAddr;}
	static bool prepareSaveas(const AosXmlTagPtr &xml);
	static AosImgProcPtr getImgProc() {return mImgProc;}
	static bool getMsgSvrUrl(vector<OmnString> &url) {url = mMsgSvrUrl; return true;}
	
	// Chen Ding, 2013/03/06
	void setRequid(const u64 &r) {mReqid = r;}
	bool isFinished() const {return mFinished;}
	void sendResp();
	bool procRequest();
};

#endif
