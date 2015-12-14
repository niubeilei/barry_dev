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
// Each SobjTransMgr is defined by a doc. The doc otype must be
// AOSOTYPE_TRANSFILEMGR. When such a doc is created, it should create
// an instance of this class. The class will create a virtual directory
// for the doc. When the doc is deleted from the system, all the files
// and virtual directory for this doc are removed from the system.
//
// Modification History:
// 02/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/TransMgr/SobjTransMgr.h"

#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/StorageMgrObj.h"
#include "SEInterfaces/IILClientObj.h"
#include "SEInterfaces/ValueMapObj.h"
#include "SEInterfaces/ConditionObj.h"
#include "SEInterfaces/SqlClientObj.h"
#include "SEUtil/DbVarNames.h"
#include "SqlUtil/TableNames.h"
#include "SmartObj/TransMgr/SobjTransFile.h"



SobjTransMgr::SobjTransMgr(const OmnString &objid, const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex()),
mMaxTrans(eDftMaxTrans),
mMaxFilesize(eDftMaxFilesize),
mTransModel(eMarkResponseOnly),
mTransidAname(AOSTAG_TRANSID),
mFlushFlag(true),
mVirtualDir(0),
mFilePrefix("transmgr"),
mTransidMapIILID(0),
mCrtSeqno(1),
mTransMgrName(objid)
{
	if (objid == "")
	{
		OmnThrowException("Missing transmgr objid");
		return;
	}

	if (!init(rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


SobjTransMgr::SobjTransMgr(
		const AosXmlTagPtr &definingDoc, 
		const AosRundataPtr &rdata)
:
mDefiningDoc(definingDoc),
mLock(OmnNew OmnMutex()),
mMaxTrans(eDftMaxTrans),
mMaxFilesize(eDftMaxFilesize),
mTransModel(eMarkResponseOnly),
mTransidAname(AOSTAG_TRANSID),
mFlushFlag(true),
mVirtualDir(0),
mFilePrefix("transmgr"),
mTransidMapIILID(0),
mCrtSeqno(1)
{
	if (definingDoc)
	{
		if (!initWithDefiningDoc(rdata))
		{
			OmnString errmsg = "Failed creating TransMgr: ";
			errmsg << rdata->getErrmsg();
			OmnThrowException(errmsg);
			return;
		}
	}
	else
	{
		OmnAlarm << "Defining doc is null. Will use the default" << enderr;
		if (!init(rdata))
		{
			OmnThrowException(rdata->getErrmsg());
		}
	}
}


SobjTransMgr::~SobjTransMgr()
{
}


bool
SobjTransMgr::init(const AosRundataPtr &rdata)
{
	// This means the transaction manager is uniquely identified by
	// the objid 'mTransMgrObjid'.
	aos_assert_rr(mTransMgrName, rdata, false);
	OmnString name = AosDbVarNames::composeTransMgrVirtualDirVarname(mTransMgrName);
	bool rslt = AosDbGetValue(AOSDBCONTAINER_TRANSMGR, name, mVirtualDir, 0, rdata);
	if (mVirtualDir == 0)
	{
		// The virtual dir has not been created yet. 
		mVirtualDir = AosCreateVirtualDir(mTransMgrName, rdata);
		aos_assert_rr(mVirtualDir != 0, rdata, false);
		rslt = AosDbSetValue(AOSDBCONTAINER_TRANSMGR, name, mVirtualDir, rdata);
		aos_assert_rr(rslt, rdata, false);
	}

	mTransidMapIILID = mDefiningDoc->getAttrU64(AOSTAG_MAP_IILID, 0);

	name = AosDbVarNames::composeTransMgrSeqnoVarname(mVirtualDir);
	u64 seqno = 0;
	rslt = AosDbGetValue(AOSDBCONTAINER_TRANSMGR, name, seqno, 0, rdata);
	mCrtSeqno = seqno;
	if (mCrtSeqno == 0)
	{
		// The current seqno is not set yet. 
		mCrtSeqno = 1;
		rslt = AosDbSetValue(AOSDBCONTAINER_TRANSMGR, name, (u64)mCrtSeqno, rdata);
		aos_assert_rr(rslt, rdata, false);
	}

	return true;
}


bool
SobjTransMgr::initWithDefiningDoc(const AosRundataPtr &rdata)
{
	aos_assert_rr(mDefiningDoc, rdata, false);
	if (mDefiningDoc->getAttrStr(AOSTAG_OTYPE) != AOSOTYPE_TRANSFILEMGR)
	{
		AosSetError(rdata, "not_transfilemgr_doc_001")
			<< ": " << mDefiningDoc->toString();
		return false;
	}

	bool modified = false;
	mTransMgrName = mDefiningDoc->getAttrStr(AOSTAG_NAME);
	if (mTransMgrName == "")
	{
		mTransMgrName = "TransMgr";
		mDefiningDoc->setAttr(AOSTAG_NAME, mTransMgrName);
		modified = true;
	}

	mVirtualDir = mDefiningDoc->getAttrU64(AOSTAG_VIRTUAL_DIR, 0);
	if (mVirtualDir == 0)
	{
		// The virtual dir has not been created yet. 
		mVirtualDir = AosCreateVirtualDir(mTransMgrName, rdata);
		aos_assert_rr(mVirtualDir != 0, rdata, false);
		mDefiningDoc->setAttr(AOSTAG_VIRTUAL_DIR, mVirtualDir);
		modified = true;
	}

	int num_events = mDefiningDoc->getAttrInt(AOSTAG_NUM_EVENTS, 0);
	if (num_events > eMaxEventLimit)
	{
		AosSetError(rdata, "too_many_events");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (num_events > 0)
	{
		AosXmlTagPtr event_tag = mDefiningDoc->getFirstChild(AOSTAG_EVENTS);
		if (!event_tag)
		{
			AosSetError(rdata, "missing_events_def_001");
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		AosXmlTagPtr tag = event_tag->getFirstChild();
		mBitmaps.resize(num_events);
		mRespPos.resize(num_events);
		for (int i=0; i<num_events; i++)
		{
			mBitmaps[i] = false;
			mRespPos[i] = false;
		}

		int idx = 0;
		while (tag)
		{
			if (tag->getAttrBool(AOSTAG_MARK, false))
			{
				mBitmaps[idx] = true;
			}

			if (tag->getAttrBool(AOSTAG_TRACK, false))
			{
				mRespPos[idx] = true;
			}
		
			idx++;
			tag = event_tag->getNextChild();
		}
	}

	mMaxTrans = mDefiningDoc->getAttrInt(AOSTAG_MAX_TRANS, -1);
	if (mMaxTrans < 0)
	{
		mMaxTrans = eDftMaxTrans;
	}

	mMaxFilesize = mDefiningDoc->getAttrInt(AOSTAG_MAX_FILESIZE, -1);
	if (mMaxFilesize < 0 || mMaxFilesize > eMaxFilesizeLimit)
	{
		mMaxFilesize = eDftMaxFilesize;
	}

	OmnString model = mDefiningDoc->getAttrStr(AOSTAG_TRANS_MODEL);
	if (model == AOSTAG_MARK_RESP_ONLY)
	{
		mTransModel = eMarkResponseOnly;
	}
	else if (model == AOSTAG_MARK_SAVE_RESP)
	{
		mTransModel = eMarkAndSaveResponse;
	}
	else if (model == AOSTAG_USE_EVENT_CONDS)
	{
		mTransModel = eUseEventConds;
	}
	else
	{
		mTransModel = eMarkResponseOnly;
	}

	mEventConds = mDefiningDoc->getFirstChild(AOSTAG_EVENT_CONDS);

	mTransidAname = mDefiningDoc->getAttrStr(AOSTAG_TRANSID_ANAME);
	if (mTransidAname == "")
	{
		AosSetError(rdata, "missing_transid_aname_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mFlushFlag = mDefiningDoc->getAttrBool(AOSTAG_FLUSH_FLAG, true);

	mFilePrefix = mDefiningDoc->getAttrStr(AOSTAG_FILE_PREFIX, "transmgr");
	if (mFilePrefix == "")
	{
		AosSetError(rdata, "file_prefix_empty_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	mTransidMapIILID = mDefiningDoc->getAttrU64(AOSTAG_MAP_IILID, 0);
	mCrtSeqno = mDefiningDoc->getAttrU32(AOSTAG_FILE_SEQNO, 0);
	if (mCrtSeqno == 0)
	{
		mCrtSeqno = 1;
		mDefiningDoc->setAttr(AOSTAG_FILE_SEQNO, mCrtSeqno);
		modified = true;
	}

	if (modified) AosModifyDoc(mDefiningDoc, rdata);
	
	return true;
}


bool 
SobjTransMgr::addTrans(
		u32 &transid, 
		const AosXmlTagPtr &trans, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	if (!mCrtTransFile) openCrtFileLocked(rdata);
	if (!mCrtTransFile)
	{
		AosSetError(rdata, "failed_open_trans_file_001");
		mLock->unlock();
		return false;
	}

	bool needCreateNewFile = false;
	bool rslt = mCrtTransFile->addTrans(transid, trans, needCreateNewFile, rdata);
	if (!rslt)
	{
		if (needCreateNewFile)
		{
			aos_assert_rl(transid, mLock, false);
			rslt = createNewFileLocked(transid, rdata);
			aos_assert_rl(rslt, mLock, false);
			aos_assert_rl(mCrtTransFile, mLock, false);
			rslt = mCrtTransFile->addTrans(transid, trans, needCreateNewFile, rdata);
			aos_assert_rl(rslt, mLock, false);
		}
		else
		{
			OmnAlarm << "Failed append transaction: " << rdata->getErrmsg() << enderr;
		}
	}
	mLock->unlock();
	return rslt;
}


bool
SobjTransMgr::procResponse(
		const AosXmlTagPtr &response, 
		const AosRundataPtr &rdata)
{
	// A response is received. It retrieves its local transid and then 
	// determine which transaction file to handle it.
	aos_assert_rr(response, rdata, false);
	aos_assert_rr(mTransidAname != "", rdata, false);
	u32 transid = response->getAttrU32(mTransidAname, 0);
	if (transid == 0)
	{
		logError(AOSTERM("invalid_transid_001", rdata), response);
		return false;
	}

	u32 seqno;
	bool rslt = transidToSeqno(seqno, transid, rdata);
	aos_assert_rr(rslt, rdata, false);
	SobjTransFilePtr file = getTransFile(seqno, rdata);
	aos_assert_rr(file, rdata, false);

	// A transaction may trigger different events. Retrieve the event id.
	bool bitmap_flag, position_flag;
	int event_id = getEventId(response, bitmap_flag, position_flag, rdata);
	if (event_id < 0)
	{
		logError(AOSTERM("invalid_event_id_001", rdata), response);
		return false;
	}

	SobjTransMgrPtr thisptr(this, false);
	return file->procResponse(transid, response, event_id, bitmap_flag, 
			position_flag, thisptr, rdata);
}


int 
SobjTransMgr::getEventId(
		const AosXmlTagPtr &response, 
		bool &mark, 
		bool &track, 
		const AosRundataPtr &rdata)
{
	// A response is received. This function determines the event based on
	// the contents of the response. 
	switch (mTransModel)
	{
	case eMarkResponseOnly:
		 // No need to record responses. Only need to mark responses.
		 mark = true;
		 track = false;
		 return eResponseEvent;

	case eMarkAndSaveResponse:
		 // Need to mark and save responses.
		 mark = true;
		 track = true;
		 return eResponseEvent;

	case eUseEventConds:
		 {
		 // Use the condition to determine the events.
		 aos_assert_rr(mEventConds, rdata, -1);
		 bool rslt = AosEvalCond(mEventConds, rdata);
		 aos_assert_rr(rslt, rdata, -1);
		 OmnString vv = rdata->getArg1(AOSARG_EVENT_ID);
		 if (vv == "") return -1;
		 int event_id = atoi(vv.data());
		 mark = (rdata->getArg1(AOSARG_BITMAP_FLAG) == "true");
		 track = (rdata->getArg1(AOSARG_POSITION_FLAG) == "true");
		 return event_id;
		 }

	default:
		 OmnAlarm << "Unrecognized event model: " << mTransModel << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


SobjTransFilePtr
SobjTransMgr::getTransFile(const u32 seqno, const AosRundataPtr &rdata)
{
	aos_assert_rr(mMaxTransFile > 0 && seqno < (u32)mMaxTransFile, rdata, 0);
	aos_assert_rr(mVirtualDir, rdata, 0);
	aos_assert_rr(mFilePrefix != "", rdata, 0);
	mLock->lock();
	if (seqno < mTransFiles.size() && mTransFiles[seqno])
	{
		SobjTransFilePtr ff = mTransFiles[seqno];
		mLock->unlock();
		return ff;
	}

	OmnString fname = AosGetFileName(mVirtualDir, seqno, mFilePrefix, rdata);
	if (fname == "")
	{
		AosSetError(rdata, "failed_create_file_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		mLock->unlock();
		return 0;
	}

	try
	{
		SobjTransFilePtr ff = OmnNew SobjTransFile(fname, 
				mTransidAname, mFlushFlag, mBitmaps, mRespPos, rdata);
		if (seqno >= mTransFiles.size()) mTransFiles.resize(seqno+1);
		mTransFiles[seqno] = ff;
		mLock->unlock();
		return ff;
	}

	catch (...)
	{
		OmnAlarm << "Failed creating transaction file" << enderr;
	}
		
	OmnShouldNeverComeHere;
	mLock->unlock();
	return 0;
}


bool
SobjTransMgr::transidToSeqno(
		u32 &seqno, 
		const u32 transid, 
		const AosRundataPtr &rdata)
{
	// This function converts the transid 'transid' to its file seqno.
	// The transid to seqno map is managed through an IIL. The IIL is
	// a persistent IIL, its IILID is stored in mDefiningDoc. 
	if (mTransidMapIILID == 0)
	{
		// No map yet. The seqno should be 0.
		seqno = 0;
		return true;
	}

	int64_t value;
	bool rslt = AosGetMappedValue(mTransidMapIILID, transid, value, rdata);
	if (!rslt) return false;
	if (value < 0) return false;
	seqno = (u64)value;
	return true;
}


bool
SobjTransMgr::updateSeqnoToDbLocked(const AosRundataPtr &rdata)
{
	if (mDefiningDoc)
	{
		mDefiningDoc->setAttr(AOSTAG_FILE_SEQNO, mCrtSeqno);
		bool rslt = AosModifyDoc(mDefiningDoc, rdata);
		aos_assert_rr(rslt, rdata, false);
		return true;
	}

	aos_assert_rr(mVirtualDir, rdata, false);
	OmnString name = AosDbVarNames::composeTransMgrSeqnoVarname(mVirtualDir);
	u64 seqno = mCrtSeqno;
	bool rslt = AosDbSetValue(AOSDBCONTAINER_TRANSMGR, name, seqno, rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}
	

bool
SobjTransMgr::createNewFileLocked(const u32 transid, const AosRundataPtr &rdata)
{
	// This function creates a new transaction file. It retrieves the current
	// seqno, increments it by one, saves the seqno, and then create the file.
	aos_assert_rr(mVirtualDir != 0, rdata, false);

	mCrtSeqno++;

	bool rslt = updateSeqnoToDbLocked(rdata);
	aos_assert_rr(rslt, rdata, false);

	OmnString fname = AosGetFileName(mVirtualDir, mCrtSeqno, mFilePrefix, rdata);
	if (fname == "")
	{
		AosSetError(rdata, "failed_create_file_001");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	try
	{
		mCrtTransFile = OmnNew SobjTransFile(fname, mCrtSeqno, transid, 
				mMaxTrans, mMaxFilesize, mTransidAname, mFlushFlag, 
				mBitmaps, mRespPos, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating transaction file" << enderr;
		return false;
	}
	
	rslt = addTransidSeqnoEntry(mCrtSeqno, transid, rdata);
	return true;
}


bool
SobjTransMgr::addTransidSeqnoEntry(
		const u32 seqno, 
		const u32 transid, 
		const AosRundataPtr &rdata)
{
	// Each transmgr maintains a map that maps transids to seqno. 
	// Each entry [transid, seqno] in the IIL means that the
	// 'seqno' start transid is 'transid'. This function updates
	// the IIL. If the IIL is not created yet, it creates the IIL.

	u64 iilid = mTransidMapIILID;
	bool rslt = AosDbSetU64ValueDocUniqueToTable(iilid, true, 
			transid, seqno, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	if (iilid != mTransidMapIILID)
	{
		mTransidMapIILID = iilid;
		if (mDefiningDoc)
		{
			mDefiningDoc->setAttr(AOSTAG_MAP_IILID, mTransidMapIILID);
			rslt = AosModifyDoc(mDefiningDoc, rdata);
			aos_assert_rr(rslt, rdata, false);
			return true;
		}

		OmnString name = AosDbVarNames::composeTransMgrIILIDVarname(mVirtualDir);
		rslt = AosDbSetValue(AOSDBCONTAINER_TRANSMGR, name, mTransidMapIILID, rdata);
		aos_assert_rr(rslt, rdata, false);
		return true;
	}
	return true;
}


bool
SobjTransMgr::openCrtFileLocked(const AosRundataPtr &rdata)
{
	// Each TransMgr is defined by a doc. The doc 
	// keeps all the information about this class. There are 
	// system defined transaction file managers. For these
	// managers, their docs are defined statically.
	aos_assert_rr(mDefiningDoc, rdata, false);
	aos_assert_rr(mVirtualDir, rdata, false);
	aos_assert_rr(mCrtSeqno, rdata, false);

	OmnString fname = AosGetFileName(mVirtualDir, mCrtSeqno, mFilePrefix, rdata);
	aos_assert_rr(fname != "", rdata, false);

	try
	{
		mCrtTransFile = OmnNew SobjTransFile(fname, 
				mTransidAname, mFlushFlag, 
				mBitmaps, mRespPos, rdata);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating transaction file" << enderr;
		return false;
	}
	
	return true;
}


bool 
AosSobjTransMgr::addTrans(  
		const AosTrans1Ptr &trans,
		const u32 host_id,
		const u32 conn_id,
		const AosXmlTagPtr &trans_xml,
		const u32 client_moduleid,
		const u32 client_id)
{
	// This function adds a transaction to the transaction file. 
	// A transaction has two transaction ids: Local Transid and
	// Global Transid. The global transid is constructed as:
	// 		Byte 8		client_moduleid	
	// 		Byte 7-5	client_id
	// 		Byte 4-1	local ID
	// It needs to set global transid to AOSTAG_TRANSID.
	// 'host_id' and 'conn_id' need to be saved for recovery.
	OmnNotImplementedYet;
	return false;
}


bool 
AosSobjTransMgr::getTransHeaders(               
		const u32 &start_local_id,
		AosTransHeader * headers,
		u32 &read_num)
{
	// This function reads in a number of transaction info (called headers).
	// Transaction Info contains the following information:
	// 		global transid
	// 		trans status
	// 		trans typej
	// 		host_id
	// 		conn_id
	// 		body seqno
	// 		body offset
	// 		body size
	// 'start_local_id' is the starting transid (local transid) to read.
	// It reads up to eTransNumPerRead number of trans or to the end of
	// the transaction file.
	OmnNotImplementedYet;
	return false;
}


bool 
AosSobjTransMgr::getTrans(const AosTransHeader &trans_header, OmnString &trans_str)
{
	// This function reads the transaction 'trans_header'. The results
	// are returned through 'trans_str'.
	OmnNotImplementedYet;
	return false;
}


bool 
AosSobjTransMgr::readDoc(const u32 seqno, const u64 &offset, char *data, const u32 docsize)
{
	// This function reads the transaction identified by 'seqno' and 'offset'. 
	OmnNotImplementedYet;
	return false;
}


bool 
AosSobjTransMgr::recover()
{
	// This function is called when the system comes up. It recovers
	// all the outstanding transactions.
	OmnNotImplementedYet;
	return false;
}


bool 
AosSobjTransMgr::clean(bool &finish)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosSobjTransMgr::removeOldFiles()
{
	OmnNotImplementedYet;
	return false;
}

