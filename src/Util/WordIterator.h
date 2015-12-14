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
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_WordIterator_h
#define Omn_Util_WordIterator_h

#include "Util/String.h"
#include "Util/StrParser.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosWordIterator : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum End
	{
		eIteratorEnd
	};

	OmnString		mStr;
	OmnString		mSep;
	OmnStrParser	mParser;
	OmnString		mWord;

public:
	AosWordIterator(const OmnString &str, const OmnString sep = ", ");
	~AosWordIterator(); 

	void begin()
	{
		mParser.reset();
	}

	void operator ++()
	{
		mWord = mParser.nextWord();
	}

	AosWordIterator::End end()
	{
		return eIteratorEnd;
	}

	OmnString getWord() const {return mWord;}

	bool operator == (const AosWordIterator::End end)
	{
		if (mWord.length() <= 0) return true;
		return false;
	}

	bool operator != (const AosWordIterator::End end)
	{
		if (mWord.length() > 0) return true;
		return false;
	}
};
#endif
