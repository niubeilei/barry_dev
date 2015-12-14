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
// 02/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupSvr/BackupProcDocModified.h"

#include "RemoteBackupSvr/BackupProc.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "XmlUtil/XmlTag.h"


AosBackupProcDocModified::AosBackupProcDocModified(const bool flag)
:
AosBackupProc(AOSREMOTEBACK_MODIFYDOC, AosRemoteBkType::eDocModified, flag)
{
}


bool 
AosBackupProcDocModified::proc(
		const u64 &transid, 
		const AosXmlTagPtr &req, 
		const AosRundataPtr &rdata)
{
	// A doc was modified, the doc was received by this server. It needs
	// to save the doc. 
	aos_assert_rr(req, rdata, false);
	AosXmlTagPtr doc = req->getFirstChild();
	aos_assert_rr(doc, rdata, false);
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "= \"" << "modifyobj" << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";

	addDocReq(docstr, doc->getAttrU64(AOSTAG_DOCID, 0), rdata);
	return false;
}

