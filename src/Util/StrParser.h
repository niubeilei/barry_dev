////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrParser.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_StrParser_h
#define Omn_Util_StrParser_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <vector>
using namespace std;

#define AOS_STANDARD_WORD_SEP ", "

class OmnStrParser1 : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	OmnString		mString;
	const char *	mData;
	int				mLen;
	int				mCrt;
	bool			mCheckDoubleQuote;
	bool			mCheckSingleQuote;
	bool			mMap[256];
	OmnString 		mDelimiters;

public:
	OmnStrParser1(const OmnString &str);
	OmnStrParser1(
			const OmnString &str,
			const OmnString &deli, 
			const bool checkDoubleQuote = false,
			const bool checkSingleQuote = false);
	~OmnStrParser1(); 

	void		setData(const OmnString &str);
	void		dump() const;
	void		reset();
    bool        hasMore() const {return mCrt < mLen;}
	bool		remainingAllSpaces() const
	{
		int len = mCrt;
		while (len < mLen && (mData[len] == ' ' || mData[len] == '\t')) len++;
		return (len >= mLen);
	}

	OmnString	nextWord() {return nextWord("");}
	OmnString	nextWord(const OmnString &dft); 
	OmnString	nextWordEx(
					const OmnString &dft, 
					const char * const deliminator); 
	OmnString	nextEngWord(const OmnString &dft = "");
	OmnString	peekWord();
	OmnString	nextDigits(const OmnString &dft);
	bool		nextInt(int64_t &value, const int dft = -1);
	bool		nextInt(u64 &value1, int64_t &value2, 
					bool &isSigned, const int dft = -1);
	bool		nextIntEx(int64_t &value1, const int dft = -1);
	bool		nextIntEx(u64 &value1, int64_t &value, 
					bool &isSigned, const int dft = -1);
	bool		nextAddr(u32 &value, const uint dft = 0);
	bool		nextMask(uint &value, const uint dft = 0);
	bool 		nextBlock(OmnString &value);
	bool		nextBool()
	{
		u64 value1;
		int64_t value2;
		bool isSigned;
		nextInt(value1, value2, isSigned);
		return (isSigned && value2 != 0) || 
			   (isSigned && value2 != 0);
	}

	char peekChar() const
	{
		if (mCrt >= mLen) return 0;
		return mData[mCrt];
	}

	char nextChar(const bool skipws = true) 
	{
		if (skipws)
		{
			char c;
			while (mCrt < mLen && (c = mData[mCrt]) <= 33) mCrt++;
		}
		if (mCrt >= mLen) return 0;
		return mData[mCrt++];
	}

	bool expectNext(const OmnString &expected, const bool move)
	{
		OmnString value;
		if (move)
		{
			value = nextWord("");
		}
		else
		{
			value = peekWord();
		}

		return (value == expected);
	}

	bool expectNext(const char expected, const bool move)
	{
		char c;
		if (move)
		{
			c = nextChar();
		}
		else
		{
			c = peekChar();
		}

		return (c == expected);
	}

	// 
	// Chen Ding, 03/21/2007
	//
	OmnString getValue(const OmnString &name, const OmnString &dflt);
	int findWord(u32 pos, const OmnString &word);
	int nextChar(const u32 start, const char c, const bool leading);
	int skipWhiteSpace();
	bool finished() const {return mCrt == mLen;}
	OmnString	getStr() const {return mString;}
	bool		getValuePair(OmnString &name, 
							 OmnString &value, 
							 const char bracket,
							 const char delimiter);
	bool 		checkMinMax(const OmnString &word, 
						  u64 &value1, 
						  int64_t &value2, 
						  bool &isSigned);
	OmnString	getSubstr(const OmnString &pattern, const OmnString &dft);

	// Chen Ding, 01/09/2010
	OmnString	getRemaining() const {return OmnString(&mData[mCrt], mLen - mCrt);}

	// Chen Ding, 01/18/2010
	bool 		resetNextWord(const char *deli);
	char		crtChar()
				{
					if (mCrt<=0 || mCrt>=mLen) return 0;
					return mData[mCrt];
				}

	OmnString subHtml(OmnString &str);
	int	trimHtml(OmnString &str, const int max_len);
	// felicia 2011/0721
	OmnString getHtml(OmnString &str, const int max_len);
	OmnString convertBorder(OmnString &str, const OmnString &browser);
	void changediv(OmnString &divcont);
	void setBorder(OmnString &stylecont, int &bstart, OmnString &value);
	void IEchangediv(OmnString &divcont);
	
	bool trimCdata(char *data, int &idx, int &str_len, 
			int &data_len, const int max_len);
	static OmnString getLastWord(const OmnString &str, const char c);
	bool nextExcelEntry(int &start, OmnString &entry);
	bool parserEntry(OmnString &entry);

	static bool containWord(
			const OmnString &words, 
			const OmnString &word, 
			const OmnString &seps = AOS_STANDARD_WORD_SEP);

	bool skipParenthesis(const char quote, AosRundata *rdata);
	bool getValueList(
			vector<OmnString> &names, 
			const char *delimitors, 
			const char terminator, 
			const int max,
			AosRundata *rdata);
};
#endif
