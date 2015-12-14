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
// 12/09/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_SOType_h
#define Aos_SemanticObj_SOType_h

class AosSOType
{
public:
	enum E
	{
		eInvalid,
		eUnknown,

		eAosHashtab,

		eMaxEntry
	};

	static AosSOType	toEnum(const std::string &name);
	static std::string	toStr(const AosSOType::E code);
};

#endif
