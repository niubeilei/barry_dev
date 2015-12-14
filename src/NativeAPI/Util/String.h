////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: String.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_String_h
#define Omn_Util_String_h

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>
#include "aosUtil/Types.h"
#include "Porting/LongTypes.h"
#include "Util/BasicTypes.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/MemPoolNew.h"
#include "Util1/Ptrs.h"
using namespace std;

class OmnString;
class OmnMemPoolMul;
class OmnDateTime;

#define IS_SPECIAL_CHAR(x)   (((x)>=32&&(x)<=47)||((x)>=58&&(x)<65)||((x)>=91&&(x)<=96)||((x)>=123&&(x)<=127))

//felicia, simple string
struct OmnSimpleStr
{
	const char * 	mData;
	const int 	len;

	OmnSimpleStr(const char *name, const int len)
	:
	mData(name),
	len(len)
	{
	}
};


// class OmnString : virtual public OmnRCObject
class OmnString
{
	// OmnDefineRCObject;

private:
	enum
	{
		eDefaultSize = 31,
		eIncreaseSize = 50,
		eMaxStrLen    = 2000000000,		// 2G
		eAllowShrink  = 0x01,
		eFrontPoison  = 123456789,
		eBackPoison    = 987654321
	};

	u64				mFrontPoison;
	char 			mDefault[eDefaultSize+1];
	char *			mDataPtr;
	char *			mMem;
	// AosMemPoolPtr	mMemPool;		// Chen Ding, 2013/01/19
	u32				mLen;
	int				mMemSize;
	char			mFlags;
	u64				mBackPoison;

public:
	OmnString();
	OmnString(const OmnSimpleStr &str);
	OmnString(const int n, const char c, const bool);
	OmnString(const char *d);
	OmnString(const char *d, const int size);
	OmnString(const std::string &rhs);
	OmnString(const OmnString &rhs);
	~OmnString(); 

	// 
	// OmnObject Interface
	//
	// operator const char *() const {return mDataPtr;}
	// operator char *() {return mDataPtr;}
	operator std::string()const {return string(mDataPtr, mLen);}
	virtual int		objSize() const {return sizeof(*this);}
	int 			length() const {return mLen;}
	static int		getMaxStringLength() {return eMaxStrLen;}

	friend ostream& operator << (ostream &s, const OmnString &rhs)
	{
		s << rhs.data();
		return s;
	}

	// Chen Ding, 2013/01/19
	// void reset() 
	// {
	// 	mLen = 0; 
	// 	if (mMem && (mFlags & eAllowShrink))
	// 	{
	// 		mMemPool->release(mMem, __FILE__, __LINE__);
	// 		mMem = 0;
	// 		mMemPool = 0;
	// 		mDataPtr = &mDefault[0];
	// 		mMemSize = eDefaultSize;
	// 	}
	// }

	bool set(char c, const uint length)
	{
		if (mLen < length)
		{
			determineMemory(length + 1);
			if (!setLength(length))
			{
				return false;
			}
		}

		memset(mDataPtr, c, length);
		return true;
	}
	
	bool setChar(char c, const uint pos)
	{
		if (pos > mLen)
		{
			return false;
		}

		mDataPtr[pos] = c;
		return true;
	}
	
	int  padChar(char c, u32 len, bool longerIsError);
	bool setLength(const int len)
	{
		// 
		// The caller knows what it is doing. We will check whether
		// 'len' is longer than the memory size. If yes, it is an error. 
		// Otherwise, we will simply set the data length to 'len'.
		//
		if (len > getMemorySize())
		{
			if (!determineMemory(len+10))
			{
				std::cout << "<" << __FILE__ << ":" << __LINE__
					<< "> ********** Len is longer than memory size: " << len
					<< ":" << getMemorySize() << std::endl;
				return false;
			}
		}

		mLen = len;
		mDataPtr[mLen] = 0;
		return true;
	}
		
	const char * 	data() const {return mDataPtr;}
	char *			getBuffer() const {return mDataPtr;}
	bool			allowShrink() const {return mFlags & eAllowShrink;}

	OmnString &	toLower() 
	{
		for (uint i = 0; i<mLen; i++)
		{
			if (mDataPtr[i] >= 'A' && mDataPtr[i] <= 'Z')
			{
				mDataPtr[i] += 32;
			}
		}

		return *this;
	}

