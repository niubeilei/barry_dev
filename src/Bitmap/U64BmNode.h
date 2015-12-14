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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Bitmap_U64BitmapNode_h
#define Aos_Bitmap_U64BitmapNode_h


class AosU64BmNode 
{
protected:
	bool 	mLeafNode;

public:
	AosU64BmNode(const bool leafnode)
	:
	mLeafNode(leafnode)
	{
	}

	~AosU64BmNode()
	{
	}

	bool isLeafNode() const {return mLeafNode;}
};
#endif

