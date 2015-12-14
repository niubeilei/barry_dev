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
// 11/16/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocRelation1101_h
#define Aos_SmartDoc_SdocRelation1101_h

#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Actions/ActAddAttr.h"
#include "Actions/SdocAction.h"
#include "XmlUtil/XmlTag.h"
#include <vector>

class AosSdocRelation1101 : public AosSmartDoc
{
public:
	AosSdocRelation1101(const bool flag);
	~AosSdocRelation1101();

	virtual AosSmartDocObjPtr	clone() {return OmnNew AosSdocRelation1101(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	OmnString	calculateObjid(
			const vector<OmnString> m_values,
			const vector<OmnString> v_values,
			const vector<OmnString> a_values,
			const vector<OmnString> k_values,
			const AosRundataPtr &rdata);

	OmnString 	getVectorValue(const vector<OmnString> values);

	AosXmlTagPtr	getRelationDoc(
			const OmnString &objid,
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
};
#endif
