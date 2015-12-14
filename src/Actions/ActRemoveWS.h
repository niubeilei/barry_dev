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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActRemoveWS_h
#define Aos_SdocAction_ActRemoveWS_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include <vector>

class AosActRemoveWS : virtual public AosSdocAction
{
public:
	enum E
	{
		eInvalid,

		eLeading,
		eTail,
		eAll,

		eMax
	};


public:
	AosActRemoveWS(const bool flag);
	~AosActRemoveWS();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata)const;

private:
	E toEnum(const OmnString &type);
	bool isTrueType(const E &t);
};
#endif

