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
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_CompareFun_h
#define AOS_Util_CompareFun_h

#include "SEInterfaces/AggregationType.h"
#include "SEInterfaces/DataRecordType.h"
#include "SEInterfaces/DataFieldType.h"
#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Orders.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include <string.h>
#include <vector>
using namespace std;

#define AOSCOMPAREFUNC_U64U641	"u64u641"
#define AOSCOMPAREFUNC_U64U642 "u64u642"
#define AOSCOMPAREFUNC_StrU641 "stru641"
#define AOSCOMPAREFUNC_StrU642 "stru642"
#define AOSCOMPAREFUNC_Custom  "custom"
#define AOSCOMPAREFUNC_VarStr  "varstr"


class AosCompareFun : public OmnRCObject
{
	OmnDefineRCObject;
private:
	
public:
	enum FunType
	{
		eInvalid,

		eU64U64IIL1,
		eU64U64IIL2,
		eStrU64IIL1,
		eStrU64IIL2,
		eCustom,
		eDocAsmComp,
		eStatComp,		// Ketty 2014/05/13
		eVarStr,

		eMax
	};

	enum DataType
	{
		eInvalidDataType,

		eStr,
//		eNumStr,
		eBuffStr,
		eI32,
		eI64,
		eU32,
		eU64,
		eDouble,
		eRecord,
		eNumber,

		eMaxDataType

	};

	struct AosAgr
	{
		u32 mAgrPos;
		DataType  mAgrType;
		DataType  mAgrFieldType;
		AosDataColOpr::E mAgrFun;
	};

	bool 	mReverse;
	int 	size;
	AosDataRecordType::E	mRecordType;

	//AosDataType::E	mDataType;
	vector<AosAgr> 	mAosAgrs;
	int (*mCmpFuncs[15])(const char *lhs, const char* rhs, const int cmp_pos);

	AosCompareFun(int s, bool reverse):mReverse(reverse), size(s){
	};

	~AosCompareFun(){
	};
	virtual bool operator()(const char* lhs, const char* rhs)
	{
		return cmp(lhs, rhs) < 0;
	}
	virtual int cmp(const char* lhs, const char* rhs)
	{
		if (mReverse) return cmpPriv(lhs, rhs) * -1;
		return cmpPriv(lhs, rhs);
	}
	virtual AosCompareFunPtr clone() = 0;
	// Chen Ding, 10/21/2012
	virtual FunType getFuncType() const = 0;

	virtual int getRecordFieldsNum(){ return -1;}
	virtual bool getDataFieldsType(vector<AosDataFieldType::E> &types) { return false;}

	virtual bool isSame(const AosCompareFunPtr &rhs) const = 0;

	virtual void setReverse(const bool flag){mReverse = flag;}

	bool mergeData(char *v1, char *v2);
	bool hasMerge() {return !mAosAgrs.empty();}

private:
	virtual int cmpPriv(const char* lhs, const char* rhs) = 0;
	bool agrU64(char *v1, char *v2, AosDataColOpr::E agr_fun, u32 agr_pos);
	bool agrInt64(char *v1, char *v2, AosDataColOpr::E agr_fun, u32 agr_pos);
	bool agrDouble(char *v1, char *v2, AosDataColOpr::E agr_fun, u32 agr_pos);

public:
	virtual void setSize(const int &sz)
	{
		size = sz;
	}
	
	static AosCompareFunPtr getCompareFunc(const AosXmlTagPtr &config);
	static FunType getFunType(const OmnString &name)
	{
		if (name == AOSCOMPAREFUNC_U64U641) return eU64U64IIL1;
		if (name == AOSCOMPAREFUNC_U64U642) return eU64U64IIL2;
		if (name == AOSCOMPAREFUNC_StrU641) return eStrU64IIL1;
		if (name == AOSCOMPAREFUNC_StrU642) return eStrU64IIL2;
		if (name == AOSCOMPAREFUNC_Custom) return eCustom;
		if (name == AOSCOMPAREFUNC_VarStr) return eVarStr;
		return eInvalid;
	}

