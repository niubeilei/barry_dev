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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupClt/RemoteBkReq.h"

#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

bool
AosRemoteBkReq::toString(OmnString &str)
{
	str = "<request ";
	if (mType == AOSREMOTEBACK_SAVEIILS)
	{
		str << AOSTAG_TYPE << "=\"" << mType << "\" "
			<< AOSTAG_TRANSID << "=\"" << mTransId << "\" "
			<< AOSTAG_SITEID << "=\"" << mSiteid << "\" "
			<< AOSTAG_IILID << "=\"" << mIILId << "\" >"
			<< "<![BDATA[" << mBuff->dataLen() << ":";
		str.append(mBuff->data(), mBuff->dataLen());
		str	<< "]]>"
			<< "</request>";
		return true;
	}

	if (mType == AOSREMOTEBACK_ADDWORD)
	{
		str << AOSTAG_TYPE << "=\"" << mType << "\" "
			<< AOSTAG_TRANSID << "=\"" << mTransId << "\" "
			<< AOSTAG_SITEID << "=\"" << mSiteid << "\" "
			<< "zky_word" << "=\"" << mWord << "\" " 
			<< AOSTAG_IILID << "=\"" << mIILId << "\" />";
AosXmlParser parser;
AosXmlTagPtr record_root = parser.parse(str, "" AosMemoryCheckerArgs);
if (!record_root)
	OmnMark;
		return true;
	}

	str << AOSTAG_TYPE << "=\"" << mType 
		<< "\" " << AOSTAG_TRANSID << "=\"" << mTransId << "\" " 
		<< AOSTAG_SITEID << "=\"" << mDoc->getAttrStr(AOSTAG_SITEID) << "\">"
		<< mDoc->toString()
		<< "</request>";
	return true;
}

