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
#ifndef Aos_BigFile_BigFile_h
#define Aos_BigFile_BigFile_h	

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosBigFile: virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString 		mObjid;
	AosXmlTagPtr	mDoc;

public:
	AosBigFile(const OmnString &objid);
	~AosBigFile() {}
	
	bool getFiles(const AosRundataPtr &rdata, vector<AosAtomicFilePtr> &files);
};

#endif

