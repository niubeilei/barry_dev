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
// 01/30/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DocSelObj_h
#define Aos_SEInterfaces_DocSelObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosDocSelObj : virtual public OmnRCObject
{
private:
	static AosDocSelObjPtr smDocSelector;

public:
	virtual AosXmlTagPtr selectDoc(
			const AosXmlTagPtr &sdoc, 
			const AosRundataPtr &rdata) = 0;

	virtual AosXmlTagPtr selectDoc(
			const AosXmlTagPtr &sdoc, 
			const OmnString &tagname,
			const AosRundataPtr &rdata);

	static AosDocSelObjPtr getDocSelector() {return smDocSelector;}
	static void setDocSelector(const AosDocSelObjPtr &d) {smDocSelector = d;}
};
#endif
