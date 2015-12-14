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
// 07/27/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgrUtil_DevLocation_h
#define AOS_StorageMgrUtil_DevLocation_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

class AosDevLocation1 : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mDevName;
	OmnString		mUserDir;

public:
	AosDevLocation1(){};
	AosDevLocation1(const AosXmlTagPtr &config);
	~AosDevLocation1();

	//u32		getEstimatedMaxSize(){ return 50000; };
	OmnString	getUserDir()
	{
		// Chen Ding, 01/19/2012
		// TSK001NOTE
		// It should include the mount dir. Otherwise, it may not
		// be able to open the file.
		return mUserDir;
	}
	
	//Jozhi 2015-04-20 no one use it
	OmnString getDevName() {return mDevName; };
};
#endif
