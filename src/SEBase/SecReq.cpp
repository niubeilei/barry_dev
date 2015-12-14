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
//	06/06/2010	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "SEBase/SecReq.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "Security/Session.h"
#include "Security/SessionMgr.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEUtil/SeConfigMgr.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/UserDomainObj.h"
#include "UserMgmt/UserDomainMgr.h"
#include "SEInterfaces/UserAcctObj.h"
#include "UserMgmt/UserAcctMgr.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/AccessRcd.h"
#include "XmlUtil/AccessRcdMgr.h"
#include "XmlInterface/XmlRc.h"


// Chen Ding, 12/28/2012, Copied from SecurityMgr.cpp
static AosXmlTagPtr sgRoidAccesses[AosObjidType::eMax];	// Defines the accesses
static OmnString	sgRoidOtypes[AosObjidType::eMax]; 	// Defines the otypes binding
static OmnString	sgRoidCtnrs[AosObjidType::eMax];  	// Defines parent containers


AosSecReq::AosSecReq()
:
mLock(OmnNew OmnMutex())
{
}


AosSecReq::AosSecReq(
	const OmnString &operation,
	const AosXmlTagPtr &acc_doc,
	const AosRundataPtr &rdata)
:
mOprStr(operation),
mLock(OmnNew OmnMutex()),
mAccessedObj(acc_doc)
{
	mLock->lock();
	initLocked(rdata);
	mLock->unlock();
}


bool
AosSecReq::config(const AosXmlTagPtr &sec_config)
{
	// This function should be called by AosSecurityMgr. 'config' is 
	// already the tag for the security.
	if (!sec_config) 
	{
		return true;
	}

	// Configure the Reserved Objid Access Controls. Some objids are reserved. 
	// Using these objids are security checked. For instance, Domain OprArd objids
	// can be used by System Administrators only. Most reserved objids are 
	// tied with special otype, and possibly tied with parent containers.
	AosXmlTagPtr rsved_objid_def = sec_config->getFirstChild("reserved_objids");
	if (!rsved_objid_def) return true;
	
	AosXmlTagPtr record = rsved_objid_def->getFirstChild();
	while (record)
	{
		AosObjidType::E type = AosObjidType::toEnum(record->getAttrStr("objid_type"));
		if (!AosObjidType::isValid(type))
		{
			OmnAlarm << "Objid type invalid: " << record->toString() << enderr;
		}
		else
		{
			// The record is in the format:
			// 	<record AOSTAG_OTYPE="xxx" AOSTAG_PARENT_CONTAINER="xxx">
			// 		<access .../>
			// 	</record>
			AosXmlTagPtr access_tag = record->getFirstChild();
			if (access_tag)
			{
				sgRoidAccesses[type] = access_tag->clone(AosMemoryCheckerArgsBegin);
			}
			sgRoidOtypes[type] = record->getAttrStr(AOSTAG_OTYPE);
			sgRoidCtnrs[type] = record->getAttrStr(AOSTAG_PARENT_CONTAINER);
		}
		record = rsved_objid_def->getNextChild();
	}
	return true;
}


