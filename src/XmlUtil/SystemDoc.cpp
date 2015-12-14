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
// System Docs are optional to XML docs. System docs are mainly used to 
// keep the system/operational information that should not be normally
// available to end users. Not all XML docs have system docs. If an XML
// doc does have a system doc, its system doc's objid is:
// 		AOSZTG_SYSTEM_DOC + "_" + docid
// where 'docid' is the docid of the original XML doc.
//
// System docs are themselves XML docs. There are no standard on the format
// of system docs except:
// 	AOSTAG_OTYPE must be AOSOTYPE_SYSTEM_DOC
// 	AOSTAG_OWNER_DOCID is the docid of its associated XML doc
// 
// Modification History:
// 09/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/SystemDoc.h"

#include "Debug/Except.h"
#include "ErrorMgr/ErrmsgId.h"
#include "Rundata/Rundata.h"


AosSystemDoc::AosSystemDoc(
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
:
mDoc(doc)
{
	if (!doc)
	{
		AosSetError(rdata, AosErrmsgId::eMissingDoc);
		OmnThrowException("Missing doc");
	}
}

