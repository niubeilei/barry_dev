////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BitString.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_BitString_h
#define Omn_Util_BitString_h



template <int strLen>
class OmnBitString
{
private:
	int				mLength;
	unsigned int	mInt[strLen/32 + 1];

public:
	OmnBitString()
		:
	mLength(strLen)
	{
		for (int i=0; i<(mLength>>5)+1; i++)
		{
			mInt[i] = 0;
		}
	}

	OmnBitString(const OmnBitString &rhs)
		:
	mLength(rhs.mLength)
	{
		for (int i=0; i<(mLength>>5)+1; i++)
		{
			mInt[i] = rhs.mInt[i];
		}
	}

	~OmnBitString() {}

	bool operator == (const OmnBitString &rhs) const
	{
		for (int i=0; i<(mLength>>5)+1; i++)
		{
			if (mInt[i] != rhs.mInt[i])
			{
				return false;
			}
		}

		return true;
	}

	bool operator != (const OmnBitString &rhs) const;
	
	OmnBitString operator | (const OmnBitString &rhs) const
	{
		OmnBitString bs;
		
		bs.mLength = mLength;

		for (int i=0; i<(mLength>>5)+1; i++)
		{
			bs.mInt[i] = mInt[i] | rhs.mInt[i];
		}
		
		return bs;
	}

	OmnBitString & operator |= (const OmnBitString &rhs);
	OmnBitString operator & (const OmnBitString &rhs) const;
	OmnBitString & operator &= (const OmnBitString &rhs);

	OmnBitString & operator = (const OmnBitString &rhs);

	bool isSet(const int pos) const;
	bool set(const int pos);
	bool reset(const int pos);
	void reset()
	{
		int numBytes = (mLength >> 5) + 1;
		for (int i=0; i<numBytes; i++)
		{
			mInt[i] = 0;
		}
	}
};
#endif

