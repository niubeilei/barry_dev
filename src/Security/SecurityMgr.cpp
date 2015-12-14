////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecurityMgr.cpp
// Description:
//	This is a singleton class. It is used to provide security
//  management to the entire system. 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Security/SecurityMgr.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/ValueRslt.h"
#include "Debug/Error.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/SeErrors.h"
#include "SEUtil/SeConfigMgr.h"
#include "Security/ScrtUtil.h"
#include "Security/Operation.h"
#include "Security/Session.h"
#include "Security/SessionMgr.h"
#include "Security/SimpleAuth.h"
#include "SEBase/SecReq.h"
#include "Accesses/Access.h"
#include "Accesses/AccessType.h"
#include "Accesses/AllAccesses.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Statemachine/Statemachine.h"
#include "Thread/Mutex.h"
#include "Util/StrParser.h"
#include "Util1/Time.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/AccessRcd.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h>
#include <math.h>


const OmnString sgDftReadAType      = AOSACTP_PUBLIC;
const OmnString sgDftDelMemAType    = AOSACTP_PRIVATE;
const OmnString sgDftAddMemAType    = AOSACTP_PRIVATE;
const OmnString sgDftDeleteAType    = AOSACTP_PRIVATE;
const OmnString sgDftCopyAType      = AOSACTP_PRIVATE;
const OmnString sgDftCreateAType    = AOSACTP_PRIVATE;
const OmnString sgDftWriteAType     = AOSACTP_PRIVATE;
const OmnString sgDftNoAccessTag	= AOSACTP_PUBLIC;

bool AosSecurityMgr::smSecurityOn = false;

OmnSingletonImpl(AosSecurityMgrSingleton, 
				 AosSecurityMgr, 
				 AosSecurityMgrSelf,
				"AosSecurityMgr");

//
// Temporarily, permit is an integer. It increments one every
// time one requests it.
//
static int sgPermit = 100;
static OmnMutex sgPermitLock;
static OmnMutex sgLock;

// Moved to AosSecReq.cpp
// static AosXmlTagPtr sgRoidAccesses[AosObjidType::eMax];	// Defines the accesses
// static OmnString	sgRoidOtypes[AosObjidType::eMax]; 	// Defines the otypes binding
// static OmnString	sgRoidCtnrs[AosObjidType::eMax];  	// Defines parent containers

AosSecurityMgr::AosSecurityMgr()
:
mFilterRtpStreams(false),
mDftReadPolicy(true),
mDftWritePolicy(true),
mDftCreatePolicy(true),
mDftDeletePolicy(true),
mDftCopyPolicy(true),
mDftListingPolicy(true),
mDftAddMemPolicy(true),
mDftDelMemPolicy(true),
mDftSvrCmdPolicy(true),
mDftSendMsgPolicy(true),
mDftNoAccessTagPolicy(true),
mDftPubDuration(eDftPubDuration)
{
	AosSecurityMgrObjPtr thisptr(this, false);
	AosSecurityMgrObj::setSecurityMgr(thisptr);
}


AosSecurityMgr::~AosSecurityMgr()
{
}


bool
AosSecurityMgr::start()
{
	return true;
}


bool
AosSecurityMgr::stop()
{
	return true;
}


