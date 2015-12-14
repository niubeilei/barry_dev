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
// This is a utility to select docs.
//
// Modification History:
// 10/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_AutoAnswer_AnswerTypes_h
#define AOS_AutoAnswer_AnswerTypes_h


class AosAnswerType
{
public:
	enum E
	{
		eInvalid,

		ePattern,

		eMax
	};

	static bool isValid(const E code)
	{
		return (code > eInvalid && code < eMax);
	}

	static OmnString toEnum(const OmnString &name);
	static bool addName(const OmnString &name, const E e);
};
#endif
