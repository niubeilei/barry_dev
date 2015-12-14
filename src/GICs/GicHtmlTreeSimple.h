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
// 08/20/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_GicHtmlTreeSimple_h
#define Aos_GICs_GicHtmlTreeSimple_h

#include "GICs/GIC.h"

class AosGicHtmlTreeSimple : public AosGic
{
private:
	static OmnString	smEmptyTreeNotes;

public:
	AosGicHtmlTreeSimple(const bool flag);
	~AosGicHtmlTreeSimple();

	virtual bool generateCode(
			const AosHtmlReqProcPtr &htmlPtr,
			AosXmlTagPtr &vpd,
			const AosXmlTagPtr &obj,
			const OmnString &parentid,
			AosHtmlCode &code);
			
private:
	OmnString
	generateTemplate(OmnString &cont, const AosHtmlReqProcPtr &htmlPtr);

	OmnString
	createHtmlCode(
			const OmnString &temp,
			const OmnString &str_len, 
			const AosXmlTagPtr &record, 
			const AosHtmlReqProcPtr &htmlPtr);

	OmnString
	getNewValue(OmnString &val,const OmnString &str_len);

	bool isNode(
			const AosXmlTagPtr &record, 
			const AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj, 
			const AosRundataPtr &rdata);

	bool generateHtmlCode(
			const AosHtmlReqProcPtr &htmlPtr,
			const AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj,
			const OmnString &parentid,
			AosHtmlCode &code, 
			const AosRundataPtr &rdata);

	bool generateCssCode(
			const AosHtmlReqProcPtr &htmlPtr,
			const AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj,
			const OmnString &parentid,
			AosHtmlCode &code, 
			const AosRundataPtr &rdata);

	bool generateJsonCode(
			const AosHtmlReqProcPtr &htmlPtr,
			const AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj,
			const OmnString &parentid,
			AosHtmlCode &code, 
			const AosRundataPtr &rdata);

	bool generateJsCode(
			const AosHtmlReqProcPtr &htmlPtr,
			const AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj,
			const OmnString &parentid,
			AosHtmlCode &code, 
			const AosRundataPtr &rdata);

	bool generateEmptyTreeCode(
			const AosHtmlReqProcPtr &htmlPtr,
			const AosXmlTagPtr &vpd, 
			const AosXmlTagPtr &obj,
			const OmnString &parentid,
			AosHtmlCode &code, 
			const AosRundataPtr &rdata);
};
#endif

