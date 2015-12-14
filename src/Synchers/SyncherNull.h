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
// 2015/09/15 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Synchers_SyncherNull_h
#define Aos_Synchers_SyncherNull_h

#include "Jimo/Jimo.h"
#include "SEInterfaces/Ptrs.h"
//#include "SEInterfaces/SyncherObj.h"
#include "Synchers/Syncher.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"


class AosSyncherNull : virtual public AosSyncher
{
private:
	enum
	{
		eExpireTimer = 10		// 10 seconds
	};

	i64			mCreateTimeSec;
	OmnString 	mTestContents;

public:
	AosSyncherNull();
	AosSyncherNull(const AosSyncherNull &rhs);
	~AosSyncherNull() {}

	virtual AosSyncherType::E getType() { return AosSyncherType::eNull; }

	virtual bool proc();
	virtual bool isValid() const;
	virtual bool isExpired() const;
	virtual AosSyncherObjPtr clone() const;
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual int getDestCubeID() { return 0; }
};
#endif

