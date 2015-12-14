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
// 2015/09/15 Created by Gavin
////////////////////////////////////////////////////////////////////////////
#include "IILClientWrapper/IILClientWrapper.h"

#include "JimoAPI/JimoIndexEngine.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEUtil/IILName.h"
#include "SEInterfaces/BitmapMgrObj.h"

static map<OmnString, u64> sgMap;
static OmnMutexPtr sgLock = OmnNew OmnMutex();
static bool sgShowLog = false;

OmnSingletonImpl(AosIILClientWrapperSingleton,
				 AosIILClientWrapper,
				 AosIILClientWrapperSelf,
				 "IILClientWrapper");                       


AosIILClientWrapper::AosIILClientWrapper()
{
}


AosIILClientWrapper::~AosIILClientWrapper()
{
}

bool
AosIILClientWrapper::start()
{
	return true;
}


bool
AosIILClientWrapper::stop()
{
	return true;
}


bool		
AosIILClientWrapper::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);

	AosXmlTagPtr iilClient= config->getFirstChild("iil_client");
	aos_assert_r(iilClient, false);
	
	sgShowLog = iilClient->getAttrBool(AOSCONFIG_SHOWLOG, false);
	return true;
}


bool 
AosIILClientWrapper::incrementDocid(
		const OmnString &iilname,
		const OmnString &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoIncrementDocid(iilname, key, value, 
					persis, incValue, initValue, add_flag, rdata.getPtr());
}


bool
AosIILClientWrapper::incrementDocid(
		const OmnString &iilname,
		const u64 &key,
		u64 &value,
		const bool persis,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const u64 &dft_value,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoIncrementDocid(rdata.getPtr(), iilname, key, value,  
			        persis, incValue, initValue, add_flag, dft_value);
}

bool 
AosIILClientWrapper::incrementDocid(
		const u64 &iilid,
		const OmnString &key,
		u64 &value,
		const u64 &incValue,
		const u64 &initValue,
		const bool add_flag,
		const AosRundataPtr &rdata)
{
	return Jimo::jimoIncrementDocid(rdata.getPtr(), iilid, key, value, incValue,
							initValue, add_flag); 
}

bool 
AosIILClientWrapper::incrementDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &value,
						const bool persis,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const u64 &dftValue,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoIncrementDocid(rdata.getPtr(), iilid, key, value, persis,     
	                incValue, initValue, add_flag, dftValue);
}

bool	
AosIILClientWrapper::addU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoAddU64ValueDoc(rdata.getPtr(), allTrans, arr_len, iilname, value,
                    docid, value_unique, docid_unique, physical_id);   
}

bool
AosIILClientWrapper::addStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						int &physical_id,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoAddStrValueDoc(rdata.getPtr(), allTrans, arr_len, iilname, value,
                   docid, value_unique, docid_unique, physical_id);     
}

bool	
AosIILClientWrapper::removeU64ValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRemoveU64ValueDoc(rdata.getPtr(), allTrans, arr_len, iilname,
                    value, docid, physical_id);     
}

bool	
AosIILClientWrapper::removeStrValueDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRemoveStrValueDoc(rdata.getPtr(), allTrans, arr_len, iilname, 
	                value, docid, physical_id);                               
}

bool	
AosIILClientWrapper::addU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) 
{
	bool ispersis = false;
	return Jimo::jimoAddU64ValueDoc(rdata.getPtr(), iilid, value, docid, 
	                value_unique, docid_unique, ispersis);                           
}

//not exist
bool	
AosIILClientWrapper::addU64ValueDocToTable(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata)
{
	//Xuqi 2015/11/3
	return Jimo::jimoAddU64ValueDoc(rdata.getPtr(), iilid, value, docid,
				 value_unique, docid_unique, false);
}

bool	
AosIILClientWrapper::addStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoAddStrValueDoc(rdata.getPtr(), iilname, value, docid, 
	                value_unique, docid_unique);                           
}