	OmnString &	toUpper()
	{
		for (uint i = 0; i<mLen; i++)
		{
			if (mDataPtr[i] >= 'a' && mDataPtr[i] <= 'z')
			{
				mDataPtr[i] -= 32;
			}
		}

		return *this;
	}

	bool		retrieveInt(const u32 startIndex, u32 &length, int &value);
	bool		retrieveInt(const uint startIndex, int &length, int &value);
	bool		replace(const u32 index, const u32 length, const OmnString &str);
	bool		replace(const char ori, const char target);
	int			replace(const OmnString &pattern, const OmnString &str, const bool all);

	// Convenient functions. Added by Chen Ding, 12/22/2008
	bool	parseInt(const u32 startIdx, u32 &len, int &value) const;
	bool	parseU32(const u32 startIdx, u32 &len, u32 &value) const;
	bool	parseInt64(const u32 startIdx, u32 &len, int64_t &value) const;

	inline u32 parseU32(const u32 &dft) const
	{
		u32 len;
		u32 vv;
		if (parseU32(0, len, vv)) return vv;
		return dft;
	}

	inline u64 toU64(const u64 &dft = 0) const
	{
		return parseU64(dft);
	}

	inline i64 toInt64(const i64 dft = 0) const
	{
		return parseInt64(dft);
	}

	inline double toDouble(const double dft = 0) const
	{
		u32 len = 0;
		double value = dft;
		bool rslt = parseDouble(0, len, value);
		if (!rslt) return dft;
		return value;
	}

	inline u64 parseU64(const u64 &dft) const
	{
		u32 len;
		return parseU64(0, len, dft);
	}
	u64		parseU64(const u32 startIdx, u32 &len, const u64 &dft) const;
	bool	parseU64(u64 &value) const
	{
		u32 len;
		return parseU64Bool(0, len, value);
	}

	bool	parseU64Bool(const u32 startidx, u32 &len, u64 &value) const;
	bool	parseDouble(const u32 startIdx, u32 &len, double &value) const;
	int64_t parseInt64(const int64_t &dft) const
	{
		u32 len;
		int64_t vv;
		if (parseInt64(0, len, vv)) return vv;
		return dft;
	}

	// 
	// Chen Ding, 05/28/2005
	//
	OmnString & operator = (const char *rhs);
	
	OmnString & operator = (const OmnString &rhs);
	OmnString & operator = (const std::string &rhs);
	OmnString & operator += (const char *rhs);
    OmnString & operator += (const OmnString &rhs);
    OmnString & operator += (const std::string &rhs);
	
	//felicia, simple string
	OmnString & operator = (const OmnSimpleStr &rhs);
	OmnString & operator << (const OmnSimpleStr &rhs)
	{
		append(rhs.mData, rhs.len);
		return *this;
	}
	friend bool operator == (const OmnSimpleStr &lhs, const OmnString &rhs);
	friend bool operator == (const OmnSimpleStr &lhs, const char *rhs);
	friend bool operator != (const OmnSimpleStr &lhs, const char *rhs);
	friend bool operator != (const OmnSimpleStr &lhs, const OmnString &rhs);

	int compare1(const OmnString &rhs) const
	{
		return strcmp(mDataPtr, rhs.mDataPtr);
	}

	OmnString & operator << (const long long rhs)
	{
		char buf[100];
		sprintf(buf, "%lld", rhs);
		return OmnString::operator<<(buf);
	}

	OmnString & operator << (const char rhs)
	{
		// 
		// If mMem is null and the new size is not bigger than eDefault size, 
		// use mDefault. 
		//
		if (mLen + 1 < eDefaultSize && !mMem)
		{
			mDefault[mLen++] = rhs;
			mDefault[mLen] = 0;
			return *this;
		}

		// 
		// Otherwise, need to determine the memory.
		//
		if (mLen + 1 >= (u32)mMemSize) determineMemory(mLen + 1);
		mDataPtr[mLen++] = rhs;
		mMem[mLen] = 0;
		return *this;
	}

	OmnString & operator << (const bool rhs)
	{
		if(rhs) return operator += ("true");
		else return operator += ("false");
	}

	OmnString  operator + (const u32 rhs)
	{
		OmnString tmp = *this;
		tmp << rhs;
		return tmp;
	}

	inline OmnString & operator << (const char *rhs)
	{
		// Chen Ding, 02/29/2012
		if (!rhs) return *this;
		return operator +=(rhs);
	}

