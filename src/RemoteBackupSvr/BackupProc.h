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
#ifndef AOS_RemoteBackupSvr_BackupProc_h
#define AOS_RemoteBackupSvr_BackupProc_h

#include "Alarm/Alarm.h"
#include "RemoteBackupSvr/Ptrs.h"
#include "RemoteBackupSvr/AllBackupProcs.h"
#include "RemoteBackupUtil/RemoteBkType.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosBackupProc : public OmnRCObject
{
	OmnDefineRCObject;
protected:
	AosRemoteBkType::E			mType;
	static AosDocBackupProcPtr	smDocBackupProc;
	static AosIILBackupProcPtr	smIILBackupProc;

public:
	AosBackupProc(
		const OmnString &name,
		const AosRemoteBkType::E type, 
		const bool regflag);

	virtual bool proc(	const u64 &transid, 
						const AosXmlTagPtr &doc, 
						const AosRundataPtr &rdata) = 0;

	static AosBackupProcPtr getProc(const AosRemoteBkType::E type);
	static bool procStatic(
						const AosRemoteBkType::E type, 
						const u64 &transid, 
						const AosXmlTagPtr &doc, 
						const AosRundataPtr &rdata)
	{
		AosBackupProcPtr proc = getProc(type);
		if (!proc)
		{
			OmnAlarm << "Unrecognized backup proc: " << type << enderr;
			return false;
		}
		return proc->proc(transid, doc, rdata);
	}

	static bool checkRegistrations();

protected:
	bool addDocReq(const OmnString &req, const u64 &docid, const AosRundataPtr &rdata);
	bool addIILReq(const OmnString &req, const u64 &iilid, const AosRundataPtr &rdata);

private:
	bool registerProc(const AosBackupProcPtr &proc, const OmnString &name);
	//static bool registerAll();
	//bool	startDocBackProc();
	//bool	startIILBackProc();
};
#endif

