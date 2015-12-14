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
// 04/01/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "SmartDoc/SdocDelFloatingVpd.h"

#include "Debug/Error.h"
#include "Porting/TimeOfDay.h"
#include "Porting/GetTime.h"
#include "Security/Session.h"
#include "Util/StrSplit.h"
#include "Util/SPtr.h"
#include "Util/Locale.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

#if 0
AosSdocDelFloatingVpd::AosSdocDelFloatingVpd(const bool flag)
:
AosSmartDoc(AOSSDOCTYPE_DEL_FLOATVPD, AosSdocId::eDelFloatingVpd, flag)
{
}


AosSdocDelFloatingVpd::~AosSdocDelFloatingVpd()
{
}

bool 
AosSdocDelFloatingVpd::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// parseQuery
	// Examples :sdoc_delfloatingvpd
	// get attr by zky_startnum:
	// set attr like : zky_vote
	// <smartdoc 
	// ...
	// zky_deltagname="vpd"
	// zky_gictype= "gic_floatingvpd"
	// doc_run = "yes|no">
	//
	// <query
	// ....>
	// </query>
	//
	// </smartdoc>
	
	aos_assert_r(rdata, false);
	if (!sdoc)
	{
		rdata->setError() << "Missing Smartdoc!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	OmnString run_doc = sdoc->getAttrStr(AOSTAG_RUN);
	if (run_doc != "no")
	{
		//int ttl = 0;
		AosDocClientObj::getDocClient()->modifyAttrStr1(rdata, 
				sdoc->getAttrU64(AOSTAG_DOCID, 0), 
				sdoc->getAttrStr(AOSTAG_OBJID), 
				AOSTAG_RUN, "no", "no", false, false, 
				__FILE__, __LINE__);
	}

	bool rslt = parseQuery(sdoc, rdata);
	if (!rslt)
	{
		OmnAlarm  << rdata->getErrmsg() << enderr;
		return false;
	}

	AosXmlTagPtr query_xml = sdoc->getFirstChild(AOSTAG_QUERY);
	if (!query_xml)
	{
		rdata->setError() << "Missing Query Tag";
		return false;
	}
	query(query_xml, rdata);
	OmnString zky_tagname= sdoc->getAttrStr("zky_deltagname", "");
	OmnString zky_gictype= sdoc->getAttrStr("zky_gictype", "");
	if (zky_tagname!= "" && zky_gictype != "")
	{
		AosXmlParser parser;
		AosXmlTagPtr queryResultXml = parser.parse(rdata->getResults(), "");
		if(!queryResultXml)
		{
			rdata->setError() << "Failed to Retrieve Query Result";
			return false;
		}
		AosXmlTagPtr record = queryResultXml->getFirstChild("record");
		if (!record)
		{
			rdata->setError() << "No Record in QueryResult";
			return false;
		}
		while(record)
		{
			u64 docid = record->getAttrU64(AOSTAG_DOCID, 0);
			if (docid)
			{
				AosXmlTagPtr xml = AosDocClientObj::getDocClient()->getDoc(docid, "", rdata);
				AosXmlTagPtr doc = xml->clone();
				bool rslt = modifyDoc(doc, zky_tagname, zky_gictype);
				if (rslt)
				{
					bool rslt1 = AosDocClientObj::getDocClient()->modifyObj(rdata, doc, false, false);
					if (!rslt1) OmnAlarm << "Failing Modify!" << enderr;
				}
			}

			record = queryResultXml->getNextChild();
		}
	}

	return true;
}


bool
AosSdocDelFloatingVpd::query(
		const AosXmlTagPtr &query_xml,
		const AosRundataPtr &rdata)
{
	bool rslt = doQuery(mQuery, query_xml, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSdocDelFloatingVpd::modifyDoc(
		AosXmlTagPtr &doc,
		const OmnString &zky_tagname,
		const OmnString &zky_gictype)
{
	aos_assert_r(doc, false);
	AosXmlTagPtr gic_creators = doc->getFirstChild("gic_creators");
	OmnString gic_type = AOSTAG_GIC_TYPE;
	bool flag = false;
	if (gic_creators)
	{
		AosXmlTagPtr vpd = gic_creators->getFirstChild(zky_tagname);	
		while(vpd)
		{
			OmnString gictype = vpd->getAttrStr(AOSTAG_GIC_TYPE, "");
			if (gictype == zky_gictype)
			{
				gic_creators->removeNode1(zky_tagname, gic_type, gictype);
				flag = true;
			}
			vpd = gic_creators->getNextChild(zky_tagname);
		}
	}
	aos_assert_r(flag, false);
	return true;
}
#endif
