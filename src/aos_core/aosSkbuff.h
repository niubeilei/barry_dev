////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSkbuff.h
// Description:
//	The purpose of this file is to extend skbuff.h. There are some
//  functions that we would like to add to skbuff.h but do not do 
//  that directly so that this file is independent of Linux distribution.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_core_aosSkbuff_h
#define aos_core_aosSkbuff_h


// 
// This function checks whether the packet has sufficient room
// for 'len' to pull. It is a little bit different from the standard
// pskb_may_pull(...) in that if there isn't room, it does not
// call __pskb_pull_tail(...). 
//
static inline int aosPskbMayPullFast(struct sk_buff *skb, unsigned int len)
{
	return (likely(len <= skb_headlen(skb)))?1:0;
}

#endif