bool 
AosIILClientWrapper::bitmapQueryNewSafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoBitmapQueryNewSafe(rdata.getPtr(), iilname, query_rslt, query_bitmap, 
	                query_context);                           
}

bool	
AosIILClientWrapper::bitmapRsltQuerySafe(
						const OmnString &iilname,
				 		const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoBitmapRsltQuerySafe(rdata.getPtr(), iilname, query_rslt, query_bitmap, 
	                query_context);                           
}

bool	 
AosIILClientWrapper::BatchAdd(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchAdd(iilname, iiltype, entry_len, buff, executor_id, snap_id,
			task_docid, rdata.getPtr());                           
}

bool 	
AosIILClientWrapper::BatchDel(
						const OmnString &iilname,
						const AosIILType &iiltype,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoBatchDel(iilname, iiltype, entry_len, buff, executor_id, snap_id,
			task_docid, rdata.getPtr());                           
}

bool 
AosIILClientWrapper::createTablePublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateTablePublic(rdata.getPtr(), iilid, iiltype);                           
}
	
bool 
AosIILClientWrapper::createIILPublic(
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateIILPublic(rdata.getPtr(), iilid, iiltype);                           
}

bool 
AosIILClientWrapper::createTablePublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateTablePublic(rdata.getPtr(), iilname, iilid, iiltype);                    
}

bool 
AosIILClientWrapper::createIILPublic(
						const OmnString &iilname,
						u64 &iilid,
						const AosIILType iiltype,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoCreateIILPublic(rdata.getPtr(), iilname, iilid, iiltype);                    
}

bool 
AosIILClientWrapper::querySafe(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoQuerySafe(rdata.getPtr(), iilid, query_rslt, query_bitmap, query_context); 
}

bool 
AosIILClientWrapper::setU64ValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoSetU64ValueDocUnique(rdata.getPtr(), iilid, createFlag, key, 
							docid, must_same); 
}

bool 
AosIILClientWrapper::setU64ValueDocUniqueToTable(
						u64 &iilid,
						const bool createFlag,
						const u64 &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoSetU64ValueDocUniqueToTable(rdata.getPtr(), iilid, createFlag, key, 
							docid, must_same); 
}

bool 
AosIILClientWrapper::setStrValueDocUniqueToTable(
	 					u64 &iilid,
	 					const bool createFlag,
	 					const OmnString &key,
	 					const u64 &docid,
	 					const bool must_same,
	 					const AosRundataPtr &rdata)
{
	//Xuqi 2015/11/2
	if (iilid == 0)                                                  
	{                                                                
		if (!createFlag) return true;                                
		bool rslt = createTablePublic(iilid, eAosIILType_Str, rdata);
		aos_assert_r(rslt, false);                                   
	}                                                                
	aos_assert_r(iilid, false);                                      
	
	return Jimo::jimoSetStrValueDocUniqueToTable(rdata.getPtr(), iilid, createFlag, key, 
							docid, must_same); 
}

bool 
AosIILClientWrapper::deleteIIL(
						const u64 &iilid,
						const bool true_delete,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoDeleteIIL(rdata.getPtr(), iilid, true_delete); 
}

u64	
AosIILClientWrapper::getDocidByObjid(
						const OmnString &objid,
						const AosRundataPtr &rdata)
{
	u32 siteid = rdata->getSiteid();
	OmnString s;
	s << siteid << "_" << objid;

	//sgLock->lock();
	//map<OmnString, u64>::iterator itr = sgMap.find(s);
	//if (itr != sgMap.end())
	//{
	//	u64 docid = itr->second;
	//	sgLock->unlock();
	//	return docid;
	//}
	//if (sgMap.size() > 100000)
	//{
	//	sgMap.clear();
	//}
	//sgLock->unlock();

	u64 docid = 0;
	bool isunique = false;
	bool rslt = getDocidByObjid(siteid, objid, docid, isunique, rdata);
	if (!rslt || !isunique) return 0;

	//sgLock->lock();
	//sgMap[s] = docid;
	//sgLock->unlock();

	return docid;
}

