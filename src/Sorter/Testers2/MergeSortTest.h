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
// 2013/02/12 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_MergeSortTest_h
#define Omn_TestUtil_MergeSortTest_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/CompareFun.h"
#include "Util/String.h"
#include "Util/File.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileObj.h"
#include "Sorter/MergeFileSorter.h"
#include "Thread/ThrdShellProc.h"

struct i64_struct
{
	vector<i64>	current;
	i64	sum;
	i64	max;
	i64	min;
};

struct d64_struct
{
	vector<d64>	current;
	d64	sum;
	d64	max;
	d64	min;
};

struct u64_struct
{
	vector<u64>	current;
	u64	sum;
	u64	max;
	u64	min;
};

struct str_struct
{
	vector<OmnString> current;
	OmnString max;
	OmnString min;
};

struct aggrInfo
{
	OmnString		con_str;
	u64 			num;
	vector<i64_struct>	i64_aggr;
	vector<u64_struct>	u64_aggr;
	vector<d64_struct>	d64_aggr;
	vector<str_struct>	str_aggr;
};

struct str_hash
{
	size_t operator()(const OmnString& str) const
	{
		if(str.length()>=2)
		{
			return (u64)str.getChar(0)+(u64)str.getChar(1);
		}
		return (u64)str.getChar(0);
	}
};

struct str_cmp
{
	bool operator()(const OmnString& str1, const OmnString& str2) const
	{
		return str1==str2;
	}
};

class AosMergeSortTest : public OmnTestPkg
{
public:
	AosMergeSortTest();
	~AosMergeSortTest() {}
	
	bool 		start();
	bool 		sorttorturer();
public:
	int 						mFileLen;
	AosRundataPtr				mRundata;
	AosCompareFunPtr		 	mCmpFun;
	vector<AosNetFileObjPtr> 	mVirFiles;
	AosMergeFileSorterPtr 		mSort;
	enum 	fieldType
	{
		eI64,
		eD64,
		eU64,
		eStr
	};

	enum 	aggrType
	{
		eNorm,
		eMax,
		eMin,
		eSet
	};

private:
	u64					mNumFields;
	u64					mSeed;
	
	u64					mNumber;
	u64					mTime;
	u64					mRound;
	i64 				mMaxRoundTime;
	i64 				mMaxMemory;
	i64 				mAlarmFlag;
	OmnFilePtr			mStateFile;
	
	bool				mPrintFlag;
	i64					mNumRecords;
	u64					mNumKeyFields;
	u64					mNumAggrFields;
	u64					mFieldLen;
	i64					mHashPos;
	OmnString			mHashKey;
	u64					mFixStrLen;
	OmnString			mConf;
	vector<OmnString>	mHashKeys;
	bool*				mIfStrFromHash;
	bool 				mIfFixBin;
	bool				mIfAggr;
	bool				mIfAggrSet;
	i64*				mKeyFieldPos;
	u64*				mAggrFieldPos;
	OmnString			mCharElem;
	hash_map<OmnString, aggrInfo, str_hash, str_cmp>	mHash;
	hash_map<OmnString, aggrInfo, str_hash, str_cmp>::iterator	mHashItr;
	struct fieldInfo
	{
		fieldType type;
		int	fieldpos;
		int strlen;
		aggrType aggrtype;
	};
	vector<fieldInfo> mFieldInfo;
	struct fieldValue
	{
		fieldType type;
		OmnString str_value;
		i64	i64_value;
		u64	u64_value;
		d64 d64_value;
		i64	current;
	};
	vector<fieldValue> mFieldValues;
private:
	bool 		deleteFile();
	void		configKeyAggrField();
	bool		checkForBuff();
	bool		checkForFixBin();
	void		init();
	bool		createFiles(const AosRundataPtr &rdata);
	void 		createCmpFunc(const AosRundataPtr &rdata);
	bool		percent(const u64&);
	void		configFieldType();
	i64			getI64(const i64&, const u64&);
	u64			getU64(const i64&, const u64&);
	d64			getD64(const i64&, const u64&);
	int 		getLen(const fieldType&, const u64&);
	OmnString	getStr(const i64&, const u64&, const u64&);
	OmnString	getStr(const i64&, const u64&);
	OmnString	randStr(const u64& len);
	bool		createBuffFiles(const AosRundataPtr&);
	bool		createFixBinFiles(const AosRundataPtr&);
	OmnString			createHashKey(const vector<fieldValue>&);
	i64			strToNum(const OmnString&);
	void		updateRecord(const aggrInfo);
	void		printRecord(char*);
	bool		checkAggr(vector<fieldValue>&);
	bool		checkAggrNum(const OmnString&, const u64&);
	bool		aggrPosSort(u64*,const u64);
	bool		checkRecordSort(const char*, const char*);
	bool		checkSortByFieldValue(vector<fieldValue>&, vector<fieldValue>&);
	void 		printFieldValue(const vector<fieldValue>& field_value);
	OmnString	toString(const fieldType type)
	{
		OmnString str_type = "";
		switch(type)
		{
			case eStr:
				mIfFixBin ? str_type = "str" : str_type = "buffstr";
				return str_type;
			case eD64:
				return "double";
			case eI64:
				return "i64";
			case eU64:
				return "u64";
		}
		OmnAlarm << enderr;
		return "";
	}

	OmnString	toString(const aggrType type)
	{
		OmnString str_type = "";
		switch(type)
		{
			case eNorm:
				return "norm";
			case eMax:
				return "max";
			case eMin:
				return "min";
			case eSet:
				return "set";
		}
		OmnAlarm << enderr;
		return "";
	}

	OmnString	toString2(const fieldType type)
	{
		switch(type)
		{
			case eStr:
				return "str";
			case eD64:
				return "bin_double";
			case eI64:
				return "bin_int64";
			case eU64:
				return "bin_u64";
		}
		OmnAlarm << enderr;
		return "";
		
	}


	
};

class AosGetOutputFile: public OmnThrdShellProc
{
	OmnDefineRCObject;

	private:
	AosRundataPtr               mRundata;
	AosCompareFunPtr		 	mCmpFun;
	AosMergeFileSorterPtr 		mMergeSort;
	bool						mIsMerge;

	public:
	AosGetOutputFile(
			const AosMergeFileSorterPtr &merge_sort,
			const AosCompareFunPtr &cmo_fun,
			const bool is_merge,
			const AosRundataPtr &rdata)
		:
			OmnThrdShellProc("MutilFileSortThrd"),
			mMergeSort(merge_sort),
			mCmpFun(cmo_fun),
			mIsMerge(is_merge),
			mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
	{
	}

	virtual bool run()
	{
//		bool rslt = false;
		AosBuffPtr buff = mMergeSort->nextBuff();
		while (buff)
		{
			buff = mMergeSort->nextBuff();
		}
		return true;
	}
	virtual bool procFinished()
	{
		return true;
	}
};
#endif

