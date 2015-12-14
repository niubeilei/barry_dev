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
// 04/16/2013	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#include "Query/TermBatchQuery.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Query/TermOr.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Util/BuffArray.h"
#include "UtilTime/TimeUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlParser.h"


AosTermBatchQuery::AosTermBatchQuery(const bool regflag)
:
AosTermIILType(AOSTERMTYPE_BATCH_QUERY, AosQueryType::eBatchQuery, regflag),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(0),
mDftValue(0)
{
}


AosTermBatchQuery::AosTermBatchQuery(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosTermIILType(AOSTERMTYPE_BATCH_QUERY, AosQueryType::eBatchQuery, false),
mNumValues(0),
mDataLoaded(false),
mCrtIdx(0),
mDftValue(0),
mBuff(OmnNew AosBuff(AosMemoryCheckerArgsBegin))
{
	mIsGood = parse(def, rdata);
	if (!mIsGood) return;
}


AosTermBatchQuery::~AosTermBatchQuery()
{
}


bool 	
AosTermBatchQuery::nextDocid(
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
	finished = false;
	if (mNoMoreDocs)
	{
		docid = 0;
		finished = true;
		return true;
	}

	if (!mDataLoaded)
	{
		if(!loadData(rdata)) mNumValues=0;
		finished = true;
		docid = 0;
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
	finished = false;
	return true;
}


bool
AosTermBatchQuery::checkDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	// This function checks whether the docid 'docid' is selected by this term.
	// There is a default implementation (refer to AosQueryTerm::checkDocid(docid, rdata)).
	// If that implementation is what you need, do not override the function. 
	// Simply remove this function.
	OmnNotImplementedYet;
	return false;
}


bool 	
AosTermBatchQuery::getDocidsFromIIL(
		const AosQueryRsltObjPtr &query_rslt, 
		const AosBitmapObjPtr &bitmap,
		const AosQueryContextObjPtr &query_context,
		const AosRundataPtr &rdata)
{
    if (!mDataLoaded) loadData(rdata);
	if (query_rslt)
	{
		query_rslt->setNumDocs(mNumValues);
		query_rslt->reset();
	}

	if(query_context)
	{
		query_context->setFinished(true);
	}
	return true;
}


AosXmlTagPtr
AosTermBatchQuery::getDoc(const u64 &docid, const AosRundataPtr &rdata) 
{
	// This function creates a record. the record is:
	// <records>
	// 		<record />
	// 		...
	// 		<record />
	// </records>
	 
	return  createRecord(mCrtIdx, rdata);
}

AosXmlTagPtr
AosTermBatchQuery::createRecord(const i64 &index, const AosRundataPtr &rdata)
{
	OmnString record = "<record ";
//test start
/*
mBuff->reset();
for(i64 i=0; i<21; i++)
{
	u32 length = mBuff->getU32(0);
	u64 docid = mBuff->getU64(0);
	OmnString s;
	mBuff->getOmnStr(s);
	u64 d = mBuff->getU64(0);
OmnScreen << "length:" << length << " docid:" << docid << " string:" << s << " d:" << d << endl;
}
*/
//test end

	//u32 length = mBuff->getU32(0);//remove length
//	u64 docid = mBuff->getU64(0);//remove docid
//OmnScreen << "length : " << length << endl;
//OmnScreen << "docid : " << docid << endl;
	for(u32 i=0; i<mFieldDef.size(); i++)
	{
		OmnString value;
		bool f = getOneFieldValue(value, rdata);
		if(!f)
		{
		 	OmnAlarm << "failed to get the field value :" << mFieldDef[i].oname << enderr;
			rdata->setError() << "failed to get the field value: " << mFieldDef[i].oname;
			return NULL;
		}
		record << " " << mFieldDef[i].oname << "=\"" << value << "\"";
OmnScreen << "fieldname : " << mFieldDef[i].oname << " value:" << value  << endl;
		
	}

	record << "/>";
OmnScreen << "record : " << record << endl;
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(record, "" AosMemoryCheckerArgs);
	return xml;
}

bool 	
AosTermBatchQuery::moveTo(const i64 &pos, const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function moves the position
	// to 'pos' in the array. If pos is bigger than the size of the array, 
	// it returns false. Otherwise, it loads the data as needed, and changes
	// the position to 'pos'. 
	//
	// This is the default implementation (check AosQueryTerm::moveTo(pos, rdata).
	// If this is what you need, do not override this function.
	OmnNotImplementedYet;
	return false;
}


i64		
AosTermBatchQuery::getTotal(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids. This function returns the
	// size of the array. If this is what you need (refer to AosQueryTerm::getTotal(rdata),
	// do not override this member function.
	if (!mDataLoaded)
	{
		if(!loadData(rdata)) mNumValues=0;
	}
	return mNumValues;
}


void	
AosTermBatchQuery::reset(const AosRundataPtr &rdata)
{
	// Each condition selects an array of docids, and a position pointer that
	// is used to indicate which element the term is currently at. 
	// This function resets the position pointer to the beginning of the array.
	// If this is what you need (refer to AosQueryTerm::reset()), do not override
	// this member function.
	mNoMoreDocs = false;
	mCrtIdx = 0;
}

void
AosTermBatchQuery::toString(OmnString &str)
{
	// This function converts the term into an XML.
}


bool
AosTermBatchQuery::loadData(const AosRundataPtr &rdata)
{
	aos_assert_r(mStartIdx >=0, false);
	aos_assert_r(mSize >0, false);
	if (mDataLoaded) return true;

	mDataLoaded = true;
	const bool force_retrieve = false;
	AosXmlTagPtr doc = AosRetrieveDocByObjid(mQueryObjid, force_retrieve, rdata);
	aos_assert_r(doc, false);
	AosXmlTagPtr docids = doc->getFirstChild("docids");
	aos_assert_r(docids, false);
	i64 totalnums = doc->getAttrInt64("exist_docs_all", -1);	
	aos_assert_r(totalnums>0, false);

	AosXmlTagPtr fnames = doc->xpathGetChild("query/fnames");
	aos_assert_r(fnames, false);
	bool rslt = AosQrUtil::parseFnames(rdata, fnames, mFieldDef);
	aos_assert_rr(rslt, rdata, NULL);

	if(mStartIdx+mSize>=totalnums)
	{
		//config modified
OmnScreen << "*********config modified******** size : " << mSize << endl;
		mSize = totalnums-mStartIdx;
		mNumValues = totalnums; 
		mEndIdx = totalnums-1;
	}
	else
	{
		mNumValues = totalnums; 
		mEndIdx = mStartIdx+mSize-1;
	}

	i64 index = 0;
	i64 blocknum = 0;
	i64 blockstart = 0;
	i64 blockend = 0;
	while(1)
	{
		OmnString value;
		value << blocknum;
		AosXmlTagPtr dchild = docids->getChildByAttr("index", value);
		if(!dchild)
		{
			OmnAlarm << "Never Happen!!!!!" << enderr;
			return false;
		}
		i64 existdocs = dchild->getAttrInt64("existdocs", -1);
		aos_assert_r(existdocs>=0, false);
		blockstart = index;
		blockend = index+existdocs-1;
		if(blockend < mStartIdx)
		{
			index += existdocs;
			blocknum++;
			continue;
		}
		else if(blockstart<=mStartIdx && blockend >= mEndIdx)
		{
			//get only one block
			u64 docid = dchild->getNodeText().toU64();
			aos_assert_r(docid>0, false);
			AosBuffPtr tmpbuff;
			AosXmlTagPtr tmpdoc = AosRetrieveBinaryDoc(docid, tmpbuff, rdata);
			aos_assert_r(tmpdoc, false);
			AosBuffPtr body = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
			body->setBuff(tmpbuff->data()+sizeof(u32)*existdocs, tmpbuff->dataLen()-sizeof(u32)*existdocs);
			i64 offset1 = (mStartIdx-index)*sizeof(u32);
			if(offset1<0)
			{
				OmnAlarm << "offset error:" << offset1 << enderr;
				return false;
			}
			offset1 = (mStartIdx-index)*sizeof(u32);
			tmpbuff->reset();
			i64 start = tmpbuff->getU64(offset1, 0); 
			i64 offset2 = (mStartIdx+mSize-index-1)*sizeof(u32);
			if(offset2<0)
			{
				OmnAlarm << "offset error:" << offset2 << enderr;
				return false;
			}
			offset2 = (mEndIdx-index)*sizeof(u32);
			tmpbuff->reset();
			i64 end = tmpbuff->getU64(offset2, 0); 
			tmpbuff->reset();
			body->reset();
			u64 end_length = body->getU64(end, 0) + sizeof(u64); 
			if(mBuff->dataLen()+end-start+end_length >= eMaxSize)
			{
				OmnAlarm << "it is too big!:" << mBuff->dataLen()+end-start+end_length<< enderr;
				break;
			}
			mBuff->setBuff(body->data()+start, end-start+end_length);
			break;
		}
		else if(blockstart<=mStartIdx && blockend < mEndIdx)
		{
			//get first block
			u64 docid = dchild->getNodeText().toU64();
			aos_assert_r(docid>0, false);
			AosBuffPtr tmpbuff;
			AosXmlTagPtr tmpdoc = AosRetrieveBinaryDoc(docid, tmpbuff, rdata);
			aos_assert_r(tmpdoc, false);
			AosBuffPtr body = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
			body->setBuff(tmpbuff->data()+sizeof(u64)*existdocs, tmpbuff->dataLen()-sizeof(u64)*existdocs);
			body->reset();
			int offset = (mStartIdx-index)*sizeof(u64);
			if(offset<0)
			{
				OmnAlarm << "offset error:" << offset << enderr;
				return false;
			}
			offset = ((mStartIdx-index)*sizeof(u32));
			tmpbuff->reset();
			int start = tmpbuff->getU64(offset, 0); 
			if(mBuff->dataLen()+body->dataLen()-start >= eMaxSize)
			{
				OmnAlarm << "it is too big!:" << mBuff->dataLen()+body->dataLen()-start << enderr;
				break;
			}
			mBuff->setBuff(body->data()+start, body->dataLen()-start);
		}
		else if(blockstart>mStartIdx && blockend<mEndIdx)
		{
			//get middle block
			u64 docid = dchild->getNodeText().toU64();
			aos_assert_r(docid>0, false);
			AosBuffPtr tmpbuff;
			AosXmlTagPtr tmpdoc = AosRetrieveBinaryDoc(docid, tmpbuff, rdata);
			aos_assert_r(tmpdoc, false);
			AosBuffPtr body = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
			body->setBuff(tmpbuff->data()+sizeof(u32)*existdocs, tmpbuff->dataLen()-sizeof(u32)*existdocs);
			body->reset();
			if(mBuff->dataLen()+body->dataLen()>= eMaxSize)
			{
				OmnAlarm << "it is too big!:" << mBuff->dataLen()+body->dataLen()<< enderr;
				break;
			}
			mBuff->setBuff(body->data(), body->dataLen());
		}
		else if(blockstart>mStartIdx && blockend>=mEndIdx)
		{
			//get last block
			u64 docid = dchild->getNodeText().toU64();
			aos_assert_r(docid>0, false);
			AosBuffPtr tmpbuff;
			AosXmlTagPtr tmpdoc = AosRetrieveBinaryDoc(docid, tmpbuff, rdata);
			aos_assert_r(tmpdoc, false);
			AosBuffPtr body = OmnNew AosBuff(2000 AosMemoryCheckerArgs);
			body->setBuff(tmpbuff->data()+sizeof(u32)*existdocs, tmpbuff->dataLen()-sizeof(u32)*existdocs);
			body->reset();
			i64 offset = (mEndIdx-index)*sizeof(u32);
			if(offset<0)
			{
				OmnAlarm << "offset error:" << offset << enderr;
				return false;
			}
			offset = (mEndIdx-index)*sizeof(u32);
			tmpbuff->reset();
			i64 end = tmpbuff->getU64(offset, 0); 
			tmpbuff->reset();
			u64 end_length = body->getU64(end, 0) + sizeof(u64); 
			if(mBuff->dataLen()+end+end_length >= eMaxSize)
			{
				OmnAlarm << "it is too big!:" << mBuff->dataLen()+end+end_length<< enderr;
				break;
			}
			mBuff->setBuff(body->data(), end+end_length);
			break;
		}
		else
		{
			//Never Happen
			OmnAlarm << "Never Happen!" << enderr;
			return false;
		}
		index += existdocs;
		blocknum++;
	}
	mBuff->reset();

	return true;
}

bool
AosTermBatchQuery::getOneFieldValue(OmnString &value, const AosRundataPtr &rdata)
{
	OmnString s;
	mBuff->getOmnStr(s);
	value << s;
	return true;
}

bool
AosTermBatchQuery::parse(const AosXmlTagPtr &term, const AosRundataPtr &rdata)
{
	// XML definition of the term and the 'def' should be in fellow form:
	// <term type="batchquery" queryobjid="xxx" startidx="xxx" size="xxx" order="true|false" reverse="true|false">
	//  </term>
	
	aos_assert_r(rdata, false);
	aos_assert_rr(term, rdata, false);

	mQueryObjid = term->getAttrStr("queryid", "");
	aos_assert_r(mQueryObjid != "",false) 
	
	return true;
	
}

AosQueryTermObjPtr
AosTermBatchQuery::clone(const AosXmlTagPtr &def, 
						 const AosRundataPtr &rdata)
{
	return OmnNew AosTermBatchQuery(def, rdata);
}


