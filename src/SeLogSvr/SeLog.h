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
// Modification History:
// 06/23/2011	Created by Tom
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_SeLog_h
#define AOS_SeLogSvr_SeLog_h

#include "ErrorMgr/ErrmsgId.h"
#include "ReliableFile/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SeLogSvr/Ptrs.h" 
#include "SeLogSvr/LogAging.h"
#include "SeLogSvr/MdlLogSvr.h"
#include "SeLogSvr/SeLogEntry.h"
#include "SeLogUtil/LogUtil.h"
#include "SeLogUtil/LogOpr.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObject.h"
#include "Util/File.h"
#include "UtilHash/HashedObj.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlUtil.h"
#include <map>



class AosSeLog : public AosHashedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxLogSize = 0xffff,
		eBodyStart = 1024,
		eEndFlag = 0xffffffff,
		eQueryLog = 20,
		eLogFileReservedSize = 200,
		eLogStartPos = 100,
		eDftMaxLogLength = 1000000000,	// 1G
		eDftMaxVersions = 100,

		eContainerNotFoundSec = 10,
		eAccessDeniedSec = 5,
		eInternalErrorSec = 10,
		eDefaultErrorSec = 10,
		eMaxKeyLength = 80
	};

private:
	// The following member data need to be stored in files
	OmnString			mPctrObjid;
	OmnString			mLogName;
	int					mNumFiles;
	u32					mLastSeqno;
	OmnString			mIILName;
	AosLogType::E		mLogType;
	int					mMaxLogLength;
	int					mModuleId;

	// The following member data are not saved in files.
	OmnMutexPtr 		mLock;
	//OmnFilePtr			mLastFile;			
	AosReliableFilePtr mLastFile;		// Ketty 2012/12/05	
	u64					mWritePos;
	AosLogAgingPtr  	mAging;
	int 				mNumRecord;
	AosErrmsgId::E		mErrid;
	AosSeLogEntryPtr	mLogEntry;
	int					mCreationSec;
	AosMdlLogSvrPtr		mMdlLogSvr;
	bool				mHashLogInitFlag;
public:
	AosSeLog();
	AosSeLog(AosBuff &buff);
	AosSeLog(
			const OmnString &key,
			const AosErrmsgId::E errid);
	AosSeLog(
			const OmnString &key,
			const OmnString &pctr_objid,
			const OmnString &logname,
			const AosXmlTagPtr &log_config,
			const int &moduleId,
			const AosRundataPtr &rdata);
	AosSeLog(
			const OmnString &key,
			const OmnString &pctr_objid,
			const OmnString &logname,
			const int &moduleId,
			const AosRundataPtr &rdata);
	~AosSeLog();

	// HashedObj Interface
	virtual bool 			isSame(const AosHashedObjPtr &rhs) const;
	
	virtual int  			doesRecordMatch(
							AosBuff &buff,
							const OmnString &key_in_buff,
							const OmnString &expected_key,
							AosHashedObjPtr &obj);
	
	virtual int 			setContentsToBuff(AosBuff &buff);
	virtual AosHashedObjPtr clone();
	virtual bool 			reset();

	// Member functions
	// Ketty 2013/03/20
	//bool					procRequest(const AosXmlTagPtr &trans, const AosRundataPtr &rdata);
	
	bool					isValid() const {return mErrid != AosErrmsgId::eNoError;}
	
	int						getNumFiles() const {return mNumFiles;}
	
	OmnString 				getPctrObjid() const {return mPctrObjid;}
	
	OmnString				getLogname() const {return mLogName;}
	
	bool 					needRecreate() const
							{
								if (mErrid == AosErrmsgId::eNoError) return false;
								if (mCreationSec <= 0) return true;
								switch (mErrid)
								{
								case AosErrmsgId::eContainerNotFound:
			 						if (OmnGetSecond() - mCreationSec >= eContainerNotFoundSec) return true;
			 						return false;

								case AosErrmsgId::eAccessDenied:
			 						if (OmnGetSecond() - mCreationSec >= eAccessDeniedSec) return true;
			 						return false;

								case AosErrmsgId::eInternalError:
			 						if (OmnGetSecond() - mCreationSec >= eInternalErrorSec) return true;
			 						return false;

								default:
			 						if (OmnGetSecond() - mCreationSec >= eDefaultErrorSec) return true;
			 						return false;
								}
								return true;
							}
	
	
	bool 					removeOldestFile(const AosRundataPtr &rdata);
	
	OmnString				getIILName() const {return mIILName;};
	
	AosSeLogEntryPtr  		getLogEntry() const {return mLogEntry;};
	
	int 					getModuleId()const {return mModuleId;}
	bool					initHashSeLog(const AosRundataPtr &rdata);
	bool					hashLogIsInited()const {return mHashLogInitFlag;};
private:
	bool		 			filesSanityCheck(
							const u32 seqno, 
							const AosRundataPtr &rdata);

	bool 		 			createNewLogFile(
							const bool first, 
							const AosRundataPtr &rdata);
	
	bool 					addToIIL(
							const u64 &logid, 
							const AosRundataPtr &rdata);
	
	bool 					init(const AosRundataPtr &rdata);
	bool					initLogFile(const AosRundataPtr &rdata);
	
	bool					checkSiteId(const AosRundataPtr &rdata);
	

public:
	// Ketty 2013/03/20
	bool 	procRequest(
				const AosLogOpr::E opr,
				const AosXmlTagPtr &loginfo, 
				u64 &logid,
				const AosRundataPtr &rdata);

private:
	// Ketty 2013/03/20
	bool	addLog(
				const AosXmlTagPtr &loginfo, 
				u64 &logid,
				const AosRundataPtr &rdata);

};
#endif
