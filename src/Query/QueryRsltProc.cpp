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
// 2013/08/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QueryReq.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "QueryClient/BatchQuery.h"
#include "Util1/Wait.h"


// Chen Ding, 2013/08/31
bool
AosQueryReq::procQueryRawRslts(const AosRundataPtr &rdata)
{
	// The query finished. The results are in 'data'. This function 
	// checks whether this is what the requester wants. 
	// 'mRawRsltProc' is a jimo. It is used to proc raw results. It may
	// control whether to continue the query or terminate the query.
	aos_assert_rr(mOrTerm, rdata, false);
	mQueryRawRslts = mOrTerm->getQueryData();
	aos_assert_rr(mQueryRawRslts, rdata, false);

	bool finished = mOrTerm->queryFinished();
	if (finished)
	{
		// The query processed all the data. 
		// addLogEntry(rdata, "queried_all_data", pos);
	}

	mTimestampBlockEnd = OmnGetTimestamp();
/*	if (data && data->getNumDocs() > 0)
	{
		// mSessionNumBlocks++;
		// addStatEntry(rdata, eBlockStat, mTimestampBlockEnd - mTimestampBlockStart); 
	}
*/
	// Some results have been queried. This is the first Jimo Check Point
	// to see whether it needs to process the raw results. Note that the 
	// jimo may terminate the normal processing flow. 
	// if (mRawRsltProcFunc)
	// {
	// 	// There is a jimo to proc the raw results. 
	// 	if (!jimoProcRawRslts(rdata, query_proc, finished, data))
	// 	{
	// 		addLogEntry(rdata, "query_failed", pos);
	// 		return false;
	// 	}
	// 
	//	if (!continue_proc)
	//	{
	//		// It wants to terminate the normal processing.
	//		addLogEntry(rdata, "query_terminated_001", pos);
	//		callerQueryTerminated(rdata);
	//		return true;
	//	}
	//}

	return fetchFields(rdata);
}

#if 0
bool
AosQueryReq::jimoProcRawRslts(
		const AosRundataPtr &rdata, 
		const AosQueryRsltObjPtr &raw_data, 
		bool &continue_proc)
{
	// Query raw results have been returned. The jimo 'mRawRsltProcJimo" 
	// is configured. This function calls the jimo to proc the raw query
	// results. 
	
	// 1. Call the function
	continue_proc = true;
	/*
	aos_assert_rr(mRawRsltProcFunc, rdata, false);
	rslt = (*mRawRsltProcFunc)(rdata, raw_data);
	if (!rslt)
	{
		addLogEntry(rdata, "jimo_processing_failed", pos);
		callerQueryFailed(rdata, rdata->getErrmsg());
		return false;
	}

	// 2. Check the status
	int status = rdata->getArgInt("query_proc_status", -1);
	switch (status)
	{
	case AosQueryProcStatus::eTerminate:
		 // The jimo wants to terminate the processing. It checks 
		 // whether it needs to inform the caller about the data. 
		 addLogEntry(rdata, "terminated_by_jimo", pos);
		 continue_proc = false;
		 if (rdata->check("created_new_data", idx))
		 {
			 // The function has created some new data. It checks
			 // whether the caller supports the method. If yes, it 
			 // will inform the caller should the raw data. 
			 AosBuffPtr buff = rdata->getBuff(idx);
			 if (!buff)
			 {
				 OmnString errmsg = AosTranslate("failed_retrieve_data");
				 AosSetUserError(rdata, errmsg) << enderr;
				 addLogEntry(rdata, errmsg, pos);
				 callerQueryFailed(rdata, errmsg);
				 return false;
			 }

			 callerRawData(rdata, buff, finished);
		 }
		 else if (rdata->check("report_data_to_caller"))
		 {
			 // Anything in 'data' is considered valid data. 
			 callerRawData(rdata, data);
		 }
		 callerQueryTerminated(rdata);
		 return true;

	case AosQueryProcStatus::eNextBlock:
		 if (finished)
		 {
			 continue_proc = false;
			 addLogEntry(rdata, "finished_normally", pos);
			 callerQueryFinished(rdata);
			 return true;
		 }

		 addStatEntry(rdata, eBlockStat, mPageQueryStart, pos);
		 if (!nextBlock(rdata)) 
		 {
			 addLogEntry(rdata, "query_failed", rdata->getErrmsg(), pos);
			 queryFailed(rdata, ...);
			 return false;
		 } 
		 continue_proc = true;
		 return true;

	default:
		 AosSetErrorUser(rdata, "unrecognized_status") << status << enderr;
		 addLogEntry(rdata, "unrecognized_status_from_raw_jimo")
			 << AOSDICTERM("status") << ": " << status 
			 << AOSDICTERM("supported_status") << ": Terminate("
			 << (int)AosQueryProcStatus::eTerminate
			 << "), NextBlock(" << (int)AosQueryProcStatus::eNextBlock
			 << ")" << enderr;
		 callerQueryFailed(rdata, rdata->getErrmsg(), pos);
		 return false;
	}

	continue_proc = rdata->getArgInt("query_continue_proc", true);
	*/
	return true;
}
#endif


