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
// 11/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocRelation2106_h
#define Aos_SmartDoc_SdocRelation2106_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocRelation2106 : public AosSmartDoc
{
private:

public:
	AosSdocRelation2106(const bool flag);
	~AosSdocRelation2106();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocRelation2106(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	OmnString calculateObjid(
				const vector<OmnString> &m_values, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);
	AosXmlTagPtr getFamilyDoc(
				const OmnString &objid, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);
};
#endif
