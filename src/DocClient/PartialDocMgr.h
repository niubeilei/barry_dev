////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 07/20/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocClient_PartialDocMgr_h
#define AOS_DocClient_PartialDocMgr_h

#include "aosUtil/Types.h"
#include "IdGen/IdGenMgr.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosPartialDocMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	AosPartialDocMgr();
	~AosPartialDocMgr();

	static bool	processPartialDoc(
					AosXmlTagPtr &partial_doc, 
					const AosRundataPtr &rdata, 
					bool &doc_modified);

	static	bool modifyDocByFields(
				AosXmlTagPtr &exist_doc,
				const AosXmlTagPtr &partial_doc,
				const OmnString &fields, 
				const AosRundataPtr &rdata);

	static bool	modifyDocByFields(
					AosXmlTagPtr &exist_doc,
					const AosXmlTagPtr &partial_doc,
					const OmnString &oname, 
					const OmnString &cname,
					const OmnString &type, 
					bool &modified, 
					const AosRundataPtr &rdata);

	static bool	traversalXml(
				AosXmlTagPtr &exist_doc,
				const AosXmlTagPtr &partial_doc);
};

#endif