bool 
AosIILClientWrapper::getDocid(
						const OmnString &iilname,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocid(iilname, key, opr, reverse, docid, isunique, rdata);
}

bool 
AosIILClientWrapper::getDocid(
						const u64 &iilid,
						const u64 &key,
						u64 &docid,
						const u64 &dft,
						bool &found,
						bool &is_unique,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocid(rdata.getPtr(), iilid, key, docid, dft, found, is_unique);
}

bool 
AosIILClientWrapper::getDocid(
						const u64 &iilid,
						const OmnString &key,
						const AosOpr opr,
						const bool reverse,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocid(rdata.getPtr(), iilid, key, opr, reverse, docid, isunique);
}

bool 
AosIILClientWrapper::getDocidsByKeys(
						const u64 &iilid,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocidsByKeys(rdata.getPtr(), iilid, keys, need_dft_value,
							dft_value, buff);
}

bool 
AosIILClientWrapper::getDocidsByKeys(
						const OmnString &iilname,
						const vector<OmnString> &keys,
						const bool need_dft_value,
						const u64 &dft_value,
						AosBuffPtr &buff,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoGetDocidsByKeys(rdata.getPtr(), iilname, keys, need_dft_value,
							dft_value, buff);
}

bool 
AosIILClientWrapper::getSplitValue(
						const OmnString &iilname,
						const AosQueryContextObjPtr &context,
						const int size,
						vector<AosQueryContextObjPtr> &contexts,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoGetSplitValue(rdata.getPtr(), iilname, context, size, contexts);
}

bool 
AosIILClientWrapper::rebuildBitmap(
						const OmnString &iilname,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRebuildBitmap(rdata.getPtr(), iilname);
}

bool 
AosIILClientWrapper::querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoQuerySafe(rdata.getPtr(), iilname, query_rslt, query_bitmap, query_context);
}

bool 
AosIILClientWrapper::querySafe(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosAsyncRespCallerPtr &resp_caller,
						const u64 &reqId,
						const u64 &snapId,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoQuerySafe(rdata.getPtr(), iilname, query_rslt, query_bitmap, 
					query_context, resp_caller, reqId, snapId);
}

bool 
AosIILClientWrapper::removeU64ValueDoc(
						const u64 &iilid,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRemoveU64ValueDoc(rdata.getPtr(), iilid, value, docid);
}

bool 
AosIILClientWrapper::resetKeyedValue(
						const AosRundataPtr &rdata,
						const OmnString &iilname,
						const OmnString &key,
						u64 &value,
						const bool persis,
						const u64 &incValue) 
{
	return Jimo::jimoResetKeyedValue(rdata.getPtr(), iilname, key, value, persis, incValue);
}

bool 
AosIILClientWrapper::removeStrValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRemoveStrValueDoc(iilname, value, docid, rdata.getPtr());
}

bool 
AosIILClientWrapper::setStrValueDocUnique(
						u64 &iilid,
						const bool createFlag,
						const OmnString &key,
						const u64 &docid,
						const bool must_same,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoSetStrValueDocUnique(rdata.getPtr(), iilid, createFlag, key, docid, must_same);
}

bool 
AosIILClientWrapper::StrBatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoStrBatchAdd(rdata.getPtr(), iilname, entry_len, buff, executor_id,
							snap_id, task_docid);
}

bool 
AosIILClientWrapper::StrBatchDel(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoStrBatchDel(rdata.getPtr(), iilname, entry_len, buff, executor_id,
							snap_id, task_docid);
}

bool 
AosIILClientWrapper::StrBatchInc(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &init_value,
						const AosIILUtil::AosIILIncType incType,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoStrBatchInc(rdata.getPtr(), iilname, entry_len, buff, init_value,
							incType, snap_id, task_docid);
}

