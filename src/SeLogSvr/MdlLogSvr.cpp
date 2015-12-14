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
// This class is the overall management of logs. It maintains two files. 
// One is the Log File Sequence Number Index File that maps Log File
// Seqno into file names. The other is the Log Map that maps each log
// to its current log file. 
//
// mSeLogMap: 
// This hash table maps a log [container, logname] to an instance of 
// AosSeLogPtr. When a log request is received, it will retrieve
// the corresponding instance of AosSeLog. If not found, it creates one.
//
// mSeqnoMap:
// This is a map that maps 'siteid + seqno' to OmnFilePtr. Any time when an SeLog
// needs a file, it asks this class to retrieve the file. This class keeps
// all the file in the map. If the file is not created yet, it will create it.
//
// 06/23/2011 Create by Tom 
////////////////////////////////////////////////////////////////////////////
#include "SeLogSvr/MdlLogSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/IILClientObj.h"
//#include "DocProc/DocProc.h"
#include "MultiLang/LangTermIds.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/SystemId.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SeLogSvr/SeqnoFnameEntry.h"
#include "SeLogSvr/SeLogEntry.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/CappedHash.h"
#include "ValueSel/ValueRslt.h"
#include "SEUtil/IILName.h"

const OmnString sgLogFilePrefix = "log_";

extern int gAosLogLevel;

AosMdlLogSvr::AosMdlLogSvr(const int &mIdx)
:
mLock(OmnNew OmnMutex(true)),
mMaxLogsPerContainer(eMaxLogsPerContainer),
mModuleId(mIdx)
{
	aos_assert(init());
}


AosMdlLogSvr::~AosMdlLogSvr()
{
}


bool
AosMdlLogSvr::init()
{
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(AOS_SYS_SITEID);

	// init hash file
	mLock->lock();
	/*
	//OmnString strkey = composeSelogHashFname(mModuleId);
	// Ketty 2012/07/30
	//OmnFilePtr ff = AosStorageMgr::getSelf()->openFileByKeyPublic(strkey, true, rdata);
	//OmnFilePtr ff = AosStorageMgr::getSelf()->openFileByKey(mModuleId, strkey, true, rdata AosMemoryCheckerArgs);
	


	
	if (!ff || !ff->isGood())
	{
		OmnAlarm << "Failed to open hash file" << enderr;
		mLock->unlock();
		return false;
	}
	//OmnString fname = ff->getFileName();
	mSeLogMap = OmnNew AosStrObjHash(OmnNew AosSeLog(), fname, eMaxMapSize,
						eKeyBitMap, eBucketSize, false, errid, errmsg);
	*/
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rl(vfsMgr, mLock, false);
	
	u64 log_map_fid = ((u64)mModuleId << 32) + AOSFILEID_SELOGMGR_SELOG_HASH;
	AosReliableFilePtr log_map_file = vfsMgr->openRlbFile(rdata.getPtr(),
			         mModuleId, log_map_fid, "log_map_hash", eLogMapFileSize, true,
					 AosRlbFileType::eNormal, true);
	aos_assert_rl(log_map_file, mLock, false);

	AosErrmsgId::E errid;
	OmnString errmsg;
	mSeLogMap = OmnNew AosStrObjHash(OmnNew AosSeLog(), log_map_file, eMaxMapSize,
						eKeyBitMap, eBucketSize, false, errid, errmsg, rdata);

	/*
	strkey = composeSeqnoMapFname(mModuleId);
	// Ketty 2012/07/30
	//ff = AosStorageMgr::getSelf()->openFileByKeyPublic(strkey, true, rdata);
	ff = AosStorageMgr::getSelf()->openFileByKey(mModuleId, strkey, true, rdata AosMemoryCheckerArgs);
	if (!ff || !ff->isGood())
	{
		OmnAlarm << "Failed to open hash file" << enderr;
		mLock->unlock();
		return false;
	}
	fname = ff->getFileName();
	mSeqnoMap = OmnNew AosStrObjHash(OmnNew AosSeqnoFnameEntry(), fname, eMaxMapSize,
						eKeyBitMap, eBucketSize, false, errid, errmsg);
	*/
	u64 seq_map_fid = ((u64)mModuleId << 32) + AOSFILEID_SELOGMGR_SEQNO_HASH;
	AosReliableFilePtr seq_map_file =  vfsMgr->openRlbFile(rdata.getPtr(),
			         mModuleId, seq_map_fid, "log_seq_hash", eLogMapFileSize, true,
					 AosRlbFileType::eNormal, true);
	aos_assert_rl(seq_map_file, mLock, false);
	mSeqnoMap = OmnNew AosStrObjHash(OmnNew AosSeqnoFnameEntry(), seq_map_file, eMaxMapSize,
						eKeyBitMap, eBucketSize, false, errid, errmsg, rdata);
	mLock->unlock();
	return true;
}


