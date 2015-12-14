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
// 06/28/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_GICs_Xml_Gen_h
#define Aos_GICs_Xml_Gen_h

#include "HtmlServer/Ptrs.h"
#include "HtmlServer/HtmlCode.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosGicXmlGen : virtual public OmnRCObject
{
public:
	AosGicXmlGen();
	~AosGicXmlGen();
	virtual bool	
	generateCode(
		const AosHtmlReqProcPtr &htmlPtr,
		AosXmlTagPtr &vpd,
		const AosXmlTagPtr &obj,
		const OmnString &parentId,
		AosHtmlCode &code);

    static OmnString getVpdXml(const OmnString &gictype, const AosRundataPtr &rdata);
	static OmnString getVpdXml(const AosRundataPtr &rdata);
	static OmnString getRandGicType();

};
#endif