bool 
AosIILClientWrapper::U64BatchAdd(
						const OmnString &iilname,
						const int entry_len,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoU64BatchAdd(rdata.getPtr(), iilname, entry_len, buff, executor_id,
							snap_id, task_docid);
}

//Actions
bool 
AosIILClientWrapper::HitBatchAdd(
						const OmnString &iilname,
						const vector<u64> &docids,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoHitBatchAdd(rdata.getPtr(), iilname, docids, snap_id, task_docid);
}
	
u64	
AosIILClientWrapper::createSnapshot(
						const u32 virtual_id,
						const u64 &task_docid,
						const u64 &snap_id,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoCreateSnapshot(rdata.getPtr(), virtual_id, snap_id, task_docid);
}

bool 
AosIILClientWrapper::removeHitDoc(
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoRemoveHitDoc(rdata.getPtr(), iilname, docid);
}

bool 
AosIILClientWrapper::StrBatchAddMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &executor_id,
						const bool true_delete,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoStrBatchAddMerge(rdata.getPtr(), iilname, size, executor_id, true_delete);
}

bool 
AosIILClientWrapper::StrBatchIncMerge(
						const OmnString &iilname,
						const int64_t &size,
						const u64 &dftvalue,
						const AosIILUtil::AosIILIncType incType,
						const bool true_delete,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoStrBatchIncMerge(rdata.getPtr(), iilname, size, dftvalue, incType, true_delete);
}
	
bool 
AosIILClientWrapper::JimoTableBatchAdd(
						const OmnString &iilname,
						const AosXmlTagPtr &cmp_tag,
						const AosBuffPtr &buff,
						const u64 &executor_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoJimoTableBatchAdd(rdata.getPtr(), iilname, cmp_tag, buff, executor_id,
						snap_id, task_docid);
}

u64	
AosIILClientWrapper::getIILID(
						const OmnString &iilname,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoGetIILID(rdata.getPtr(), iilname);
}

bool 
AosIILClientWrapper::counterRange(
						const u64 &iilid,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoCounterRange(rdata.getPtr(), iilid, query_rslt, query_bitmap, 
					query_context, counter_query);
}

bool 
AosIILClientWrapper::counterRange(
						const OmnString &iilname,
						const AosQueryRsltObjPtr &query_rslt,
						const AosBitmapObjPtr &query_bitmap,
						const AosQueryContextObjPtr &query_context,
						const AosCounterQueryPtr &counter_query,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoCounterRange(rdata.getPtr(), iilname, query_rslt, query_bitmap, 
					query_context, counter_query);
}

bool 
AosIILClientWrapper::commitSnapshot(
						const u32 &virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoCommitSnapshot(rdata.getPtr(), virtual_id, snap_id, task_docid); 
}

bool 
AosIILClientWrapper::rollBackSnapshot(
						const u32 virtual_id,
						const u64 &snap_id,
						const u64 &task_docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRollBackSnapshot(rdata.getPtr(), virtual_id, snap_id, task_docid); 
}

//IdGen
u64 
AosIILClientWrapper::nextId1( 
						const u32 siteid,
						const u64 &iilid,
						const u64 &init_value,
						const OmnString &id_name,
						const u64 &incValue,
						const AosRundataPtr &rdata) 
{
    //Xuqi 2015/11/2
	u64 cid = 0;                                     
	u32 sid = rdata->setSiteid(siteid);              
	bool rslt = incrementDocidToTable(iilid, id_name,
			    cid, incValue, init_value, true, rdata);     
	rdata->setSiteid(sid);                           
	aos_assert_rr(rslt, rdata, 0);                   
	return cid;                                      
}

//Job
bool 
AosIILClientWrapper::mergeSnapshot(
						const u32 virtual_id,
						const u64 &target_snap_id,
						const u64 &merge_snap_id,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoMergeSnapshot(rdata.getPtr(), virtual_id, target_snap_id, merge_snap_id); 
}

//JQLStatement
bool 
AosIILClientWrapper::deleteIIL(
						const OmnString &iilname,
						const bool true_delete,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoDeleteIIL(rdata.getPtr(), iilname, true_delete); 
}
	
