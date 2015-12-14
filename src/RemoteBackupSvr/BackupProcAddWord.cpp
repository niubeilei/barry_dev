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
#include "RemoteBackupSvr/BackupProcAddWord.h"

#include "IILUtil/IILFuncType.h"
#include "RemoteBackupSvr/BackupProc.h"
#include "Rundata/Rundata.h"
#include "SearchEngine/DocReq.h"
#include "XmlUtil/XmlTag.h"


AosBackupProcAddWord::AosBackupProcAddWord(const bool flag)
:
AosBackupProc(AOSREMOTEBACK_ADDWORD, AosRemoteBkType::eAddWord, flag)
{
}


bool 
AosBackupProcAddWord::proc(
		const u64 &transid, 
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	// A doc was created, the doc was received by this server. It needs
	// to save the doc. 
	OmnString docstr = "<trans ";
	docstr<< "operation =\"" << AosIILFuncType::eAddWord << "\" "
		<< "word = \"" << doc->getAttrStr("zky_word", "") << "\" "
		<< "iilid =\"" << doc->getAttrStr(AOSTAG_IILID, "")<< "\" >"
		<< "</trans>";

	addIILReq(docstr, doc->getAttrU64(AOSTAG_IILID, 0), rdata);
	return false;
}

