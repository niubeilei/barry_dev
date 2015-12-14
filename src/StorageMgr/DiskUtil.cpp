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
// 08/16/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/DiskUtil.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include <dirent.h>
#include <sys/types.h>


/////////// The Follower func is for AosStorageDirInfo.
AosStorageDirInfo::AosStorageDirInfo(const OmnString &base_dir, const OmnString &dirname)
:
mLock(OmnNew OmnMutex()),
mBaseDirname(base_dir),
mDirname(dirname),
mDataDirname("__Dir"),
mCrtDataDirSeqno(0),
mCrtDataFileNum(0),
mInited(false)
{
	if(mDirname != "" )
	{
		u32 len = mDirname.length();
		if(mDirname.data()[len-1] != '/')	mDirname << "/";
	}
}

	
bool
AosStorageDirInfo::init()
{
	DIR *dir = 0;
	struct dirent *file = 0;

	OmnString fullDirname = mBaseDirname;
	fullDirname <<  mDirname;
	u32 seqno = 0, tryTimes = 0;
	while((dir = opendir(fullDirname.data())) || (tryTimes >= eMaxTry))
	{
		u32 numFiles = 0;
		//caculate the number of files in directory
		while((file= readdir(dir)) != NULL)
		{
			OmnString fileName = file->d_name;
			if(fileName == "." || fileName == "..") continue;
			numFiles++;
		}
		closedir(dir);
		if(numFiles < eMaxFileNumPerDir)
		{
			mCrtDataDirSeqno = seqno;
			mCrtDataFileNum = numFiles;
			mInited = true;
			return true;
		}
		fullDirname << mDataDirname << "_" << ++seqno << "/";
		tryTimes++;
	}
	
	if(tryTimes >= eMaxTry)
	{
		// Will not happend.
		OmnAlarm << "Can't get the dirname!" << enderr;
		return false;
	}

	// create this dir.
	mkdir(fullDirname.data(), 0755);
	mCrtDataDirSeqno = seqno;
	mCrtDataFileNum = 0;
	mInited = true;
	return true;
}


OmnString
AosStorageDirInfo::getNextDataDirname()
{
	mLock->lock();
	if(!mInited)
	{
		bool rslt = init();
		aos_assert_rl(rslt, mLock, "");
	}
	mCrtDataFileNum++;

	if(mCrtDataFileNum >= eMaxFileNumPerDir)
	{
		mCrtDataDirSeqno++;
		OmnString fullDirname = mBaseDirname;
		fullDirname <<  mDirname;
		for(u32 i=1; i <=mCrtDataDirSeqno; i++)
		{
			fullDirname << mDataDirname << "_"  << i << "/";
		}

		DIR *dir = opendir(fullDirname.data());
		if(!dir)
		{
			// make the new dir.
			mCrtDataFileNum = 0;
			mkdir(fullDirname.data(), 0755);
		}
		else
		{
			u32 numFiles = 0;
			//caculate the number of files in directory
			struct dirent *file = 0;
			while((file= readdir(dir)) != NULL)
			{
				OmnString fileName = file->d_name;
				numFiles++;
			}
			closedir(dir);
			
			mCrtDataFileNum = numFiles;
			aos_assert_rl(numFiles < eMaxFileNumPerDir, mLock, "");
		}
	}
	
	OmnString dirname = mDirname;
	for(u32 i=1; i <=mCrtDataDirSeqno; i++)
	{
		dirname << mDataDirname << "_"  << i << "/";
	}
	mLock->unlock();
	return dirname;
}


bool
AosStorageDirInfo::getAllFnames(vector<OmnString> &fnames)
{
	mLock->lock();
	if(!mInited)
	{
		bool rslt = init();
		aos_assert_rl(rslt, mLock, false);
	}

	DIR *dir = 0;
	struct dirent *file = 0;

	OmnString fullDirname = mBaseDirname;
	fullDirname << mDirname;
	OmnString subDirname = mDirname;
	for(u32 seqno=0; seqno <= mCrtDataDirSeqno; seqno++)
	{
		if(seqno >0)
		{
			subDirname << mDataDirname << "_" << seqno << "/";
			fullDirname << mDataDirname << "_" << seqno << "/";
		}

		dir = opendir(fullDirname.data());
		aos_assert_rl(dir, mLock, false);

		while((file= readdir(dir)) != NULL)
		{
			OmnString name = file->d_name;
			if(name == "." || name == ".."  )   continue;

			OmnString crtDataDirname = mDataDirname;
			//crtDataDirname << "_" << seqno;	// Ketty 2012/0726
			crtDataDirname << "_" << (seqno+1);
			if(name == crtDataDirname)  continue;

			OmnString fname = subDirname;
			fname << name;
			fnames.push_back(fname);
		}
		closedir(dir);
	}
	mLock->unlock();
	return true;
}


