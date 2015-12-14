////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SlabMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util1_aosSlabMgr_h
#define Omn_Util1_aosSlabMgr_h

#define eAosMaxSlabs	300

struct aosSlabMgr
{
	struct aosSlab *	mSlabs[eAosMaxSlabs];
	int					mNumSlabs;
};


extern struct aosSlabMgr *aosSlabMgr_constructor();
extern int aosSlabMgr_destructor();
extern int aosSlabMgr_start();
extern int aosSlabMgr_stop();
extern int aosSlabMgr_addSlab(struct aosSlabMgr *self, 
					  struct aosSlab *slab);

#endif