bool
AosQueryReq::addLogEntry(
		const AosRundataPtr &rdata, 
		const OmnString &msg, 
		const int64_t pos)
{
	return true;
}


bool
AosQueryReq::callerQueryFailed(
		const AosRundataPtr &rdata, 
		const OmnString &errmsg)
{
/*	aos_assert_rr(mCaller, rdata, false);
	AosQueryReqObjPtr thisptr(this, false);
	return mCaller->queryFailed(rdata, thisptr, errmsg);
*/
	return true;
}


bool
AosQueryReq::fetchFields(const AosRundataPtr &rdata)
{
	// Some resuls have been queried and fields need to be fetched
	// for them. It checks whether a jimo has been configured. If yes, 
	// it uses the jimo to process the results. 

	//Jozhi 2014-07-11 may be complie error
	OmnNotImplementedYet;
	return false;
	/*
	AosXmlTagPtr batchxml = mConfig->getFirstChild("batch_query");
	OmnString id;  
	id << mQueryId;
 	mBatchQuery = OmnNew AosBatchQuery(id, batchxml, rdata, AosMemoryCheckerArgsBegin);	
	// Pack the fields
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin); 
	u32 size = mFieldDef.size();
	buff->setU32(size);
	for (u32 i=0; i<size; i++)
	{
		mFieldDef[i].serializeToBuff(buff);
	}
	
	mBatchNum = mQueryRawRslts->getNumDocs();
	mBatchQuery->addQueryRslt(rdata, buff, mFieldDef,  mQueryRawRslts);
	mColumns = mBatchQuery->getData();
	return procJoins(rdata);
	*/

	/*
	//print field values
	map<u64, struct AosBatchRecord>::iterator it;
	for ( it= data.begin() ; it !=  data.end(); it++ )
	{
		u64 docid = (*it).first;
		OmnScreen << "docid : " << docid << endl;

		struct AosBatchRecord batchbuff = (*it).second; 
		AosBuffPtr buff = batchbuff.r_buff;
		int startidx = batchbuff.r_buff_start;
		int length = batchbuff.r_buff_len;
		buff->setCrtIdx(startidx + sizeof(u32) + sizeof(u64));
		for(u32 j=0; j<size; j++)
		{
			OmnString value = buff->getOmnStr("");
			OmnScreen << "field value : " << value << endl;
		}
	}
	*/

	/*
	aos_assert_rr(mFuncFetchFields, rdata, false);
	// Fields are fetched by the user provided jimo.
	if (!(* mFuncFetchFields)(rdata, thisptr))
	{
		callerQueryFailed(rdata);
		return false;
	}

	if (!rdata->getArgBool("continue_proc", true))
	{
		// It wants to terminate the normal processing.
		addLogEntry(rdata, "query_terminated_002", pos);
		callerQueryTerminated(rdata);
		return true;
	}

	if (rdata->getArgBool("next_block", false))
	{
		// It wants to stop the current processing and gets the
		// next block.
		return nextBlock(rdata);
	}

	if (rdata->getArgBool("async_call", false))
	{
		// This is an async call. The jimo will call back.
		return true;
	}

	if (mFuncPostFetchFields) return postFetchFields(rdata);
*/
	return true;
	
}


bool
AosQueryReq::procJoins(const AosRundataPtr &rdata)
{
	// 'data' is a AosBuff. The buff is interpreted by mDataRecord. 
	// If joins are used, there should be some fields that are either
	// objids or docids to other (joined) docs. The join processing
	// is to use these referencing fields to fetch more fields from 
	// the referenced docs. It is similar to fetch fields.
	//
	
	return generateRecords(rdata);


	/* Temporarily, it is turned off
	AosDataScanner scanner(rdata, data, mDataRecord);
	int crt_join_idx = mCrtJoinIdx++;
	aos_assert_rr(crt_join_idx >= 0, rdata, false);
	if (mJoins.size() <= 0 || crt_join_idx >= mJoins.size())
	{
		return generateRecords(rdata);
	}

	AosDataRecordObjPtr record;
	int field_idx = mJoins[crt_join_idx].field_idx;
	if (mJoins[crt_join_idx].isObjid())
	{
		int idx = 0;
		AosObjidShuffler shuffler;
		while ((record = scanner.nextRecord(rdata)))
		{
			OmnString objid = record->getFieldStr(field_idx, "");
			shuffler.appendEntry(idx++, objid);
		}
		AosQueryReqObjPtr thisptr(this, false);
		return shuffler.shuffle(rdata, thisptr, mJoins[crt_join_idx].fields);
	}
	
	AosDocidShuffler shuffler;
	while ((record = scanner.nextRecord(rdata)))
	{
		u64 docid = record->getFieldU64(field_idx, 0);
		shuffler.appendEntry(idx++, docid);
	}
	AosQueryReqObjPtr thisptr(this, false);
	return shuffler.shuffle(rdata, thisptr, mJoins[crt_join_idx].fields);
	*/
}


