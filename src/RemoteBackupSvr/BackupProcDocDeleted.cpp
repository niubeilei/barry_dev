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
#include "RemoteBackupSvr/BackupProcDocDeleted.h"

#include "RemoteBackupSvr/BackupProc.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "XmlUtil/XmlTag.h"


AosBackupProcDocDeleted::AosBackupProcDocDeleted(const bool flag)
:
AosBackupProc(AOSREMOTEBACK_DELETEDOC, AosRemoteBkType::eDocDeleted, flag)
{
}


bool 
AosBackupProcDocDeleted::proc(
		const u64 &transid, 
		const AosXmlTagPtr &req, 
		const AosRundataPtr &rdata)
{
	// A doc was delete, the doc was received by this server. It needs
	// to save the doc. 
	aos_assert_rr(req, rdata, false);
	AosXmlTagPtr doc = req->getFirstChild();
	aos_assert_rr(doc, rdata, false);
	aos_assert_r(doc, false);
	OmnString docstr = "<trans ";
	docstr << AOSTAG_TYPE << "= \"" << "deleteobj" << "\" "
		<< AOSTAG_DOC_REQTYPE << "=\"" << AosDocReq::eDeleteDoc<< "\" "
		<< AOSTAG_DOCID << "=\"" << doc->getAttrU64(AOSTAG_DOCID, 0)<< "\" >"
		<< "<origdocroot>" << doc->toString() << "</origdocroot>"
		<< "</trans>";

	addDocReq(docstr, doc->getAttrU64(AOSTAG_DOCID, 0), rdata);
	return false;
}

