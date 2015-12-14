////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMtr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_NMS_MemMtr_h
#define Snt_NMS_MemMtr_h

#include "aosUtil/Types.h"
#include "Util/String.h"
#include "NMS/HouseKpObj.h"
#include "Util/ValList.h"
#include "NMS/CapProc.h"

#include "SingletonClass/SingletonTplt.h"

#include "Obj/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/SoMemMtr.h"
#include "Util/RCObjImp.h"
#include "XmlParser/Ptrs.h"



OmnDefineSingletonClass(OmnMemMtrSingleton,
						OmnMemMtr,
						OmnMemMtrSelf,
						OmnSingletonObjId::eMemMtr,
						"memMtr");



struct Entry
{
    bool        mIsGood;
    char        mFile[200];
    uint		mLine;
    uint		mLocId;
    uint		mNewNum;
    uint		mDelNum;
	size_t		mSize;

public:
    Entry()
    :
    mIsGood(false),
    mLine(0),
    mLocId(0),
    mNewNum(0),
    mDelNum(0),
	mSize(0)
    {
        mFile[0] = 0;
    }
};

class OmnMemMtr : public OmnHouseKpObj
{
private:
	enum 
	{
		eWriteFreq 		= 30, 		  // 2 seconds per call
		eMaxColumnSize 	= 20,
		eEntrySize  	= 1024,
    };

	static Entry		 	mEntry[eEntrySize][eMaxColumnSize];
	static OmnSoMemMtr 	    mSo[eEntrySize];

public:
	static int64_t			mLastWriteTick;
	static int 				mTotalNew;
	static int 				mTotalNewArray;
	static int				mTotalDel;
	static int 				mTotalDelArray;
		

public:
	OmnMemMtr();
	virtual ~OmnMemMtr();
	
	static void memInitStat();

	static int		memCreated( const char *file, 
								const int 	line,
								const size_t size);

	static void 	memDeleted( const char *file,
          	        		   	const int 	line,
            		    	   	const int  	locId);

	static int		getLocId(const char    *file, 
							 const int 		line,
							 const size_t 	size);

	static void		writeToDb();
	
	static OmnMemMtr *	getSelf();
	bool		start();
	bool		stop()  { return true;}
	bool		config(const OmnXmlParserPtr &parser) { return true;}

	virtual void		procHouseKeeping(const int64_t &tick);
	virtual OmnString	getName() const {return "MemMtr";}
};

#endif