// Ketty 2013/03/11
bool
AosMdlLogSvr::stop()
{
	OmnNotImplementedYet;
	//AosStrObjHashPtr		mSeLogMap;
	//AosStrObjHashPtr		mSeqnoMap;
	return true;
}


// Modify by Ketty 2013/03/20	
bool
AosMdlLogSvr::addLog(
		//const AosXmlTagPtr &trans_data,
		const AosLogOpr::E opr,
		const OmnString &pctr_objid,
		const OmnString logname,
		const AosXmlTagPtr &loginfo, 
		u64 &logid,
		const AosRundataPtr &rdata)
{
	// This function is called when a request is received from the network. 
	// It routes the request to the corresponding SeLog. The request format is:
	// <log log_ctnr="xxx" oprator="get/add">
	// 	   <logcontents/>
	// </log>
	aos_assert_r(mSeLogMap, false);
	//OmnString pctr_objid = trans_data->getAttrStr(AOSTAG_LOG_PCTROBJID);
	//OmnString logname = trans_data->getAttrStr("logname", AOSTAG_DFT_LOGNAME);
	if (pctr_objid == "")
	{
		AosSetError(rdata, AosErrmsgId::eLogContainerEmpty);
		OmnAlarm << rdata->getErrmsg() << ". Siteid: " << rdata->getSiteid()
			<< ". Container: " << pctr_objid 
			<< ". Logname: " << logname << enderr;
		return false;
	}
	if (logname == "")
	{
		AosSetError(rdata, AosErrmsgId::eLogNameIsNull);
		OmnAlarm << rdata->getErrmsg() << ". Siteid: " << rdata->getSiteid()
			<< ". Container: " << pctr_objid 
			<< ". Logname: " << logname << enderr;
		return false;
	}

	OmnString key = composeLogKey(rdata->getSiteid(), pctr_objid, logname);
	if (key == "")
	{
		AosSetError(rdata, AosErrmsgId::eLogContainerEmpty);
		OmnAlarm << rdata->getErrmsg() << ". Siteid: " << rdata->getSiteid()
			<< ". Container: " << pctr_objid 
			<< ". Logname: " << logname << enderr;
		return false;
	}
	
	if (gAosLogLevel >= 100) 
	{

		OmnScreen << "----------------------------------" << endl;
		OmnScreen << "proc key: " << key << endl;
		OmnScreen << "----------------------------------" << endl;
	}
	// If a log has already been loaded into memory, it is stored in 
	// the map 'mSeLogMap'. It checks whether the log is already loaded
	// in memory. If not, it loads the log into memory and add it to 
	// the map. 
	mLock->lock();
	AosHashedObjPtr hashed_obj = mSeLogMap->find(key, rdata);
	AosSeLogPtr selog;
	if (!hashed_obj)
	{
		// Did not find it. Need to create one.
		if (gAosLogLevel >= 100) OmnScreen << "To create a log: " 
			<< pctr_objid << ":" << logname << endl;
		selog = createLog(pctr_objid, logname, mModuleId, rdata);
		if (!selog)
		{
			// Failed creating the log.
			mLock->unlock();
			return false;
		}
	}
	else
	{
		if (gAosLogLevel >= 100) OmnScreen << "Retrieved log: " 
			<< pctr_objid << ":" << logname << endl;
		selog = (AosSeLog *)hashed_obj.getPtr();
		if (!selog->hashLogIsInited()) selog->initHashSeLog(rdata);
	}

	if (!selog->isValid())
	{
		// To prevent network attacking, if it failed creating a log, it will
		// create a 'faked' log and add it to the hash so that the next time 
		// when one wants to create the same log, the 'log' appears being
		// already created. 
		//
		// It is not a valid log. Need to check whether it should re-try to create.
		if (selog->needRecreate())
		{
			mSeLogMap->erase(key, rdata);
			selog = createLog(pctr_objid, logname, mModuleId, rdata);
			if (!selog)
			{
				// Failed creating the log.
				mLock->unlock();
				return false;
			}
	
			if (!selog->isValid())
			{
				mLock->unlock();
				return false;
			}
		}
		else
		{
			mLock->unlock();
			return false;
		}
	}
	mLock->unlock();

	if (!selog->isValid()) 
	{
		AosSetError(rdata, AosErrmsgId::eLogNotExist);
		OmnAlarm << rdata->getErrmsg() << ". Log: " << pctr_objid
			<< ". Logname: " << logname << enderr;

		OmnString error;
		error << "<error>" << rdata->getErrmsg() << "</error>";
		rdata->setResults(error);
		return false;
	}
	
	// Found the log. Ask the log to process the request.
	//bool rslt = selog->procRequest(trans_data, rdata);
	bool rslt = selog->procRequest(opr, loginfo, logid, rdata);
	//OmnScreen << "add version, logid:" << logid
	//		<< "; version:" << loginfo->getAttrStr(AOSTAG_VERSION)
	//		<< "; log:" << loginfo->toString()
	//		<< endl;
	
	showInformation();
	return rslt;
}


