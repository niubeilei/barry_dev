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
// 01/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_SmartDocObj_h
#define Aos_SEInterfaces_SmartDocObj_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSmartDocObj : virtual public OmnRCObject
{
private:
	static AosSmartDocObjPtr smSmartDoc;

public:
	// Chen Ding, 11/28/2012
	virtual bool config() = 0;
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) = 0;
	virtual bool runSmartdocs(const OmnString &sdoc_objids, const AosRundataPtr &rdata) = 0;
	// virtual AosXmlTagPtr createDoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) = 0;
	virtual bool runSmartdoc(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata) = 0;
	virtual bool procSmartdocs(const OmnString &sdoc_objids, const AosRundataPtr &rdata) = 0;
	virtual bool runSmartdoc(const u64 &sdocid, const AosRundataPtr &rdata) = 0;

	virtual void setSdoc(const AosXmlTagPtr &sdoc) = 0;
	virtual AosSmartDocObjPtr getNext() const = 0;
	virtual AosSmartDocObjPtr getPrev() const = 0;
	virtual void setNext(const AosSmartDocObjPtr &s) = 0;
	virtual void setPrev(const AosSmartDocObjPtr &s) = 0;

	static bool runSmartdocStatic(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);
	static bool procSmartdocsStatic(const OmnString &sdoc_objids, const AosRundataPtr &rdata);
	static AosSmartDocObjPtr getSmartDocObj() {return smSmartDoc;}
	static void setSmartDocObj(const AosSmartDocObjPtr &d) {smSmartDoc = d;}
	static bool procSmartdocStatic(const u64 &sdocid, const AosRundataPtr &rdata);
	static bool runSmartdocsStatic(const OmnString &sdoc_objids, const AosRundataPtr &rdata);
};
#endif
