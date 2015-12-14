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
// 04/07/2013	Created by kerong li
////////////////////////////////////////////////////////////////////////////

#include "Util/String.h"

#ifndef AosSengTester_StType_h
#define AosSengTester_StType_h

class AosSecOpr
{
public:

	enum E
	{
		eInvalid,
		eRead,
		eWrite,
		eDelete,
		eCreate,
		eCreateAcct,
		eModifyAcd,
		eCreateDomainOprArd,
		eMax,
	};

	static E toEnum(OmnString str) { return eInvalid;};
	static OmnString toString(E e) { return ""; };
	static OmnString toString(int i) { return ""; };
	static OmnString getDftAccess(E e) { return "";};
	static bool isValid(OmnString str) { return false;};
	static bool isValid(E e) { return false;};

};


class AosAccessType
{
public:

	enum E
	{
		eInvalid,
		ePrivate,
		eGroup,
		eRole,
		ePublic,
		eLoggedPublic,
		eTimedPublic,
		eMax,
	};

	static E toEnum(OmnString str) { return eInvalid; };
	static OmnString toString(E e) { return ""; };
	static OmnString toString(int i) { return ""; };
	static bool isValid(OmnString str) { return false; };
	static bool isValid(E e) { return false; };
};

#endif

