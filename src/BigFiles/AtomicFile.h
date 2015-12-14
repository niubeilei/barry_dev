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
//
// Modification History:
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_BigFile_AtomicFile_h
#define Omn_BigFile_AtomicFile_h	

#include "BigFile/AtomicFile.h"

class AosAtomicFile: public AosBigFile
{
	OmnDefineRCObject;

private:
	OmnString		mFname;

public:
	AosAtomicFile(const int phyid, const OmnString &name);
	~AosAtomicFile();

	virtual int64_t getFileLength() const;
};

#endif

