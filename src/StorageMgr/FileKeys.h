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
// 09/19/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_FileKey_h
#define AOS_StorageMgr_FileKey_h

/*
#define AOSFILEKEY_DOCFILEMGR_GRP			"__zkf_0001"		// Ketty 2011/09/19
#define AOSFILEKEY_IIL_STORAGE_APP			"__zkf_0002"		// Ketty 2011/09/19
#define AOSFILEKEY_DOC_STORAGE_APP			"__zkf_0003"		// Ketty 2011/09/19
#define AOSFILEKEY_SELOGMGR_SELOG_HASH		"__zkf_0004"
#define AOSFILEKEY_SELOGMGR_SEQNO_HASH		"__zkf_0005"

#define AOSFILEKEY_SIZEID_MAP				"__zkf_0006"			// Chen Ding, 2012/03/08
#define AOSFILEKEY_SIZEID_VIRTFILESYS		"__zkf_0007"			// Ken, 2012/03/28
*/

//#define AOSFILEKEY_STORAGE_APP				"__zkf_0001"		// Ketty 2012/11/29
//#define AOSFILEKEY_DOCFILEMGR_GRP			"__zkf_0002"			// Ketty 2012/11/29

//#define AOSFILEKEY_SELOGMGR_SELOG_HASH		"__zkf_0004"		// Ketty 2012/11/29
//#define AOSFILEKEY_SELOGMGR_SEQNO_HASH		"__zkf_0005"		// Ketty 2012/11/29

#define AOSFILEKEY_SIZEID_MAP				"__zkf_0006"			// Chen Ding, 2012/03/08
#define AOSFILEKEY_SIZEID_VIRTFILESYS		"__zkf_0007"			// Ken, 2012/03/28

//#define AOSFILEKEY_WORD_HASH				"__zkf_0008"			// Ketty, 2012/11/29
//#define AOSFILEKEY_WORD_HASH_IDX			"__zkf_0009"			// Ketty, 2012/11/29
#define AOSFILEKEY_RELIABLE_FILE_ONE		"__zkf_0010"			// Chen Ding, 2012/08/14
#define AOSFILEKEY_RELIABLE_FILE_TWO		"__zkf_0011"			// Chen Ding, 2012/08/14
#define AOSFILEKEY_SNAPSHOT_FILENAME		"__zkf_0012"			// Chen Ding, 2012/12/13

class AosFilekey
{
public:
	inline static OmnString getSizeIdMapFileKey(const int virtualid, const u32 siteid)
	{
		OmnString fkey = AOSFILEKEY_SIZEID_MAP;
		fkey << "_" << virtualid << "_" << siteid;
		return fkey;
	}

	inline static OmnString getSizeIdVirtFileSysFileKey(const u32 sizeid)
	{
		OmnString fkey = AOSFILEKEY_SIZEID_VIRTFILESYS;
		fkey << "_" << sizeid;
		return fkey;
	}
};
#endif