//Microblog
u64	
AosIILClientWrapper::getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname1,
						const OmnString &keyvalue1,
						const OmnString &keyname2,
						const OmnString &keyvalue2,
						bool &duplicated,
						const AosRundataPtr &rdata)
{
	// This function retrieves the XML document for the object that
	// is in the container 'ctnr_objid', where the container is on
	// the site 'siteid'.

	// 1. Retrieve the container docid
	if (siteid == 0)
	{
		rdata->setError() << "SiteId is missing!";
		return AOS_INVDID;
	}

	if (ctnr_objid == "")
	{
		rdata->setError() << "No container specified!";
		return AOS_INVDID;
	}

	u64 docid = AOS_INVDID;
	bool isunique;
	bool rslt = getDocidByObjid(siteid, ctnr_objid, docid, isunique, rdata);
	if (!rslt || docid == AOS_INVDID)
	{
		rdata->setError() << "Invalid container: " << ctnr_objid << ". SiteId: " << siteid;
		return AOS_INVDID;
	}

	if (!isunique)
	{
		rdata->setError() << "The container is not unique: " << ctnr_objid;
		return AOS_INVDID;
	}

	// 2. Retrieve the container IIL:
	OmnString ctnr_iilname = AosIILName::composeCtnrMemberObjidIILName(ctnr_objid);
	OmnString iilname1 = AosIILName::composeAttrIILName(keyname1);
	AosBitmapObjPtr query_bitmap = AosBitmapMgrObj::getBitmapStatic();
	// SHAWN_WARN, how to set the page size?????
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(keyvalue1);
	query_context->setBlockSize(0);

	rslt = querySafe(iilname1, 0, query_bitmap, query_context, rdata);
	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}
	
	OmnString iilname2 = AosIILName::composeAttrIILName(keyname2);
	query_context->setOpr(eAosOpr_eq);
	query_context->setStrValue(keyvalue2);
	query_context->setBlockSize(0);
	
	// SHAWN_WARN, how to set the page size?????
	rslt = querySafe(iilname2, 0, query_bitmap, query_context, rdata);

	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}
	
	query_context->setOpr(eAosOpr_an);
	query_context->setBlockSize(0);
	// SHAWN_WARN, how to set the page size?????
	rslt = querySafe(ctnr_iilname, 0, query_bitmap, query_context, rdata);

	if (!rslt || query_bitmap->isEmpty())
	{
		AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
		query_bitmap = 0;
		return 0;
	}

	query_bitmap->reset();

	u64 did = 0; 
	if (!query_bitmap->nextDocid(did))
	{
		rdata->setError() << "Object not found (003). Key Name1: "
			<< keyname1 << "; Key Value1: " << keyvalue1
			<< "Key Name2: " << keyname2 << "; Key Value2: " << keyvalue2;
	}
		
	AosBitmapMgrObj::returnBitmapStatic(query_bitmap);
	query_bitmap = 0;
	return did;
}

//Query
bool 
AosIILClientWrapper::preQuerySafe(
						const OmnString &iilname,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoPreQuerySafe(rdata.getPtr(), iilname, query_context);
}

bool 
AosIILClientWrapper::preQuerySafe(
						const u64 &iilid,
						const AosQueryContextObjPtr &query_context,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoPreQuerySafe(rdata.getPtr(), iilid, query_context); 
}

bool 
AosIILClientWrapper::getDocidByObjid(
						const u32 siteid,
						const OmnString &objid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata)
{
	aos_assert_r(siteid != 0 && objid != "", false);

	OmnString tmpname = AosIILName::composeObjidListingName(siteid);
	aos_assert_r(tmpname != "", false);

	OmnString iilname = tmpname;
	if (OmnApp::getBindObjidVersion() != OmnApp::eDftBindObjidVersion)
	{	
		int vid = AosGetCubeId(objid);
		iilname = AosIILName::composeCubeIILName(vid, tmpname);
		aos_assert_r(iilname != "", false);
	}

	return getDocid(iilname, objid, eAosOpr_eq, false, docid, isunique, rdata);
}

