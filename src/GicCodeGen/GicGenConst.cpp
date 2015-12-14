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
#include "GicCodeGen/GicGenConst.h"

AosGicGenConst::AosGicGenConst(const bool flag)
:
AosGicCodeGen(AOSGICGEN_DATACOL, AosGicGenType::eDataCol)
{
}


AosGicGenConst::~AosGicGenConst()
{
}


bool 
AosGicGenConst::generateCodeStatic(
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
	//  </gicdef>
	aos_assert_rr(vpd, rdata, false);
	aos_assert_rr(obj, rdata, false);
	aos_assert_rr(gic_def, rdata, false);
	
	OmnString str;
	OmnString ss = gic_def->getNodeText();
	if (ss == "") return true;
	
	if (!AosHtmlProc::procHtml(ss, vpd, obj, rdata))
	{
		AosSetError(rdata, "failed_proc_html") << ": " << gic_def->toString();
		rdata->logError();
		OmnString msg = AOSTERM("failed_proc_html_err");
		str = AosGicErrorProc::getErrHtml(vpd, obj, gic_def, gic, __FILE__, __LINE__, rdata);
		code.appendHtml(str);
		return false;
	}
	str << ss;
	code.appendHtml(str);
	return true;
}

