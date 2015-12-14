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
//	
// Modification History:
// 08/16/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "StorageMgrUtil/FileInfo.h"

#include "API/AosApiG.h"
#include "SEBase/SysVersion.h"
#include "SEInterfaces/VfsMgrObj.h"


u32 AosStorageFileInfo::smNumFilesPerDir = 25000;


OmnString
AosStorageFileInfo::getFullFname() const
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, "");
	
	OmnString full_fname = vfsMgr->getBaseDir(deviceId);

	// Chen Ding, 2014/08/02
	if (AosSysVersion::getCrtVfsVersion() == AosSysVersion::eVfsVersion_2)
	{
		// This version assumes the following file structure
		// /<device_base_dir>/VirtualFiles/<siteid>/<dir_seqno>/<cubeid>/<fileid>_<fname>
		u32 dir_seqno = getDirSeqno();
		u32 cubeid = getCubeId();
		u32 local_file_id = getLocalFileId();
		full_fname << "/VirtualFiles/" << siteId << "/" << dir_seqno
			<< "/" << cubeid << "/" << local_file_id << "_";
	}

	if (fname.indexOf(full_fname, 0) == 0)
	{
		return fname;
	}

	full_fname << fname;
	return full_fname;
}

	
int
AosStorageFileInfo::getBkpSvrid()
{
	// Ketty 2013/07/17
	OmnNotImplementedYet;
	return -1;
	//u32 virtual_id = (u32)(fileId>> 32);
	//int backup_svrid = AosGetBackupSvrId(virtual_id);
	//return backup_svrid;
}