bool
AosQueryReq::generateRecords(const AosRundataPtr &rdata)
{
	// This function generates records based on the contents
	// being collected so far:
	// 	mQueryRawRslts	
	// 	mFetchedData
	// 'mQueryRawRslts' is a QueryRslt. It has two AosBuff, one for
	// its field values and one for docids. 
	// 'mFetchedData' is a map that maps docids to its field contents.
	// This function will compose the final results based on these
	// two datasets. 
	//
	AosAsyncRespCallerPtr thisPtr(this, false);
	mNumPendingReqs = 0;
	AosQrUtil::createRecords(rdata, mColumns, mFieldDef, mBatchData, 
			thisPtr, mNumPendingReqs);
	if (mNumPendingReqs > 0)
	{
		mCondVar->wait(mLock);
	}

	// 
/*
	while (mNumPendingReqs > 0)
	{
		OmnWait::getSelf()->wait(0, 100000);
	}
*/

	/*
	if (mJimoPreGenerateRecords)
	{
		OmnString status = jimoPreGenerateRecords(rdata);
		if (!procStatus(rdata, status)) return true;
	}

	if (mJimoRecordGen)
	{
		OmnString status = jimoRecordGen(rdata);
		if (!procStatus(rdata, status)) return true;
	}

	// It will use the default record generator. 
	AosQueryReqObjPtr thisptr(this, false);
	AosRecordGen record_gen;
	bool rslt = record_gen.generateRecords(rdata, thisptr);
	aos_assert_rr(rslt, rdata, false);

	if (mJimoPostGenerateRecords)
	{
		OmnString status = jimoPostGenerateRecords(rdata);
		if (!procStatus(rdata, status)) return true;
	}

	aodAddLog(rdata, "query_finished", pos);
	AosQueryReqObjPtr thisptr(this, false);
	callerQueryFinished(rdata, thisptr);
	*/
	return true;
}


void
AosQueryReq::callback(
		const AosTransPtr &trans,
		const AosBuffPtr &resp,
		const bool svr_death)
{
	// This is the callback for 'generateRecords(...)'. This function
	// is called back for each column.
	// 'data' should be in the following format:
	// 		field_idx		int
	// 		field_value
	// 		field_value
	// 		...
	mLock->lock();
	mNumPendingReqs--;

	int field_idx = resp->getInt(0);
	int len = resp->dataLen()-sizeof(int);
	AosBuffPtr data = resp->getBuff(len, true AosMemoryCheckerArgs);
	mBatchData[field_idx] = data;
	if (mNumPendingReqs == 0)
	{
		mCondVar->signal();
	}
	mLock->unlock();
	return;
}


bool
AosQueryReq::fetchFieldsFinished(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	// XML joins
	// Create records
	// 	Create CSV
	// 	XML
	// 	JSON
	// 	SQL
	OmnNotImplementedYet;
	return false;
}


#if 0
bool
AosQueryReq::postFetchFields(const AosRundataPtr &rdata)
{
	if (!mFuncPostFetchFields(rdata, thisptr))
	{
		addLogEntry(rdata, "query_failed", pos);
		callerQueryFailed(rdata);
		return false;
	}

	if (!rdata->getArgBool("continue_proc", true))
	{
		// It wants to terminate the normal processing.
		addLogEntry(rdata, "query_terminated_002", pos);
		callerQueryTerminated(rdata);
		return true;
	}

	if (rdata->getArgBool("next_block", false))
	{
		// It wants to stop the current processing and gets the
		// next block.
		return nextBlock(rdata);
	}

	if (rdata->getArgBool("async_call", false))
	{
		// This is an async call. The jimo will call back.
		return true;
	}

	return true;
}