bool
AosSecReq::initLocked(const AosRundataPtr &rdata)
{
	// 1. Retrieved the accessed ARD
	if (mAccessedObj && AosAccessRcd::isAccessRecord(mAccessedObj))
	{
		// The accessed obj itself is an access record. 
		mAccessedAR = AosAccessRcdMgr::getSelf()->convertToAccessRecord(rdata, mAccessedObj);
	}

	if (!mAccessedAR && mAccessedObj)
	{
		// Chen Ding, 2013/12/07
		// Check whether the doc has its own access record.
		if (mAccessedObj->getAttrBool("zky_use_selfaccs", false))
		{
			// Need to retrieve the accessed AR.
			u64 docid = mAccessedObj->getAttrU64(AOSTAG_DOCID, AOS_INVDID);
			if (docid <= 0)
			{
				AosSetErrorU(rdata, "invalid_accessed_doc") << ": " 
					<< mAccessedObj->toString() << enderr;
				return false;
			}
			// Chen Ding, 2013/06/10
			mAccessedAR = AosGetAccessRcd(rdata, mAccessedObj, docid, "", true);
		}
	}

	// 2. Get the Requester's account
	if (rdata->getUserid())
	{
		// Chen Ding, 2013/06/10
		mRequesterAcct = AosGetUserAcct(rdata->getUserid(), rdata);
	}

	// 3. Retrieve the requester's user domain
	if (mRequesterAcct)
	{
		mRequesterDomain = AosGetUserDomain(mRequesterAcct, rdata);
	}

	// 4. Retrieve the accessed account
	AosUserMgmtObjPtr usermgmt_obj = AosUserMgmtObj::getUserMgmt();
	if (!usermgmt_obj)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return false;
	}

	if (mAccessedObj)
	{
		//if (AosUserAcct::isUserAcct(mAccessedObj))
		if (AosUserAcctObj::isUserAcct(mAccessedObj))
		{
			mAccessedAcct = usermgmt_obj->getUserAcct(mAccessedObj, rdata);
		}
		else
		{
			OmnString creator;
			if (AosAccessRcd::isAccessRecord(mAccessedObj))
			{
				// The accessed object itself is an access record. In this case,
				// retrieve the owner docid, and use the docid to retrieve
				// the doc. From the doc, we can retrieve its creator's cloudid. 
				// From that cloudid, we can retrieve the creator.
				AosXmlTagPtr owndoc = AosGetDocByDocid(
						mAccessedObj->getAttrU64(AOSTAG_OWNER_DOCID, 0), rdata);
				if (!owndoc)
				{
					AosSetErrorU(rdata, "invalid_doc") << mAccessedObj->toString() << enderr;
					return false;
				}
				creator = owndoc->getAttrStr(AOSTAG_CREATOR, "");
			}
			else
			{
				creator = AosGetCreator(mAccessedObj);
			}

			if (creator != "")
			{
				mAccessedAcct = usermgmt_obj->getUserAcctByCloudid(creator, rdata);
				if (!mAccessedAcct)
				{
					// Backward Compatibility
					u64 did = atoll(creator.data());
					if (did > 0)
					{
						mAccessedAcct = usermgmt_obj->getUserAcctByDocid(did, rdata);
					}
				}
			}
		}
	}

	// 5. Retrieve the Accessed domain
	if (mAccessedAcct)
	{
		mAccessedDomain = AosGetUserDomain(mAccessedAcct, rdata);
		OmnString objid = mAccessedAcct->getDomainObjid();
 		if (objid == "")
	  	{
			if (mAccessedAcct->isSysRoot())
			{
				objid = AosObjid::getRootCtnrObjid(rdata->getSiteid());
			}
			else
			{
		   		AosSetError(rdata, AosErrmsgId::eMissingParentContainer);
		    	OmnAlarm << rdata->getErrmsg() << enderr;
			 	return false;
			}
		}
		
		try
		{
			mAccessedDomain = usermgmt_obj->getUserDomain1(objid, mAccessedAcct, rdata);
			return true;
		}
		
		catch (...)
		{
			AosSetError(rdata, AosErrmsgId::eExceptionCreateUserDomain)
				<< mAccessedAcct->getAttrStr(AOSTAG_OBJID);
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	return true;
}


bool
AosSecReq::doesRequesterOwnAccessed(const AosRundataPtr &rdata)
{
	// It checks whether the requester owns the requested. 
	mLock->lock();
	if (!isSameDomainLocked())
	{
		mLock->unlock();
		AosSetErrorU(rdata, "access_denied") << enderr;
		return false;
	}

	if (mAccessedAcct->isUnknownUser() ||
		mAccessedAcct->isSameAccount(mRequesterAcct)) 
	{
		mLock->unlock();
		return true;
	}

	if (!mAccessedDomain->supervisorEnabled()) 
	{
		mLock->unlock();
		AosSetErrorU(rdata, "access_denied") << enderr;
		return false;
	}

	if (mAccessedAcct->isSupervisor(mRequesterAcct, mAccessedDomain, 
				mRequesterDomain, rdata)) 
	{
		mLock->unlock();
		AosSetErrorU(rdata, "access_denied") << enderr;
		return true;
	}

	mLock->unlock();
	AosSetErrorU(rdata, "access_denied") << enderr;
	return false;
}


AosXmlTagPtr
AosSecReq::getAccesses(
		const OmnString &opr_str, 
		const AosRundataPtr &rdata)
{
	// Accesses are defined by the access record. Access records are in 
	// the following format:
	//  <ard ...>
	//  	<AOSTAG_ACCESSES>
	//  		<opr_id ...>
	//  			<access .../>
	//  			<access .../>
	//  			...
	//  		</opr_id>
	//  		...
	//  	</AOSTAG_ACCESSES>
	//  </ard>
	// If there are no access records or the access record does not have the
	// corresponding accesses, it will use the default. 
	mLock->lock();
	if (!mAccessedAR) 
	{
		mLock->unlock();
		return 0; 
	}

	// It uses 'opr_str' to identify the accesses, if any. 
	AosXmlTagPtr tag = mAccessedAR->getAccess(opr_str, rdata);
	if (!tag)
	{
		// Did not find the access tag. Check whether it is to access 
		// the reserved objids. 
		if (opr_str.length() == 15 && opr_str == AOSSOPR_RESERVED_OBJID)
		{
			tag = mReservedObjidAccesses;
		}
	}
	mLock->unlock();
	return tag;
}


bool
AosSecReq::isRoot(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mRequesterAcct && mRequesterAcct->isSysRoot()) 
	{
		mLock->unlock();
		return true;
	}
	if (mRequesterAcct && mRequesterAcct->isSuperUser()) 
	{
		mLock->unlock();
		return true;
	}
	if (!isSameDomainLocked()) 
	{
		mLock->unlock();
		return false;
	}

	bool rslt = mRequesterAcct->isRoot(mAccessedDomain, mRequesterDomain);
	mLock->unlock();
	return rslt;
}


