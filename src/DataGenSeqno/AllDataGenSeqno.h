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
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataGenSeqno_AllDataGenSeqno_h
#define Aos_DataGenSeqno_AllDataGenSeqno_h

class AosAllDataGenSeqno
{
	public:
		AosAllDataGenSeqno();

		bool check() {return true;}
};
extern AosAllDataGenSeqno gAosAllDataGenSeqno;

#endif

