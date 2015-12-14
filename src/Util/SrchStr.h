////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SrchStr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_SrchStr_h
#define Omn_Util_SrchStr_h

#include "Alarm/Alarm.h"
#include "Util/String.h"



class OmnSrchStr  
{
private:
	OmnString	mStr;
	char		mChar1;
	char		mChar2;
	int			mNumChars;
	int			mIndex;
	char *		mBuff;
	int			mTrimIndex;

public:
	OmnSrchStr(const OmnString &str)
	:
	mStr(str),
	mNumChars(0),
	mIndex(0),
	mBuff(str.getBuffer()),
	mTrimIndex(mStr.length())
	{
	}

	~OmnSrchStr()
	{
	}

	void	setSrchChar(const char c) 
	{
		switch (mNumChars)
		{
		case 0:
			 mNumChars = 1;
			 mChar1 = c;
			 break;

		case 1:
			 mNumChars = 2;
			 mChar2 = c;
			 break;

		default:
			 OmnAlarm << "Too many chars. Maximum 2" << enderr;
		}

		return;
	}

	void resetLoop() {mIndex = 0; mTrimIndex = mStr.length();}
	OmnString	crtValue() const
	{
		// 
		// It returns a substring from the beginning to the char
		// that is just before mIndex.
		//
		if (mIndex < 0 || mIndex > mStr.length())
		{
			// 
			// Should never happen
			//
			OmnAlarm << "Program error: " << mIndex << enderr;
			return "";
		}

		return OmnString(mBuff, mIndex);
	}

	OmnString nextToken()
	{
		// 
		// It moves the index to the next position that matches
		// mChar1 or mChar2 or to the end of the string. It returns
		// the substring between two search chars.
		//
		if (mNumChars <= 0)
		{
			OmnAlarm << "No matching characters set: " 
				<< mStr << enderr;
			return "";
		}

		if (mIndex >= 0)
		{
			mIndex++;
		}

		// 
		// If it reaches the end, it should return a null string.
		//
		if (mIndex >= mStr.length())
		{
			return "";
		}

		int tokenStartIndex = mIndex;
		// 
		// mIndex is the position to start with. 
		//
		while (mIndex < mStr.length() && 
			   !(mBuff[mIndex] == mChar1 ||
			  	 (mNumChars == 2 && mBuff[mIndex] == mChar2)))
		{
			mIndex++;
		}

		if (mIndex >= mStr.length())
		{
			return mStr;
		}

		return OmnString(&mBuff[tokenStartIndex], mIndex-tokenStartIndex);
	}	

	OmnString next()
	{
		// 
		// It moves the index to the next position that matches
		// mChar1 or mChar2 or to the end of the string. It returns
		// the substring from the beginning to the next stop.
		//
		if (mNumChars <= 0)
		{
			OmnAlarm << "No matching characters set: " 
				<< mStr << enderr;
			return "";
		}

		if (mIndex > 0)
		{
			mIndex++;
		}

		// 
		// If it reaches the end, it should return a null string.
		//
		if (mIndex >= mStr.length())
		{
			return "";
		}

		// 
		// mIndex is the position to start with. 
		//
		while (mIndex < mStr.length() && 
			   !(mBuff[mIndex] == mChar1 ||
			  	 (mNumChars == 2 && mBuff[mIndex] == mChar2)))
		{
			mIndex++;
		}

		if (mIndex >= mStr.length())
		{
			return mStr;
		}

		return OmnString(mBuff, mIndex);
	}	

	bool hasMore() const
	{
		return mIndex < mStr.length();
	}

	bool previous()
	{
		// 
		// It moves the index to the previous location. 
		// 
		if (mIndex <= 0)
		{
			return false;
		}

		mIndex--;
		while (mIndex > 0 && 
			   !(mBuff[mIndex] == mChar1 ||
			  	 (mNumChars == 2 && mBuff[mIndex] == mChar2)))
		{
			mIndex--;
		}

		return true;
	}

	bool removeToken()
	{
		// 
		// It remove the token and move index to the previous location. 
		// 
		if (mIndex <= 0)
		{
			return false;
		}
		int curIndex = mIndex;

		mIndex--;
		while (mIndex > 0 && 
			   !(mBuff[mIndex] == mChar1 ||
			  	 (mNumChars == 2 && mBuff[mIndex] == mChar2)))
		{
			mIndex--;
		}

		bool status = mStr.remove(mIndex, curIndex-mIndex);
		mBuff = mStr.getBuffer();
		mTrimIndex -= curIndex - mIndex;

		return status;
	}

	char nextChar()
	{
		// 
		// It moves the index to the next character and returns the
		// current character.
		//
		if (mIndex >= mStr.length())
		{
			OmnAlarm << "Out of bound: " << mStr
				<< ":" << mIndex << enderr;
			return 0;
		}
		return mBuff[mIndex++];
	}

	OmnString remainingStr()
	{
		if (mIndex < 0 || mIndex > mStr.length())
		{
			OmnAlarm << "Out of bound: " << mStr
				<< ":" << mIndex << enderr;
			return "";
		}

		if (mIndex == mStr.length())
		{
			// 
			// this is possible. Simply return "";
			//
			return "";
		}

		return OmnString(&mBuff[mIndex], mStr.length() - mIndex);
	}

	OmnString getRemaining()
	{
		// 
		// It returns the remaining substring starting from mIndex. 
		// If there are leading searched characters, they are not 
		// included in the remaining string.
		//
		if (mIndex >= mStr.length())
		{
			return "";
		}

		// 
		// Skip the leading searched character (only skip one).
		//
		if (mBuff[mIndex] == mChar1 ||
			(mNumChars == 2 && mBuff[mIndex] == mChar2))
		{
			mIndex++;
		}

		if (mIndex >= mStr.length())
		{
			return "";
		}

		return OmnString(&mBuff[mIndex], mStr.length() - mIndex);
	}

	OmnString trimPrev()
	{
		// 
		// This function trims off the string from the current location and
		// moves the pointer to the previous search character.
		//
		if (mTrimIndex < 0)
		{
			// 
			// Can't trim anymore
			//
			return "";
		}

		if (mTrimIndex > mStr.length())
		{
			OmnAlarm << "Incorrect mTrimIndex: " << mTrimIndex 
				<< ":" << mStr.length();
			return "";
		}

		//int index = mTrimIndex;
		while (mTrimIndex > 0)
		{
			mTrimIndex--;
			if (mNumChars == 1 && mBuff[mTrimIndex] == mChar1)
			{ // Found the next search character. Stop
				break;
			}
			else if (mNumChars == 2) 
			{
				if (mBuff[mTrimIndex] == mChar1 ||
			  	  mBuff[mTrimIndex] == mChar2)
				{ // Found the next search character. Stop
					break;
				}
			}
		}

		return OmnString(&mBuff[0], mTrimIndex);
	}
};

#endif