bool
AosSecReq::isRequesterAdmin(const AosRundataPtr &rdata)
{
	// This function checks whether the requester is an admin. 
	u64 userid = rdata->getUserid();
	if (!userid) return false;
	AosXmlTagPtr userdoc = AosGetDocByDocid(userid, rdata);
	aos_assert_r(userdoc, false);
	//return AosUserAcct::isAdmin(userdoc);
	return AosUserAcctObj::isAdmin(userdoc);
}


bool
AosSecReq::isAdminLocked(const AosRundataPtr &rdata)
{
	if (!isSameDomainLocked())
	{
		return false;
	}

	return mRequesterAcct->isAdmin(mAccessedDomain, mRequesterDomain);
}


bool			
AosSecReq::isUserLoggedInLocked(const AosRundataPtr &rdata)
{
	if (!isSameDomainLocked())
	{
		return false;
	}

	return true;
}


bool
AosSecReq::requireLogin(OmnString &hpvpd)
{
	// This function checks whether the accessed requires
	// login. An accessed doc requires login if:
	// 1. If the accessed obj requires so,
	// 2. If the owner of the accessed obj requires so,
	// 3. If the parent of the accessed obj requires so, 
	// Otherwise, it does not require login.
	//
	// If login is required, it should also be able to 
	// specify a login vpd.
	
	mLock->lock();
	if (mAccessedAR)
	{
		OmnString hpvpd = mAccessedAR->getHomeVpd();
		mLock->unlock();
		if (hpvpd != "") return true;
	}

	if (mRequesterAcct)
	{
		OmnString hpvpd = mRequesterAcct->getAttrStr(AOSTAG_REQUIRED_LOGIN);
		mLock->unlock();
		if (hpvpd != "") return true;
	}

	mLock->unlock();
	return false;
}


// AosAccessRcdPtr 
// AosSecReq::getAccessedAR() const
// {
	// return mAccessedAR;
// }


