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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_IILSelector_SelectorIds_h
#define Omn_IILSelector_SelectorIds_h

#include "Util/String.h"



class AosIILSelectorId
{
public:
	AosIILSelectorId();

	enum E 
	{
		eInvalid,

		eAttr,
		eContainer,
		eLog,
		eMicroblog,
		eTags,
		eCounter,
		eKeyword,
		eIILName,
		eIILId,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &reqid);
	static bool addName(const OmnString &name, const E e, OmnString &errmsg);
};
#endif