	inline OmnString & operator << (const OmnString &rhs)
	{
		return operator +=(rhs);
	}

	inline OmnString & operator << (const std::string &rhs)
	{
		return operator +=(rhs);
	}


	inline OmnString & operator << (const float rhs)
	{
		char buf[100];
    	sprintf(buf, "%f", rhs);
		return operator +=(buf);
	}

	inline OmnString & operator << (const double rhs)
	{
		char buf[100];
    	sprintf(buf, "%f", rhs);
		return operator +=(buf);
	}

	OmnString & operator << (const long rhs);
	OmnString & operator << (const OmnDateTime &rhs);
	OmnString & operator << (const u64 rhs);
	OmnString & operator << (const int rhs);
	OmnString & operator << (const u32 rhs);
	char & 	operator [] (const uint pos) 
	{
		if (pos >= mLen)
		{
			std::cout << __FILE__ << ":" << __LINE__
				<< "********** Retrieving string out of bound" << std::endl;
			return mDataPtr[0];
		}
	
		return mDataPtr[pos]; 
	}

	inline bool operator != (const OmnString &rhs) const
	{
		if (rhs.mLen != mLen) return true;
		return !(operator == (rhs));
	}

	bool operator != (const char *rhs) const
	{
		if (!rhs)
		{
			if (mLen == 0)
			{
				// both are empty string
				return false;
			}
			return true;
		}

		if (strlen(rhs) != (unsigned int)mLen)
		{
			return true;
		}

		return (memcmp(mDataPtr, rhs, mLen) != 0);
	}

	bool	operator == (const OmnString &rhs) const 
	{
		if (mLen != rhs.mLen)
		{
			return false;
		}

		return (memcmp(mDataPtr, rhs.mDataPtr, mLen) == 0);
	}

	bool	operator == (const char *rhs) const 
	{
		if (!rhs)
		{
			if (mLen == 0)
			{
				return true;
			}
			return false;
		}

		return (rhs && (unsigned int)mLen == strlen(rhs) &&
				memcmp(mDataPtr, rhs, mLen) == 0);
	}

	bool operator < (const OmnString &rhs) const 
	{
		 return (strcmp(mDataPtr, rhs.mDataPtr) < 0);
	}

	// Chen Ding, 05/14/2011
	bool operator <= (const OmnString &rhs) const
	{
		return (strcmp(mDataPtr, rhs.mDataPtr) <= 0);
	}

	bool operator > (const OmnString &rhs) const
	{
		return (strcmp(mDataPtr, rhs.mDataPtr) > 0);
	}

	bool operator >= (const OmnString &rhs) const
	{
		return (strcmp(mDataPtr, rhs.mDataPtr) >= 0);
	}

	bool	isNull() const { return mLen == 0; }

	OmnString & assign(const char * const data, const int len);
	OmnString & assign(const uint numChars, const char c);

	void	hexStringToStr(const char *data,const int len); 

	OmnString &	append(const char *data, const u32 length);
	OmnString &	appendChar(const char data);
	OmnString & trimLastChars(const unsigned int numOfChar);

	u32 getHashKey() const
	{
	 	char c;
		u32 hashkey = 5381;
	   	for (u32 i=0; i<mLen; i++)
		{
			c = mDataPtr[i];
			hashkey = ((hashkey << 5) + hashkey) + c; // i.e., hash * 33 + c
		}
		return hashkey;
	}

	OmnString toString() const
	{
		return *this;
	}

	OmnString operator - (const OmnString &rhs) const
	{
		// 
		// This function returns a substring of this class
		// by removing the longest leading substrings that match 'rhs'.
		// 

		// 
		// If this is an empty string, return.
		//
		if (mLen == 0)
		{
			return "";
		}

		// 
		// If 'rhs' is an empty string, return this.
		//
		if (rhs == "")
		{
			return *this;
		}

		OmnString tmp(*this);

		uint index = 0;
		while (index < mLen && index < (uint)rhs.length() &&
			   mDataPtr[index] == rhs.mDataPtr[index])
		{
			index++;
		}

		tmp.remove(0, index);
		return tmp;
	}	
	
	bool remove(const uint starting, const int length);

	OmnString addSlashes() const;
	void chgBackSlashes() ;
	OmnString toHex() const
	{
		OmnString s(0, mLen);
		s.hexStringToStr(mDataPtr,mLen);
		return s;
	}