AosXmlTagPtr
AosSecReq::getAccessedParentCtnr(const AosRundataPtr &rdata)
{
	// Chen Ding, 2013/04/19
	if (mAccessedParentCtnr) return mAccessedParentCtnr;

	mLock->lock();
	if (!mAccessedObj) 
	{
		mLock->unlock();
		return 0;
	}
	OmnString parent_objid = mAccessedObj->getAttrStr(AOSTAG_PARENTC, "");
	if (parent_objid == "")
	{
		mLock->unlock();
		// Chen Ding, 2013/04/19
		// AosSetErrorU(rdata, "missing_parent_ctnr") << enderr;
		return 0;
	}
	mAccessedParentCtnr = AosGetDocByObjid(parent_objid, rdata);
	mLock->unlock();
	return mAccessedParentCtnr;
}


OmnString
AosSecReq::getRequesterUserGroups() const
{
	mLock->lock();
	if (!mRequesterAcct) 
	{
		mLock->unlock();
		return "";
	}

	OmnString group = mRequesterAcct->getUserGroups();
	mLock->unlock();
	return group;
}


OmnString
AosSecReq::getRequesterUserRoles() const
{
	mLock->lock();
	if (!mRequesterAcct) 
	{
		mLock->unlock();
		return "";
	}
	OmnString role = mRequesterAcct->getUserRoles();
	mLock->unlock();
	return role;
}


OmnString
AosSecReq::getRequesterUsername()
{
	mLock->lock();
	if (!mRequesterAcct) 
	{
		mLock->unlock();
		return "";
	}

	OmnString name = mRequesterAcct->getUsername();
	mLock->unlock();
	return name;
}


AosXmlTagPtr	
AosSecReq::getRequesterOprArd(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (!mRequesterAcct) 
	{
		mLock->unlock();
		return 0;
	}

	AosXmlTagPtr tag = mRequesterAcct->getUserOprArd(rdata);
	mLock->unlock();
	return tag;
}


AosXmlTagPtr	
AosSecReq::getAccessedDomainOprArd(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (!mAccessedDomain) 
	{
		mLock->unlock();
		return 0;
	}

	AosXmlTagPtr tag = mAccessedDomain->getUserDomainOprArd(rdata);
	mLock->unlock();
	return tag;
}


OmnString
AosSecReq::toString() const 
{
	OmnString str;
	// str << "SecReq: operation=" << AosSecOpr::toString(mOpr);
	str << "SecReq: operation=" << mOprStr;
	mLock->lock();
	if (mRequesterAcct) str << "; Requester=" << mRequesterAcct->getUsername();
	if (mAccessedObj) str << "; AccessedObj=" << mAccessedObj->getAttrStr(AOSTAG_OBJID);
	mLock->unlock();
	return str;
}


u64
AosSecReq::getAccessedDocUserid(
		const AosXmlTagPtr &accessed_doc, 
		const AosRundataPtr &rdata)
{
	if(!accessed_doc)
	{
		AosSetErrorU(rdata, "missing_accessed_doc") << enderr;
		return AOS_INVDID;
	}

	//cid or uid
	OmnString cid = accessed_doc->getAttrStr(AOSTAG_CREATOR);
	if (cid =="")
	{
		AosSetErrorU(rdata, "missing_creator") 
			<< accessed_doc->getAttrStr(AOSTAG_OBJID) << enderr;
		return AOS_INVDID;
	}

	AosXmlTagPtr udoc = AosGetDocByCloudid(cid, rdata);
	if(!udoc)
	{
		// Backward compatibility
		u64 docid = accessed_doc->getAttrU64(AOSTAG_CREATOR, 0);
		if (docid == AOS_INVDID)
		{
			AosSetErrorU(rdata, "missing_creator")
				<< accessed_doc->getAttrStr(AOSTAG_OBJID) << enderr;
			return AOS_INVDID;
		}

		udoc = AosGetDocByDocid(docid, rdata);
	}

	if (!udoc)
	{
		AosSetErrorU(rdata, "failed_ret_userdoc") << cid << enderr;
		return AOS_INVDID;
	}

	u64 uid = udoc->getAttrU64(AOSTAG_DOCID, 0);
	if (uid == AOS_INVDID)
	{
		AosSetErrorU(rdata, "missing_docid") 
			<< udoc->getAttrStr(AOSTAG_OBJID) << enderr;
		return AOS_INVDID;
	}
	return uid;
}


