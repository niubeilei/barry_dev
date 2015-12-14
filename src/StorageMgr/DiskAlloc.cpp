////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 09/07/2011 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/DiskAlloc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/DiskUtil.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/StoragePolicy.h"
//#include "StorageMgr/VirtualFileSysMgr.h"
#include "StorageMgrUtil/FileInfo.h"
#include "XmlUtil/XmlTag.h"
#include <dirent.h>
#include <sys/types.h>

AosDiskAlloc::AosDiskAlloc(const AosXmlTagPtr &config)
:
mLock(OmnNew OmnMutex()),
mIsDamageDevice(false),
mIsNewDevice(false),
//mDeviceId(deviceId),
mTotalSize(0),
mAllocedSize(0)
{
	// config format:
	// <partition dev="/dev/sda1" userdir="/home/ketty/AOS/Data" 
	// 		mountpoint="/" totalsize="..." >
	// 		<policy ... />
	// </partition>
	// if not config the userdir, use mountpoint.
	if (!config)
	{
		OmnThrowException("Missing configuration");
		return;
	}
	mDeviceId = config->getAttrInt("device_id", -1);
	if(mDeviceId == -1)
	{
		OmnThrowException("Missing device_id");
		return;
	}

	try
	{
		mDevLocation = OmnNew AosDevLocation1(config);
		AosXmlTagPtr policyConfig = config->getFirstChild(AOSCONFIG_STORAGE_POLICY);
		mPolicy = AosStoragePolicy::createStoragePolicy(policyConfig);
		mTotalSize = config->getAttrU64("totalsize", 0) * 1024 * 1024 *1024;	// GB
		
		// Chen Ding, 12/24/2012
		if (mTotalSize <= 0)
		{
			OmnThrowException("Missing total size");
			return;
		}

		init();
	}
	catch (...)
	{
		OmnAlarm << "Failed creating DiscAlloc: " << config->toString() << enderr;
		OmnThrowException("Failed creating DiskAlloc");
		return;
	}
}


void
AosDiskAlloc::init()
{
	// init the mDirInfo.
	mDirInfo = OmnNew AosStorageDirInfo(getBaseDir(), "");

	vector<OmnString> vir_fnames;
	bool rslt = mDirInfo->getAllFnames(vir_fnames);
	aos_assert(rslt);
	for(u32 i=0; i<vir_fnames.size(); i++)
	{
		OmnString sub_fname_vir = vir_fnames[i].substr(vir_fnames[i].find('/', true) + 1);
		if(!sub_fname_vir.hasPrefix("vir_"))    continue;

		int end_loc = sub_fname_vir.find('_', true);
		if(end_loc == -1)	end_loc = sub_fname_vir.length();
		u32 virtual_id = atoi(sub_fname_vir.substr(strlen("vir_"), end_loc).data());

		OmnString postfix = sub_fname_vir.substr(end_loc +1);
		if(postfix == "backup")
		{
			// Ketty 2013/07/17
			OmnNotImplementedYet;
			//virtual_id = AosGetBackupVirtualId(virtual_id); 
		}
		else
		{
			if(postfix != "orig")
			{
				OmnAlarm << "unknown dirname:" << vir_fnames[i] << enderr;
				continue;
			}
		}

		AosVirtualDirInfoPtr v_dirinfo = OmnNew AosVirtualDirInfo(virtual_id, getBaseDir(), sub_fname_vir);
		mVirtualDirMap.insert(make_pair(virtual_id, v_dirinfo));
	}
	
	checkDevice();
}


AosDiskAlloc::~AosDiskAlloc()
{
}

u64
AosDiskAlloc::getAvailableSize()
{
	return mPolicy->getAvailableSize(mAllocedSize, mTotalSize);		
}


bool
AosDiskAlloc::allocateSpace(
		AosRundata *rdata,
		const u32 virtual_id,
		const u64 request_space,
		const bool reserve_flag,
		OmnString &sub_dir_name)
{
	// This function allocate and reserved some space.
	// generate an sub_dir_name 
	
	aos_assert_r(rdata, false);
	u32 site_id = rdata->getSiteid();
	aos_assert_r(site_id, false);

	// Ketty 2013/03/22
	//bool rslt = mPolicy->canAllocateSpace(request_space, mAllocedSize, mTotalSize, rdata);
	//if(!rslt)	{	return false;	}

	//if(reserve_flag)
	//{
	mLock->lock();
	mAllocedSize += request_space;
	mLock->unlock();
	//}
	
	sub_dir_name = getNextSubDirname(virtual_id, site_id);
	aos_assert_r(sub_dir_name != "", false);
	return true;
}


OmnString
AosDiskAlloc::getSysDir()
{
	OmnString base_dir = getBaseDir();
	base_dir << "AOS_SYSTEM";
		
	DIR *dir = opendir(base_dir.data());
	if(!dir)
	{
		// make the new dir.
		mkdir(base_dir.data(), 0755);
	}
	base_dir << "/";
	return base_dir;
}


OmnString
AosDiskAlloc::getVirtualSysDir(const u32 virtual_id)
{
	AosVirtualDirInfoPtr v_dirinfo = getVirtualDirInfo(virtual_id);
	aos_assert_r(v_dirinfo, "");
	return v_dirinfo->getDirname();	
}