	bool setSize(const uint size)
	{
		// 
		// This function expands the string internal memory to the specified size
		// (if it is not that big yet). 
		//
		return determineMemory(size);
	}

	int getMemorySize() const
	{
		return mMemSize;
	}

	int find(const char c, const bool fromEnd) const 
	{
		// 
		// This function finds the first occurance of 'c'. If 'fromEnd' 
		// is true, it searches from the end. If not found, return -1.
		// If found, it returns the index of the char.
		//
		if (fromEnd)
		{
			for (int index=mLen-1; index >= 0; index--)
			{
				if (mDataPtr[index] == c)
				{
					return index;
				}
			}

			return -1;
		}

		for (uint index=0; index < mLen; index++)
		{
			if (mDataPtr[index] == c)
			{
				return index;
			}
		}

		return -1;
	}

	static int findPrefix(const char *str1, const char *str2)
	{
		int len1 = strlen(str1);
		int len2 = strlen(str2);
		int i = 0;
		while (i < len1 && i < len2 && str1[i] == str2[i]) i++;
		return i-1;
	}

	OmnString trimLastWord();
	int numWords() const;

	void	removeLeadingWhiteSpace();
	void	removeTailWhiteSpace();
	void	removeWhiteSpaces() 
			{
				removeLeadingWhiteSpace(); 
				removeTailWhiteSpace();
			}
	void	replaceStrWithSpace();

	void	removeLeadingSymbols();
	void	removeTailSymbols();

	// Cody, 2011/04/07
	int 	findSubString(
				const OmnString &sub,
				const int startPos, 
				const bool reverse = false) const
	{
		return findSubStrStatic(mDataPtr, mLen, sub.data(), sub.length(), startPos, reverse);
	}

	int static findSubStrStatic(
				const char *data, 
				const int data_len,
				const char *sub, 
				const int sub_len,
				const int start_pos,
				const bool reverse = false);
	
	bool	insert(const OmnString &str,const int insertPos);

	int		getWord(const unsigned int index, OmnString &substr) const;
	int		getSegment(unsigned int order, const char token, OmnString &substr) const;


	// add by Kevin Yang
	bool readBlock(const OmnString &start, const OmnString &end, OmnString &contents, bool &err);


	bool	hasPrefix(const OmnString &subStr) const;
	
	// 
	// Chen Ding, 02/15/2007
	//
	int		toInt() const;

	// Chen Ding, 02/20/2007
	OmnString	substr(const u32 start, const u32 end = 0) const;
	OmnString	subString(const u32 start, const u32 length) const;

	// Chen Ding, 02/28/2007
	bool	normalizeWhiteSpace(const bool trimLeadingSpaces, const bool trimTailingSpaces);

	// Chen Ding, 03/01/2007
	OmnString	getLine(const bool removeFlag, const u32 lineIndex = 1);

	// Chen Ding, 03/20/2007
	u32		substitute(char oldc, char newc, bool all);

	// Chen Ding, 05/10/2007
	int		removeLine(const u32 startPos);

	// Chen Ding, 05/16/2007
	bool	isEmptyLine() const;
	bool	isDigitStr(const bool allow_sign = true, const bool allow_dot = true) const;
	static bool	isSymbol(const char c);

	// Chen Ding, 10/05/2009
	bool	escapeFnameWS();
	bool	escapeSymbal(const char c, const char e);
	//james 11/1/2010
	bool	escapeSymbols();

	// Chen Ding, 01/10/2010
	bool	removeQuotes(const bool flag);

	// Chen Ding, 01/18/2010
	int		indexOf(const int startidx, const char pattern, const bool reverse=false) const;
	int		indexOf(const OmnString &pattern, const int idx) const;

	// Chen Ding, 05/21/2010
	bool	increment();

	// Chen Ding, 06/06/2010
	bool	hasCommonWords(const OmnString &rhs, const OmnString &sep) const;

	// Chen Ding, 06/21/2010
	bool	trim(const int num)
			{
				// It trims off 'num' number of characters from the end.
				if (num < 0) return false;
				if ((u32)num > mLen) return false;
				if (!mDataPtr) return false;
				mLen -= num;
				mDataPtr[mLen] = 0;
				return true;
			}

