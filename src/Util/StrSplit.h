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
// 08/06/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_StrSplit_h
#define Omn_Util_StrSplit_h

#include "alarm_c/alarm.h"
#include "Util/String.h"

#include <vector>
using namespace std;

struct AosStrRange
{
	bool		mLeftInclusive;
	OmnString	mLeft;
	bool 		mRightInclusive;
	OmnString	mRight;

	AosStrRange()
	:
	mLeftInclusive(true),
	mRightInclusive(true)
	{
	}

	AosStrRange(const bool incleft, 
			const OmnString &left, 
			const bool incright,
			const OmnString &right)
	:
	mLeftInclusive(incleft),
	mLeft(left),
	mRightInclusive(incright),
	mRight(right)
	{
	}
};

struct AosU64Range
{
	bool		mLeftInclusive;
	u64			mLeft;
	bool 		mRightInclusive;
	u64			mRight;

	AosU64Range()
	:
	mLeftInclusive(true),
	mLeft(0),
	mRightInclusive(true),
	mRight(0)
	{
	}

	AosU64Range(const bool incleft, 
			const u64 &left, 
			const bool incright,
			const u64 &right)
	:
	mLeftInclusive(incleft),
	mLeft(left),
	mRightInclusive(incright),
	mRight(right)
	{
	}
};

struct AosInt64Range
{
	bool		mLeftInclusive;
	int64_t		mLeft;
	bool 		mRightInclusive;
	int64_t		mRight;
};


class AosStrSplit : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString *	mStrings;
	int			mStrSize;
	int			mEntries;
	vector<OmnString>	mStringsV;
	int 		mIdx;

public:
	AosStrSplit();
	AosStrSplit(const OmnString &contents, 
			const char *sep); 
	AosStrSplit(const char *contents, 
			const char *sep); 
	AosStrSplit(const char *contents, 
			const char *sep, 
			OmnString *strs,
			const int max, 
			bool &finished);

	~AosStrSplit(); 

	int splitStr(const OmnString &data, const char *seps, OmnString *rslts, const int max)
	{
		bool finished;
		return splitStr(data.data(), seps, rslts, max, finished);
	}

	int splitStr(const char *data, const char *seps, OmnString *rslts, const int max)
	{
		bool finished;
		return splitStr(data, seps, rslts, max, finished);
	}

	inline static int splitStrByChar(const OmnString &contents, 
					const char *sep, 
					vector<OmnString> &substrs, 
					const int max)
	{
		bool finished;
		// Chen Ding, 2013/12/31
		// return splitStrByChar(contents.data(), sep, substrs, max, finished);
		return splitStrBySubstr(contents.data(), sep, substrs, max, finished);
	}

	inline static int splitStrByChar(const char *contents, 
					const char *sep, 
					vector<OmnString> &substrs, 
					const int max)
	{
		bool finished;
		return splitStr(contents, sep, substrs, max, finished);
	}

	static int splitStrByChar(const char *contents, 
	 				const char *sep, 
	 				vector<OmnString> &substrs, 
	 				const int max, 
	 				bool &fnished);

	static int splitStrByChar(
					const char *orig_str, 
					const char sep, 
					vector<OmnString> &substrs, 
					const int max, 
					bool &finished);

	inline static int splitStrBySubstr(const OmnString &contents, 
					const char *sep, 
					vector<OmnString> &substrs, 
					const int max)
	{
		bool finished = false;
		return splitStrBySubstr(contents.data(), sep, substrs, max, finished);
	}

	static int splitStrBySubstr(
					const OmnString &contents, 
					const char *sep, 
					vector<OmnString> &containers, 
					vector<OmnString> &members, 
					const int max);

	static int splitStrBySubstr(
					const OmnString &contents, 
					const char *sep, 
					vector<OmnString> &containers, 
					vector<OmnString> &members, 
					vector<int> &numstrs,
					const int max);

	inline static int splitStrBySubstr(const char *contents, 
					const char *sep, 
					vector<OmnString> &substrs, 
					const int max)
	{
		bool finished = false;
		return splitStrBySubstr(contents, sep, substrs, max, finished);
	}

	static int splitStrBySubstr(const char *contents, 
					const char *sep, 
					vector<OmnString> &substrs, 
					const int max, 
					bool &fnished);

	int 	splitStr(const char *, const char *, OmnString *, const int, bool &);
	int 	splitStr(const char *, const char *);
	int		entries() const {return mEntries;}
	OmnString	getEntry(const int idx)
			{
				aos_assert_r(mStrings, "");
				aos_assert_r(idx < mEntries, "");
				return mStrings[idx];
			}
	OmnString operator [](const int idx) {return mStrings[idx];}
	bool	deleteEntry(const int idx);

	static bool	getWords(
					const OmnString &str,
					OmnString *words, 
					int &numWords, 
					const int maxsize, 
					const char *sep);
	OmnString 	nextWord();
	bool		hasMore();
	void		reset();
	vector<OmnString>& entriesV(){return mStringsV;}

	// Chen Ding, 04/03/2012
	bool parseStrRanges(const OmnString &data, 
					const OmnString &fieldSep, 
					const OmnString &entrySep, 
					vector<AosStrRange> &ranges, 
					const int max); 

	bool parseU64Ranges(const OmnString &data, 
					const OmnString &fieldSep, 
					const OmnString &entrySep, 
					vector<AosU64Range> &ranges, 
					const int max);

	static int splitStr(
	 				const char *orig_str, 
	 				const char *sep_map, 
	 				vector<OmnString> &substrs, 
	 				const int max, 
	 				bool &finished);

};
#endif
