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
// 08/19/2011	Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocFenHong.h"

#include "Actions/ActSeqno.h"
#include "DocSelector/DocSelector.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"
#include "SEInterfaces/DocClientObj.h"

#include "SEUtil/IILName.h"
#include "Rundata/Rundata.h"
#include "SEModules/ObjMgr.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

#if 0

AosSdocFenHong::AosSdocFenHong(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_FENHONG, AosSdocId::eFenHong, flag)
{
}


AosSdocFenHong::~AosSdocFenHong()
{
}

bool
AosSdocFenHong::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) 
{
	// 	<sdoc type="xxx" is_public="true|false" cid_required="true|false" prefix="xxx">
	//		<usr_docsel .../>
	//		<cash_attr_valsel .../>
	//		<pre_attr_valsel ../>
	//		<cash_valsel ../>
	//		<levels>
	//			<level percent="xxx" />
	//			<level percent="xxx" />
	//			.....
	//		</levels>
	// 		.....
	// 	</sdoc>
	
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// retrieve the doc
	// AosXmlTagPtr usr_doc = AosDocSelector::selectDoc(sdoc, "usr_docsel", rdata);
	AosXmlTagPtr usr_doc = AosRunDocSelector(rdata, sdoc, "usr_docsel");
	if (!usr_doc)
	{
		rdata->setError() << "Failed to get usr1 doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	usr_doc = usr_doc->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(usr_doc, false);
	
	// retrieve the cash attribute name
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, "cash_attr_valsel", rdata);
	if (!rslt || value.getTypeStr() != AOSDATATYPE_STRING)
	{
		rdata->setError() << "Failed to get the cash attribute name!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString cash_attr = value.getValueStr1("");
	if (cash_attr == "")
	{
		rdata->setError() << "cash attribute name is null!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	// retrieve the cash to add
	rslt = AosValueSel::getValueStatic(value, sdoc, "cash_valsel", rdata);
	u64 cash;
	if (rslt)
	{
		if (!value.getU64Value(cash, rdata.getPtr())) return false;
	}

	if (!cash)
	{
		cash = 0;
	}
	
	// add the user's cash
	u64 oldcash = usr_doc->getAttrU64(cash_attr, 0);
	u64 newcash = cash + oldcash;
	rslt = usr_doc->setAttr(cash_attr, newcash);
	aos_assert_r(rslt, false);
	
	u64 userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));

	rslt = AosDocClientObj::getDocClient()->modifyObj(usr_doc, rdata);

	rdata->setUserid(userid);
	if (!rslt)
	{
		rdata->setError() << "Failed to modify usr1 doc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// retrieve the persenter attribute name
	rslt = AosValueSel::getValueStatic(value, sdoc, "pre_attr_valsel", rdata);
	OmnString pre_attr;
	if (rslt)
	{
		pre_attr = value.getValueStr1();
	}
	if (pre_attr == "")
	{
		return true;
	}
	
	OmnString objid = usr_doc->getAttrStr(pre_attr, "");
	if (objid == "")
	{
		return true;
	}

	AosXmlTagPtr levels = sdoc->getFirstChild("levels");	
	if (!levels)
	{
		return true;
	}
	AosXmlTagPtr level = levels->getFirstChild();
	userid = rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));
	while (level)
	{
		usr_doc = AosDocClientObj::getDocClient()->getDocByCloudid(objid, rdata);
		if (usr_doc)
		{
			usr_doc = usr_doc->clone(AosMemoryCheckerArgsBegin);
			aos_assert_r(usr_doc, false);
			OmnString per = level->getAttrStr("percent", 0);
			float percent = atof(per.data())/100;
			oldcash = usr_doc->getAttrU64(cash_attr, 0);
			cash = cash * percent;
			newcash = oldcash + cash;
			usr_doc->setAttr(cash_attr, newcash);
			rslt = AosDocClientObj::getDocClient()->modifyObj(usr_doc, rdata);
			if (!rslt)
			{
				rdata->setUserid(userid);
				rdata->setError() << "Failed to modify user doc!";
				OmnAlarm << rdata->getErrmsg() << enderr;
				return false;
			}
			objid = usr_doc->getAttrStr(pre_attr, "");
		}
		else
		{
			break;
		}
		level = levels->getNextChild();
	}
	rdata->setUserid(userid);
	return true;
}

#endif
