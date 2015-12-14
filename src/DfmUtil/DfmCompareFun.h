////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 07/09/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DfmUtil_DfmCompareFun_h
#define AOS_DfmUtil_DfmCompareFun_h

#include "DfmUtil/Ptrs.h"
#include "DfmUtil/DfmDoc.h"
#include "Util/Buff.h"

#include <set>

struct AosDfmCompareFunDocid 
{
	bool operator() (const AosDfmDocPtr &lhs, const AosDfmDocPtr &rhs)
	{
		return lhs->getDocid() < rhs->getDocid();
	}
};

struct AosDfmCompareFunSeqOff
{
	bool operator() (const AosDfmDocPtr &lhs, const AosDfmDocPtr &rhs)
	{
		if (lhs->getOpr() == rhs->getOpr())
		{	
			if (lhs->getBodySeqno() == rhs->getBodySeqno())
			{
				if (lhs->getBodyOffset() == rhs->getBodyOffset())
				{
					if (lhs->getOpr() == AosDfmDoc::eDelete)
					{
						//OmnAlarm << "missing " << enderr;
						return lhs->getDocid() < rhs->getDocid();
						//return false;
					}
					if (lhs->getBodyBuff()->dataLen() == rhs->getBodyBuff()->dataLen())
					{
						return lhs->getDocid() < rhs->getDocid();
					}
					return lhs->getBodyBuff()->dataLen() < rhs->getBodyBuff()->dataLen();
				}
				return lhs->getBodyOffset() < rhs->getBodyOffset();
			}
			return lhs->getBodySeqno() < rhs->getBodySeqno();
		}
		return lhs->getOpr() < rhs->getOpr();
	}
};

struct AosDfmCompareFunType
{
	typedef set<AosDfmDocPtr, AosDfmCompareFunDocid> set_f;
	typedef set<AosDfmDocPtr, AosDfmCompareFunDocid> ::iterator setitr_f;

	typedef set<AosDfmDocPtr, AosDfmCompareFunSeqOff> set_fs;
	typedef set<AosDfmDocPtr, AosDfmCompareFunSeqOff> ::iterator setitr_fs;
};
#endif