bool
AosSecurityMgr::config(const AosXmlTagPtr &conf)
{
	//
	// Call this function to configure this class. 
	// It assumes the following:
	//
	//	<config ...>
	//		<SecurityMgr>
	//		</SecurityMgr>
	//		...
	//	</config>
	AosInitSecurityAccesses();

	aos_assert_r(conf, false);
    AosXmlTagPtr sec_config = conf->getFirstChild(AOSCONFIG_SECURITYMGR);
	if (!sec_config) 
	{
		OmnAlarm << "Missing security configuration tag: <security>" << enderr;
		exit(-1);
	}
	
	smSecurityOn = sec_config->getAttrBool("security_on", true);
	AosXmlTagPtr def = sec_config->getFirstChild(AOSCONFIG_DFT_ARCD);
	if (!def)
	{
		OmnAlarm << "Missing the default access control tag" << enderr;
		exit(-1);
	}

	mReadAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_READ, sgDftReadAType);
	mDelMemAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_DELMEM, sgDftDelMemAType);
	mAddMemAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_ADDMEM, sgDftAddMemAType);
	mDeleteAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_DELETE, sgDftDeleteAType);
	mCopyAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_COPY, sgDftCopyAType);
	mCreateAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_CREATE, sgDftCreateAType);
	mWriteAccType = def->getAttrStr(AOSCONFIG_ACCTYPE_WRITE, sgDftWriteAType);

	mDftReadPolicy 			= def->getAttrBool("dft_read_policy", true);
	mDftWritePolicy 		= def->getAttrBool("dft_write_policy", true);
	mDftCreatePolicy 		= def->getAttrBool("dft_create_policy", true);
	mDftDeletePolicy 		= def->getAttrBool("dft_delete_policy", true);
	mDftCopyPolicy 			= def->getAttrBool("dft_copy_policy", true);
	mDftListingPolicy 		= def->getAttrBool("dft_listing_policy", true);
	mDftAddMemPolicy 		= def->getAttrBool("dft_add_mem_policy", true);
	mDftDelMemPolicy 		= def->getAttrBool("dft_del_mem_policy", true);
	mDftSvrCmdPolicy 		= def->getAttrBool("dft_svr_cmd_policy", true);
	mDftSendMsgPolicy 		= def->getAttrBool("dft_send_msg_policy", true);
	mDftNoAccessTagPolicy 	= def->getAttrBool("dft_no_access_tag_policy", true);

	mDftPubDuration = sec_config->getAttrInt(AOSCONFIG_DFTPUBDURATION, eDftPubDuration);

	// Set the defalt accesses
	// for (int i=AosSecOpr::eRead; i<AosSecOpr::eMax; i++)
	// {
	// 	OmnString access = sec_config->getAttrStr(AosSecOpr::toString((AosSecOpr::E)i));
	// 	if (access != "")
	// 	{
	// 		AosSecOpr::setDefaultAccess((AosSecOpr::E)i, access);
	// 	}
	// }

	// Chen Ding, 12/28/2012. The following is moved to AosSecReq.cpp
	// Configure the Reserved Objid Access Controls. Some objids are reserved. 
	// Using these objids are security checked. For instance, Domain OprArd objids
	// can be used by System Administrators only. Most reserved objids are 
	// tied with special otype, and possibly tied with parent containers.
	// AosXmlTagPtr rsved_objid_def = sec_config->getFirstChild("reserved_objids");
	// if (rsved_objid_def)
	// {
	// 	AosXmlTagPtr record = rsved_objid_def->getFirstChild();
	// 	while (record)
	// 	{
	// 		AosObjidType::E type = AosObjidType::toEnum(
	// 				record->getAttrStr("objid_type"));
	// 		if (!AosObjidType::isValid(type))
	// 		{
	// 			OmnAlarm << "Objid type invalid: " << record->toString() << enderr;
	// 		}
	// 		else
	// 		{
	// 			// The record is in the format:
	// 			// 	<record AOSTAG_OTYPE="xxx"
	// 			// 		AOSTAG_PARENT_CONTAINER="xxx">
	// 			// 		<access .../>
	// 			// 	</record>
	// 			AosXmlTagPtr access_tag = record->getFirstChild();
	// 			if (access_tag)
	// 			{
	// 				sgRoidAccesses[type] = access_tag->clone(AosMemoryCheckerArgsBegin);
	// 			}
	// 			sgRoidOtypes[type] = record->getAttrStr(AOSTAG_OTYPE);
	// 			sgRoidCtnrs[type] = record->getAttrStr(AOSTAG_PARENT_CONTAINER);
	// 		}
	// 		record = rsved_objid_def->getNextChild();
	// 	}
	// }
	bool rslt = AosSecReq::config(sec_config);
	aos_assert_r(rslt, false);

	rslt = AosAccess::config(sec_config);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSecurityMgr::checkAccess(
		const AosRundataPtr &rdata,
		const OmnString &opr_str,
		const AosXmlTagPtr &container,
		const AosXmlTagPtr &accessed_doc)
{
	if (!smSecurityOn) return true;

	if (!accessed_doc)
	{
		AosSetErrorUser(rdata, "missing_accessed_doc") << enderr;
		return false;
	}

    AosSecReq &sec_req = rdata->getSecReq();
	sec_req.setData(opr_str, container, accessed_doc, rdata);

	// Check whether it is a root access.
	if (sec_req.isRoot(rdata))
	{
		logRootAccess(rdata, opr_str, sec_req, container, accessed_doc);
		rdata->setOk();
		return true;
	}

	// Check whether it is an internal call
	if (rdata->isInternalCall())
	{
		// It is an internal call. 
		logInternalCall(rdata, opr_str, sec_req, container, accessed_doc);
		rdata->setOk();
		return true;	
	}
    return checkAccessPriv(opr_str, sec_req, rdata);
}


