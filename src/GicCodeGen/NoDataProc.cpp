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
// 02/05/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "GicCodeGen/NoDataProc.h"


OmnString 
AosNoDataProc::getNoDataHtml(
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const OmnString &tagname,
		const AosXmlTagPtr &gic_def,
		const AosXmlTagPtr &gic,
		const AosRundataPtr &rdata)
{
	// No data found. 
	AosXmlTagPtr nodata_tag = gic_def->getFirstChild(tagname);
	OmnString html;
	if (nodata_tag)
	{
		html = nodata_tag->getNodeText();
	}

	if (html == "")
	{
		AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
		aos_assert_rr(docclient, rdata, "");
		OmnString objid = AosObjid::composeNoDataObjid();
		AosXmlTagPtr doc = docclient->getDocByObjid(objid, rdata);

		OmnString html;
		if (!doc)
		{
			if (!sgDefaultDocRead)
			{
				readDefaultDoc();
			}

			doc = sgDefaultDocRead;
		}

		if (doc)
		{
			OmnString xpath = gic->getAttrStr(AOSTAG_GICTYPE);
			if (xpath != "")
			{
				xpath << "/_#text";
				html = tag->xpathQuery(xpath);
			}

			if (html == "")
			{
				xpath = AOSTAG_DEFAULT_NODATA;
				xpath << "/_#text";
				html = tag->xpathQuery(xpath);
			}
		}

		if (html == "")
		{
			html = sgHtml;
		}
	}

	AosHtmlProc::proc(html, vpd, obj, gic_def, gic, rdata);
	return html;
}

