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
#ifndef Aos_SEInterfaces_DocMgrObj_h
#define Aos_SEInterfaces_DocMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosDocMgrObj : virtual public OmnRCObject
{
private:
	static AosDocMgrObjPtr smDocMgr;

public:
	virtual bool	addCopy(const u64 &docId,
							const AosXmlTagPtr &doc,
							const AosRundataPtr &rdata) = 0;
	virtual bool	addDoc(const AosXmlTagPtr &doc, 
							const u64 &docid,
							const AosRundataPtr &rdata) = 0;
	virtual bool	deleteDoc(const u64 &docId,
							const AosRundataPtr &rdata) = 0;
	virtual AosXmlTagPtr 	getDocByDocid(
								const u64 &docId,
								const AosRundataPtr &rdata) = 0;
	virtual AosXmlTagPtr 	getDocByDocid(
								const OmnString &docId,
								const AosRundataPtr &rdata) = 0;
	virtual OmnString getLogs() = 0;

	static AosDocMgrObjPtr getDocMgr() {return smDocMgr;}
	static void setDocMgr(const AosDocMgrObjPtr &d) {smDocMgr = d;}
};
#endif