	// Fei Pei, 06/05/2014
	bool	trimZerosInNumber()
			{
				int pos;
				u32 i;

				// It trims off ".0000" from "34.0000" 
				// The caller need to make sure it is 
				// number string
				pos = indexOf(".", 0);
				if (pos < 0)
					return false;

				 for (i = pos + 1; i < mLen; i++)
				 {
					 if (mDataPtr[i] != '0')
						 break;
				 }

				 if (i >= mLen)
				 {
					 //all the chars after "." are '0'
					 //need to trim
					 trim(mLen - pos);
				 }

				 return true;
			}

	// Chen Ding, 08/04/2010
	void removeNonprintables(const bool removeflag = false)
	{
		if (!removeflag)
		{
			for (u32 i=0; i<mLen; i++)
			{
				if (mDataPtr[i] == 10 || mDataPtr[i] == 13) mDataPtr[i] = 32;
			}
		}
		else
		{
			for (u32 i=0; i<mLen; i++)
			{
				if (mDataPtr[i] == 10 || mDataPtr[i] == 13) 
				{
					memmove(&mDataPtr[i], &mDataPtr[i+1], mLen - i - 1);
					mLen--;
					i--;
				}
			}
		}
	}

	// James, 08/31/2010
	void removeChars(const char *chars, const int len, const bool printable)
	{
		if (!chars)
		{
			for (u32 i=0; i<mLen; i++)
			{
				if (printable && (mDataPtr[i] < 32 || mDataPtr[i] == 127))
				{
					memmove(&mDataPtr[i], &mDataPtr[i+1], mLen - i - 1);
				}
			}
		}
		else
		{
			for (u32 i=0; i<mLen; i++)
			{
				for (int j=0; j<len; j++)
				{
					if (mDataPtr[i] == chars[j] ||
						(printable && (mDataPtr[i] < 32 || mDataPtr[i] == 127)))
					{
						memmove(&mDataPtr[i], &mDataPtr[i+1], mLen - i - 1);
						mLen--;
						i--;
					}
				}
			}
		}
	}

	int removeWord(const OmnString &word, const OmnString &sep, const bool removeall);
	
	// Chen Ding, 10/14/2010
	bool	isEmpty() const {return mLen == 0;}

	// Cody, 2011/04/07
	int findParentDiv(const int start);

	// Lynch yang 08/17/2011
	void removeCDATA()
	{
		int pos = findSubString("]]>", 0);
		if (pos == -1) return;
		if ((u32)pos+2 < mLen)
		{
			mDataPtr[pos] = '}';
			mDataPtr[pos+1] = '}';
			mDataPtr[pos+2] = '>';
		}
	}

	inline bool containNull() const
	{
		for (u32 i=0; i<mLen; i++) if (!mDataPtr[i]) return true;
		return false;
	}

	// Chen Ding, 12/22/2011
	bool trimByChar(const char c);
	inline char getChar(const int idx) const 
	{
		if (idx >= 0 && (u32)idx < mLen) return mDataPtr[idx];
		return 0;
	}

	// Chen Ding, 06/02/2012
	OmnString	postfix(const char sep) const; 
	OmnString	getPrefix(const char sep) const; 

private:
	bool	determineMemory(const uint newsize);
	void init(const char *d, const int size);

public:
	// Chen Ding, 2013/01/19
	static int64_t getNumInstances();
	static int64_t getTotalInstances();

	// Chen Ding, 2013/05/21
	void convertToScientific();

	// Chen Ding, 2014/01/29
	bool trimSpaces(const bool leading = true, const bool trail = true);
};

inline bool
operator == (const OmnSimpleStr &lhs, const OmnString &rhs)
{
	if ((unsigned int)lhs.len != rhs.mLen)
	{
		return false;
	}
	return (memcmp(lhs.mData, rhs.mDataPtr, lhs.len) == 0);
}

inline bool
operator == (const OmnSimpleStr &lhs, const char *rhs)
{
	return (rhs && (unsigned int)lhs.len == strlen(rhs) &&
			 memcmp(lhs.mData, rhs, lhs.len) == 0);
}

inline bool
operator != (const OmnSimpleStr &lhs, const char *rhs)
{
	if (strlen(rhs) != (unsigned int)lhs.len) return true;
	return !(operator == (lhs, rhs));
}

inline bool
operator != (const OmnSimpleStr &lhs, const OmnString &rhs)
{
	if (rhs.mLen != (unsigned int)lhs.len) return true;
	return !(operator == (lhs, rhs));
}

#endif