bool
AosQueryReq::config()
{
	if (mJimoRawRsltProc)
	{
		AosMethod method = mJimoRawRsltProc->getMethod(mRawRsltMethodName, 
				 	"AosRundataPtr+AosBuffPtr", mRawRsltVersion);
		if (!method.isValid())
		{
			AosSetUserError(rdata, "invalid_raw_results_method") << enderr;
			return false;
		}

		mFuncRawRsltProc = method.getFunc();
		aos_assert_rr(mRawRsltProcFunc, rdata, false);
	}

	if (mJimoPreFetchFields)
	{
		AosMethod method = mJimoPreFetchFields->getMethod(mPreFetchFieldsMethodName, 
				 	"AosRundataPtr+AosQueryReqObj", mPreFetchFieldsVersion);
		if (!method.isValid())
		{
			AosSetUserError(rdata, "invalid_pre_fetch_fields_methd") << enderr;
			return false;
		}

		mFuncPreFetchFields = method.getFunc();
		aos_assert_rr(mFuncPreFetchFields, rdata, false);
	}

	if (mJimoPostFetchFields)
	{
		AosMethod method = mJimoPostFetchFields->getMethod(mPostFetchFieldsMethodName, 
				 	"AosRundataPtr+AosQueryReqObj", mPostFetchFieldsVersion);
		if (!method.isValid())
		{
			AosSetUserError(rdata, "invalid_post_fetch_fields_methd") << enderr;
			return false;
		}

		mFuncPostFetchFields = method.getFunc();
		aos_assert_rr(mFuncPostFetchFields, rdata, false);
	}
}


bool
AosQueryReq::callerRawData(const AosRundataPtr &rdata, const AosBuffPtr &buff) 
{
	aos_assert_rr(mCaller, rdata, false);
	AosQueryReqObjPtr thisptr(this, false);

}


bool
AosQueryReq::fieldsFetched(const AosRundataPtr &rdata, const AosBuffArrayPtr &data)
{
	// Fields have been fetched. This function continues the processing. 
	AosQueryReqObjPtr thisptr(this, false);
	mQueriedData = data;
	if (mFuncPostFetchFields)
	{
		if (!(*mFuncPostFetchFields)(rdata, thisptr))
		{
			callerQueryFailed(rdata);
			return false;
		}

		if (!rdata->getArgBool("continue_proc", true))
		{
			// It wants to terminate the normal processing.
			addLogEntry(rdata, "query_terminated_002", pos);
			callerQueryTerminated(rdata);
			return true;
		}

		if (rdata->getArgBool("next_block", false))
		{
			// It wants to stop the current processing and gets the
			// next block.
			return nextBlock(rdata);
		}

		if (rdata->getArgBool("async_call", false))
		{
			// This is an async call. The jimo will call back.
			return true;
		}
	}

	// Next, it needs to process the joins. 
	return procJoins(rdata);
}


bool
AosQueryReq::joinCallback(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &data)
{
	// One join has finished. The results are in 'data', which is in the 
	// following format:
	// 		field, field, ..., field
	// 		field, field, ..., field
	// 		...
	// The fields are interpreted by the record defined in the join. 
	// For the entries whose joined fields do not identify valid docs, 
	// there are no values. The record will determine whether to provide
	// default values, and if yes, what the default values should be.
	// It simply saves the data in the query. 

	if (mJimoJoinCallback)
	{
		OmnString status = jimoJoinsCallback(rdata);
		if (!procStatus(rdata, status)) return true;
	}

	aos_assert_rr(mCrtJoinIdx >= 1, rdata, false);
	aos_assert_rr(mCrtJoinIdx-1 < mJoins.size(), rdata, false);
	mJoins[mCrtJoinIdx-1].result_data = data;

	if (mCrtJoinIdx < mJoins.size()) return procJoins(rdata);

	// All joins are finished. 
	if (mJimoPostJoinsProc)
	{
		OmnString status = jimoPostJoinsProc(rdata);
		if (!procStatus(rdata, status)) return true;
	}

	return generateRecords(rdata);
}


bool
AosQueryReq::procStatus(
		const AosRundataPtr &rdata, 
		const OmnString &status)
{
	if (status == AOS_RUNDATA_STATUS_ERROR)
	{
		addLogEntry(rdata, "query_failed", pos);
		callerQueryFailed(rdata);
		return false;
	}

	if (status == AOS_RUNDATA_STATUS_TERMINATE)
	{
		// It wants to terminate the normal processing.
		addLogEntry(rdata, "query_terminated_002", pos);
		callerQueryTerminated(rdata);
		return false;
	}

	if (status == AOS_RUNDATA_STATUS_NEXT_BLOCK)
	{
		// It wants to abandon the current results and fetch the next
		// block, if any.
		if (finished)
		{
			addLogEntry(rdata, "query_finished", pos);
			callerQueryFinished(rdata);
			return true;
		}

		if (!nextBlock(rdata)) return false;
		return true;
	}

	// All other status are ignored. 
	if (status != AOS_RUNDATA_STATUS_CONTINUE)
	{
		OmnString entry = "status:";
		entry << status;
		AosSetErrorUser(rdata, "invalid_status") << status << enderr;
		addLogEntry(rdata, "invalid_status") << entry;
	}

	return true;
}
#endif
