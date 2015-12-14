////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosPktStat.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosCoreComm.h"

int aosPktStat_reset(struct aosPktStat *stat)
{
	stat->mTotalPkts = 0;
	stat->mTotalBytes = 0;
	stat->mErrorPkts = 0;
	stat->mErrorBytes = 0;
	return 0;
}