void
AosStorageDirInfo::removeDataDir()
{
	OmnString full_data_dir = mBaseDirname;
	full_data_dir << mDirname;
	for(u32 i=1; i<=mCrtDataDirSeqno; i++)
	{
		full_data_dir << mDataDirname << "_"  << i << "/";
		unlink(full_data_dir.data());		// The data file in the site_dir has already deleted.
	}
}


/////////// The Follower func is for AosVirtualDirInfo.
AosVirtualDirInfo::AosVirtualDirInfo(
		const u32 virtual_id, 
		const OmnString &base_dir, 
		const OmnString &dirname)
:
mLock(OmnNew OmnMutex()),
mVirtualId(virtual_id),
mBaseDirname(base_dir),
mDirInfo(OmnNew AosStorageDirInfo(base_dir, dirname))
{
	init();
}


void
AosVirtualDirInfo::init()
{
	vector<OmnString> site_fnames;                                                           
	bool rslt = mDirInfo->getAllFnames(site_fnames);
	aos_assert(rslt);
	for(u32 i=0; i<site_fnames.size(); i++)
	{
		OmnString sub_fname_site = site_fnames[i].substr(site_fnames[i].find('/', true) + 1);
		if(!sub_fname_site.hasPrefix("site_"))    continue;
				    
		u32 site_id = atoi(sub_fname_site.substr(strlen("site_")).data());
		aos_assert(site_id);
		AosStorageDirInfoPtr site_dirinfo = OmnNew AosStorageDirInfo(mBaseDirname, site_fnames[i]);
		mSiteDirMap.insert(make_pair(site_id, site_dirinfo));
	}
}


OmnString
AosVirtualDirInfo::getNextSubDirname(const u32 site_id)
{
	AosStorageDirInfoPtr site_dirinfo;
	mLock->lock();
	map<u32, AosStorageDirInfoPtr>::iterator it = mSiteDirMap.find(site_id);
	if(it == mSiteDirMap.end())
	{
		// create a site Dir for this site_id.    
		OmnString s_dirname = mDirInfo->getNextDataDirname();
		s_dirname << "site_" << site_id;
		site_dirinfo = OmnNew AosStorageDirInfo(mBaseDirname, s_dirname); 
	}
	else
	{
		site_dirinfo = it->second;
	}
	mLock->unlock();
	
	return site_dirinfo->getNextDataDirname();
}


bool
AosVirtualDirInfo::getTotalFiles(set<AosStorageFileInfo> &files, const u32 disk_id)
{
	// for disk recover
	bool rslt;
	vector<OmnString> fnames;
	AosStorageFileInfo file_info;	
	mLock->lock();
	map<u32, AosStorageDirInfoPtr>::iterator itr = mSiteDirMap.begin();
	for(; itr != mSiteDirMap.end(); itr++)
	{
		u32 site_id = itr->first;	
		AosStorageDirInfoPtr dir_info = itr->second;
		
		rslt = dir_info->getAllFnames(fnames);
		aos_assert_rl(rslt, mLock, false);
		for(u32 i=0; i<fnames.size(); i++)
		{
			OmnString fn = fnames[i];
			u64 fid = getFileIdByFname(fn);
			file_info = AosStorageFileInfo(fid, site_id, disk_id, 0, fn);
			files.insert(file_info);
		}
		fnames.clear();
	}

	mLock->unlock();
	return true;
}

u64
AosVirtualDirInfo::getFileIdByFname(const OmnString &fname)
{
	OmnString str_seq = fname.substr(fname.find('_', true) + 1);
	u32 file_seq = atoi(str_seq.data());
	
	u64 file_id = mVirtualId;
	file_id = (file_id << 32) + file_seq;
	aos_assert_r(file_id, 0);
	return file_id;
}


