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
#include "RemoteBackupSvr/BackupProcDocCreated.h"

#include "RemoteBackupSvr/BackupProc.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "XmlUtil/XmlTag.h"


AosBackupProcDocCreated::AosBackupProcDocCreated(const bool flag)
:
AosBackupProc(AOSREMOTEBACK_CREATEDOC, AosRemoteBkType::eDocCreated, flag)
{
}


bool 
AosBackupProcDocCreated::proc(
		const u64 &transid, 
		const AosXmlTagPtr &req, 
		const AosRundataPtr &rdata)
{
	// A doc was created, the doc was received by this server. It needs
	// to save the doc. 
	AosXmlTagPtr doc = req->getFirstChild();
	aos_assert_r(doc, false);
	OmnString docstr = "<trans ";
	docstr<< AOSTAG_TYPE << "=\"" << "createdoc"<< "\" "
		<< AOSTAG_DOC_REQTYPE << "=\"" << AosDocReq::eCreateDoc << "\" >"
		<< "<doc>" << doc->toString() << "</doc>"
		<< "</trans>";

	addDocReq(docstr, doc->getAttrU64(AOSTAG_DOCID, 0), rdata);
	return false;
}

