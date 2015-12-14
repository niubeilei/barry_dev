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
// 04/28/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILMgrBig_IILDistrPolicy_h
#define AOS_IILMgrBig_IILDistrPolicy_h

class AosIILDistrPolicy
{
public:
	enum E
	{
		eInvalid,

		eNoDistribution,
		eAtSecondLevel,

		eMax
	};
};
#endif

