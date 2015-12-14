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
// 08/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SEInterfaces_VirtualFileObj_h
#define Aos_SEInterfaces_VirtualFileObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/VirtualFileCreator.h"
#include "SEInterfaces/VirtualFileType.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Buff.h"

class AosVirtualFileObj : public OmnRCObject
{
protected:
	AosVirtualFileType::E	mType;

	static AosVirtualFileCreatorObjPtr		smCreator;

public:
	AosVirtualFileObj();
	AosVirtualFileObj(
			const OmnString &name, 
			const AosVirtualFileType::E type, 
			const bool flag);
	~AosVirtualFileObj();

	virtual bool append(const char *buff, const int len, const bool flush = false)
	{
		return false;
	}
	virtual int64_t read(
			vector<AosBuffPtr> &buffs, 
			const int64_t offset, 
			const int64_t len, 
			const AosRundataPtr &rdata)
	{
		return false;
	}

	virtual int64_t length(AosRundata *rdata)
	{
		OmnNotImplementedYet;
		return -1;
	}

	virtual bool readData(
			const u64 &reqId, 
			const int64_t &blocksize,
			AosRundata *rdata)
	{ 
		OmnNotImplementedYet;
		return false;
	};

	virtual void setCaller(const AosFileReadListenerPtr &caller)
	{
		OmnNotImplementedYet;
	};

	virtual AosFileReadListenerPtr getCaller()
	{
		OmnNotImplementedYet;
		return 0;
	};

	//felicia, 2013/05/22
	virtual void seek(const int64_t start_pos)
	{
		OmnNotImplementedYet;
	};

	virtual AosBuffPtr getBuff()
	{
		OmnNotImplementedYet;
		return 0;
	};

	static AosVirtualFileObjPtr createVirtualFile(
							const AosXmlTagPtr &def, 
							const AosRundataPtr &rdata);
	
	static bool registerVirtualFile(const OmnString &name, const AosVirtualFileObjPtr &);

	static void setCreator(const AosVirtualFileCreatorObjPtr &creator) {smCreator = creator;}
	static AosVirtualFileCreatorObjPtr getCreator() {return smCreator;}
};

#endif
#endif
