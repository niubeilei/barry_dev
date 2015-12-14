////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Seqno.h
// Description:
//	This class generates sequence number. Everytime an instance is created,
//	it guarantees that it generates the next sequence number. There can be
//  multiple sequences, which are identified by sequence IDs. If one does
//	not care about the sequence, it does not need to specify the 'seqno'
//  when getting the next seqno. 
//
//	Seqno is unsigned int. It starts from 0. If it overflows, it will 
//  restart from 0.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Seqno_h
#define Omn_Util_Seqno_h


class OmnSeqno
{
private:
	unsigned int		mSeqno;
	static unsigned int	mCrt;

public:
	OmnSeqno();
	OmnSeqno(const int seqId);
	~OmnSeqno() {}

	unsigned int		getSeqno() const {return mSeqno;}
	static unsigned int	getCrt() {return mCrt;}
};
#endif