bool
AosSecurityMgr::checkAccessPriv(
		const OmnString &oprstr,
		AosSecReq &sec_req, 
		const AosRundataPtr &rdata) 
{
	// This function checks whether the access (defined in 'sec_req')
	// is allowed. 
	// 1. If it is root, grant it.
	// 2. The access is controlled by its access record:
	// 	<access_record ...>
	// 		<op
	// 	</access_record>
	if (oprstr == "")
	{
		AosSetErrorU(rdata, "internal_error") << ": " << oprstr
			<< ". Access not defined properly: " << enderr;
		return false;
	}

	rdata->setAccessOperation(oprstr);
	if (sec_req.isRoot(rdata))
	{
		rdata->setOk();
		return true;
	}

	// if (sec_req.isAdminPublic(rdata))
	// {
	// 	// For most accesses, administrators may by-pass the access controls.
	// 	// But for some operations, only root may by-pass the acceses.
	// 	//
	// 	// Chen Ding, 01/27/2012
	// 	// if (only_root_can_bypass)
	// 	// {
	// 	// 	// It means only the root can bypass the access control.
	// 	// 	// Need to check whether it is the root.
	// 	// 	if (sec_req.isRoot(rdata))
	// 	// 	{
	// 	// 		rdata->setOk();
	// 	// 		AOSLOG_LEAVE(rdata);
	// 	// 		return true;
	// 	// 	}
	// 	// }
	// 	// else
	// 	// {
	// 		// It means admin can by-pass the accesses. 
	// 		// Chen Ding, 01/27/2012
	// 		// In the current implementations, admin can bypass allt he 
	// 		// access controls.
	// 		rdata->setOk();
	// 		return true;
	// 	// }
	// }

	// Chen Ding, 12/24/2011
	// Check accesses by operations
	bool continue_access_check = true;
	bool statemachines_started = false;
	if (checkUserDefinedOperations(sec_req, continue_access_check, rdata))
	{
		// Start statemachines as needed.
		AosStatemachine::startStatemachines(rdata);
		AosStatemachine::lookupStatemachines(rdata);
		statemachines_started = true;

		// The access is granted. The access is translated into an operation, 
		// which is in 'opr_id'. It will check whether there are statemachines
		// associated with the operation. If yes, the statemachines may want
		// to reject the operation.
		aos_assert_rr(rdata->getUserOperation() != "", rdata, false);
		bool denied = false;
		bool rslt = AosStatemachine::checkOperation(denied, rdata);
		if (!rslt || denied)
		{
			// The operation is rejected by one of the statemachines. 
			AOSLOG_LEAVE(rdata);
			return false;
		}

		// The operation is granted by statemachines, if any.
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}

	// In the security model, there are two types of Access Operations. One is System
	// Defined Access Operations and the other User Defined Access Operations. 
	// User defined access operations are checked first. 
	// This is done by the above function call 'checkUserDefinedOperations(...)'.
	// If that function grants the access, it will not come to this point. 
	// When User Defined Access Operations cannot grant the access, it has
	// two options: either reject the access ('continue_access_check' set to false), 
	// or the access should be checked using the System Defined Access Checking
	// ('continue_access_check' set to true).
	if (!continue_access_check)
	{
		// Access denied by operations
		return false;
	}

	// Chen Ding, 2013/05/31
	if (!statemachines_started)
	{
		// 	// Start statemachines as needed.
		// 	AosStatemachine::startStatemachines(rdata);
		// 	AosStatemachine::lookupStatemachines(rdata);
		// 
		//	// Check the accesses through statemachines, if any
		//	bool denied = false;
		//	bool rslt = AosStatemachine::checkOperation(denied, rdata);
		//	if (!rslt || denied)
		//	{
		//		// The operation is rejected by one of the statemachines. 
		//		return false;
		//  }
	}

	AosXmlTagPtr accesses_tag = sec_req.getAccesses(oprstr, rdata);
	if (!accesses_tag)
	{
		// No access tag is found. 
		if (mDftNoAccessTagPolicy) return true;
		
		AosSetError(rdata, "access_denied");
		return false;
	}

	bool rslt = checkAccessPriv(sec_req, accesses_tag, rdata);
	if (!rslt)
	{
		// Access is denied.
		return false;
	}

	// Granted
	return true;
}


bool 
AosSecurityMgr::checkUserDefinedOperations(
		AosSecReq &sec_req,
		bool &continue_access_check,
		const AosRundataPtr &rdata)
{
	// This function checks accesses by operations.
	rdata->setUserOperation("");
	continue_access_check = true;

	// Check whether it can be translated into an operation
	OmnString opr_id = AosOperation::determineOperation(sec_req, rdata);
	if (opr_id == "") return false;
	rdata->setUserOperation(opr_id);

	// It is an operation. Need to retrieve the user domain operation
	// access record.
	AosXmlTagPtr opr_ard = sec_req.getAccessedDomainOprArd(rdata);
	bool granted, denied;
	if (opr_ard)
	{
		checkUserDomainOprArd1(opr_id, opr_ard, granted, denied, 
				continue_access_check, sec_req, rdata);
		if (granted)
		{
			rdata->setOk();
			return true;
		}

		if (denied) 
		{
			return false;
		}
	}

	// Try user operation access record
	opr_ard = sec_req.getRequesterOprArd(rdata);	
	if (!opr_ard)
	{
		continue_access_check = true;
		return true;
	}

	checkUserOprArd(opr_id, opr_ard, denied, granted, rdata);
	if (granted)
	{
		rdata->setOk();
		return true;
	}

	if (denied) 
	{
		continue_access_check = false;
		return false;
	}

	continue_access_check = true;
	return true;
}


