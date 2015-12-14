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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SDocDelFloatingVpd_h
#define Aos_SmartDoc_SDocDelFloatingVpd_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocDelFloatingVpd : public AosSmartDoc
{
public:
	AosSdocDelFloatingVpd(const bool flag);
	~AosSdocDelFloatingVpd();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocDelFloatingVpd(false);}
	virtual	bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	bool query(
		const AosXmlTagPtr &query_xml,
		const AosRundataPtr &rdata);
	bool	modifyDoc(
		AosXmlTagPtr &doc,
		const OmnString &zky_tagname,
		const OmnString &zky_gictype);
};
#endif
