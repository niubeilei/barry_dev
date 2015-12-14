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
// 02/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_RemoteBkCltObj_h
#define Aos_SEInterfaces_RemoteBkCltObj_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Docid.h"
#include "SEInterfaces/Ptrs.h"
#include "UserMgmt/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosRemoteBkCltObj : virtual public OmnRCObject
{
private:
	static bool 				smWithRemoteBackup;
	static AosRemoteBkCltObjPtr smRemoteBkClt;

public:
	virtual bool docCreated(const AosXmlTagPtr &doc, const AosRundataPtr &rdata) = 0;
	virtual bool docDeleted(const AosXmlTagPtr &doc, const AosRundataPtr &rdata) = 0;
	virtual bool docModified(const AosXmlTagPtr &doc, const AosRundataPtr &rdata) = 0;
	virtual bool saveIILs(const u64 &iilid, const AosBuffPtr &tmpfilebuff, const AosRundataPtr &rdata) = 0;
	virtual bool addWord(const u64 &iilid, const OmnString &word, const AosRundataPtr &rdata) = 0;

	static AosRemoteBkCltObjPtr getRemoteBkClt() {return smRemoteBkClt;}
	static void setRemoteBkClt(const AosRemoteBkCltObjPtr &d) {smRemoteBkClt = d;}
	static bool withRemoteBackup() {return smWithRemoteBackup;}
	static void setRemoteBackup(const bool b) {smWithRemoteBackup = b;}
};

inline bool AosRemoteBkDocCreated(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	if (!AosRemoteBkCltObj::withRemoteBackup()) return true;
	AosRemoteBkCltObjPtr client = AosRemoteBkCltObj::getRemoteBkClt();
	aos_assert_r(client, 0);
	return client->docCreated(doc, rdata);
}

inline bool AosRemoteBkDocModified(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	if (!AosRemoteBkCltObj::withRemoteBackup()) return true;
	AosRemoteBkCltObjPtr client = AosRemoteBkCltObj::getRemoteBkClt();
	aos_assert_r(client, 0);
	return client->docModified(doc, rdata);
}

inline bool AosRemoteBkDocDeleted(const AosXmlTagPtr &doc, const AosRundataPtr &rdata)
{
	if (!AosRemoteBkCltObj::withRemoteBackup()) return true;
	AosRemoteBkCltObjPtr client = AosRemoteBkCltObj::getRemoteBkClt();
	aos_assert_r(client, 0);
	return client->docDeleted(doc, rdata);
}

inline bool AosRemoteBkIIL(const u64 &id, const AosBuffPtr &tmpfilebuff, const AosRundataPtr &rdata)
{
	if (!AosRemoteBkCltObj::withRemoteBackup()) return true;
	AosRemoteBkCltObjPtr client = AosRemoteBkCltObj::getRemoteBkClt();
	aos_assert_r(client, 0);
	return client->saveIILs(id, tmpfilebuff, rdata);
}

inline bool AosRemoteBkAddWord(const u64 &id, const OmnString &word, const AosRundataPtr &rdata)
{
	if (!AosRemoteBkCltObj::withRemoteBackup()) return true;
	AosRemoteBkCltObjPtr client = AosRemoteBkCltObj::getRemoteBkClt();
	aos_assert_r(client, 0);
	return client->addWord(id, word, rdata);
}
#endif

