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
// File format is:
// 		length 			(4 bytes)
// 		entry type		(char)
// 		front poison	(u32)
// 		reference count	(u32)
// 		flags			(u32)
// 		signature		(optional)
// 		data			(variable)
// 		length			(4 bytes)
//
// Modification History:
// 09/20/2011	Created by Brian
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_SeLogEntryVersion_h
#define AOS_SeLogSvr_SeLogEntryVersion_h

#include "Util/Buff.h"
#include "ReliableFile/Ptrs.h"
#include "SeLogSvr/SeLogEntry.h"

class AosSeLogEntryVersion : public AosSeLogEntry
{
private:
	int			mMaxVersions;

public:
	AosSeLogEntryVersion();
	AosSeLogEntryVersion(
			const AosXmlTagPtr &ctnr_doc,
			const OmnString &container,
			const OmnString &logname,
			const AosRundataPtr &rdata);
	AosSeLogEntryVersion(const int maxEntrySize, AosBuff &buff);
	~AosSeLogEntryVersion();

	// AosSeLogEntry Interface
	virtual AosSeLogEntryPtr clone();
    virtual u64 appendToFile(
			const AosSeLogPtr &selog,
			const u32 &seqno,
			const AosXmlTagPtr &log,
			//const OmnFilePtr &file,
			const AosReliableFilePtr &file,
			u64 &writePos,
			const AosRundataPtr &rdata);
	void	setContentsToBuff(AosBuff &buff)
	{
		buff.setInt(mMaxVersions);
	}
	static inline OmnString composeVersionListingKey(
			const OmnString &objid,
			const OmnString &version)
	{
		OmnString key = objid;
		key << "_" << version;
		return key;
	}

	static inline OmnString composeVersionListingPrefix(const OmnString &objid)
	{
		OmnString key = objid;
		key << "_";
		return key;
	}


private:
	bool checkNumVersions( 
         const OmnString &ctnr_objid,    
         const OmnString &objid,         
         const u64 &old_count,           
         const AosRundataPtr &rdata);     
};
#endif