AosVirtualDirInfoPtr
AosDiskAlloc::getVirtualDirInfo(const u32 virtual_id, bool create)
{
	mLock->lock();
	map<u32, AosVirtualDirInfoPtr>::iterator it = mVirtualDirMap.find(virtual_id);
	if(it != mVirtualDirMap.end())
	{
		mLock->unlock();
		return it->second;
	}
	if(!create)
	{
		mLock->unlock();
		return 0;
	}

	// create a virtual Dir for this virtual_id.	
	OmnString v_dirname = mDirInfo->getNextDataDirname();
	//bool is_backup = AosVirtualFileSys::isBackupVFS(virtual_id);
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);

	bool is_backup = false;
	//bool rslt = vfsMgr->isBackupVirtual(virtual_id, is_backup);
	//bool rslt = AosVirtualFileSysMgr::getSelf()->isBackupVirtual(virtual_id, is_backup);
	//aos_assert_r(rslt, 0);

	if(is_backup)
	{
		//u32 orig_vid = AosVirtualFileSys::getPureVirtualId(virtual_id);
		//v_dirname << "vir_" << orig_vid << "_backup";
		v_dirname << "vir_" << virtual_id << "_backup";
	}
	else
	{
		v_dirname << "vir_" << virtual_id  << "_orig";
	}
	AosVirtualDirInfoPtr v_dirinfo = OmnNew AosVirtualDirInfo(virtual_id, getBaseDir(), v_dirname);
	mVirtualDirMap.insert(make_pair(virtual_id, v_dirinfo));
	mLock->unlock();
	return v_dirinfo;	
}


OmnString 
AosDiskAlloc::getNextSubDirname(const u32 virtual_id, const u32 site_id)
{
	AosVirtualDirInfoPtr v_dirinfo = getVirtualDirInfo(virtual_id);
	aos_assert_r(v_dirinfo, "");
	
	return v_dirinfo->getNextSubDirname(site_id); 
}


bool
AosDiskAlloc::removeDir(const u32 virtual_id)
{
	// This func is called only when move Data
	OmnNotImplementedYet;
	/*
	AosStorageDirInfoPtr virtual_dirinfo = getDirInfo(virtual_id);

	vector<OmnString> site_fnames;
	bool rslt = virtual_dirinfo->getAllFnames(site_fnames);
	aos_assert_r(rslt, false);
	for(u32 i=0; i<site_fnames.size(); i++)
	{
		OmnString sub_fname_site = site_fnames[i].substr(site_fnames[i].find('/', true) + 1);
		if(!sub_fname_site.hasPrefix("site_"))    continue;
		u32 site_id = atoi(sub_fname_site.substr(strlen("site_")).data());
		aos_assert_r(site_id, false);

		AosStorageDirInfoPtr site_dirinfo = getDirInfo(virtual_id, site_id);
		site_dirinfo->removeDataDir();
	}

	OmnString full_vir_dir = virtual_dirinfo->mBaseDirname;
	full_vir_dir << virtual_dirinfo->mDirname;
	unlink(full_vir_dir.data());
	*/	
	return true;
}

OmnString
AosDiskAlloc::getDiskInfo()
{
	u32 k = 1024 * 1024;
	OmnString cont = "<disk ";
	//Jozhi 2015-04-20 no one use it 
	//cont << "devname=\"" << mDevLocation->getDevName() << "\" "
	cont << "userdir=\"" << mDevLocation->getUserDir() << "\" "
		 << "totalsize=\"" << (mTotalSize/k) << "MB\" "
		 << "allocedsize=\"" << (mAllocedSize/k) << "MB\" "
		 << "/>";
	return cont;
}


bool
AosDiskAlloc::getTotalFiles(set<AosStorageFileInfo> &files, const u32 cube_id)
{
	// for disk recover
	AosVirtualDirInfoPtr v_dirinfo = getVirtualDirInfo(cube_id, false);
	if(!v_dirinfo)	return true;

	return v_dirinfo->getTotalFiles(files, mDeviceId); 
}


bool
AosDiskAlloc::checkDevice()
{
	// check whether disk is good. or disk is new.
	mIsDamageDevice = false;
	OmnScreen << "check device damaged not implemented yet." << endl;

	OmnString device_fname = getBaseDir(); 
	device_fname << "device_imp";
	OmnFilePtr device_file = OmnNew OmnFile(device_fname, 
			OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!device_file || !device_file->isGood())
	{
		// means this is a new file. means is a new device.
		mIsNewDevice = true;
		return true;
	}
		
	u32 dd = device_file->readBinaryU32(0, 0);
	aos_assert_r(dd == ePoison, false);
	mIsNewDevice = false;
	return false;
}


bool
AosDiskAlloc::setNewDiskAvail()
{
	if(!mIsNewDevice)	return true;

	OmnString device_fname = getBaseDir(); 
	device_fname << "device_imp";
	OmnFilePtr device_file = OmnNew OmnFile(device_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!device_file->isGood())
	{
		OmnAlarm << "Failed to create the device file: " << device_fname << enderr;
		return false;
	}
	
	device_file->setU32(0, ePoison, true);	
	mIsNewDevice = false;
	return true;
}