//SeLogClient
bool 
AosIILClientWrapper::addU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) 
{
	bool ispersis = false;
	return Jimo::jimoAddU64ValueDoc(rdata.getPtr(), iilname, value, docid, 
						value_unique, docid_unique, ispersis); 
}
	
bool 
AosIILClientWrapper::removeU64ValueDoc(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRemoveU64ValueDoc(rdata.getPtr(), iilname, value, docid); 
}

//SeLogSvr not exist
bool 
AosIILClientWrapper::addStrValueDocToTable(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata)
{
	OmnNotImplementedYet
	return false;
}

bool 
AosIILClientWrapper::removeStrFirstValueDoc(
						const OmnString &iilname,
						const OmnString &value,
						u64 &docid,
						const bool reverse,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoRemoveStrFirstValueDoc(rdata.getPtr(), iilname, value, docid, reverse); 
}
	
//not exist
bool 
AosIILClientWrapper::addU64ValueDocToTable(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet
	return false;
}
		
//SEModules
u64	
AosIILClientWrapper::getCtnrMemDocid(
						const u32 siteid,
						const OmnString &ctnr_objid,
						const OmnString &keyname,
						const OmnString &keyvalue,
						bool &duplicated,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoGetCtnrMemDocid(rdata.getPtr(), siteid, ctnr_objid, keyname, 
						keyvalue, duplicated); 
}

//SeReqProc
bool 
AosIILClientWrapper::appendManualOrder(
						const OmnString &iilname,
						const u64 &docid,
						u64 &value,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoAppendManualOrder(rdata.getPtr(), iilname, docid, value);
}

bool 
AosIILClientWrapper::incrementInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const u64 incValue,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoIncrementInlineSearchValue(rdata.getPtr(), iilname, value, seqid, incValue);
}

bool
AosIILClientWrapper::moveManualOrder(
						const OmnString &iilname,
						u64 &value1,
						const u64 &docid1,
						u64 &value2,
						const u64 &docid2,
						const OmnString flag,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoMoveManualOrder(rdata.getPtr(), iilname, value1, docid1, value2, docid2, flag);
}
	
bool 
AosIILClientWrapper::removeManualOrder(
						const OmnString &iilname,
						const u64 &value,
						const u64 &docid,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoRemoveManualOrder(rdata.getPtr(), iilname, value, docid);
}

bool 
AosIILClientWrapper::swapManualOrder(
						const OmnString &iilname,
						const u64 &value1,
						const u64 &docid1,
						const u64 &value2,
						const u64 &docid2,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoSwapManualOrder(rdata.getPtr(), iilname, value1, docid1, value2, docid2);
}

//SmartDoc
bool 
AosIILClientWrapper::getBatchDocids(
						const AosRundataPtr &rdata,
						const AosBuffPtr &column,
						const OmnString &iilname) 
{
	return Jimo::jimoGetBatchDocids(rdata.getPtr(), column, iilname);
}
	
bool 
AosIILClientWrapper::getDocids(
						const OmnString &iilname,
						int &iilidx,
						int &idx,
						u64 *docids,
						const int array_size,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoGetDocids(rdata.getPtr(), iilname, iilidx, idx, docids, array_size);
}

bool 
AosIILClientWrapper::updateKeyedValue(
						const OmnString &iilname,
						const u64 &key,
						const bool &flag,
						const u64 &delta,
						u64 &new_value,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoUpdateKeyedValue(rdata.getPtr(), iilname, key, flag, delta, new_value);
}

bool 
AosIILClientWrapper::addInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoAddInlineSearchValue(rdata.getPtr(), iilname, value, seqid, splitType);
}

