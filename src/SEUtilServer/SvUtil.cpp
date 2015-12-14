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
// 2011/03/01	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtilServer/SvUtil.h"

#include "SEUtil/DocTags.h"
#include "SEUtil/Ptrs.h"
#include "Util/UtUtil.h"
#include "Security/Session.h"
#include "XmlUtil/XmlTag.h"

const OmnString sgSystemKey = "datg643jdsata64od";

//Zky2789
OmnString  AosCalHashkey(const AosSessionPtr &session, 
		const AosXmlTagPtr &doc)
{
	// This function calculates a hash based on:
	// 		session_id + system_key + docid + session_secret 
	// and set the hash value to 'doc'.
	// Note that the hashkey is converted to ASCII using Base64 algorithm.
	
	aos_assert_r(session, "");
	aos_assert_r(doc, "");

	u64 session_id = session->getSessionId();
	u64 timestamp = session->getSecretKey();
	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	OmnString hashkey = AosCalHashkey(session_id, timestamp, docid, sgSystemKey);

	return hashkey;
}


OmnString AosVerifyHashkey(const AosSessionPtr &session,
		 const AosXmlTagPtr &doc)
{
	OmnNotImplementedYet;
	return "";
}

