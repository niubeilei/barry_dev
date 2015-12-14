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
//	Refer to the description in the .cpp file.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Security_SecurityMgr_h
#define Omn_Security_SecurityMgr_h

//#include "Config/ConfigEntity.h"
#include "Debug/Rslt.h"
#include "EventMgr/EventHook.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/ObjidType.h"
#include "SEBase/SecOpr.h"
#include "Security/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"

OmnDefineSingletonClass(AosSecurityMgrSingleton, 
						AosSecurityMgr,
						AosSecurityMgrSelf,
						OmnSingletonObjId::eSecurityMgr, 
						"SecurityMgr");

class AosSecReq;

class AosSecurityMgr : virtual public AosSecurityMgrObj
{
	OmnDefineRCObject;

private:
	enum 
	{
		eMaxAccesses = 50,
		eMaxOperationChecks = 20,
		eDftPubDuration = 600,		// 10 minutes
		eMaxDirSize = 102400		//kB

	};

	bool			mFilterRtpStreams;
	OmnString		mMyPublicKey;

	bool			mDftReadPolicy;
	bool			mDftWritePolicy;
	bool			mDftCreatePolicy;
	bool			mDftDeletePolicy;
	bool			mDftCopyPolicy;
	bool			mDftListingPolicy;
	bool			mDftAddMemPolicy;
	bool			mDftDelMemPolicy;
	bool			mDftSvrCmdPolicy;
	bool			mDftSendMsgPolicy;
	bool			mDftNoAccessTagPolicy;

	int				mDftPubDuration;

	OmnString		mReadAccType;
	OmnString		mDelMemAccType;
	OmnString		mAddMemAccType;
	OmnString		mDeleteAccType;
	OmnString		mCopyAccType;
	OmnString		mCreateAccType;
	OmnString		mWriteAccType;
	AosObjMgrObjPtr	mObjMgr;
	static bool 	smSecurityOn;

public:
	AosSecurityMgr();
	~AosSecurityMgr();

	static AosSecurityMgr *	getSelf();
	virtual bool		start();
	virtual bool		stop();
	virtual bool		config(const AosXmlTagPtr &config);

	bool		verifyAuthentication(const OmnString &origChallenge,
					const OmnString &respChallenge,
					const OmnString &pid);
	bool		verifyAuthentication(const OmnString &origChallenge,
					const OmnString &respChallenge,
					const int entityId);
	int			getPermit();

	OmnString	getChallenge();
	OmnString	getChallengeResp(const OmnString &challenge);
	OmnString	getMyPublicKey() const {return mMyPublicKey;}

	bool	isRoot(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	static bool	isRequesterAdmin(const AosRundataPtr &rdata);
	bool	isAdmin(const AosXmlTagPtr &doc, const AosRundataPtr &rdata);
	OmnString getDftReadAccType() const {return mReadAccType;}
	OmnString getDftDelMemAccType() const {return mDelMemAccType;}
	OmnString getDftAddMemAccType() const {return mAddMemAccType;}
	OmnString getDftDeleteAccType() const {return mDeleteAccType;}
	OmnString getDftCopyAccType() const {return mCopyAccType;}
	OmnString getDftCreateAccType() const {return mCreateAccType;}
	OmnString getDftWriteAccType() const {return mWriteAccType;}

	AosObjMgrObjPtr		getObjMgr();

private:
	void	getDirSize(const char* path, int depth, int &size);				

public:
	bool 	checkMemberOfListing(
				const OmnString &ctnr_objid, 
				const OmnString &parent_ctnr_objid, 
				const AosRundataPtr &rdata);

	// Chen Ding, 12/28/2012
	virtual bool checkAccess(
				const AosRundataPtr &rdata,
				const OmnString &opr_str,
				const AosXmlTagPtr &container,
				const AosXmlTagPtr &accessed_doc);

	virtual bool checkAccess(
				const AosRundataPtr &rdata,
				const OmnString &opr_str,
				const AosXmlTagPtr &accessed_doc)
	{
		return checkAccess(rdata, opr_str, 0, accessed_doc);
	}

	inline bool checkAccess(
				const OmnString &opr_str, 
				const AosXmlTagPtr &accessed_doc, 
				const AosRundataPtr &rdata)
	{
		return checkAccess(rdata, opr_str, 0, accessed_doc);
	}

	bool checkCreateStatemachine(
				const AosXmlTagPtr &stmc, 
				const AosRundataPtr &rdata)
	{
		// return checkAccess(AosSecOpr::eCreateStatemachine, stmc, rdata);
		return checkAccess(AOSSOPR_CREATE_STMC, stmc, rdata);
	}

	static bool isUserInDomains(const OmnString &domains, const AosRundataPtr &rdata);

	virtual bool checkIsLogin(const AosRundataPtr &rdata);
private:
	bool checkUserDefinedOperations(
				AosSecReq &sec_req,
				bool &rollback,
				const AosRundataPtr &rdata);

	bool checkUserDomainOprArd1(
				const OmnString &operation,
				const AosXmlTagPtr &opr_ard, 
				bool &access_denied,
				bool &granted,
				bool &rollback,
				AosSecReq &sec_req,
				const AosRundataPtr &rdata);

	bool checkUserOprArd(
				const OmnString &operation,
				const AosXmlTagPtr &opr_ard, 
				bool &access_denied,
				bool &granted,
				const AosRundataPtr &rdata);

	bool checkReservedObjid(
				const OmnString &objid, 
				const AosXmlTagPtr &doc,
				const AosObjidType::E objid_type, 
				const AosRundataPtr &rdata);

	bool checkAccessPriv(
				const OmnString &opr_str,
				AosSecReq &request,
				const AosRundataPtr &rdata); 

	bool checkAccessPriv(
				AosSecReq &sec_req,
				const AosXmlTagPtr &accesses_tag,
				const AosRundataPtr &rdata);

	bool checkRegisterHook(
				const AosEventHook hook,
				const OmnString &key,
				const OmnString &sdoc_objid,
				const AosRundataPtr &rdata);

public:
	bool	checkUploadToPubFolder(
				const OmnString &folder,
				const int filesize,
				const AosRundataPtr &rdata);

	bool	checkAddObjid(
				const OmnString &target_cid,
				const AosRundataPtr &rdata);

	bool 	checkCreateDoc(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &ctnr,
				const AosXmlTagPtr &doc_to_create);

	bool 	checkModify(
				const AosXmlTagPtr &mod_doc,
				const AosRundataPtr &rdata);

	bool 	checkDelete(
				const AosXmlTagPtr &del_doc,
				const AosRundataPtr &rdata);

	bool 	checkRead(
				const AosXmlTagPtr &doc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_READ, doc, rdata);
	}

