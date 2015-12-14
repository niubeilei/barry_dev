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
// This is a wrap on access record. Its member data 'mAcdDoc' is an 
// access record.
//
// Modification History:
// 09/01/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/AccessRcd.h"

#include "Debug/Except.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


AosAccessRcd::AosAccessRcd(
		const AosXmlTagPtr &acd_doc, 
		const AosRundataPtr &rdata)
:
mAcdDoc(acd_doc)
{
	if (!acd_doc)
	{
		AosSetError(rdata, "accessrcd_missing_doc") << enderr;
		OmnThrowException("Missing doc");
	}
}


bool
AosAccessRcd::checkAccessRecord(const AosRundataPtr &rdata)
{
	// This function checks the following:
	// 1. Its docid must not be 0
	// 2. AOSTAG_OTYPE must be AOSOTYPE_ACCESS_RCD.
	aos_assert_r(mAcdDoc, false);
	u64 acd_docid = mAcdDoc->getAttrU64(AOSTAG_DOCID, 0);
	if (acd_docid == 0)
	{
		AosSetError(rdata, "accessrcd_docid_is_null") << mAcdDoc << enderr;
		return false;
	}

	if (mAcdDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_ACCESS_RCD)
	{
		AosSetError(rdata, "accessrcd_not_access_rcd") << mAcdDoc << enderr;
		return false;
	}

	return true;
}


bool 
AosAccessRcd::modifyStatemachineState(
		const OmnString &state, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mAcdDoc, rdata, false);
	AosXmlTagPtr tag = mAcdDoc->getFirstChild(AOSTAG_STATEMACHINE);
	if (!tag)
	{
		tag = mAcdDoc->addNode1(AOSTAG_STATEMACHINE, "");
		aos_assert_rr(tag, rdata, false);
	}
	if (state == "")
	{
		return tag->removeAttr(AOSTAG_CURRENT_STATE);
	}
	return tag->setAttr(AOSTAG_CURRENT_STATE, state);
}


bool 
AosAccessRcd::isAccessRecord(const AosXmlTagPtr &doc)
{
	aos_assert_r(doc, false);
	return doc->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_ACCESS_RCD;
}


// Chen Ding, 12/28/2012
AosXmlTagPtr
AosAccessRcd::getAccess(
		const OmnString &opr_id, 
		const AosRundataPtr &rdata)
{
	// Chen Ding, 12/28/2012
	// We will no longer support old format!!!!!!!!!!!!!!!! 
	//
	// Accesses are defined based on 'opr_id'. Each 'opr_id' 
	// may be associated with one or more accesses. There are
	// two ways to specify accesses: (1) the old way (should
	// not use anymore) and (2) the new way. The old way is
	// through attributes. If there are multiple accesses, 
	// they are concatenated by commas. In the new way, accesses
	// are defined by subtags.
	// 	1. Old Way:
	// 	<ard read="group,role,public,..."
	// 		 write="xxx,xxx,..."/>
	//  2. New Way:
	//  <ard ...>
	//  	<AOSTAG_ACCESSES>
	//  		<opr_id ...>
	//  			<access groups="group,group,..." roles="role,role,..." />
	//  			<access groups="group,group,..." roles="role,role,..." />
	//  			...
	//  		</opr_id>
	//  		...
	//  	</AOSTAG_ACCESSES>
	//  </ard>
	// This function retrieves the new form first. If not, it checks
	// the old form.
	// accesses_str = "";	Chen Ding, 12/28/2012
	aos_assert_rr(mAcdDoc, rdata, 0);
	aos_assert_rr(opr_id != "", rdata, 0);

	// Check whether there are subtag <AOSTAG_ACCESSES>
	// Chen Ding, 12/28/2012
	AosXmlTagPtr accesses = mAcdDoc->getFirstChild(AOSTAG_ACCESSES);
	if (!accesses) return 0;
	return accesses->getFirstChild(opr_id);

	// Chen Ding, 12/28/2012. We will no longer support the old form.
	// It is the old form
	// accesses_str = mAcdDoc->getAttrStr(opr_id);
	// return true;
}


OmnString 
AosAccessRcd::getAttrStr(const OmnString &name, const OmnString &dft)
{
	if (!mAcdDoc) return dft;
	return mAcdDoc->getAttrStr(name, dft);
}


OmnString 
AosAccessRcd::getHomeVpd()
{
	aos_assert_r(mAcdDoc, "");
	return mAcdDoc->getAttrStr(AOSTAG_REQUIRED_LOGIN);
}


/*
OmnString 
AosAccessRcd::getGroups(const AosSecOpr::E opr)
{
	if (!mAcdDoc) return "";
	OmnString aname = AosSecOpr::toString(opr);
	if (aname == "") return "";
	//aname << "_" << AOSTAGID_GROUPS;
	aname << "_values";	// Ketty 2012/03/01
	return mAcdDoc->getAttrStr(aname);
}


OmnString 
AosAccessRcd::getRoles(const AosSecOpr::E opr)
{
	if (!mAcdDoc) return "";
	OmnString aname = AosSecOpr::toString(opr);
	if (aname == "") return "";
	//aname << "_" << AOSTAGID_ROLES;
	aname << "_values";	// Ketty 2012/03/01
	return mAcdDoc->getAttrStr(aname);
}
*/


u64 
AosAccessRcd::getOwnerDocid() const
{
	aos_assert_r(mAcdDoc, 0);
	return mAcdDoc->getAttrU64(AOSTAG_OWNER_DOCID, 0);
}


u64 
AosAccessRcd::getAttrU64(const OmnString &name, const u64 &dft)
{
	if (!mAcdDoc) return dft;
	return mAcdDoc->getAttrU64(name, dft);
}


AosXmlTagPtr 
AosAccessRcd::getDoc() const 
{
	return mAcdDoc;
}


OmnString 
AosAccessRcd::getParentContainerObjid() const
{
	if (!mAcdDoc) return "";
	return mAcdDoc->getAttrStr(AOSTAG_PARENTC);
}


OmnString 
AosAccessRcd::getStatemachineState() const
{
	if (!mAcdDoc) return "";
	AosXmlTagPtr tag = mAcdDoc->getFirstChild(AOSTAG_STATEMACHINE);
	if (!tag) return "";
	return tag->getAttrStr(AOSTAG_CURRENT_STATE);
}


u64 
AosAccessRcd::getDocid() const
{
	if (!mAcdDoc) return 0;
	return mAcdDoc->getAttrU64(AOSTAG_DOCID, 0);
}


