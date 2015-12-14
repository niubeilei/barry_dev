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
// 	Created: 04/23/2010	by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SearchEngine_DataReader_h
#define AOS_SearchEngine_DataReader_h

#include "aosUtil/Types.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"

class AosSeDataReader : public virtual OmnRCObject
{
	OmnDefineRCObject;

	OmnMutexPtr		mLock;
	u64				mCrtDocid;

public:
	AosSeDataReader();
	~AosSeDataReader();

	AosXmlTagPtr startRead(const u64 docid);
	AosXmlTagPtr readNext();
};
#endif

