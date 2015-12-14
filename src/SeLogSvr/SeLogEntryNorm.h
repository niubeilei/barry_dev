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
#ifndef AOS_SeLogSvr_SeLogEntryNorm_h
#define AOS_SeLogSvr_SeLogEntryNorm_h

#include "SeLogSvr/SeLogEntry.h"

class AosSeLogEntryNorm : public AosSeLogEntry
{
private:

public:
	AosSeLogEntryNorm();
	AosSeLogEntryNorm(
			const AosXmlTagPtr &ctnr_doc,
			const OmnString &container,
			const OmnString &logname,
			const AosRundataPtr &rdata);
	AosSeLogEntryNorm(const int maxEntrySize);
	AosSeLogEntryNorm(
			const int maxEntrySize,
			const OmnString &iilname);
	~AosSeLogEntryNorm();

	// AosSeLogEntry Interface
	virtual AosSeLogEntryPtr clone();

	void	setContentsToBuff(AosBuff &buff)
	{
	}
};
#endif
