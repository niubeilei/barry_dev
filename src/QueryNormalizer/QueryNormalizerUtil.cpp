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
// 2015/02/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QueryNormalizerUtil.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


bool
AosQueryNormalizerUtil::analyzeJoins(
		AosRundata *rdata, 
		const AosQueryStmtPtr &crt_query, 
		AosJoinMap &join_map)
{
	// This function goes over the conditions to find the
	// join conditions. The simplest form is:
	// 		t1.f1 = t2.f2 and
	// 		t1.f3 = t2.f4 and ...
	// But join conditions are logical expressions. As long
	// as there are form of:
	// 		t1.f1 = t2.f2
	// it is considered t1 joins t2, regardless of how it is
	// embedded in the conditions. 
	crt_query->resetJoinClause();
	join_map.clear();
	AosJoinClausePtr join = crt_query->getFirstJoinClause();
	while (join)
	{

		join_map.addJoin(rdata, join);
		join = crt_query->getNextJoinClause();
	}

	return true;
}


bool
AosJoinMap::addJoin(AosRundata *rdata, const AosJoinClausePtr &join)
{
	OmnString lhs_tname = join_clause->getLHSTableName();
	OmnString lhs_fname = join_clause->getLHSFieldname();
	OmnString rhs_tname = join_clause->getRHSTableName();
	OmnString rhs_fname = join_clause->getRHSFieldname();
	JoinType join_type = join_clause->getJoinType();
	
	itr = mMap.find(lhs_tname);
	if (itr == mMap.end())
	{
		// Not in the map yet. Add it.
		DbEntry entry(rhs_tname, join_type, lhs_fname, rhs_fname);
		mMap[lhs_tname] = entry;
		return true;
	}
	
	// The entry is found. Check whether it is the same
	// type and joins the same table.
	itr->second.appendEntry(rhs_tname, join_type, lhs_fname, rhs_fname);
	return true;
}


bool
AosQueryNormalizer::DbEntry::appendEntry(
		const OmnString &rhs_tname, 
		const AosJoinType type,
		const OmnString &lhs_fname,
		const OmnString &rhs_fname)
{
	for (u32 i=0; i<mEntries.size(); i++)
	{
		if (
		

		addMapEntry(lhs_tname, lhs_fname, rhs_tname, rhs_fname);
		join_clause = crt_query->nextJoinClause();
	}

	// All join terms are collected and added to mJoinMap.
	if (isSubquery())
	{
		// This is a sub-query. Pick all the root join terms. 
		// A root join term is a join term whose table is not
		// in this query's from-clause.
	}
	else
	{
		// This is a root query.
		int num_tables = getNumFromTables();
		if (num_tables == 0)
		{
			// This is allowed in JimoDB. 
			if (mJoinMap.size() > 0)
			{
				AosLogError(rdata, true, "cannot_join_with_no_tables")
					<< AosFN("Query") << mOrigQuery->toString() << enderr;
				return false;
			}
			return true;
		}

		if (num_tables == 1)
		{
			// There is only one table. It shall have no joins
			if (mJoinMap.size() > 0)
			{
				AosLogError(rdata, true, "cannot_join_with_no_tables")
					<< AosFN("Query") << mOrigQuery->toString() << enderr;
				return false;
			}
			return true;
		}


}


bool
AosQueryNormalizerUtil::DbEntry::appendEntry(
		const OmnString &rhs_tname, 
		const JoinType join_type,
		const OmnString &lhs_fname, 
		const OmnString &rhs_fname)
{
	for (u32 i=0; i<mTables.size(); i++)
	{
		// Find the rhs_table name
		if (mTables[i].table_name == rhs_tname)
		{
			mTables[i].append(join_type, lhs_fname, rhs_fname);
		}
	}

	// Not exist. Need to add it
	mTables.push_back(JoinedEntry(rhs_tname, join_type, lhs_fname, rhs_fname));
	return true;
}


AosQueryFieldSimple::JoinEntry::append(
		const JoinType join_type,
		const OmnString &lhs_fname, 
		const OmnString &rhs_fname)
{
	for (u32 i=0; i<entries.size(); i++)
	{
		if (entries[i].join_type == join_type)
		{
			entries[i].append(lhs_fname, rhs_fname);
		}
	}

	// Not exist. Need to add one
	entries.push_back(FieldEntry(join_type, lhs_fname, rhs_fname));
	return true;
}


bool
AosQueryFieldSimple::FieldEntry::append(
		const OmnString &lhs_fname, 
		const OmnString &rhs_fname)
{
	for (u32 i=0; i<pairs.size(); i++)
	{
		if (pairs[i].lhs_fname == lhs_fname &&
			pairs[i].rhs_fname == rhs_fname)
		{
			// The pair already exists. 
			return true;
		}
	}

	// The pair does not exist.
	pairs.push_back(FieldPair(lhs_fname, rhs_fname));
	return true;
}