void
AosMdlLogSvr::showInformation()
{
////////////////////////Just For Test//////////////////////////////////////////////////////
	if (gAosLogLevel >= 100)	
	{
		Map_t map1 = mSeqnoMap->getMap();
		Map_t map = mSeLogMap->getMap();

		OmnScreen << "-----------------------------------" << endl;
		OmnScreen << "The Number of log: " << map.size() << endl;
		OmnScreen << "-----------------------------------" << endl;

		OmnScreen << "-----------------------------------" << endl;
		OmnScreen << "The Number of log: " << map.size() << endl;
		OmnScreen << "-----------------------------------" << endl;
	}
/*
		MapItr_t itr;
		Map_t map = mSeLogMap->getMap();
		if (!map.empty())
		{
			for (itr=map.begin(); itr!=map.end(); itr++)
			{
				OmnScreen << "log key: " << itr->first << endl;
			}
		}
	
		Map_t map1 = mSeqnoMap->getMap();
		if (!map1.empty())
		{
			for (itr=map1.begin(); itr!=map1.end(); itr++)
			{
				OmnScreen << "seqno key: " << itr->first 
					<< ". FileName: " << ((AosSeqnoFnameEntry *)itr->second.getPtr())->getFname() << endl;
			}
		}
		OmnScreen << "-----------------------------------" << endl;
		*/
	////////////////////////////////////////////////////////////////////////////////////
}


AosReliableFilePtr
//OmnFilePtr
AosMdlLogSvr::getLogFile(
		const u32 seqno,
		const u32 moduleId,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	// Given (seqno, moduleid, siteid), it should map to a file. This function
	// retrieves the file. If the file is not there yet, it will create it.
	//OmnFilePtr ff;
	AosReliableFilePtr ff;
	mLock->lock();
	OmnString key = composeSeqnoKey(rdata->getSiteid(), seqno);
	if (key == "")
	{
		mLock->unlock();
		AosSetError(rdata, AosErrmsgId::eMissingSiteid);
		OmnAlarm << rdata->getErrmsg() << ". Seqno: " << seqno << enderr;
		return 0;
	}
	AosHashedObjPtr entry = mSeqnoMap->find(key, rdata);
	if (!entry)
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreatingFile);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Siteid: " << rdata->getSiteid()
			<< ". Seqno: " << seqno 
			<< ". key: " << key << enderr;
		mLock->unlock();
		return 0;
	}

	AosSeqnoFnameEntryPtr fname_entry = (AosSeqnoFnameEntry *)entry.getPtr();
	u64 file_id = fname_entry->getFileId();
	aos_assert_rl(file_id>0, mLock, 0);
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_rl(vfsMgr, mLock, 0);

	//ff = AosStorageMgr::getSelf()->openFile(file_id, rdata AosMemoryCheckerFileLine);
	ff = vfsMgr->openRlbFile(file_id, AosRlbFileType::eNormal, rdata.getPtr());
	mLock->unlock();
	aos_assert_rr(ff && ff->isGood(), rdata, 0);
	return ff;
}


