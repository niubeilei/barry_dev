////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataShuffler_DataShuffler_h
#define Aos_DataShuffler_DataShuffler_h

#include "DataShuffler/Ptrs.h"
#include "DataShuffler/DataShufflerType.h"
#include "DataCacher/DataCacher.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/DataTypes.h"

class AosDataShuffler : virtual public OmnRCObject
{
	OmnDefineRCObject;        
public:
//	enum DataType
//	{
//		eInvalidDataType,
//
//		eStr,
//		eU32,
//		eU64,
//
//		eMaxiDataType
//
//	};

	enum
	{
	    eMinSizeToRunInThreads = 100000
	};

public:
	class Shuffler : public OmnThrdShellProc 
	{
		OmnDefineRCObject;
	public:
		AosDataShufflerPtr			mDataShuffler;
		AosDataCacherPtr 			mDataCacher;
		vector<AosDataCacherPtr> 	mDataCachers;
		int64_t 					mStartIdx;
		int64_t						mNumEntries;
		AosRundataPtr 				mRundata;
	
	public:
		Shuffler(
				const AosDataShufflerPtr &shuffler,
				const AosDataCacherPtr &cacher,
				vector<AosDataCacherPtr> &cachers,	
				int64_t	start_idx,
				int64_t num_entries,	
				const AosRundataPtr &rdata)
		:
		mDataShuffler(shuffler),
		mDataCacher(cacher),
		mDataCachers(cachers),
		mStartIdx(start_idx),
		mNumEntries(num_entries),
		mRundata(rdata)
		{
		}

		virtual bool run();
		virtual bool procFinished();
	};
protected:
	AosDataShufflerType::E		mType;
	vector<AosDataCacherPtr>	mDataCachers;
	int 						mNumServers;
	AosDataType::E				mDataType;
	int         				mNumThrds;    
	//bool        				mRunMultiThrd;

public:
	AosDataShuffler(
			const OmnString &name, 
			const AosDataShufflerType::E type,
			const bool flag);
	~AosDataShuffler();

	virtual bool shuffle(
					const AosDataCacherPtr &cacher, 
					vector<AosDataCacherPtr> &cachers, 
					const AosRundataPtr &rdata) = 0;

	virtual bool appendEntry(const char *data, const int len, const AosRundataPtr &rdata) = 0;

	virtual AosDataShufflerPtr clone(
					const AosXmlTagPtr &def, 
					const AosRundataPtr &rdata) = 0;

	static AosDataShufflerPtr getDataShuffler(const AosXmlTagPtr &def, const AosRundataPtr &rdata);

	//bool sendFinish(const AosRundataPtr &rdata);
	//bool sendStart(const AosRundataPtr &rdata);
	
	AosDataShufflerPtr getDistMapObj(
			const AosXmlTagPtr &def,
			const AosRundataPtr &rdata);

	vector<AosDataCacherPtr> getDataCachers(){return mDataCachers;}
	void updateDistMap(const AosBuffArrayPtr &mm);

protected:
	virtual bool shuffle(
				const AosDataCacherPtr &cacher,
				vector<AosDataCacherPtr> &cachers,
				const int64_t &start_idx,
				const int64_t &num_entries,
				const AosRundataPtr &rdata) = 0;
 
	bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata);
	bool registerShuffler(const OmnString &name, const AosDataShufflerPtr  &blob);
	bool init(const AosRundataPtr &rdata);

	/*
	DataType getDataType(const OmnString &name)
	{
		const char *c = name.data();
		switch(c[0])
		{
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

		default:
			break;
		}
		return eInvalidDataType;
	}

	bool isValid(const DataType code)
	{
	    return code > eInvalidDataType && code < eMaxiDataType;
	}

	char *getKey(const char *data, const int startPos, const int key_len)
	{
		switch (mDataType)
		{
		case eStr:
			 // strncpy(kk.getBuffer(), &data[startPos], key_len); 
			 return &data[startPos]; 
		
		case eU64:
			 kk << *(u64*)(data + startPos); 
			 return kk;

		case eU32:
			 kk << *(u32*)(data + startPos);
			 return kk;

		default:
			 break;
		}
		return "";
	}
	*/

	bool	shufflerInThreads(
				const AosDataCacherPtr &cacher,
				vector<AosDataCacherPtr> &cachers,	
				const AosRundataPtr &rdata);
};

#endif

