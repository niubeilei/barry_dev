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
// 02/11/2014	Copued from JqlStament.h by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SEUtil_DeltaBeanOpr_h
#define Omn_SEUtil_DeltaBeanOpr_h

#include "Util/String.h"


class AosDeltaBeanOpr
{
public:
	enum E
	{
		eInvalid,
	
		eAdd, 
		eDelete, 
		eModify,
		eIncrement,

		eMax
	};


	static std::string toStr(E opr)
	{
		switch(opr)
		{
		case eAdd:			 			return "add";
		case eDelete:					return "delete";
		case eModify:					return "modify";
		case eIncrement:				return "increment";
		default:			 			return "";
		}
		return "";
	}

	static E toEnum(const OmnString &name)
	{
		if (name == "add") 				return eAdd;
		else if (name == "delete") 		return eDelete;
		else if (name == "modify") 		return eModify;
		else if (name == "increment") 	return eIncrement;
		else 							return eInvalid;
	}

	static bool isValid(const E code)
	{
		if (code > eInvalid && code < eMax) return true;

		return false;
	}

};


#endif