	bool 	checkRead(
				const AosXmlTagPtr &doc,
				OmnString &hpvpd,
				const AosXmlTagPtr &urldoc,
				const AosRundataPtr &rdata);

	bool 	checkAddMember1(
				const AosXmlTagPtr &pctnr,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_ADDMEM, pctnr, rdata);
	}

	bool 	checkCreateUserAcct(
				const AosXmlTagPtr &container,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_CREATEACCT, container, rdata);
	}

	bool 	checkCreateUrl(
				const OmnString &url,
				const AosRundataPtr &rdata);

	bool 	checkAddPubObjid(
				const OmnString &objid,
				const AosRundataPtr &rdata);

	bool	checkReadAcd(
				const AosXmlTagPtr &ard,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_READACD, ard, rdata);
	}

	bool	checkCreateAcd(
				const AosXmlTagPtr &accessed_doc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_CREATEACD, accessed_doc, rdata);
	}

	bool 	checkLoadImage(
				const OmnString &container_obji,
				const AosRundataPtr &rdata);
	
	bool	checkUploadFile(
				const int filesize,
				const OmnString &dirname,
				const AosRundataPtr &rdata);

	bool	checkCreateLog(
				const AosXmlTagPtr &ctnr_doc,
				const OmnString &logname,
				const AosRundataPtr &rdata);

	virtual bool checkCreateJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(rdata, AOSSOPR_CREATE_JOB, jobdoc);
	}

	virtual bool checkRunJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(rdata, AOSSOPR_RUN_JOB, jobdoc);
	}

	virtual bool checkAbortJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_ABORT_JOB, jobdoc, rdata);
	}

	virtual bool checkPauseJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_PAUSE_JOB, jobdoc, rdata);
	}

	virtual bool checkResumeJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_RESUME_JOB, jobdoc, rdata);
	}

	virtual bool checkPauseTask(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(AOSSOPR_PAUSE_TASK, jobdoc, rdata);
	}

	virtual bool checkResumeTask(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata)
	{
		return checkAccess(rdata, AOSSOPR_RESUME_TASK, 0, jobdoc);
	}

private:
	bool logRootAccess(
				const AosRundataPtr &rdata, 
				const OmnString &opr_str, 
				AosSecReq &sec_req, 
				const AosXmlTagPtr &container, 
				const AosXmlTagPtr &accessed_doc);

	bool logInternalCall(
				const AosRundataPtr &rdata, 
				const OmnString &opr_str, 
				AosSecReq &sec_req, 
				const AosXmlTagPtr &container, 
				const AosXmlTagPtr &accessed_doc);

	// Chen Ding, 2013/12/09
	AosAccessRcdPtr getSystemDftAcd(const AosRundataPtr &rdata);
};
#endif
