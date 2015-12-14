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
// 2015/05/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocServer_DfmEntry_h
#define AOS_DocServer_DfmEntry_h

struct AosModuleEntry
{
	AosDocFileMgrNewObj *	mDFMs;
	u32						mNumVirtuals;

	ModuleEntry()
	:
	mDFMs(0),
	mNumVirtuals(0)
	{
	}

	~ModuleEntry()
	{
		delete mDFMS;
	}

	AosDocFileMgrNewObj * getDFM(const u32 vid)
	{
		if (vid >= mNumVirtuals) return 0;
		return mDFMs[vid];
	}

	bool setDFM(AosDocFileMgrNewObj *dfm, const u32 vid)
	{
		if (vid >= mNumVirtuals)
		{
			AosDocFileMgrNewObj **entries = new AosDocFileMgrNewObj*[vid+1];
			for (u32 i=0; i<
};

struct AosDfmEntry
{
	ModuleEntry * 	mModules;
	u32				mNumModules;	

	DfmEntry()
	:
	mModules(0),
	mNumModules(0)
	{
	}

	~DfmEntry()
	{
		delete [] mModules;
	}

	AosDocFileMgrNewObj *getDFM(const u32 module_id, const u32 vid)
	{
		if (module_id >= mNumModules) return 0;
		return mModules[module_id].getDFM(vid);
	}

	bool setDFM(AosDocFileMgrNewObj *dfm, const u32 module_id, const u32 vid)
	{
		aos_assert_r(module_id < eMaxModuleID, false);
		if (module_id >= mNumModules) 
		{
			ModuleEntry *entries = new ModuleEntry[module_id+1];
			for (u32 i=0; i<mNumModules; i++) entries[i] = mModules[i];
			delete [] mModules;
			mModules = entries;
			mNumModules = module_id+1;
		}
		return setDFM(dfm, vid);
	}
};
#endif
