////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashUtil.h
// Description:
// 	This class is the super class for all hash tables.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_HashUtil_h
#define Snt_Util_HashUtil_h

#include "Util/BasicTypes.h"
#include "Util/String.h"
#include <map>
#include <vector>
#include <ext/hash_map>

using namespace std;


class OmnHashUtil 
{
private:
	enum
	{
		eMaxNumBits = 32
	};

	static unsigned int		mBitMasks[eMaxNumBits];

public:
	static unsigned int	getBitmask(const int numBits);
	static unsigned int	getNearestBitmask(const uint value);
	static bool			verifyBitmask(const uint value);
};

namespace std
{
	using namespace __gnu_cxx;
	struct Omn_Str_hash
	{
		inline size_t operator()(const OmnString& str) const
		{
			/*unsigned long __h = 0;
			for (size_t i=0; i<(size_t)str.length(); i++)
				__h = 5*__h + str[i];
			return size_t(__h);*/
			return __stl_hash_string(str.getBuffer());
		}
	};

	struct GenericStrHash
	{
		inline size_t operator()(const OmnString& str) const
		{
			const char *word = str.data();
			int wordLen = str.length();

			u64 hashkey = 5381;
			char c;
			for (int i=0; i<wordLen; i++)
			{
    			c = word[i];
    			hashkey = ((hashkey << 5) + hashkey) + c; // i.e., hash * 33 + c 
			}
			return hashkey;	
		}
	};

	struct char_str_hash
	{
		inline size_t operator() (const char *str) const
		{
			return __stl_hash_string(str);
		}
	};

	struct compare_str
	{
		inline bool operator()(const OmnString &p1, const OmnString &p2) const
		{
			return p1==p2;
		}
	};  

	struct str_less
	{
		inline bool operator()(const OmnString &s1, const OmnString &s2) const
		{
			return strcmp(s1.data(), s2.data()) < 0;
		}
	};  

	struct compare_charstr 
	{
		inline bool operator()(const char *p1, const char *p2) const
		{
			return strcmp(p1, p2) == 0;
		}
	};  

	struct lessthan_charstr 
	{
		inline bool operator()(const char *p1, const char *p2) const
		{
			return strcmp(p1, p2) < 0;
		}
	};  

	struct AosU64Hash1
	{
		size_t operator()(const u64& value) const
		{
			u16 __h = 0;
			__h = ((u16*)(&value))[0] +
			  	((u16*)(&value))[1] +
			  	((u16*)(&value))[2] +
			  	((u16*)(&value))[3];
        	return size_t(__h);
		}
	};

	struct u32_hash
	{
	    size_t operator()(const u32 i) const
	    {
	        return i;
	    }
	};

	struct u64_hash
	{
	    size_t operator()(const u64 i) const
	    {
	        return size_t(i);
	    }
	};

	struct u32_cmp
	{
	    bool operator()(const u32 &p1, const u32 &p2) const
	    {
	        return p1==p2;
	    }
	};

	struct u64_cmp
	{
	    bool operator()(const u64 &p1, const u64 &p2) const
	    {
	        return p1==p2;
	    }
	};
};

typedef hash_map<const char *, u8, char_str_hash, compare_charstr> AosCharS2U8_t;
typedef hash_map<const char *, u8, char_str_hash, compare_charstr>::iterator AosCharS2U8Itr_t;

typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str> AosStr2U64_t;
typedef hash_map<const OmnString, u64, Omn_Str_hash, compare_str>::iterator AosStr2U64Itr_t;

typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str> AosStr2Str_t;
typedef hash_map<const OmnString, OmnString, Omn_Str_hash, compare_str>::iterator AosStr2StrItr_t;

typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str> AosStr2U32_t;
typedef hash_map<const OmnString, u32, Omn_Str_hash, compare_str>::iterator AosStr2U32Itr_t;

typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str> AosStr2Int_t;
typedef hash_map<const OmnString, int, Omn_Str_hash, compare_str>::iterator AosStr2IntItr_t;

typedef hash_map<const OmnString, int64_t, Omn_Str_hash, compare_str> AosStr2Int64_t;
typedef hash_map<const OmnString, int64_t, Omn_Str_hash, compare_str>::iterator AosStr2Int64Itr_t;

// Chen Ding, 2013-04-14
// typedef hash_map<const u64, u32, u32_hash, u32_cmp> AosU642U32_t;
// typedef hash_map<const u64, u32, u32_hash, u32_cmp>::iterator AosU642U32Itr_t;
typedef hash_map<const u64, u32, u64_hash, u32_cmp> AosU642U32_t;
typedef hash_map<const u64, u32, u64_hash, u32_cmp>::iterator AosU642U32Itr_t;

typedef hash_map<const u64, u64, u64_hash, u64_cmp> AosU642U64_t;
typedef hash_map<const u64, u64, u64_hash, u64_cmp>::iterator AosU642U64Itr_t;

typedef hash_map<const u64, int, u64_hash, u64_cmp> AosU642Int_t;
typedef hash_map<const u64, int, u64_hash, u64_cmp>::iterator AosU642IntItr_t;

typedef hash_map<const u64, char, u64_hash, u64_cmp> AosU642Char_t;
typedef hash_map<const u64, char, u64_hash, u64_cmp>::iterator AosU642CharItr_t;

typedef hash_map<const u64, OmnString, u64_hash, u64_cmp> AosU642Str_t;
typedef hash_map<const u64, OmnString, u64_hash, u64_cmp>::iterator AosU642StrItr_t;

typedef hash_map<const u32, OmnString, u32_hash, u32_cmp> AosU322Str_t;
typedef hash_map<const u32, OmnString, u32_hash, u32_cmp>::iterator AosU322StrItr_t;

// Chen Ding, 2013-04-14
typedef hash_map<const u64, bool, u64_hash, u32_cmp> AosU64BoolHash;
typedef hash_map<const u64, bool, u64_hash, u32_cmp>::iterator AosU64BoolHashItr;

//Ken Lee 2013/04/01
u64 AosGetHashKeyInMap(const char *word, const int wordLen);

// Chen Ding, 04/03/2012
inline u64 AosGetHashKey(const char *word, const int wordLen, const bool use_manual_hash = false)
{
	if (use_manual_hash)
	{
		u64 hashkey_inmap = AosGetHashKeyInMap(word, wordLen);
		if(hashkey_inmap != 0) return hashkey_inmap;	
	}
	
	u64 hashkey = 5381;
	char c;
	for (int i=0; i<wordLen; i++)
	{
    	c = word[i];
    	hashkey = ((hashkey << 5) + hashkey) + c; // i.e., hash * 33 + c 
	}
	hashkey &= 0x3fffffff;
	return hashkey;	
}

inline u64 AosGetHashKey(const OmnString &word)
{
	return AosGetHashKey(word.data(), word.length(), true);
}

#endif