bool
AosSecurityMgr::checkUserDomainOprArd1(
		const OmnString &opr_id,
		const AosXmlTagPtr &opr_ard, 
		bool &granted,
		bool &denied,
		bool &rollback,
		AosSecReq &sec_req,
		const AosRundataPtr &rdata)
{
	// The record is in the form:
	// 	<opr_ard ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<oprcode AOSTAG_OVERRIDE="true|false">
	// 				<access type="xxx">xxx</access>
	// 				<access type="xxx">xxx</access>
	// 				...
	// 			</oprcode>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</opr_ard>
	denied = false;
	granted = false;
	AosXmlTagPtr records = opr_ard->getFirstChild(AOSTAG_OPERATIONS);
	if (!records) return true;
	AosXmlTagPtr opr_rcd = records->getFirstChild(opr_id);
	if (!opr_rcd) 
	{
		// The corresponding tag is not found.
		return true;
	}

	// Found the corresponding record, which is in the form:
	// If 'override' is false, it means it should not try the personal 
	// operation access record. By default, 'override' is true. 
	AosXmlTagPtr access_tag = opr_rcd->getFirstChild();
	int guard = eMaxOperationChecks;
	while (guard-- && access_tag)
	{
		// 'access_tag' is an operation based access record. 
		// In the current implementations, the following are supported:
		// 	<access type="role">role,role,...</access>
		// 	<access type="group">group,group,...</access>
		// 	<access type="user">user,user,...</access>
		AosAccess::checkAccessStatic(rdata, access_tag, sec_req, granted, denied);
		//if (granted || denied) return true;
		if (granted && !denied) return true;
		access_tag = opr_rcd->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetError(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		denied = true;
		return true;
	}

	// Did not pass the user domain operation access record. 
	// Check whether it should check the personal operation access record.
	if (!opr_rcd->getAttrBool(AOSTAG_OVERRIDE, true))
	{
		AosSetError(rdata, AosErrmsgId::eAccessDenied);
		AOSLOG_LEAVE(rdata);
		denied = true;
		rollback = true;
		return true;
	}

	// It should roll back to the normal access controls
	rollback = false;
	denied = false;
	granted = false;
	return true;
}


bool
AosSecurityMgr::checkUserOprArd(
		const OmnString &opr_id,
		const AosXmlTagPtr &opr_ard, 
		bool &access_denied,
		bool &granted,
		const AosRundataPtr &rdata)
{
	// The record is in the form:
	// 	<opr_ard ...>
	// 		<AOSTAG_OPERATIONS>
	// 			<opr_id AOSTAG_ACCESS=AOSVALUE_GRANT|AOSVALUE_DENY/>
	// 			<opr_id AOSTAG_ACCESS="xxx"/>
	// 			...
	// 		</AOSTAG_OPERATIONS>
	// 	</opr_ard>
	access_denied = false;
	granted = false;
	aos_assert_r(opr_ard, false);
	AosXmlTagPtr records = opr_ard->getFirstChild(AOSTAG_OPERATIONS);
	if (!records) return true;
	AosXmlTagPtr opr_rcd = records->getFirstChild(opr_id);
	if (!opr_rcd) return true;
	OmnString access = opr_rcd->getAttrStr(AOSTAG_ACCESS);
	if (access == AOSVALUE_GRANT)
	{
		granted = true;
		return true;
	}

	if (access == AOSVALUE_DENY)
	{
		granted = false;
		access_denied = true;
		return true;
	}

	access_denied = false;
	granted = false;
	return true;
}


OmnString
AosSecurityMgr::getChallenge()
{
	return "";
}


bool
AosSecurityMgr::verifyAuthentication(
		const OmnString &origChallenge,
        const OmnString &respChallenge,
        const OmnString &pid)
{
	return true;
}


bool
AosSecurityMgr::verifyAuthentication(
		const OmnString &origChallenge,
        const OmnString &respChallenge,
        const int entitiId)
{
	return true;
}


OmnString   
AosSecurityMgr::getChallengeResp(const OmnString &challenge)
{
	OmnNotImplementedYet;
	return OmnString(challenge) << ":OKeyed";
}


int
AosSecurityMgr::getPermit()
{
	sgPermitLock.lock();
	int permit = sgPermit++;
	sgPermitLock.unlock();
	return permit;
}


bool
AosSecurityMgr::checkAccessPriv(
		AosSecReq &sec_req,
		const AosXmlTagPtr &accesses_tag,
		const AosRundataPtr &rdata)
{
	// The caller has already determined the operation (such as 'reading', 
	// 'writing', etc.)
	// This function determines whether the request should be granted
	// or denied based on the tag 'accesses_tag', which is in the form:
	// 	<opr_id ...>
	// 		<access AOSTAG_DENY_ACCESS="true|false" .../>
	// 		<access AOSTAG_DENY_ACCESS="true|false" .../>
	// 		...
	// 	</opr_id>
	//
	// All <access> are considered ORed unless the attribute AOSTAG_DENY_ACCESS
	// is 'true', which explicitly denies the access.
	aos_assert_rr(accesses_tag, rdata, false);

	AosXmlTagPtr access = accesses_tag->getFirstChild();
	int guard = eMaxAccesses;
	while (guard-- && access)
	{
		bool granted = false;
		bool denied = false;
		if (AosAccess::checkAccessStatic(rdata, access, sec_req, granted, denied))
		{
			if (granted) return true;
			if (denied) return false;
		}
		access = accesses_tag->getNextChild();
	}

	if (guard <= 0)
	{
		AosSetError(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// If it comes to this place, it should be denied.
	AosSetError(rdata, "access_denied");
	return false;
}


bool	
AosSecurityMgr::checkUploadToPubFolder(
		const OmnString &folder,
		const int filesize,
		const AosRundataPtr &rdata)
{
	// This function checks whether the user has the right to add 
	// a file of 'filesize' into the public folder 'folder'.
	OmnNotImplementedYet;
	return true;
}


/////////////////////////////////////////////////////////////////
// Below are the check functions
/////////////////////////////////////////////////////////////////
bool 
AosSecurityMgr::checkCreateDoc(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &ctnr,
		const AosXmlTagPtr &doc_to_create)
{
	// This function checks whether the user has the right to 
	// create a doc into its containers. 
	//
	// If container is empty, it is an error. The request will be 
	// rejected. 
	
	if (!ctnr)
	{
		AosSetError(rdata, "securitymgr_missing_container") << enderr;
		return false;
	}

	if (!doc_to_create)
	{
		AosSetError(rdata, "securitymgr_missing_doc") << enderr;
		return false;
	}

	OmnString objid = doc_to_create->getAttrStr(AOSTAG_OBJID);
	if (objid != "")
	{
		// Check whether it is a valid objid.
		AosObjidType::E objid_type;
		if (!AosObjid::checkObjid(objid, objid_type, rdata))
		{
			return false;
		}

		if (AosObjidType::isReservedObjid(objid_type))
		{
			// It is reserved objid. Check whether it is allowed.
			if (!checkReservedObjid(objid, doc_to_create, objid_type, rdata))
			{
				return false;
			}
		}
	}

	if (doc_to_create->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		return checkAccess(rdata, AOSSOPR_CREATEACCT, ctnr, doc_to_create);
	}

	return checkAccess(rdata, AOSSOPR_CREATEDOC, ctnr, doc_to_create);
}


bool
AosSecurityMgr::checkReservedObjid(
		const OmnString &objid, 
		const AosXmlTagPtr &doc,
		const AosObjidType::E objid_type, 
		const AosRundataPtr &rdata)
{
	// This function assumes the objid is reserved. It checks whether
	// it is allowed.
	//
	// In addition to the normal access control checking, reserved objids 
	// may subject to the following checking:
	// 	1. Some reserved objids tie with specific otypes.
	// 	2. Some reserved objids tie with specific parent containes.
	// 
	// Reserved objids are defined by the following:
	// 	sgRoidAccesses[]:	Defines the accesses
	// 	sgRoidOtypes[]: 	Defines the otype binding
	// 	sgRoidCtnrs[]:  	Defines the parent container binding
	//
	// Chen Ding, 12/28/2012
	// The following is moved to AosSecReq.cpp
	// if (!AosObjidType::isValid(objid_type))
	// {
	// 	// Should never happen
	// 	AosSetError(rdata, "internal_error");
	// 	OmnAlarm << rdata->getErrmsg() << enderr;
	// 	return false;
	// }
	// 
	// aos_assert_rr(doc, rdata, false);
	// if (sgRoidOtypes[objid_type] != "")
	// {
	// 	if (doc->getAttrStr(AOSTAG_OTYPE) != sgRoidOtypes[objid_type])
	// 	{
	// 		AosSetError(rdata, "otype_mismatch_001") << ": " 
	// 			<< doc->getAttrStr(AOSTAG_OTYPE) << ":" 
	// 			<< sgRoidOtypes[objid_type];
	// 		return false;
	// 	}
	// }
	// 
	// if (sgRoidCtnrs[objid_type] != "")
	// {
	// 	if (doc->getAttrStr(AOSTAG_PARENTC) != sgRoidCtnrs[objid_type])
	// 	{
	// 		AosSetError(rdata, "ctnr_mismatch_001")
	// 			<< doc->getAttrStr(AOSTAG_PARENTC) << ":" 
	// 			<< sgRoidCtnrs[objid_type];
	// 		return false;
	// 	}
	// }

	AosSecReq &sec_req = rdata->getSecReq();
	if (!sec_req.checkReservedObjid(objid, doc, objid_type, rdata))
	{
		return false;
	}

	return checkAccess(AOSSOPR_RESERVED_OBJID, doc, rdata);
}


bool 
AosSecurityMgr::checkModify(
		const AosXmlTagPtr &mod_doc,
		const AosRundataPtr &rdata)
{
	// This function checks whether the user ('userid') has the right to 
	// modify the doc 'mod_doc'
	aos_assert_r(mod_doc, false);

	// Check whether it is modifying user account. 
	if (mod_doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		return checkAccess(AOSSOPR_MODIFYACCT, mod_doc, rdata);
	}
	
	return checkAccess(AOSSOPR_MODIFYDOC, mod_doc, rdata);
}


bool 
AosSecurityMgr::checkDelete(
		const AosXmlTagPtr &del_doc,
		const AosRundataPtr &rdata)
{
	// This function checks whether the user ('userid') has the right to 
	// delete the doc 'mod_doc'

	// Need to check whether it is to delete a user account
	if (del_doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT)
	{
		return checkAccess(AOSSOPR_DELETEACCT, del_doc, rdata);
	}
	return checkAccess(AOSSOPR_DELETEDOC, del_doc, rdata);
}


bool 	
AosSecurityMgr::checkRead(
		const AosXmlTagPtr &accessedDoc,
		OmnString &hpvpd,
		const AosXmlTagPtr &urldoc,
		const AosRundataPtr &rdata)
{
	// This function checks whether the user has the right to 
	// read the doc that is identified by either 'docid' or 'oid'.
	// It will also check the logins. The doc to be read is
	// identified by 'docid'. 
	AosSessionObjPtr session_doc = rdata->getSession();

	if (!session_doc && urldoc)
	{
		u64 did = urldoc->getAttrU64(AOSTAG_DOCID, 0);
		if (did != 0)
		{
			OmnString cookie_name = AosComposeSsidCookieName(did);
			OmnString ssid = rdata->getCookie(cookie_name);
			if (ssid != "")
			{
				session_doc = AosSessionMgr::getSelf()->getSession1(ssid, hpvpd, rdata);
				if (session_doc)
				{
					rdata->setSession(session_doc);
				}
			}
		}
	}

	if (!accessedDoc)
	{
		AosSetError(rdata, "securitymgr_missing_accessed_doc") << enderr;
		return false;
	}

	// If there is a session, check whether it is a login session. 
	OmnString requester_cid;
	if (session_doc)
	{
		requester_cid = session_doc->getCid();
		if (session_doc->checkLogin(accessedDoc, hpvpd, rdata))
		{
			rdata->setOk();
			return true;
		}

		// It is not a login session. Check whether the session is still good
		// Chen Ding, 2013/06/01
		// if (rdata->getErrcode() != eAosXmlInt_Ok)
		if (!rdata->isOk())
		{
			// The session is no longer good. It can be the case that
			// it needs relogin or there are some internal errors. 
			// In any case, it should reject the request.
			return false;
		}
	}

	return checkAccess(AOSSOPR_READ, accessedDoc, rdata);
}


bool
AosSecurityMgr::checkCreateUrl(
		const OmnString &url,
		const AosRundataPtr &rdata)
{
	// Creating URL is a little bit different from normal access controls.
	// It does not have the accessed doc. URLs are shared by all the 
	// user domains. 
	//
	// 1. If the user has logged in, retrieve the URL doc from the 
	//    user domain. If the has not logged in, retrieve the default
	//    URL doc. The URL doc serves as the accessed doc.
	
	// Not implementedYet;
	rdata->setOk();
	AOSLOG_LEAVE(rdata);
	return true;
}


bool
AosSecurityMgr::checkAddPubObjid(
		const OmnString &objid,
		const AosRundataPtr &rdata)
{
	static int lsDftPrefLen = strlen(AOSOBJIDPRE_DFTOBJID);

	// Chen Ding, 2011/01/13                                                          
	//If the objid prefix matches AOSOBJIDPRE_DFTOBJID, return true.
	if (strncmp(AOSOBJIDPRE_DFTOBJID, objid.data(), lsDftPrefLen) == 0)
	{
		return true;
	}

	// Chen Ding, 01/22/2012
	// Check whether the objid is in the private format:
	// 		<objid>.<cloudid>
	// where <cloudid> matches the requester's cloud. In that case, 
	// it is actually a private objid. Return true.
	OmnString cid = rdata->getCid();
	if (cid != "" && AosObjid::doesCidMatch(objid, cid))
	{
		// It is a private objid. Return true;
		return true;
	}

	// It checks the requester has the right to add an objid to the 
	// public domain. 
	AosObjMgrObjPtr objmgr = AosObjMgrObj::getObjMgr();
	aos_assert_r(objmgr, false);
	AosXmlTagPtr accessedObj = objmgr->createPubNamespace(rdata);
	if (!accessedObj)
	{
		AosSetError(rdata, "internal_error"); 
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosSecReq &sec_req = rdata->getSecReq();
	sec_req.setData(AOSSOPR_ADDPUBLICOBJID, 0, accessedObj, rdata);
	if(sec_req.isRoot(rdata))
	{
		rdata->setOk();
		return true;
	}

	bool rslt = checkAccessPriv(AOSSOPR_ADDPUBLICOBJID, sec_req, rdata);
	if (!rslt)
	{
		AosSetError(rdata, "access_denied_002") << ": " << objid;
		return false;
	}
	rdata->setOk();
	return true;
}


/* Chen Ding, 12/28/2012
bool
AosSecurityMgr::checkVote(
		const AosXmlTagPtr &accessed_doc,
		const OmnString &votetype,
		const AosRundataPtr &rdata)
{
	// Each doc can configure itself about whether it can be voted. 
	// If yes, who may vote it on which vote type. Vote type means
	// a doc can be voted based on different types, such as whether
	// it was written well, whether the contents are relavent, 
	// whether readers like it, etc. The system defines a few
	// standard votes. Each doc can define its own vote types. 
	//
	// Access Control check on voting is done as:
	// 1. It retrieves its AR. If no AR, it cannot be voted. 
	// 2. It checks whether the attribute:
	// 		AOSZTG_VOTETYPE + ":" + votetype
	//    is defined. If yes, use its value to check the accesses.
	//    Otherwise, it checks whether the attribute:
	//    	AOSTAG_VOTE_ACCESSES
	//    is defined. If yes, use its value to check the accesses. 
	//    Otherwise, the access is rejected. 
	AOSLOG_ENTER_R(rdata, false);
	
	AosSecReq &sec_req = rdata->getSecReq();
	sec_req.setData(AosSecOpr::eVote, accessed_doc, rdata);
	if(sec_req.isRoot(rdata))
	{
		rdata->setOk();
		AOSLOG_LEAVE(rdata);
		return true;
	}

	AosAccessRcdPtr ar = sec_req.getAccessedAR();
	if (!ar) 
	{
		AosSetError(rdata, AosErrmsgId::eAccessDenied);
		AOSLOG_LEAVE(rdata);
		return false;
	}

	OmnString aname = AosObjid::composeVoteAttr(votetype);
	OmnString accesses = ar->getAttrStr(aname);
	if (accesses == "")
	{
		AOSLOG_LINE(rdata);
		accesses = ar->getAttrStr(AOSTAG_VOTE_ACCESSES);
	}
	AOSLOG_LEAVE(rdata);
	return checkAccessPriv(sec_req, accesses, AosSecOpr::eVote, rdata);
}
*/

/*
bool
AosSecurityMgr::checkDelMember(
		const OmnString &pctnr,
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr accessedObj = AosGetDocByObjid(pctnr, rdata);
	if (!accessedObj)
	{
		AosSetError(rdata, "accessed_not_found") << ": " << pctnr;
		AOSLOG_LEAVE(rdata);
		return false;
	}

	return checkAccess(AOSSOPR_DELMEM, accessedObj, rdata);
}
*/

bool
AosSecurityMgr::checkLoadImage(
		const OmnString &container_obji,
		const AosRundataPtr &rdata)
{
	return true;
}


bool	
AosSecurityMgr::checkUploadFile(
		const int filesize,
		const OmnString &dirname,
		const AosRundataPtr &rdata)
{
	//James 2011/02/14 eUploadFile
	//1. check the size of file is out of the range.

	OmnNotImplementedYet;
	return true;
	/*
	int dirsize = 0;
	getDirSize(dirname.data(), 0, dirsize);
	if ((dirsize + filesize/1024) >= eMaxDirSize)
	{
		rdata->setError();
		return false;
	}

	//2. check the user right.
	AosSecReq &sec_req = rdata->getSecReq();
	sec_req.setData(AosSecOpr::eUploadFile, 0, "", 0);
	if(sec_req.isRoot())
	{
		rdata->setOk();
		return true;
	}
	return checkAccessPriv(sec_req, rdata);
	*/
}


//James 2011-2-15 11:38:43
void 
AosSecurityMgr::getDirSize(const char* path, int depth, int &size)
{
    DIR *d; 
    struct dirent *file; 
    struct stat sb;
    char fname[200];  
    if(!(d = opendir(path)))
    {
        return;
    }
    while((file = readdir(d)) != NULL)
    {
        if(strncmp(file->d_name, ".", 1) == 0)
            continue;
        //get the absolute path of file or directory.
        sprintf(fname, "%s/%s", path, file->d_name);
        if(stat(fname, &sb)>= 0)
        {
        	double fsize = ceil((double)sb.st_size/1024);
      		size += (int)fsize;  	
        	//travesal depth is 3.
        	if(S_ISDIR(sb.st_mode) && depth <= 3)
        	{
        		
            	getDirSize(fname, depth + 1, size);
        	}
        }
    }
    closedir(d);
}


bool
AosSecurityMgr::isRoot(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);
	AosSecReq &sec_req = rdata->getSecReq();
	sec_req.setData("", 0, doc, rdata);
	return sec_req.isRoot(rdata);
}


bool
AosSecurityMgr::isRequesterAdmin(const AosRundataPtr &rdata)
{
	return AosSecReq::isRequesterAdmin(rdata);
}


bool
AosSecurityMgr::isAdmin(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	aos_assert_r(doc, false);
	AosSecReq &sec_req = rdata->getSecReq();
	sec_req.setData("", 0, doc, rdata);
	return sec_req.isAdminPublic(rdata);
}


bool
AosSecurityMgr::checkCreateLog(
		const AosXmlTagPtr &ctnr_doc,
		const OmnString &logname, 
		const AosRundataPtr &rdata)
{
//	OmnNotImplementedYet;
	return true;
}


bool 	
AosSecurityMgr::checkMemberOfListing(
		const OmnString &ctnr_objid, 
		const OmnString &parent_ctnr_objid, 
		const AosRundataPtr &rdata)
{
//	OmnNotImplementedYet;
	return true;
}


AosObjMgrObjPtr
AosSecurityMgr::getObjMgr() 
{
	//return mObjMgr;
	sgLock.lock();
	if (!mObjMgr)
	{
		mObjMgr = AosObjMgrObj::getObjMgr(); 
	}
	sgLock.unlock();
	return mObjMgr;
}


bool 
AosSecurityMgr::isUserInDomains(const OmnString &domains, const AosRundataPtr &rdata)
{
	// This function checks whether the user (through 'rdata') is in one of
	// the user domains in 'domains'. 'domains' is a string consisting of
	// one or more user domain names (i.e., user domain contaner objids), 
	// separated through commas. 
	aos_assert_rr(domains != "", rdata, false);
	OmnString user_domains = rdata->getRequesterDomains();
	if (user_domains == "") return false;
	return domains.hasCommonWords(user_domains, ",");
}

/*
bool
AosSecurityMgr::checkCreateDomainOprArd(
		const AosXmlTagPtr &domain_doc,
		const AosRundataPtr &rdata)
{
	if (!domain_doc)
	{
		AosSetError(rdata, "missing_domain_doc");
		return false;
	}

	// Check whether the doc already exists
	OmnString did = domain_doc->getAttrStr(AOSTAG_DOCID);
	if (did == "")
	{
		AosSetError(rdata, "missing_docid_001") << ": " 
			<< domain_doc->getAttrStr(AOSTAG_OBJID);
		return false;
	}

	OmnString objid = AosObjid::composeUserDomainOprArdObjid(did);
	AosXmlTagPtr dd = AosGetDocByObjid(objid, rdata);
	if (dd)
	{
		// It already exists.
		AosSetError(rdata, "access_denied_001") << ": " << objid;
		return false;
	}

	// It is not there yet. Check whether the requester can do
	// the operation.
	return checkAccess(AOSSOPR_CREATE_DMOPRARD, domain_doc, rdata);
}
*/


bool
AosSecurityMgr::logRootAccess(
		const AosRundataPtr &rdata, 
		const OmnString &opr_str, 
		AosSecReq &sec_req, 
		const AosXmlTagPtr &container, 
		const AosXmlTagPtr &accessed_doc)
{
return true;
	OmnNotImplementedYet;
	return false;
}


bool
AosSecurityMgr::logInternalCall(
		const AosRundataPtr &rdata, 
		const OmnString &opr_str, 
		AosSecReq &sec_req, 
		const AosXmlTagPtr &container, 
		const AosXmlTagPtr &accessed_doc)
{
return true;
	OmnNotImplementedYet;
	return false;
}


AosAccessRcdPtr
AosSecurityMgr::getSystemDftAcd(const AosRundataPtr &rdata)
{
	return AosGetAccessRecord(rdata, AOSDOCID_SYSTEM_DEFAULT_ACD, false);
}

bool 
AosSecurityMgr::checkIsLogin(const AosRundataPtr &rdata)
{
	OmnString ssid = rdata->getSsid();                                                       
	if(ssid == "")                                                                           
	{                                                                                        
		OmnString errmsg = "it has not login!";                                              
		AosSetError(rdata, "docclient_has_not_login") << errmsg;                             
		rdata->setReturnCode(AosReturnCode::eAccessDenied);                                  
		return false;                                                                        
	}                                                                                        
	OmnString loginvpd;                                                                      
	AosSessionObjPtr session = AosSessionMgr::getSelf()->getSession1(ssid, loginvpd, rdata); 
	if(session)                                                                              
	{                                                                                        
		u64 userid = session->getUserid();                                                   
		rdata->setUserid(userid);                                                            
		rdata->setCid(session->getCid());                                                    
		rdata->setSession(session);                                                          
		return true;                                                                         
	}                                                                                        
	else                                                                                     
	{                                                                                        
		OmnString errmsg = "session is invalid!";                                            
		AosSetError(rdata, "docclient_session_is_invalid") << errmsg;                        
		rdata->setReturnCode(AosReturnCode::eInvalidSession);                                
		return false;                                                                        
	}                                                                                        
}