bool 
AosIILClientWrapper::modifyInlineSearchValue(
						const OmnString &iilname,
						const OmnString &old_value,
						const OmnString &new_value,
						const u64 &old_seqid,
						const u64 &new_seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoModifyInlineSearchValue(rdata.getPtr(), iilname, old_value, new_value,
					old_seqid, new_seqid, splitType);
}

bool 
AosIILClientWrapper::modifyStrValueDoc(            
						const OmnString &iilname,
						const OmnString &oldvalue,
						const OmnString &newvalue,
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique,
						const bool override,
						const AosRundataPtr &rdata)
{
	return Jimo::jimoModifyStrValueDoc(iilname, oldvalue, newvalue,
					docid, value_unique, docid_unique, override, rdata);
}

bool 
AosIILClientWrapper::removeInlineSearchValue(
						const OmnString &iilname,
						const OmnString &value,
						const u64 &seqid,
						const OmnString &splitType,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoRemoveInlineSearchValue(rdata.getPtr(), iilname, value, seqid, splitType);
}
	
//StatUtil
bool 
AosIILClientWrapper::incrementDocidToTable(
						const u64 &iilid,
						const OmnString &key,
						u64 &value,
						const u64 &incValue,
						const u64 &initValue,
						const bool add_flag,
						const AosRundataPtr &rdata) 
{
	//Xuqi 2015/11/2
	return Jimo::jimoIncrementDocidToTable(rdata.getPtr(), iilid, key, value, incValue, initValue,
							add_flag);
}

//DocClient
bool 
AosIILClientWrapper::bindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoBindCloudid(rdata.getPtr(), cloudid, docid);
}

bool 
AosIILClientWrapper::bindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoBindObjid(rdata.getPtr(), objid, docid);
}

bool 
AosIILClientWrapper::unbindCloudid(
						const OmnString &cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoUnbindCloudid(rdata.getPtr(), cloudid, docid);
}

bool 
AosIILClientWrapper::unbindObjid(
						const OmnString &objid,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoUnbindObjid(rdata.getPtr(), objid, docid);
}

bool 
AosIILClientWrapper::rebindCloudid(
						const OmnString &old_cloudid,
						const OmnString &new_cloudid,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoRebindCloudid(rdata.getPtr(), old_cloudid, new_cloudid, docid);
}

bool 
AosIILClientWrapper::rebindObjid(
						const OmnString &old_objid,
						const OmnString &new_objid,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoRebindObjid(rdata.getPtr(), old_objid, new_objid, docid);
}

bool 
AosIILClientWrapper::getDocidByCloudid(
						const u32 siteid,
						const OmnString &cid,
						u64 &docid,
						bool &isunique,
						const AosRundataPtr &rdata) 
{
	aos_assert_rr(siteid != 0 && cid != "", rdata, false);                               
	aos_assert_rr(cid.length() > 0 && cid.length() < eAosMaxCloudidLength, rdata, false);
	docid = 0;                                                                           
	OmnString iilname = AosIILName::composeCloudidListingName(siteid);                   
	return getDocid(iilname, cid, eAosOpr_eq, false, docid, isunique, rdata);   
}

bool 
AosIILClientWrapper::addHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						int &physical_id,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoAddHitDoc(rdata.getPtr(), allTrans, arr_len, iilname, docid, physical_id);
}

bool 
AosIILClientWrapper::removeHitDoc(
						vector<AosTransPtr> *allTrans,
						const u32 arr_len,
						const OmnString &iilname,
						const u64 &docid,
						const AosRundataPtr &rdata) 
{
	return Jimo::jimoRemoveHitDoc(rdata.getPtr(), allTrans, arr_len, iilname, docid);
}

AosXmlTagPtr
AosIILClientWrapper::getXmlFromBuff(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(buff, 0);
	
	u32 len = buff->getU32(0);
	aos_assert_r(len, 0);

	AosBuffPtr b = buff->getBuff(len, false AosMemoryCheckerArgs);
	aos_assert_r(b, 0);

	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(b->data(), len, "" AosMemoryCheckerArgs);
	aos_assert_r(xml, 0);

	return xml;
}




