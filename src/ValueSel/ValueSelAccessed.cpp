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
// This value selector selects the accessed. It can select one of the 
// following:
// 		1. The accessed's user account doc
// 		2. The accessed's cid
// 		3. The accessed's user name
//		4. The accessed's userid
//		5. The accessed's login status (not supported yet)
//		6. The accessed's last login (not supported yet)
// This is determined by sdoc's AOSTAG_ACCESSED_TYPE (AOSTAG_USER_TYPE 
// for backward compatibility) attribute. If not specified, it defaults 
// to AOSVALUE_USERID. It is possible to specify a default value. 
//
// Modification History:
// 01/13/2012 Created by Chen Ding
// 12/28/2012 Changed 'AOSTAG_USER_TYPE' to 'AOSTAG_ACCESSED_TYPE'. 
// 2013/12/04 Documented by Chen Ding, removed 'dft_value' and 'AOSVALUE_CONST'
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelAccessed.h"

#include "Actions/ActUtil.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#if 0

AosValueSelAccessed::AosValueSelAccessed(const bool reg)
:
AosValueSel(AOSACTOPRID_ACCESSED, AosValueSelType::eAccessed, reg)
{
}


AosValueSelAccessed::AosValueSelAccessed(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_ACCESSED, AosValueSelType::eAccessed, false)
{
}


AosValueSelAccessed::~AosValueSelAccessed()
{
}


bool
AosValueSelAccessed::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	// This function uses the smart doc 'sdoc' to retrieve a value. 
	// The value is determined by the attribute AOSTAG_USER_TYPE |
	// AOSTAG_VALUE_TYPE. 
	// The smartdoc format is:
	// 	<sdoc ... 
	// 		AOSTAG_USER_TYPE=
	// 				AOSVALUE_USERID |
	// 				AOSVALUE_ACCESSED_DOC |
	// 				AOSVALUE_CID |
	// 				AOSVALUE_USERNAME |
	// 				AOSVALUE_CONST
	// 		AOSTAG_USERNAME="xxx">
	// 		the default value
	// 	</sdoc>
	aos_assert_r(rdata, false);
	aos_assert_rr(sdoc, rdata, false);

	// retrieve the requester account
	AosUserAcctObjPtr req_acct = rdata->getRequesterAcct();
	if (!req_acct)
	{
		valueRslt.reset();
		return true;
	}

	AosXmlTagPtr doc = req_acct->getDoc();
	if (!doc)
	{
		AosSetErrorUser(rdata, "missing_acct_doc") << enderr;
		valueRslt.reset();
		return false;
	}

	// Determine the requester type
	OmnString type = sdoc->getAttrStr(AOSTAG_ACCESSED_TYPE);
	if (type == "")
	{
		// Chen Ding, 12/28/2012, Backward compatibility
		type = sdoc->getAttrStr(AOSTAG_USER_TYPE);
	}

	// Chen Ding, 12/28/2012
	// Chen Ding, 2013/12/05
	// OmnString dft_value = sdoc->getNodeText();
	if (type == "")
	{
		// It defaults to get the accessed's userid (i.e., docid)
		type = AOSVALUE_USERID;
	}

	if (type == AOSVALUE_ACCESSED_DOC)
	{
		// It is to get requester's doc
		valueRslt.setXml(rdata->getAccessedDoc());
		return true;
	}

	if (type == AOSVALUE_CID)
	{
		// It is to get the requester's cloud id
		valueRslt.setStr(rdata->getAccessedCid());
		return true;
	}

	if (type == AOSVALUE_USERID)
	{
		// It is to get the requester's docid
		valueRslt.setU64(rdata->getAccessedUserid());
		return true;
	}

	if (type == AOSVALUE_USERNAME)
	{
		// It is to get the requester's username
		valueRslt.setStr(rdata->getAccessedUsername());
		return true;
	}

	// Chen Ding, 2013/12/04
	// This does not belong to here.
	// if (type == AOSVALUE_CONST)
	// {
	// 	// It is to get a constant value from the smart doc.
	// 	valueRslt.setValue(sdoc->getAttrStr(AOSTAG_USERNAME));
	// 	return true;
	// }

	AosSetError(rdata, "valueselaccessed_unrecog_type") << "Type: " << type
		<< ". SmartDoc dump: " << sdoc->toString() << enderr;
	return false;
}


OmnString 
AosValueSelAccessed::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << tagname << AOSTAG_USER_TYPE << "=\"";
	switch (rule->percent(AOSTAG_USER_TYPE, 5))
	{
	case 0:
		 docstr << AOSVALUE_USERID << "\"/>";
		 break;

	case 1:
		 docstr << AOSVALUE_ACCESSED_DOC << "\"/>";
		 break;

	case 2:
		 docstr << AOSVALUE_CID << "\"/>";
		 break;
		 
	case 3:
		 docstr << AOSVALUE_USERNAME << "\"/>";
		 break;
		 
	case 4:
		 docstr << AOSVALUE_CONST << "\" " << AOSTAG_USERNAME
			 << "=\"" << rule->getWord(AOSTAG_USERNAME) << "\"/>";
		 break;

	default:
		 docstr << AOSVALUE_USERNAME << "\"/>";
		 break;
	}

	return docstr;
}


AosValueSelObjPtr
AosValueSelAccessed::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		AosValueSel *obj = OmnNew AosValueSelAccessed(sdoc, rdata);
		return obj;
	}

	catch (...)
	{
		AosSetError(rdata, "failed_clone_object") << enderr;
		return 0;
	}
}
#endif
