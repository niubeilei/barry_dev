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
// 'fileId' is in the following format:
// 		Type 			1 byte
// 		CubeId			3 bytes
// 		Local FileId	4 bytes
//	
// Modification History:
// 08/16/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgrUtil_FileInfo_h
#define AOS_StorageMgrUtil_FileInfo_h

#include "Util/Buff.h"
#include "Util/String.h"
#include "StorageMgrUtil/SignatureAlgo.h"


struct AosStorageFileInfo
{
	u64 		fileId;
	u32 		siteId;
	int 		deviceId;
	u64 		requestSize;
	OmnString 	fname;
	
	static u32 smNumFilesPerDir;

	AosStorageFileInfo()
	:
	fileId(0),
	siteId(0),
	deviceId(-1),
	requestSize(0)
	{
	}	

	AosStorageFileInfo(
			const u64 file_id,
			const u32 site_id, 
			const int device_id,
			const u64 request_size,
			const OmnString &f_name)
	:
	fileId(file_id),
	siteId(site_id),
	deviceId(device_id),
	requestSize(request_size),
	fname(f_name)
	{
		//if (device_id >= 0) fname = getFullFname();
	}
	
	bool operator < (const AosStorageFileInfo &lhs) const
	{
		return fileId < lhs.fileId;
	}
	
	void 	init(
				const u64 file_id,
				const u32 site_id, 
				const int device_id,
				const u64 request_size,
				const OmnString &f_name)
	{
		fileId = file_id;
		siteId = site_id;
		deviceId = device_id;
		requestSize = request_size;
		fname = f_name;
		//if (device_id >= 0) fname = getFullFname();
	}	
	
	OmnString getPureFname()
	{
		int pos = fname.find('/', true);
		if(pos == -1)	return fname;

		OmnString pure_fname = fname.substr(pos+1);
		return pure_fname;
	}
	
	OmnString getFullFname() const;

	int getBkpSvrid();

	bool	isValid()
	{
		return (fileId != 0 && fname != "");
	}

	OmnString toString() const
	{
		OmnString ss;
		ss << "FileInfo: FileId=" << fileId 
			<< ", SiteId=" << siteId 
			<< ", DeviceId=" << deviceId
			<< ", RequestSize=" << requestSize
			<< ", FileName=" << fname;
		return ss;
	}

	inline u32 getDirSeqno() const
	{
		return ((u32)fileId) / smNumFilesPerDir;
	}

	inline u32 getCubeId() const
	{
		return (fileId >> 32) & 0xffffff;
	}

	inline u32 getLocalFileId() const
	{
		return (u32)fileId;
	}
};

#endif

