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
#include "GicCodeGen/GicGenDataCol.h"

AosGicGenDataCol::AosGicGenDataCol(const bool flag)
:
AosGicCodeGen(AOSGICGEN_DATACOL, AosGicGenType::eDataCol)
{
}


AosGicGenDataCol::~AosGicGenDataCol()
{
}


bool 
AosGicGenDataCol::generateCodeStatic(
		const AosXmlTagPtr &vpd, 
		const AosXmlTagPtr &obj, 
		const AosXmlTagPtr &gic_def,
		const AosXmlTagPtr &gic,
		AosHtmlCode &code, 
		const AosRundataPtr &rdata)
{
	// This function generates the code for the gic 'gic'. 
	// It assumes that 'gic_def' is in the following format:
	// 	<gicdef ...>
	// 		<contents .../>
	// 		<nodata><![CDATA[xxx]]></nodata>
	//  </gicdef>
	aos_assert_rr(vpd, rdata, false);
	aos_assert_rr(obj, rdata, false);
	aos_assert_rr(gic_def, rdata, false);
	AosXmlTagPtr postfix = gic_def->getFirstChild(AOSTAG_POSTFIX);
	OmnString poststr;
	
	// Retrieve <contents>
	OmnString str;

	// Retrieve the data collector tagname
	OmnString dcl_tagname = gic_def->getAttrStr(AOSTAG_DCL_TAGNAME);
	if (dcl_tagname == "")
	{
		AosSetError(rdata, "missing dcl_tagname") << ": " << gic_def->toString();
		rdata->logError();
		OmnString msg = AOSTERM("missing_dcl_tagname_err");
		str = AosGicErrorProc::getErrHtml(vpd, obj, gic_def, gic, __FILE__, __LINE__, rdata);
		code.appendHtml(str);
		return false;
	}

	AosXmlTagPtr datacol = vpd->getFirstChild(dcl_tagname);
	if (!datacol)
	{
		AosSetError(rdata, "datacol_not_found") << ": " << gic_def->toString()
			<< ": " << vpd->getAttrStr(AOSTAG_OBJID);
		rdata->logError();
		OmnString msg = AOSTERM("datacol_not_found_err");
		str = AosGicErrorProc::getErrHtml(vpd, obj, gic_def, gic, __FILE__, __LINE__, rdata);
		code.appendHtml(str);
		return false;
	}

	AosXmlTagPtr results = AosDataCol::retrieveData(datacol, vpd, obj, gic_def, rdata);
	if (!results)
	{
		str = AosNoDataProc::getHtml(vpd, obj, AOSTAG_NODATA, gic_def, gic, rdata);
		code.appendHtml(str);
		return true;
	}

	// Retrieved the data. Will loop over the contents.
	AosXmlTagPtr entry = results->getFirstChild();
	if (!entry)
	{
		str = AosNoDataProc::getHtml(vpd, obj, AOSTAG_NODATA, gic_def, gic, rdata);
		code.appendHtml(str);
		return true;
	}

	int num = 0;
	while (entry)
	{
		OmnString ss = generateEntry(vpd, obj, entry, gic_def, gic, rdata);
		if (ss != "")
		{
			str << ss;
			num++
		}
		entry = results->getNextChild();
	}

	if (num == 0)
	{
		str = AosNoDataProc::getHtml(vpd, obj, AOSTAG_NODATA, gic_def, gic, rdata);
		code.appendHtml(str);
		return true;
	}

	if (poststr)
	{
		str << poststr;
	}

	code.appendHtml(str);
	return true;
}