	static OmnString getDataTypeStr(const DataType type)
	{
		switch(type)
		{
		case eStr:
			return "str";
	//	case eNumStr:
	//		return "";
		case eBuffStr:
			return "buffstr";
		case eI32:
			return "i32";
		case eI64:
			return "i64";
		case eU32:
			return "u32";
		case eU64:
			return "u64";
		case eDouble:
			return "double";
		case eRecord:
			return "record";
		default:
			return "";
		}
	}
	static DataType getDataType(const OmnString &name)
	{
		const char *c = name.data();
		switch(c[0])
		{
		case 'i':
			 if (name == "i32")
				 return eI32;
			 if (name == "i64")
				 return eI64;
			 break;

		case 'u':
			 if (name == "u32")
				 return eU32;
			 if (name == "u64")
				 return eU64;
			 break;

		case 's':
			 if (name == "str")
				 return eStr;
			 break;

//		case 'n':
//			 if (name == "nstr")
//				 return eNumStr;
//			 break;

		case 'd':
			 if (name == "double")
				 return eDouble;
			 break;

		case 'b':
			 if (name == "buffstr")
				 return eBuffStr;
			 break;
		case 'r':
			 if (name == "record")
				 return eRecord;

		default:
			 break;
		}
		return eInvalidDataType;
	}
};

/*
class AosFunU64U641: public AosCompareFun
{
	//the entrie size should  16 bytes.
	//the entrie format should: 0-7 key(value), 9-16 value(docid).
public:
	virtual FunType getFuncType() const {return eU64U64IIL1;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eU64U64IIL1;
	}

	AosFunU64U641(bool reverse = false):AosCompareFun(sizeof(u64)*2, reverse){}
	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosFunU64U641(mReverse);
	}
private:
	virtual int cmpPriv(const char* lhs, const char* rhs)
	{
		int rslt;
		u64 l = *(u64*)lhs;
		u64 r = *(u64*)rhs;
		if (l==r) 
			rslt = 0;
		else if(l<r) 
			rslt = -1;
		else 
			rslt = 1;
		return rslt;
	}
};


class AosFunU64U642: public AosCompareFun
{
	//the entrie size should  16 bytes.
	//the entrie format should: 0-7 key(value), 9-16 value(docid).
public:
	virtual FunType getFuncType() const {return eU64U64IIL2;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eU64U64IIL2;
	}

	AosFunU64U642(bool reverse = false):AosCompareFun(sizeof(u64)*2, reverse){}
	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosFunU64U642(mReverse);
	}
private:
	virtual int cmpPriv(const char* lhs, const char* rhs)
	{
		int rslt;
		u64 l = *(u64*)lhs;
		u64 r = *(u64*)rhs;
		if (l==r) 
		{
			u64 l_ = *(u64*)(lhs+sizeof(u64));
			u64 r_ = *(u64*)(rhs+sizeof(u64));
			if (l_==r_) 
			{
				rslt = 0;
			}
			else if(l_<r_) 
			{
				rslt = -1;
			}
			else 
			{
				rslt = 1;
			}
		}
		else if(l<r) 
		{
			rslt = -1;
		}
		else 
		{
			rslt = 1;
		}
		return rslt;
	}
};
*/
class AosFunStrU641: public AosCompareFun
{
	// This compare function assumes entries are 'size' number of
	// bytes long (fixed length). 'key' is the first 'size' - sizeof(u64)
	// bytes and the value is the last 8 bytes, converted to u64.
	//
	// It does not support numerical alphabetic sorting.
	//
	//the entrie size should  size bytes.
	//the entrie format should: 0-size--8 key(value), size-8--size value(docid).
	bool mNumAlphabetic;
public:
	virtual FunType getFuncType() const {return eStrU64IIL1;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eStrU64IIL1;
	}

	AosFunStrU641(int size, bool reverse = false, bool numalpha = false)
		:AosCompareFun(size, reverse), mNumAlphabetic(numalpha){}
	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosFunStrU641(size, mReverse, mNumAlphabetic);
	}
private:
	virtual int cmpPriv(const char* lhs, const char* rhs)
	{
		// Chen Ding, 06/16/2012
		int rslt;
		if (mNumAlphabetic)
		{
			int lLen = strlen(lhs);
			int rLen = strlen(rhs);
			if(lLen < rLen)
			{
				rslt = -1;
				return rslt;
			}
			else if(lLen > rLen)
			{
				rslt = 1;
				return rslt;
			}
		}
		rslt = strcmp(lhs, rhs);
		return rslt;
	}
};

class AosFunVarStr : public AosCompareFun
{
	bool mNumAlphabetic;

public:
	virtual FunType getFuncType() const {return eVarStr;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eVarStr;
	}

