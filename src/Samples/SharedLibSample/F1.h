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
// 03/25/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Samples_SharedLibSample_F1_h
#define Omn_Samples_SharedLibSample_F1_h


class SharedLibObj
{

private:
	int		mMember1[2000000];
	float	mMember2;

public:
	SharedLibObj();
	~SharedLibObj();
};

#endif

