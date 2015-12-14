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
// 11/09/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocRelation2107_h
#define Aos_SmartDoc_SdocRelation2107_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocRelation2107 : public AosSmartDoc
{
private:

public:
	AosSdocRelation2107(const bool flag);
	~AosSdocRelation2107();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocRelation2107(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	OmnString 	calculateObjid(
				const vector<OmnString> &values, 
				const AosXmlTagPtr &sdoc,
				OmnString &sha1,
				const AosRundataPtr &rdata);

	AosXmlTagPtr getDoc(
				const OmnString &objid, 
				const AosXmlTagPtr &sdoc,
				const AosRundataPtr &rdata);

	OmnString	calculateRelationObjid(
				const OmnString &m_objid,
				const OmnString &b_objid,
				const AosRundataPtr &rdata);

	bool	incrementValue(
				const AosXmlTagPtr &sdoc,
				const AosXmlTagPtr &family_doc,
				const AosRundataPtr &rdata);
};
#endif