//OmnFilePtr
AosReliableFilePtr
AosMdlLogSvr::createNewLogFile(
		u32 &seqno,
		const OmnString &pctr_objid,
		const OmnString &log_name,
		const u64 &moduleId,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	// This function returns the next seqno and the new file name.
	mLock->lock();
	OmnString key = composeSiteLastSeqnoKey(rdata->getSiteid());
	aos_assert_rl(key != "", mLock, 0);
	AosHashedObjPtr entry = mSeqnoMap->find(key, rdata);
	//OmnFilePtr ff;
	AosReliableFilePtr ff;
	u64 file_id = 0;
	if (!entry)
	{
		// This means the site has never created any log yet. 
		// We will create an entry for it.
		seqno = eInitialSeqno;
		seqno++;

		ff = createLogFile(seqno, moduleId, file_id, rdata AosMemoryCheckerFileLine);
		if (!ff)
		{
			mLock->unlock();
			AosSetError(rdata, AOSLT_FAILED_CREATE_FILE) << ". "
				<< seqno << ":" << moduleId;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
		entry = OmnNew AosSeqnoFnameEntry(key, seqno, file_id);
		aos_assert_rl(mSeqnoMap->addDataPublic(key, entry, true, rdata), mLock, 0);
		mLock->unlock();
	}
	else
	{
		// The entry already exists. 
		AosSeqnoFnameEntryPtr seqno_entry = (AosSeqnoFnameEntry *)entry.getPtr();
		seqno = seqno_entry->getNextSeqno();
		aos_assert_rl(mSeqnoMap->modifyDataPublic(key, seqno_entry, rdata), mLock, 0);
		ff = createLogFile(seqno, moduleId, file_id, rdata AosMemoryCheckerFileLine);
		if (!ff)
		{
			mLock->unlock();
			AosSetError(rdata, AOSLT_FAILED_CREATE_FILE) << ". "
				<< seqno << ":" << moduleId;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return 0;
		}
	}
	mLock->unlock();

	// Add the entry [key, seqno] to the IIL
	OmnString iilname = AosIILName::composeLogMgrSeqnoIILName(moduleId);
	key = composeLogKey(rdata->getSiteid(), pctr_objid, log_name);
	if (key == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingSiteid);
		OmnAlarm << rdata->getErrmsg() << ". Container: " << pctr_objid 
			<< ". Logname: " << log_name << enderr;
		return 0;
	}

//OmnScreen << "key: " << key << ", seqno: " << seqno << endl;
	bool rslt = AosIILClientObj::getIILClient()->addStrValueDocToTable(iilname, 
								key, seqno, false, false, rdata);
	aos_assert_rr(rslt, rdata, 0);
	return ff;
}


bool
AosMdlLogSvr::removeLogFilePriv(const u32 seqno, const AosRundataPtr &rdata)
{
	// This function removes the log file identified by 'seqno'. 
	// !!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// IMPORTANT: There is an IIL to keep track of [key, seqno]. This IIL
	// is important for us to retrieve which Seqnos being used by a given 
	// log. The caller should have removed the entry. 
	// !!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//
	// Log files are managed through the hash map 'mSeqnoMap', whose key
	// is composed of 'siteid' + "_" + 'seqno'. 
	OmnString key = composeSeqnoKey(rdata->getSiteid(), seqno);
	if (key == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingSiteid);
		OmnAlarm << rdata->getErrmsg() << ". Seqno: " << seqno
			<< ". Siteid: " << rdata->getSiteid() << enderr;
		return false;
	}

	AosHashedObjPtr entry = mSeqnoMap->find(key, rdata);
	if (!entry)
	{
		AosSetError(rdata, AosErrmsgId::eFailedRetrieveLogFile);
		OmnAlarm << rdata->getErrmsg() << ". Siteid: " << rdata->getSiteid()
			<< ". Seqno: " << seqno << enderr;
		return false;
	}

	AosSeqnoFnameEntryPtr seqno_entry = (AosSeqnoFnameEntry*)entry.getPtr();
	u64 file_id = seqno_entry->getFileId();
	if (file_id == 0)
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << ". Siteid: " << rdata->getSiteid()
			<< ". Seqno: " << seqno << ". File ID: " << file_id <<  enderr;
		return false;
	}

	// Now we need to remove all logid from crtime iil
	aos_assert_r(removeFromIIL(file_id, rdata), false);

	seqno_entry->setDeleteFlag();
	bool rslt = mSeqnoMap->modifyDataPublic(key, entry, rdata);
	mSeqnoMap->erase(key, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedModifyHashFile);
		OmnAlarm << rdata->getErrmsg() << ". Siteid: " << rdata->getSiteid()
			<< ". Seqno: " << seqno << enderr;
		return false;
	}

	// Ketty 2013/01/21
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	//rslt = AosStorageMgr::getSelf()->removeFile(file_id, rdata);
	rslt = vfsMgr->removeFile(file_id, rdata.getPtr());
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFileDeleted);
		OmnAlarm << rdata->getErrmsg() 
			<< ". File Seqno: " << file_id << enderr;
		return false;
	}
	return true;
}


