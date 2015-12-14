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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
// Modification History:
// 05/30/2013	Created by Jackie
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocFileMgr_RawFiles_h
#define AOS_DocFileMgr_RawFiles_h

#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosRawFiles : public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxFiles = 30000 
	};

	struct CtrlDoc
	{
		u16			cCrtDirSeqno;
		u32			cCrtFileSeqno;
		OmnString	xFname;
		u64			xOffset;
	};

private:
	struct CtrlDoc	mCtrlDoc;
	u16				mDeviceNo;
	OmnFilePtr		mXmlFile;
	OmnString		mPrefix;
	OmnString		mBaseDirName;
	OmnMutexPtr		mLock;

public:
	AosRawFiles(const OmnString &path, const u16 &deviceno);

	~AosRawFiles();

	bool saveFile(const AosXmlTagPtr &doc, const AosBuffPtr &buff, const AosRundataPtr &rdata);

	AosBuffPtr getFile(const AosXmlTagPtr &doc, const u64 &id, const AosRundataPtr &rdata);

private:
	bool init();

	bool readCtrlDoc();

	bool saveCtrlDoc(const AosXmlTagPtr &doc);

	OmnString getFileNameLocked(const OmnString &rawname);

	OmnString  getFileName(const OmnString &rawname, const u64 &id);

	bool closeCtrlFile();

	u64 encodeId(const u16 &dirno, const u16 &seqno, const u32 &fileno);

	void decodeId(const u64 &id, u16 &seqno, u32 &fileno);
};
#endif
