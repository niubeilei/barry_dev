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
// 2015/01/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QueryNormalizer.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQueryNormalizer::AosQueryNormalizer()
{
}


AosQueryNormalizer::~AosQueryNormalizer()
{
}


bool 
AosQueryNormalizer::setQuery(
		AosRundata *rdata, 
		const AosJqlSelectPtr &query)
{
	aos_assert_rr(query, rdata, false);
	mOrigQuery = query;
	mParentQuery = query;
	mCrtQuery = query;
}


bool
AosQueryNormalizer::normalize(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	// This function is called 
	// It runs 
	// This is an asynchronous call. It is the entry point for all new 
	// query processing. The query statement (select statement) is saved 
	// in mStatement. 
	//
	// If 'orig_statement' is the same as 'crt_statement', it is a root
	// level query.
	
	addTimestamp(eTimestampStart);
	mCaller = caller;
	mStatus = eStarted;
	mIndexAllFields = smIndexAllFields;
	mNeedReduce = true;

	if (!AosQueryNormalizerUtil::analyzeJoins(rdata, this))
	{
		AosLogError(rdata, true, "failed_analyze_joins") << enderr;
		return false;
	}

	return runNormalizer(rdata);
}


bool
AosQueryNormalizer::runNormalizer(AosRundata *rdata)
{
	// Normalizer is implemented as async calls. This means
	// that there will be multiple interactions between
	// this class and the execution. Any time a response
	// is received, it needs to call this function to check
	// what needs to be done next.
	while (1)
	{
		switch (mStatus)
		{
		case eToRunNormalizeSubqueriesInWhereClause:
			 if (!normalizeSubqueriesInWhereClause(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunNormalizeWhereConditions:
			 if (!normalizeWhereConditions(rdata)) 
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunNormalizeSubqueriesInSelectedFields:
		 	 if (!normalizeSubqueriesInSelectedFields(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunNormalizeSelectedFields:
			 if (!normalizeSelectedFields(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunNormalizeFromClause:
			 if (!normalizeFromClause(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunNormalizeJoins:
			 if (!normalizeJoins(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunNormalizeIndexes:
			 if (!normalizeIndexes(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunFinalCheck:
			 if (!finalCheck(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eToRunReduceQuery:
			 if (!reduceQuery(rdata))
			 {
				 AosLogError(rdata, false, "internal_error") << enderr;
				 return callFailed(rdata);
			 }
			 break;

		case eFailed:
			 return callFailed(rdata);

		case eFinished:
			 return callFinished(rdata);

		default:
			 AosLogError(rdata, false, "internal_error") << enderr;
			 return callFailed(rdata);
		}
	}

	OmnShouldNeverComeHere;
	return true;
}


bool
AosQueryNormalizer::normalizeToSimpleQuery(
		AosRundata *rdata, 
		const bool index_all_fields,
		const AosQueryCallerPtr &caller)
{
	// The differences are:
	// 1. Fields may not need to be indexed,
	// 2. It does not reduce the query
	addTimestamp(eTimestampStart);
	mCaller = caller;
	mStatus = eStarted;
	mIndexAllFields = index_all_fields;

	if (!AosQueryNormalizerUtil::analyzeJoins(rdata, this))
	{
		AosLogError(rdata, true, "failed_analyze_joins") << enderr;
		return false;
	}

	return runNormalizer(rdata);
}


bool
AosQueryNormalizer::normalizeSubqueriesInWhereClause(AosRundata *rdata)
{
	// This function goes over all the conditions in the
	// where clause. For each term, it recursively calls
	// its function to normalize sub-queries. When finishing,
	// all sub-queries should have been replaced with virtual
	// fields.
	// 
	switch (mSubqueryInWhereClauseStatus)
	{
	case eNotStartedYet;
	     return kickOffNormalizeSubqueriesInWhereClause(rdata);
	
	case eKickedOff:
		 // It is normalizing the sub-queries. Need to wait.
		 return eContinue;

	case eFinished:
		 return eFinished;
	
	default:
		 break;
	}

	AosLogError(rdata, false, "internal_error")
		<< AosFN("Code") << mSubqueryInWhereClauseStatus << enderr;
	return eError;
}


AosQueryProc::ReturnCode
AosQueryProc::kickOffNormalizeSubqueriesInWhereClause(AosRundata *rdata)
{
	aos_assert_rr(mSubqueryInWhereClauseStatus == eNotStartedYet, rdata, eError); 
	mSubqueryInWhereClauseStatus = eKickedOff;
	if (!mWhereConds)
	{
		// There are no where conditions. 
		mSubqueryInWhereClauseStatus = eFinished;
		return eFinished;
	}

	AosQueryCondTermPtr term = mWhereConds->firstTerm(rdata);
	AosQueryCallerPtr thisptr(this, false);
	mCondTerms.clear();
	while (term)
	{
		term->setIndex(mCondTerms.size());
		mCondTerms.push_back(term);
		if (!term->normalizeSubqueries(rdata, thisptr))
		{
			AosLogError(rdata, false, "failed_normalize_subqueries") << enderr;
			mSubqueryInWhereClauseStatus = eError;
			return eError;
		}
		term = mWhereConds->nextTerm(rdata);
	}

	// Check whether all terms are finished
	for (u32 i=0; i<mCondTerms.size(); i++)
	{
		if (!mCondTerms[i]->isNormalizeSubqueryFinished()) return eContinue;
	}
	return mSubqueryInWhereClauseStatus = eFinished;
}	


bool
AosQueryProc::condTermNormalizeSubqueryFinished(
		AosRundata *rdata,
		const OmnString &errmsg,
		const u32 idx)
{
	aos_assert_rr(idx < mCondTerms.size(), rdata, false);
	mCondTerms[i]->setErrmsg(rdata, errmsg);
	normalize(rdata);
	return true;
}


bool
AosQueryNormalizer::normalizeSelectedFields(AosRundata *rdata)
{
	// This function normalizes all the selected fields. 
	aos_assert_rr(mStatus == eStarted, rdata, false);
	mStatus = eNormalizeSelectedFields;

	// Get the selected fields. It assumes each element in 
	// 'mSelectedFields' already resolved its table, its
	// current query, its parent query, and its root query.
	mSelectedFields = mCrtStmt->getSelectedFields(rdata);
	if (mSelectedFields.size() <= 0)
	{
		AosLogError(rdata, true, "no_selected_fields") << enderr;
		return queryFailed(rdata, eNoSelectedFields);
	}

	int num_finished = 0;
	for (u32 i=0; i<mSelectedFields.size(); i++)
	{
		if (!mSelectedFields[i]->normalize(rdata))
		{
			// It failed the processing. 
			AosLogError(rdata, true, "failed_normalize_field") 
				<< AosFN("Field") << mSelectedFields[i]->toString() 
				<< enderr;
			return queryFailed(rdata, eFailedProcessingField);
		}
		if (mSelectedFields[i]->isNormalizeFinished()) num_finished++;
	}

	if (num_finished >= mSelectedFields.size())
	{
		// All selected fields were processed. Proceed to process
		// the conditions.
		mStatus = eSelectedFieldsNormalized;
		return true;
	}

	// Need to wait until all selected fields finish their processing.
	return true;
}


bool
AosQueryNormalizer::normalizeWhereConditions(AosRundata *rdata)
{
	// A where clause is an OR logical expression. If there
	// is only one OR term, it must be an AND term. 
	mStatus = eNormalizeWhereConds;
	mOrTerm = mCrtStmt->getWhereClause(rdata);
	u32 num_finished = 0;
	for (u32 i=0; i<mOrTerm.size(); i++)
	{
		if (!mOrTerm[i]->normalizeSubquery(rdata)) return queryFailed(rdata);
		if (mOrTerm[i]->normalizedSubqueryFinished()) num_finished++;
	}

	if (num_finished == mOrTerm.size()) 
	{
		mStatus = eWhereCondsNormalized;
		return true;
	}

	return true;
}


bool
AosQueryNormalizer::normalizeTables(AosRundata *rdata)
{
	// This function normalizes the selected tables. Below
	// is an example:
	// 	select * from t1, 
	// 	   (
	// 	      select * from t2
	// 	      where f2 = "abc"
	// 	   );
	// In this example, we need to replace the inner select
	// statement with an intermediate table.
}


bool
AosQueryNormalizer::normalizeIndexes(AosRundata *rdata)
{
	// This function checks whether all conditioned fields
	// were indexed. If not, it will create the indexes
	// automatically.
	mCondFields = getCondFields(rdata);
	if (mCondFields.size() <= 0)
	{
		// There are no conditioned fields. This is a full
		// table scanning with no conditions. 
		mStatus = eReadyFinalCheck;
		return true;
	}

	int num_no_indexes = 0;
	for (u32 i=0; i<mCondFields.size(); i++)
	{
		if (!mCondFields[i]->isIndexed(rdata))
		{
			if (!buildIndex(rdata, mCondFields[i]))
			{
				AosLogError(rdata, false, "failed_building_index") 
					<< AosFN("Query") << mOrigStmt->toString() 
					<< AosFN("CondField") << mCondFields[i] << enderr;
				return false;
			}

			num_no_indexes++;
		}
	}

	if (num_no_indexes > 0)
	{
		// Check whether all indexes were finished building
		for (u32 i=0; i<mCondFields.size(); i++)
		{
			if (mCondFields[i]->isBuildingIndex(rdata))
			{
				mStatus = eBuildingIndex;
				return true;
			}
		}
	}

	// All index building finished.
	mStatus = eAllIndexesBuilt;
	return true;
}


bool
AosQueryNormalizer::normalizeFieldFinished(
		AosRundata *rdata, 
		const int idx)
{
	// One of the selected field finished the processing. A selected
	// field needs asynchronous processing if the field is a sub-query
	// and the virtual field is not available. When it comes back, 
	// the virtual field (and the map) should have been created. 
	// What this function does now is to replace the sub-query with
	// the virtual field.
	if (idx < 0 || (u32)idx >= mSelectedFields.size())
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return checkFinish(rdata);
	}

	if (!mSelectedFields[i]->simpleField())
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return checkFinish(rdata);
	}

	if (!mSelectedFields[i]->replace(rdata))
	{
		AosLogUserError(rdata, "failed_replacing_virtual_field") << enderr;
		return checkFinish(rdata);
	}

	return checkFinish(rdata);
}


bool
AosQueryNormalizer::whereClauseFinished(
		AosRundata *rdata, 
		const int idx)
{
	if (idx < 0 || (u32)idx >= mOrTerm.size())
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return queryFailed(rdata);
	}

	mOrTerm[idx]->termFinished(rdata);
	if (!mOrTerm[idx]->reduce(rdata))
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return queryFailed(rdata);
	}

	return checkFinish(rdata);
}


bool
AosQueryNormalizer::queryNormalizeFinished(AosRundata *rdata)
{
	// This query finished normalization. 
	
	// Check whether all fields were normalized
	for (u32 i=0; i<mSelectedFields.size(); i++)
	{
		if (!mSelectedFields[i]->isNormalized(rdata))
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}
	}

	// All selected fields were normalized. Check the where clause
	if (!mOrTerm->isNormalized(rdata))
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	// The normalization finished. 

}


bool
AosQueryNormalizer::checkFinish(AosRundata *rdata)
{
	while (1)
	{
		switch (mStatus)
		{
		case eProcSelectedFields:
			 for (u32 i=0; i<mSelectedFields.size(); i++)
			 {
				 if (!mSelectedFields[i]->normalizeFinished()) return false;
			 }
			 mStatus = eProcWhereClause;
			 if (!procWhereClause(rdata)) return queryFinished(rdata);
			 break;

		case eProcWhereClause:
			 for (u32 i=0; i<mWhereFields.size(); i++)
			 {
				 if (!mWhereFields[i]->normalizeFinished()) return false;
			 }
			 mStatus = eFinished;
			 return queryFinished(rdata);

		default:
			 AosLogError(rdata, false, "internal_error") << mStauts << enderr;
			 return queryFailed(rdata);
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosQueryNormalizer::parseCompoundJoin(AosRundata *rdata)
{
	// A Compound Join is a query that has more than two 
	// tables. 
	// 	MasterTable joins Table1 joins Table2 ... joins Tablen
	// It traces from the master table all the way to the end.
	// It replaces the last join with a virtual data field.
	//
	// Below is an example:
	// select ...,
	//   (
	// 		select sum(ad.act_weight)
	// 		from am, ad
	// 		where am.order_no = ad.order_no and
	//   		am.advice_num = ad.advice_num and
	//     		am.advice_status > '00' and
	//       	am.advice_num like 'A%' and
	//         	ad.order_no = t.order_no and
	//          ad.product_id = t.product_id
	//   ),
	//   ...
	// from t;
	//
	// The master table is 't', which joins 'ad':
	// 			t.order_no = ad.order_no and
	// 			t.product_id = ad.product_id
	// The table 'ad' then joins am:
	// 			ad.order_no = am.order_no and
	// 			ad.advice_num = am.advice_num
	//
	// What we should do is to create a virtual field to replace
	// the join(ad, am):
	//   (
	// 		select sum(ad.act_weight)
	// 		from ad
	// 		where ad.virtual_field == 1 and
	// 		  ad.order_no = t.order_no and
	// 		  ad.product_id = t.product_id;
	//   )
	// It then creates anothe virtual field to replace the join:

	// Need to pick a table to replace. It starts from the 
	// master table. 
	AosTableObjPtr crt_table = mMasterTable;
	AosTableObjPtr prev_table;
	while (1)
	{
		AosTableObjPtr table = findJoinedTable(rdata, crt_table);
		if (!table)
		{
			if (!prev_table)
			{
				AosLogUserError(rdata, "internal_error") << enderr;
				return false;
			}

			// Found the leaf join: [prev_table, crt_table]
			return replaceJoin(rdata, prev_table, crt_table);
		}
		else
		{
			prev_table = crt_table;
			crt_table = table;
		}
	}

	OmnShouldNeverComeHere;
	return false;
}


vector<AosDataFieldObjPtr> 
AosDatafieldByQuery::getSelectedFields(
		AosRundata *rdata, 
		const AosTableObjPtr &table)
{
	// This function retrieves the selected fields of 
	// 'table'. As an example, 
	// 	select f4,  
	// 		(
	// 	    	select s.f1 
	// 	    	from s 
	// 	    	where t.f2 = s.f3;
	// 		)
	//  from t;
	// In this example, this virtual field is for:
	// 		select s.f1
	// 		from s
	// 		where t1.f2 = s.f3
	// and the selected fields for 's' is 's.f1'.
	vector<AosSelectExprPtr> selected_fields = mQuery->getSelectedFieldExprs();
	for (u32 i=0; i<selected_fields.size(); i++)
	{
	}
}


bool
AosQueryNormalizer::analyzeTables(AosRundata *rdata)
{
	// For the outmost query, the from clause lists one
	// or more tables. If multiple tables are used, 
	// they can be reduced. If an inner sub-query
	// uses one or more outer tables, 
	// 
	// When using tables for joins, joins can assume tables 
	// from its own select statement and all its outer
	// select statements. Each join query needs to 
	// reduce its own joins, and then the outer joins
	// by adding virtual fields for them.
}


bool
AosQueryNormalizer::replaceJoin(
		AosRundata *rdata, 
		const vector<OmnString> &selected_fields,
		const AosTableObjPtr &lhs,
		const AosTableObjPtr &rhs)
{
	vector<AosDataFieldObjPtr> selected_fields = getSelectedFields(rdata, prev_table);
	vector<AosJqlCondTermObjPtr> conds = getConditions(rdata, crt_table);

	if (selected_fields.size() > 0)
	{
		// The join is used to select values. 
		vector<AosDataFieldObjPtr> join_fields = getJoinFields(rdata, lhs, rhs);
		if (join_fields.size() <= 0)
		{
			AosLogUserError(rdata, "internal_error") << enderr;
			return false;
		}

		vector<AosDataFieldObjPtr> virtual_fields;
		for (u32 i=0; i<selected_joins.size(); i++)
		{
			AosDataFieldObjPtr field = selected_fields[i];
			AosJoinMapObjPtr map = rhs->doesMapExist(rdata, join_fields, field, conds);
			if (map)
			{
				AosDataFieldObjPtr vf = lhs->doesJoinVFExist(rdata, field, map);
				if (vf)
				{
					// The join virtual field exist. It can use this virtual field
					// instead of the actually joining the table.
					virtual_fields.push_back(vf);
					continue;
				}
			
				// The virtual field does not exist, but the map exist. 
				// This is an error.
				AosLogUserError(rdata, "internal_error");
				return false;
			}

			// The map does not exist. Need to create the map.
			return rhs->createJoinMap(rdata, join_fields, field, conds);
		}

		// When it comes to this point, all the selected fields can be
		// replaced with a virtual field on lhs table. The original 
		// query can be replaced by removing the join.
		removeJoins(rdata, join_fields, conds);
		replaceSelectedFields(rdata, selected_fields, virtual_fields);
		return true;
	}

	// The join is used as filters. It needs to create an Exist 
	// Join Virtual Field.
	AosJoinMapObjPtr map = rhs->doesMapExist(rdata, join_fields, conds);
	if (map)
	{
		// The map exists. Need to create the virtual field.
		return lhs->createExistJoinVirtualField(rdata, map);
	}

	// The map does not exist.
	return rhs->createJoinMap(rdata, join_fields, conds);
}


bool
AosQueryNormalizer::procStatQuery(AosRundata *rdata)
{
	// 1. If it has conditions, there are two options for
	//    it. One is to filter out the records and the
	//    other is to treat the conditioned fields as
	//    key fields. The former builds a stat model 
	//    that can only be used when the conditions are
	//    met. The latter can be used for handle any
	//    combinations of the conditions.
	//
	//    Below is an example.
	//    select phonenum, sum(call_dur) from t
	//    where call_type=1 and fee_type=2
	//    group by phonenum;
	//
	OmnNotImplementedYet;
	return false;
}


/*
bool
AosQueryNormalizer::procFullTableScanning(AosRundata *rdata)
{
	// This function assumes:
	// 1. The query has no joins,
	// 2. The query has no sub-queries,
	// 3. No fields are indexed. 
	AosQueryProcPtr proc = createQueryProc(rdata, sgFullTableScannerClassname);
	if (!proc)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	AosQueryCallerPtr thisptr(this, false);
	return proc->runQuery(rdata, mOrigStatement, mCrtStatement, thisptr);
}


bool
AosQueryNormalizer::procPartialIndexedQuery(AosRundata *rdata)
{
	// This function assumes:
	// 1. The query has no joins,
	// 2. The query has no sub-queries,
	// 3. Some of the conditioned fields are indexed, but not all
	AosQueryProcPtr proc = createQueryProc(rdata, sgPartialIndexedQueryProcClassname);
	if (!proc)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	AosQueryCallerPtr thisptr(this, false);
	return proc->runQuery(rdata, mOrigStatement, mCrtStatement, thisptr);
}


bool
AosQueryProcBaase::procSmartQuery(AosRundata *rdata)
{
	// This function is called when the query:
	// 1. Has no joins and sub-queries
	// 2. All conditioned fields are indexed
	// The query is processed by a jimo. If users did not specify
	// the jimo, it uses the system defined (default) jimo. 
	AosQueryProcPtr proc = createQueryProc(rdata, sgSmartQueryProcClassname);
	if (!proc)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	AosQueryCallerPtr thisptr(this, false);
	return proc->runQuery(rdata, mOrigStatement, mCrtStatement, thisptr);
}


bool
AosQueryNormalizer::procJoins(AosRundata *rdata)
{
	// This function assumes the query has joins. It uses
	// the join reduction rules to replace the joins.
	mJoins = mCrtStatement->getJoins(rdata);
	if (mJoins.size() <= 0) 
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	for (u32 i=0; i<mJoins.size(); i++)
	{
		procJoin(rdata, mJoins[i].getPtrNoLock());
	}

	return true;
}


bool
AosQueryNormalizer::procJoin(
		AosRundata *rdata, 
		AosJoinObj *join, 
		bool &finished)
{
	// This function processes the join 'join'. 
	if (join->canBeReplaced())
	{
		// The join can be replaced by a join virtual field
		bool rslt = join->replaceJoin(rdata, mOrigStatement, mCrtStatement);
		aos_assert_rr(rslt, rdata, false);
		finished = true;
		return true;
	}

	return join->procJoin(rdata, thisptr);
}


bool
AosQueryNormalizer::joinFailed(AosRundata *rdata, AosJoinClauseObj *join)
{
	if (join->isCancelled()) 
	{
		// This join was cancelled by someone. Its mCrtStatement should
		// also been cancelled. Otherwise, it is an error.
		if (!mCrtStatement->isCancelled())
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}
		return true;
	}

	// Need to cancel all other joins
	mLock->lock();
	mCrtStatement->cancelQuery(rdata);
	for (u32 i=0; i<mJoins.size(); i++)
	{
		if (!mJoins[i]->finished()) mJoins[i]->cancelJoin(rdata);
	}
	mLock->unlock();

	mCaller->queryFailed(rdata, rdata->getErrmsg());
	return true;
}


bool
AosQueryNormalizer::joinFinished(AosRundata *rdata, AosJoinClauseObj *join)
{
	bool rslt = replaceJoin(rdata, join);

	// Check whether all joins are replaced
	mLock->lock();
	for (u32 i=0; i<mJoins.size(); i++)
	{
		if (!mJoins[i].getPtrNoLock()->finished())
		{
			mLock->unlock();
			return true;
		}
	}

	// All joins have been processed and converted. 
	mLock->unlock();
	if (mCrtStatement->hasJoins())
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		mCaller->queryFailed(rdata, rdata->getErrmsg());
		return false;
	}

	bool rslt = runQuery(rdata, mOrigStatement, mCrtStatement, mCaller);
	if (!rslt) mCaller->queryFailed(rdata, rdata->getErrmsg());
	if (mCrtStatement->finished()) mCaller->queryFinished(rdata, mCrtStatement);
	return true;
}


AosQueryProcPtr
AosQueryNormalizer::createQueryProc(AosRundata *rdata, const OmnString &jimo_name)
{
	OmnString name = mCrtStatement->getQueryJimoName();
	if (name != "")
	{
		AosJimoPtr jimo = AosCreateJimoByName(rdata, name);
		if (jimo)
		{
			if (jimo->getJimoType() == AosJimoType::eQueryProc)
			{
				AosQueryProcPtr proc = dynamic_cast<AosQueryProc *>(jimo.getPtr());
				if (proc) 
				{
					return proc;
				}
				else
				{
					AosLogMsg(rdata, "internal_error");
				}
			}
			else
			{
				AosLogMsg(rdata, "jimo_type_mismatch")
					.attr("expected_jimotype", "QueryProc")
					.attr("actual_jimotype", jimo->getJimoTypeStr());
			}
		}
	}

	// Create the default simple query proc jimo
	mLock->lock();
	itr = mJimos.find(jimo_name);
	if (itr == mJimos.end())
	{
		AosJimoPtr jimo = AosCreateJimoByClassname(rdata, jimo_name);
		if (!jimo)
		{
			AosLogUserError(rdata, "internal_error") << enderr;
			mLock->unlock();
			return 0;
		}
	 
		if (jimo->getJimoType() != AosJimoType::eQueryProc)
		{
			AosLogUserError(rdata, "internal_error") << enderr;
			mLock->unlock();
			return 0;
		}

		AosQueryProcPtr proc = dynamic_cast<AosQueryProc *>(jimo.getPtr());
		if (!proc) 
		{
			AosLogUserError(rdata, "internal_error") << enderr;
			mLock->unlock();
			return 0;
		}
		mJimos[jimo_name] = proc;
	}
	else
	{
		proc = itr->second;
	}
	mLock->unlock();
	return proc;
}
*/