AosSeLogPtr
AosMdlLogSvr::createLog(
		const OmnString &pctnr_objid, 
		const OmnString &logname,
		const int &moduleId,
		const AosRundataPtr &rdata)
{
	// This function creates a new log. It retrieves the container. Based on 
	// the container, it determines whether it is allowed to create the 
	// log. If yes, it creates the log. Otherwise, it returns the error
	// messages. 
	//
	// 	<container ...>
	// 		<AOSTAG_LOG_DEFINITION>
	// 			<AOSTAG_DFT_LOGNAME>
	OmnString key = composeLogKey(rdata->getSiteid(), pctnr_objid, logname);
	AosXmlTagPtr ctnr_doc = AosDocClientObj::getDocClient()->getDocByObjid(pctnr_objid, rdata);
	if (!ctnr_doc)
	{
		if (pctnr_objid == AOSOBJIDPRE_ROOTCTNR || pctnr_objid == AOSSTYPE_SYSACCT)
		{
			// This is root_ctnr
			try
			{
				AosSeLogPtr selog = OmnNew AosSeLog(key, 
						pctnr_objid, logname, moduleId, rdata);
				AosHashedObjPtr data = (AosHashedObj*)selog.getPtr();
				bool rslt =	mSeLogMap->addDataPublic(key, data, true, rdata);
				aos_assert_r(rslt, 0);
				return selog;
			}

			catch(...)
			{
				AosSetError(rdata, AosErrmsgId::eExceptionCreateSeLog);
				OmnAlarm << rdata->getErrmsg() 
					<< ". Container: " << pctnr_objid 
					<< ". Logname: " << logname << enderr;
				return 0;	
			}
		}
		AosSetError(rdata, AosErrmsgId::eContainerNotFound);
		OmnAlarm << rdata->getErrmsg() << ". Container: " << pctnr_objid
			<< ". Logname: " << logname << enderr;

		// Create a 'faked' SeLog to prevent network attacking
		AosSeLogPtr selog = OmnNew AosSeLog(key, AosErrmsgId::eContainerNotFound);
		AosHashedObjPtr data = selog.getPtr();
		mSeLogMap->addDataPublic(key, data, true, rdata);
		return 0;
	}

	if (!AosSecurityMgrObj::getSecurityMgr()->checkCreateLog(ctnr_doc, logname, rdata))
	{
		// Access denied.
		AosSeLogPtr selog = OmnNew AosSeLog(key, AosErrmsgId::eAccessDenied);
		AosHashedObjPtr data = (AosHashedObj*)selog.getPtr();
		mSeLogMap->addDataPublic(key, data, true, rdata);
		return 0;
	}

	try
	{
		AosSeLogPtr selog = OmnNew AosSeLog(key, pctnr_objid, 
								logname, ctnr_doc, moduleId, rdata);
		AosHashedObjPtr data = (AosHashedObj*)selog.getPtr();
		bool rslt =	mSeLogMap->addDataPublic(key, data, true, rdata);
		aos_assert_r(rslt, 0);
		return selog;
	}

	catch (...)
	{
		AosSetError(rdata, AosErrmsgId::eExceptionCreateSeLog);
		OmnAlarm << rdata->getErrmsg() 
			<< ". Container: " << pctnr_objid 
			<< ". Logname: " << logname << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	AosSetError(rdata, AosErrmsgId::eRunOutOfMemory);
	OmnAlarm << rdata->getErrmsg() << enderr;
	return 0;
}


bool
AosMdlLogSvr::removeOldestFile(
		const OmnString &pctr_objid,
		const OmnString &log_name, 
		const u64 &moduleId,
		const AosRundataPtr &rdata)
{

	mLock->lock();
	OmnString iilname = AosIILName::composeLogMgrSeqnoIILName(moduleId);
	OmnString key = composeLogKey(rdata->getSiteid(), pctr_objid, log_name);
	u64 seqno = 0;
	//bool rslt = AosIILClient::getSelf()->removeStrFirstValueDoc(iilname, key, seqno, false, rdata);
	bool rslt = AosIILClientObj::getIILClient()->removeStrFirstValueDoc(iilname, key, seqno, false, rdata);
	if (!rslt || seqno == 0)
	{
		AosSetError(rdata, AosErrmsgId::eFailedRetrieveSeqIIL);
		OmnAlarm << rdata->getErrmsg() << ". Container: " << pctr_objid
			<< ". Logname: " << log_name << ". Siteid: " << rdata->getSiteid() << enderr;
		mLock->unlock();
		return false;
	}

	// if the file is a version file, we should not remove the file.
	// Ketty 2012/12/05
	//OmnFilePtr file = getLogFile(seqno, moduleId, rdata AosMemoryCheckerArgs);
	AosReliableFilePtr file = getLogFile(seqno, moduleId, rdata AosMemoryCheckerArgs);
	aos_assert_rl(file, mLock, false);
	//OmnString logname = OmnString(file->readStr(201, 100, "", rdata).data());
	OmnString logname = file->readStr(201, 100, "", rdata.getPtr());
	if (logname == AOSLOGNAME_VERSION)
	{
		mLock->unlock();
		return true;
	}
	rslt = removeLogFilePriv(seqno, rdata);
	if (!rslt)
	{
		AosSetError(rdata, AosErrmsgId::eFailedDeleteOldestFile);
		OmnAlarm << rdata->getErrmsg() << ". Container: " << pctr_objid
			<< ". Logname: " << log_name << ". Siteid: " << rdata->getSiteid() << enderr;
		mLock->unlock();
		return false;
	}
	
	mLock->unlock();
	return true;
}


bool
AosMdlLogSvr::saveSeLog(
		const AosSeLogPtr &selog, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(rdata->getSiteid() != 0, rdata, false);
	aos_assert_rr(selog, rdata, false);
	OmnString pctr_objid = selog->getPctrObjid();
	OmnString logname = selog->getLogname();
	aos_assert_r(pctr_objid != "", false);
	aos_assert_r(logname != "", false);

	OmnString key = composeLogKey(rdata->getSiteid(), pctr_objid, logname);
	aos_assert_rr(key != "", rdata, false);
	AosHashedObjPtr data = (AosHashedObj*)selog.getPtr();
	bool rslt =	mSeLogMap->modifyDataPublic(key, data, rdata);
	return rslt;
}


AosXmlTagPtr
AosMdlLogSvr::retrieveLog(
		const u64 &logid, 
		const AosRundataPtr &rdata)

{
	AosXmlTagPtr log = AosSeLogEntry::readFromFile(logid, rdata);
	if (!log)
	{
		AosSetError(rdata, AosErrmsgId::eFailedRetrieveLog);
		OmnAlarm << rdata->getErrmsg() << ". Logid: " << logid << enderr;
		return 0;
	}
	return log;
}


bool
AosMdlLogSvr::removeFromIIL(
		const u64 &fileSeqno,
		const AosRundataPtr &rdata)
{
	// When removing a log file, we need to remove the IIL entries
	// contained by this file. Log entries in a log file is arranged
	// in the following format:
	// header:
	//     siteid, container,logname
	// body:
	// 		Entry Length
	// 		...
	// 		Entry Length
	// 		EntryLength
	// 		...
	//
	// The IIL that stores the logids of all the log entries is 
	//OmnFilePtr file = AosStorageMgr::getSelf()->openFile(fileSeqno, rdata AosMemoryCheckerArgs);
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	
	AosReliableFilePtr file = vfsMgr->openRlbFile(fileSeqno, AosRlbFileType::eNormal, rdata.getPtr());
	aos_assert_r(file && file->isGood(), rdata);

	u64 crtTime;
	u64 logid;
	bool rslt;
	AosXmlParser parser;
	u64 startPos = AosSeLog::eBodyStart;
	
	showInformation();
	
	// 1. Get the siteid 
	//OmnString siteid_str = OmnString(file->readStr(0, 100, "").data());
	OmnString siteid_str = file->readStr(0, 100, "", rdata.getPtr()).data();
	aos_assert_rr(siteid_str != "", rdata, false);
	aos_assert_rr(siteid_str.isDigitStr(), rdata, false);
			
	u32 siteid = atol(siteid_str.data());

	// 2. Get the container 
	//OmnString container = OmnString(file->readStr(101, 200, "").data());
	OmnString container = file->readStr(101, 100, "", rdata.getPtr()).data();
	aos_assert_r(container != "", false);
	
	// 3. Get the logname 
	//OmnString logname = OmnString(file->readStr(201, 300, "").data());
	OmnString logname = file->readStr(201, 100, "", rdata.getPtr()).data();
	aos_assert_r(logname != "", false);

	OmnString key = composeLogKey(siteid, container, logname);
	aos_assert_r(key != "", false);

	// Get time iilname
	OmnString iilname = AosIILName::composeLogListingIILName(siteid, container, logname);
	aos_assert_r(iilname != "", false);

	AosHashedObjPtr hashed_obj = mSeLogMap->find(key, rdata);
	aos_assert_r(hashed_obj, false);
	AosSeLogPtr selog = (AosSeLog *)hashed_obj.getPtr();
	aos_assert_r(selog, false);

	while(startPos < file->getLength())
	{
		u32 length;
		AosXmlTagPtr loginfo = AosSeLogEntry::readFromFile(file, startPos, length, rdata);
		if (length == 0)
		{
			// The end of the file reached
			break;
		}
		aos_assert_r(loginfo, false);
		AosXmlTagPtr child = loginfo->getFirstChild();
		aos_assert_r(child, false);
		AosXmlTagPtr log = child->getFirstChild();
		aos_assert_r(log, false);

		crtTime = log->getAttrU64(AOSTAG_CT_EPOCH, 0);
		aos_assert_r(crtTime > 0, false);
		logid = log->getAttrU64(AOSTAG_LOGID, 0);
		aos_assert_r(logid > 0, false);
		
		// Remove IIL: crtTime iil
//OmnScreen << "=======================: " << endl;
		//rslt = AosIILClient::getSelf()->removeU64ValueDoc(iilname, crtTime, logid, rdata);
		rslt = AosIILClientObj::getIILClient()->removeU64ValueDoc(iilname, crtTime, logid, rdata);
		if (!rslt)
		{
			AosSetError(rdata, AosErrmsgId::eFailedDeleteFromIIL);
			OmnAlarm << rdata->getErrmsg() << ". IILId: " << iilname 
				<< ". Log: " << log->toString() << enderr;
			return false;
		}
		// Add Request to DocProc to remove Indexed iil
		rslt = AosDocClientObj::getDocClient()->addDeleteLogRequest(
			log, rdata->getUserid(), rdata);
		aos_assert_rr(rslt, rdata, false);
		startPos += length;
	}
	return true;
}


//OmnFilePtr 
AosReliableFilePtr
AosMdlLogSvr::createLogFile(
		const u32 seqno,
		const u32 moduleId,
		u64 &file_id,
		const AosRundataPtr &rdata AosMemoryCheckDecl)
{
	OmnString key = composeSeqnoKey(rdata->getSiteid(), seqno);
	if (key == "")
	{
		AosSetError(rdata, AosErrmsgId::eMissingSiteid);
		OmnAlarm << rdata->getErrmsg() << ". Seqno: " << seqno << enderr;
		return 0;
	}
	OmnString prefix = composeFilePrefixName(rdata->getSiteid(), seqno, moduleId);
	aos_assert_r(prefix != "", 0);
	// Ketty 2012/07/30
	//OmnFilePtr ff  = AosStorageMgr::getSelf()->createNewFilePublic(
	//		file_id, prefix, 0, false, rdata);
	//OmnFilePtr ff  = AosStorageMgr::getSelf()->createNewFile(rdata,
	//		mModuleId, file_id, prefix, 0, false);
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, 0);
	
	AosReliableFilePtr ff  = vfsMgr->createRlbFile(rdata.getPtr(),
			mModuleId, prefix, 0, AosRlbFileType::eNormal, false);
	if (!ff || !ff->isGood())
	{
		AosSetError(rdata, AosErrmsgId::eFailedCreatingFile);
		OmnAlarm << rdata->getErrmsg() 
			<< ". prefix: " << prefix << enderr;
		return 0;
	}
	file_id = ff->getFileId();
	AosHashedObjPtr data = OmnNew AosSeqnoFnameEntry(key, seqno, file_id);
	if (gAosLogLevel >= 100)
	{
		OmnScreen << "Add seqnomap: " << key
			<< ". File ID: " << file_id << endl;
	}
	aos_assert_r(mSeqnoMap->addDataPublic(key, data, true, rdata), 0);
	return ff;
}

