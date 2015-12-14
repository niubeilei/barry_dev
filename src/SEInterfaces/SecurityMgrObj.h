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
// 05/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SecurityMgrObj_h
#define Aos_SEInterfaces_SecurityMgrObj_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
//#include "TransUtil/ModuleId.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSecurityMgrObj : virtual public OmnRCObject
{
private:
	static AosSecurityMgrObjPtr	smSecurityMgr;

public:
	virtual bool checkAccess(
				const OmnString &opr_str,
				const AosXmlTagPtr &accessed_doc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkCreateDoc(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &container,
				const AosXmlTagPtr &doc) = 0;

	virtual bool checkCreateJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkRunJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkAbortJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkPauseJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkResumeJob(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkPauseTask(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkResumeTask(
				const AosXmlTagPtr &jobdoc,
				const AosRundataPtr &rdata) = 0;
	
	virtual bool checkIsLogin(const AosRundataPtr &rdata) = 0;
public:
	static void setSecurityMgr(const AosSecurityMgrObjPtr &d);
	static AosSecurityMgrObjPtr getSecurityMgr(); 

public:
	//Add by Yazong Ma 2015-1-16
	virtual bool isRoot(
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata) = 0;

	virtual bool isAdmin(
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata) = 0;

	virtual bool checkAddPubObjid(
				const OmnString &objid,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkAddMember1(
				const AosXmlTagPtr &pctnr,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkRead(
				const AosXmlTagPtr &doc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkRead(
				const AosXmlTagPtr &doc,
				OmnString &hpvpd,
				const AosXmlTagPtr &urldoc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkModify(
				const AosXmlTagPtr &mod_doc,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkDelete(
				const AosXmlTagPtr &del_doc,
				const AosRundataPtr &rdata) = 0;
	
	virtual bool checkMemberOfListing(
				const OmnString &ctnr_objid, 
				const OmnString &parent_ctnr_objid, 
				const AosRundataPtr &rdata) = 0;

	virtual bool checkCreateLog(
				const AosXmlTagPtr &ctnr_doc,
				const OmnString &logname,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkCreateUrl(
				const OmnString &url,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkLoadImage(
				const OmnString &container_obji,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkCreateUserAcct(
				const AosXmlTagPtr &container,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkReadAcd(
				const AosXmlTagPtr &ard,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkCreateAcd(
				const AosXmlTagPtr &accessed_doc,
				const AosRundataPtr &rdata) = 0;
	
	virtual bool checkUploadToPubFolder(
				const OmnString &folder,
				const int filesize,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkUploadFile(
				const int filesize,
				const OmnString &dirname,
				const AosRundataPtr &rdata) = 0;

	virtual bool checkCreateStatemachine(
				const AosXmlTagPtr &stmc, 
				const AosRundataPtr &rdata) = 0;
	
	virtual OmnString getDftReadAccType() const = 0;

	virtual	OmnString getDftDelMemAccType() const = 0; 

	virtual	OmnString getDftAddMemAccType() const = 0;

	virtual	OmnString getDftDeleteAccType() const = 0;

	virtual	OmnString getDftCopyAccType() const = 0;

	virtual	OmnString getDftCreateAccType() const = 0;

	virtual	OmnString getDftWriteAccType() const = 0;
};

#endif