	AosFunVarStr(int size, bool reverse, bool numalpha)
	:
	AosCompareFun(size, reverse), 
	mNumAlphabetic(numalpha){}

	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosFunVarStr(size, mReverse, mNumAlphabetic);
	}

	virtual int cmp(const char* lhs, const int llen,
					const char* rhs, const int rlen)
	{
		if (mReverse) return cmpPriv(lhs, llen, rhs, rlen) * -1;
		return cmpPriv(lhs, llen, rhs, rlen);
	}

private:
	virtual int cmpPriv(const char* lhs, const char* rhs);
	virtual int cmpPriv(const char* lhs, const int llen,
						const char* rhs, const int rlen);
};

/*
class AosFunStrU642: public AosCompareFun
{
	//the entrie size should  size bytes.
	//the entrie format should: 0-size--8 key(value), size-8--size value(docid).
	bool mNumAlphabetic;
public:
	virtual FunType getFuncType() const {return eStrU64IIL2;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eStrU64IIL2;
	}

	AosFunStrU642(int size, bool reverse = false, bool numalpha = false)
		:AosCompareFun(size, reverse), mNumAlphabetic(numalpha){}
	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosFunStrU642(size, mReverse, mNumAlphabetic);
	}
private:
	virtual int cmpPriv(const char* lhs, const char* rhs)
	{
		int rslt;
		if (mNumAlphabetic)
		{
			int lLen = strlen(lhs);
			int rLen = strlen(rhs);
			if(lLen < rLen)
			{
				rslt = -1;
				return rslt;
			}
			else if(lLen > rLen)
			{
				rslt = 1;
				return rslt;
			}
		}
		rslt = strcmp(lhs, rhs);
		if (rslt == 0)	
		{
			u64 l = *(u64*)(lhs + size - sizeof(u64));
			u64 r = *(u64*)(rhs + size - sizeof(u64));
			if (l==r) 
				rslt = 0;
			else if(l<r) 
				rslt = -1;
			else 
				rslt = 1;
		}
		return rslt;
	}
};
*/

class AosFunCustom: public AosCompareFun
{
	struct CmpFieldInfo
	{
		int 		mCmpPos;
		DataType	mDataType;
		int			mCmpLen;
		DataType	mFieldType;
		bool		mIsReserve;

		CmpFieldInfo(){}
	};

	CmpFieldInfo 					mFields[50];
	u32								mFieldSize;
	vector<AosDataFieldType::E>		mFieldsType;
	int								mRcdFdsNum;
public:
	virtual int getRecordFieldsNum(){return mRcdFdsNum;}
	virtual bool getDataFieldsType(vector<AosDataFieldType::E> &types) { types = mFieldsType; return true;}
	virtual FunType getFuncType() const {return eCustom;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eCustom;
	}

	AosFunCustom(int size, bool reverse,  const AosXmlTagPtr &config);
	
	virtual AosCompareFunPtr clone()
	{
		return 0;
	}
private:
	virtual int cmpPriv(const char* lhs, const char* rhs);
	//virtual int compare(const char* lhs, const char* rhs, const int cmp_pos, const DataType &cmp_type, const int cmp_len, DataType &field_type, const bool isReserve);
	//virtual int startCompare(const char* lhs, const char* rhs, const int cmp_pos, const DataType &cmp_type, const int cmp_len);
};


/*
class AosFunDocAsmComp: public AosCompareFun
{
	// This compare function assumes entries are 'size' number of
	// bytes long (fixed length). 'key' is the first 'size' - sizeof(u64)
	// bytes and the value is the last 8 bytes, converted to u64.
	//
	// It does not support numerical alphabetic sorting.
	//
	//the entrie size should  size bytes.
	//the entrie format should: 0-size--8 key(value), size-8--size value(docid).
public:
	virtual FunType getFuncType() const {return eDocAsmComp;}
	virtual bool isSame(const AosCompareFunPtr &rhs) const 
	{
		aos_assert_r(rhs, false);
		return rhs->getFuncType() == eDocAsmComp;
	}

	AosFunDocAsmComp(int size, bool reverse = false)
		:AosCompareFun(size, reverse){}
	virtual AosCompareFunPtr clone()
	{
		return OmnNew AosFunDocAsmComp(size, mReverse);
	}
private:
	virtual int cmpPriv(const char* lhs, const char* rhs)
	{
		u64 l = *(u64*)lhs;
		u64 r = *(u64*)rhs;
		int rslt = 0;
		if (l < r)
		{
			rslt = -1;
		}
		else if (l > r)
		{
			rslt = 1;
		}
		return rslt;
	}
};
*/

#endif

