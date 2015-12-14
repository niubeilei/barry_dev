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
// This is a Term Template. When creating a new term, one should copy
// this file and modify it. 
//
// Modification History:
// 2013/08/12  Andy Zhang     
////////////////////////////////////////////////////////////////////////////
#include "Query/TermRunMysqlSelect.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IILSelector/IILSelector.h"
#include "SEUtil/IILName.h"
#include "QueryCond/CondNames.h"
#include "QueryUtil/QrUtil.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"
#include "API/AosApi.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "Thread/Mutex.h"
#include "Util/UtUtil.h" 
#include "Util/CodeConvertion.h" 
#include "MySQL/DataStoreMySQL.h"


AosTermRunMysqlSelect::AosTermRunMysqlSelect(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_RUNMYSQLSELECT, AosQueryType::eRunMysqlSelect, regflag),
mDataBasePort(0),
mNumOfField(0),
mCrtIdx(-1),
mNumValues(0)
{
}


AosTermRunMysqlSelect::AosTermRunMysqlSelect(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_RUNMYSQLSELECT, AosQueryType::eRunMysqlSelect, false),
mDataBasePort(0),
mNumOfField(0),
mCrtIdx(-1),
mNumValues(0)
{
	mIsGood = parse(def, rdata);
}


AosTermRunMysqlSelect::~AosTermRunMysqlSelect()
{
}


bool 	
AosTermRunMysqlSelect::nextDocid(
		const AosQueryTermObjPtr &parent_term,
		u64 &docid, 
		bool &finished, 
		const AosRundataPtr &rdata)
{
	// This function retrieves the next docid that is selected by this term.
	// What it does is to retrieve the data based on the condition, and then 
	// returns the next docid. Normally you do not need to override this function. 
	// Refer to AosQueryTerm::nextDocid(...) to see whether it is the one you
	// need. If yes, do not override it. Remove this function. 
	if (!mDataLoaded) loadData(rdata);

	finished = false;
	if (mNoMoreDocs)
	{
		docid = 0;
		finished = true;
		return true;
	}

	mCrtIdx++;
	if (mCrtIdx < 0 || mCrtIdx >= mNumValues) 
	{
		mNoMoreDocs = true;
		finished = true;
		docid = 0;
		return true;
	}

	docid = mCrtIdx;
	finished = false;
	return true;
}


bool
AosTermRunMysqlSelect::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermRunMysqlSelect::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
	if (query_rslt) 
	{
		query_rslt->appendDocid(0);
		query_rslt->reset();
	}
	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermRunMysqlSelect::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	OmnRslt rslt(true);
	AosXmlTagPtr record = AosXmlParser::parse("<record></record>" AosMemoryCheckerArgs);
	for(i64 j = 0; j < mNumOfField; j++)
	{
		OmnString value = mRecords[docid]->getStr(j, "", rslt);
//		aos_assert_r(rslt, 0);
//		u32 len = value.length();
//		AosBuffPtr buff = OmnNew AosBuff(len*2+2 AosMemoryCheckerArgs);

//		int64_t newlen = CodeConvertion::convert("gb2312", "utf-8", 
//				        value.getBuffer(), len, buff->data(), len*2+2);
//
//	    buff->setDataLen(newlen);
		if (j < (i64)mHeads.size())
		{
			record->setAttr(mHeads[j],value);
		}
		else
		{
			record->setAttr("zky_null", value);
		}
	}
	return record;
}


bool 	
AosTermRunMysqlSelect::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	
	mCrtIdx = pos;

	return true;
}


i64		
AosTermRunMysqlSelect::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	return mNumValues;
}


void	
AosTermRunMysqlSelect::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
	mNoMoreDocs = false;
	mCrtIdx = -1;
}

void
AosTermRunMysqlSelect::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermRunMysqlSelect::loadData(const AosRundataPtr &rdata)
{
	if (mDataLoaded) return true;
	OmnDataStoreMySQLPtr mySql = OmnNew OmnDataStoreMySQL(mUser, mPasswd, mIp, mDatabaseName, mDataBasePort, "");
	if (!mySql)
	{
		rdata->setError() << "Failed to retrieve the store!";
		return false;
	}

	OmnRslt rslt = mySql->openDb();
	if (!rslt)
	{
		rdata->setError()<< "Failed to retrieve the store!";
		return false;
	}

	if ( !(mySql->query(mSql, mRaw)))
	{
		rdata->setError()<< "Failed To Run SQL: " << mSql << enderr;
		return false;
	}

	while(mRaw->hasMore())
	{
		OmnDbRecordPtr record = mRaw->next();
		mRecords.push_back(record);
	}
	mNumValues = mRecords.size();

	rslt = mySql->closeDb();
	if (!rslt)
	{
		rdata->setError()<< "Failed to retrieve the store!";
		return false;
	}
	mDataLoaded = true;
	return true;
}



bool
AosTermRunMysqlSelect::parse(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// def looks like :
	//  <term zky_type="query||insert||delete"                           
	//       zky_user="xxxx" 
	//       zky_passwd="xxx"
	//       zky_database_ip="xxx"
	//       zky_database_prot="xxxx"
	//       zky_num_of_field="xx"
	//       zky_database_name="xxxx" >
	//      <head>xxxx,xxxx,xxxxx,xxxx,xxx,xxx</head>
	//      <sql><![CDATA[ select * from xxxxx]]></sql>
	//  </term>
	mUser = def->getAttrStr("zky_user");
	aos_assert_r(mUser != "", false);

	mPasswd = def->getAttrStr("zky_passwd");
	aos_assert_r(mPasswd != "", false);

	mDatabaseName = def->getAttrStr("zky_database_name");
	aos_assert_r(mDatabaseName != "", false);

	mIp = def->getAttrStr("zky_database_ip", "127.0.0.1");
	aos_assert_r(mIp != "", false);

	mDataBasePort = def->getAttrU32("zky_database_prot", 3306);

	mNumOfField = def->getAttrInt64("zky_num_of_field", 0);

	AosXmlTagPtr headnode = def->getFirstChild("head");
	aos_assert_r(headnode, false);

	OmnString head = headnode->getNodeText();
	AosStrSplit split(head, ",");
	mHeads = split.entriesV();
	AosXmlTagPtr sqlnode = def->getFirstChild("sql");
	aos_assert_r(sqlnode, false);

	mSql = sqlnode->getNodeText();
	aos_assert_r(mSql != "", false);

	mIsGood = true;
	return true;
}


AosQueryTermObjPtr
AosTermRunMysqlSelect::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermRunMysqlSelect(def, rdata);
}