AosXmlTagPtr
AosSecReq::getAccessedCtnr(const AosRundataPtr &rdata) const
{
	mLock->lock();
	if (!mAccessedObj)
	{
		mLock->lock();
		AosSetErrorU(rdata, "internal_error") << enderr;
		return 0;
	}
	
	// Modify By Brian Zhang 2012/02/03
	// OmnString ctnr_objid = mAccessedObj->getAttrStr(AOSTAG_OBJID);
	OmnString ctnr_objid = mAccessedObj->getAttrStr(AOSTAG_PARENTC);
	mLock->unlock();
	if (ctnr_objid == "") return 0;
	//aos_assert_rr(ctnr_objid != "", rdata, 0);

	return AosGetDocByObjid(ctnr_objid, rdata);
}


OmnString		
AosSecReq::getRequesterDomains() const
{
	mLock->lock();
	if (!mRequesterAcct) 
	{
		mLock->unlock();
		return "";
	}
	OmnString domain = mRequesterAcct->getUserDomains();
	mLock->unlock();
	return domain;
}


void
AosSecReq::setReservedObjidAccesses(const AosXmlTagPtr &acc)
{
	mLock->lock();
	mReservedObjidAccesses = acc;
	mLock->unlock();
}


bool
AosSecReq::checkReservedObjid(
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
	if (!AosObjidType::isValid(objid_type))
	{
		// Should never happen
		AosSetError(rdata, "internal_error");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	aos_assert_rr(doc, rdata, false);
	if (sgRoidOtypes[objid_type] != "")
	{
		// The otype is not empty. This means that this type of 
		// objid must be associated with the specified otype. If
		// not, the access is rejected.
		if (doc->getAttrStr(AOSTAG_OTYPE) != sgRoidOtypes[objid_type])
		{
			AosSetErrorU(rdata, "otype_mismatch_001") 
				<< doc->getAttrStr(AOSTAG_OTYPE) << ":" 
				<< sgRoidOtypes[objid_type] << enderr;
			return false;
		}
	}

	if (sgRoidCtnrs[objid_type] != "")
	{
		// The container is not empty. This means that this type of 
		// objid must be under the specified container. If not, 
		// the access is rejected.
		if (doc->getAttrStr(AOSTAG_PARENTC) != sgRoidCtnrs[objid_type])
		{
			AosSetErrorU(rdata, "ctnr_mismatch_001")
				<< doc->getAttrStr(AOSTAG_PARENTC) << ":" 
				<< sgRoidCtnrs[objid_type] << enderr;
			return false;
		}
	}

	mReservedObjidAccesses = sgRoidAccesses[objid_type];
	return true;
}


bool 
AosSecReq::isSameDomainLocked() const
{
	// This function determines whether the requester and the requested
	// domains are the same or friend domains.
	if (!mRequesterDomain || !mAccessedDomain) return false;
	if (!mAccessedAcct || !mRequesterAcct) return false;
	return mRequesterDomain->isFriendDomain(mAccessedDomain);
}


void 
AosSecReq::reset()
{
	mAccessedObj = 0;
	mAccessedParentCtnr = 0;
	mAccessedAcct = 0;
	mAccessedAR = 0;
	mAccessedDomain = 0;
	mRequesterAcct = 0;
	mRequesterDomain = 0;
	mReservedObjidAccesses = 0;
}


AosUserAcctObjPtr  
AosSecReq::getAccessedAcct() const 
{
	return mAccessedAcct;
}	


AosUserAcctObjPtr  
AosSecReq::getRequesterAcct() const 
{
	return mRequesterAcct;
}	


bool 
AosSecReq::setData(
		const OmnString &operation,
		const AosXmlTagPtr &accessed_container,
		const AosXmlTagPtr &acc_doc,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	reset();
	mOprStr = operation;
	mAccessedObj = acc_doc;
	mAccessedParentCtnr = accessed_container;
	bool rslt = initLocked(rdata);
	mLock->unlock();
	return rslt;
}


AosXmlTagPtr	
AosSecReq::getAccessedObj() const 
{
	return mAccessedObj;
}
