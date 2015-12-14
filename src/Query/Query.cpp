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
// 10/24/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/Query.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "UtilComm/TcpClient.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlInterface/WebRequest.h"

#if 0
const OmnString sgRecordHeader = "<records num=\"   \" idx=\"        \">";

bool AosProcQuery(
		const AosWebRequestPtr &req,
		OmnString &contents, 
		AosXmlRc &errcode, 
		OmnString &errmsg) 
{
	// The receited 'req' contains a query. This function
	// processes the query. The query should be in the form
	// 	<request>
	// 		<query uname="xxx" aname="xxx" psize="xxx" 
	// 			qtype="xxx">
	// 			<term>word1,word2,...,wordn</term>
	// 			<term>word1,word2,...,wordn</term>
	// 			...
	// 			<term>word1,word2,...,wordn</term>
	// 		</query>
	// 	</request>
	//
	errcode = eAosXmlInt_General;
	errmsg = "";

	// 1. Parse the document.
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(req->getDataBuff(), "");
	aos_assert_r(xml, false);
	AosXmlTagPtr child = xml->getFirstChild();
	aos_assert_r(child, false);

	// 2. Retrieve the query. 
	AosXmlTagPtr qq = child->getFirstChild("query");	
	if (!qq)
	{
		errmsg = "Missing the query tag!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	AosQuery query(child, qq, errmsg);
	if (!query.isGood()) return false;

	return query.process(contents, errmsg);
}


AosQuery::AosQuery(
		const AosXmlTagPtr &root, 
		const AosXmlTagPtr &query, 
		OmnString &errmsg)
{
	mIsGood = true;
	mRoot = root;
	mQuery = query;

	mPagesize = mQuery->getAttrInt64("psize", mDftPageSize);
	if (mPagesize > mMaxPageSize) mPagesize = mMaxPageSize;

	// 3. Retrieve the first term. In the current implementation
	// we only support one term (i.e., we do not support OR queries).
	AosXmlTagPtr term = mQuery->getFirstChild();
	if (!term)
	{
		errmsg = "Missing the keywords!";
		OmnAlarm << errmsg << enderr;
		mIsGood = false;
		return;
	}
	mTerms[0] = term;
	OmnGetTimeOfDay(&mStartTime);
}


AosQuery::~AosQuery()
{
	if (mIIL) mIIL->removeRefCount();
	for (i64 i=0; i<mNumIILs; i++)
	{
		mIILs[i]->removeRefCount();
		mSortedIILs[i]->removeRefCount();
	}
}

bool
AosQuery::process(
		OmnString &contents, 
		OmnString &errmsg)
{
	i64 textLen;
	char *data = (char *)mTerms[0]->getText(textLen);
	AosStrSplit splitter(data, textLen, ",", AosQuery::eMaxWords);
	if (splitter.entries() == 1)
	{
		mWords[0] = splitter.getEntry(0);
		return singleWordQuery(contents, errmsg);
	}
	else
	{
		setWords(splitter);
		return multiWordQuery(contents, errmsg);
	}
}


void
AosQuery::setWords(AosStrSplit &splitter)
{
	for (i64 i=0; i<splitter.entries(); i++)
	{
		mWords[i] = splitter[i];
	}
	mNumWords = splitter.entries();
}


bool
AosQuery::singleWordQuery(
		OmnString &contents, 
		OmnString &errmsg) 
{
	// This function is searches only one IIL. If the request
	// contains the tag named 'qsn', qsn should be in the form:
	// 	<request>
	// 		...
	// 		<item name='qsn' idx="nnn"/>>
	// 	</request>
	//
	mIIL = AosIILMgrObj::getIILMgr()->getIIL(mWords[0]);
	if (!mIIL)
	{
		// Did not find the IIL. 
		contents = "<records num=\"0\" total=\"0\"/>";
		return true;
	}

	// retrieve the docs from the IIL.
	i64 startIdx = mQuery->getAttrInt64("idx", 0);
	mNumDocsFound = 0;
	i64 idx = startIdx;
	if (idx < 0) idx = 0;
	while (mNumDocsFound < mPagesize && idx < mIIL->getNumDocs())
	{
		mDocsFound[mNumDocsFound++] = mIIL->getDocId(idx++);
	}

	// Construct the header, which should be in the form:
	// 	<records num="nnn" idx="iii" total="ttt" time="sss">
	// where 'nnn' is the number of records contained in this
	// response; 'iii' is the next start index; 'total'
	// is the total number of docs found; and 'time' is the
	// time used in processing this query (in ms).
	contents = "<records num=\"";
	contents << mNumDocsFound << "\" idx=\""
		<< idx << "\" total=\""
		<< mIIL->getTotalDocs() << " time=\"";
	i64 timeIdx = contents.length();
	contents << "ss.mmm\">";
	constructBody(contents, errmsg);
	
	setTime(contents, timeIdx);
	return true;
}


bool
AosQuery::setTime(OmnString &contents, const i64 &timeIdx)
{
	// Set the time. Time is set in the header, in the form:
	//		"ss.mmm"
	
	timeval tp;
	OmnGetTimeOfDay(&tp);
	i64 sec = tp.tv_sec - mStartTime.tv_sec;
	i64 msec = (tp.tv_usec - mStartTime.tv_usec)/1000;
	if (msec < 0)
	{
		sec--;
		msec += 1000;
	}
	char buff[10];
	sprintf(buff, "%2d.%3d", sec, msec);
	char *data = (char *)contents.data();
	for (i64 i=0; i<6; i++)
		data[timeIdx+i] = buff[i];
	return true;
}


bool
AosQuery::form2Body(
		OmnString &contents, 
		OmnString &errmsg) 
{
	// It is to retrieve the entire document.
	// 	<records num="xxx" idx="xxx">
	// 		<record><the document as an XML document></record>
	// 		<record><the document as an XML document></record>
	// 		...
	// 		<record><the document as an XML document></record>
	// 	</records>
	// It retrieves 
	u64 docId;
	AosXmlDoc doc;
	for (i64 i=0; i<mNumDocsFound; i++)
	{
		docId = mDocsFound[i];
		if (!doc.readDoc(docId))
		{
			OmnAlarm << "Failed reading the doc: " << docId << enderr;
			continue;
		}
		contents << "<record>";
		contents.append(doc.getData(), doc.getDataLength());
		contents << "</record>";
	}
	contents << "</records>";
	return true;
}


bool
AosQuery::form3Body(
		OmnString &contents, 
		OmnString &errmsg) 
{
	// It retrieves some of the fields from the documents. It is
	// like SQL select statements. The fields are specified
	// in the query:
	// 	<query fields="fname,rname,type,fname,rname,type,..." .../>
	//
	// where 'fname' is the name (including the path) of the field
	// to be retrieved; 'rname' is the name to be used in the response;
	// 'type' indicates whether the results should be expressed as 
	// attribute(1) or subtag(2). If it is attribute, the attribute
	// name is 'rname'. If it is a subtag, the subtag name is 'rname'.
	// Subtag is always escaped by CDATA.
	//
	// The response will be in the form:
	// 	<records num="xxx" idx="xxx">
	// 		<record attr="xxx" attr="xxx">
	// 			<tag><[!CDATA[xxx]]></tag>
	// 			<tag><[!CDATA[xxx]]></tag>
	// 			...
	// 		</record>
	// 		...
	// 	</records>

	// Retrieve the fields
	i64 len;
	char *fds = (char *)mQuery->getAttr("fields", len);
	if (!fds || len <= 0)
	{
		errmsg = "Failed to retrieve the field names!";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	AosStrSplit splitter(fds, len, ",", eMaxFields);
	OmnString fields[eMaxFields];
	OmnString rnames[eMaxFields];
	OmnString types[eMaxFields];
	i64 numFields = 0;
	i64 idx = 0;
	while (numFields < eMaxFields && idx < splitter.entries())
	{
		fields[numFields] = splitter.getEntry(idx++);
		rnames[numFields] = splitter.getEntry(idx++);
		types[numFields] = splitter.getEntry(idx++);
		numFields++;
	}

	// It retrieves 
	AosXmlTagPtr doc;
	char *cc; 
	i64 cclen;
	u64 docId;
	for (i64 i=0; i<mNumDocsFound; i++)
	{
		docId = mDocsFound[i];
		if (!(doc = AosDocServerSelf->getDoc(docId)))
		{
			OmnAlarm << "Failed to retrieve the thumbnail and name "
				<< "for the Doc: " << mIIL->getDocId(idx) << enderr;
			continue;
		}

		// first round, process attributes only
		contents << "<record";
		for (i64 i=0; i<numFields; i++)
		{
			char *type = (char *)types[i].data();
			if (type[0] == eAttr)
			{
				cc = (char *)doc->getAttr(fields[i], cclen);
				if (cc && cclen > 0)
				{
				 	contents << " " << rnames[i] << "\"";
					contents.append(cc, cclen);
					contents << "\"";
				}
			}
		}

		// Second round, process subtags
		contents << ">";
		for (i64 i=0; i<numFields; i++)
		{
			if (types[i].data()[0] == eSubtag)
			{
				cc = (char *)doc->getAttr(fields[i], cclen);
				if (cc && cclen > 0)
				{
					contents << "<" << rnames[i] << "><![CDATA[";
					contents.append(cc, cclen);
					contents << "]]></" << rnames[i] << ">";
				}
			}
		}

		contents << "</record>";
	}
	contents << "</records>";
	return true;
}


bool
AosQuery::multiWordQuery(
		OmnString &contents, 
		OmnString &errmsg) 
{
	// 'splitter' contains all the words. For each word, 
	// there are two IILs, one sorted logically and 
	// the other sorted based on DocIds. This function
	// loops over the number of words to match, starting
	// from all. For each loop, it then loops 
	// The query may contain the following information:
	// 	<query nw="numWords" idx="idx1:idx2:..."

	// 1. Retrieve the IILs
	mNumMatchedWords = mQuery->getAttrInt64("nmw", mNumWords);
	aos_assert_r(retrieveIILIdx(), false);
	aos_assert_r(retrieveIILs(), false);
	if (mNumIILs == 0)
	{
		// Nothing found for the query. 
		contents = "<records num=\"0\" total=\"0\"/>";
		return true;
	}
	
	mNumDocsFound = 0;

	// Retrieve mCrtIIL
	mCrtIIL = mQuery->getAttrInt64("crtiil", 0);
	if (mCrtIIL >= mNumIILs) mCrtIIL = 0;

	u64 docId;
	for (; mNumMatchedWords>0; mNumMatchedWords--)
	{
		// Loop for the docs that match 'numWords'. 	
		while ((docId = getNextDoc()) != AOS_INVDID)
		{
			mDocsFound[mNumDocsFound++] = docId;
			if (mNumDocsFound >= mPagesize) break;
		}
		if (mNumDocsFound >= mPagesize) break;

		// Finished the search that matches 'mNumMatchedWords'. 
		// The next loop will decrement it by one. Need to 
		// reset mIILIdx[]. 

		for (i64 i=0; i<mNumIILs; i++) mIILIdx[i] = 0;
	}

	// Ready to send the response. First construct the response
	// header, which should be in the form:
	// 	<records num="nnn" total="ttt" time="sss"
	//		nmw="mmm"
	//		crtiil="xxx"
	//		idx="nn:nn:..."
	//		...>
	contents = "<records num=\"";
	contents << mNumDocsFound << " total=\""
		<< mTotalDocs << " time=\"";
	i64 timeIdx = contents.length();
	contents << "ss.mmm\" nmw=\"" << mNumMatchedWords 
		<< " crtiil=\"" << mCrtIIL 
		<< " idx=\"";

	// Construct the idx
	for (i64 i=0; i<mNumIILs; i++) 
	{
		if (i != 0) contents << ":";
		contents << mIILIdx[i];
	}
	contents << "\"";
	
	constructBody(contents, errmsg);

	// Modifies the header
	setTime(contents, timeIdx);
	return true;
}


u64 
AosQuery::getNextDoc()
{
	// This function finds the next word that matches 
	// 'mNumMatchedWords'. It should use 'mCrtIIL' to search 
	// for the one that matches 'mNumMatchedWords'. 
	AosIILPtr IIL = mIILs[mCrtIIL];
	u64 docId;
	i64 idx = mIILIdx[mCrtIIL];
	while ((docId = IIL->getDocId(idx++)) != AOS_INVDID)
	{
		// Need to ensure the DocId exists in mNumMatchedWords
		// number of other IILs. 
		i64 found = 0;
		for (i64 i=0; i<mNumIILs; i++)
		{
			if (i == mCrtIIL) continue;
			if (mSortedIILs[i]->docExist(docId)) 
			{
				found++;
				if (found == mNumMatchedWords) return docId;
			}
		}
	}

	// This means it could not find any more Docs. 
	return AOS_INVDID;
}


bool
AosQuery::retrieveIILs()
{
	// It retrieves all the IILs for 'mWords[]'. Note that
	// IILs may be loaded in pages. The pages may be specified
	// by the query.
	mNumIILs = 0;
	mTotalDocs = 0;
	/*
	for (i64 i=0; i<mNumWords; i++)
	{
		AosIILPtr IIL = AosIILMgrObj::getIILMgr()->getIIL(mWords[i]);
		if (IIL)
		{
			mIILs[mNumIILs] = IIL;
			mTotalDocs += IIL->getTotalDocs();
			mSortedIILs[mNumIILs] = AosDocServerSelf->
				getSortedIIL(mWords[i]);
			mWords[mNumIILs] = mWords[i];
			mNumIILs++;
		}
	}
	*/

	if (mNumMatchedWords > mNumIILs) mNumMatchedWords = mNumIILs;
	return true;
}


bool
AosQuery::retrieveIILIdx()
{
	for (i64 i=0; i<mNumWords; i++) mIILIdx[i] = 0;

	i64 len;
	char *xx = (char *)mQuery->getAttr("idx", len);
	if (xx)
	{
		// xx contains a number of integers. We use it to 
		// initialize the 'mIILIdx[]'.
		AosStrSplit sp(xx, len, ":", mNumWords);
		for (i64 i=0; i<sp.entries(); i++)
		{
			mIILIdx[i] = atoi(sp[i]);
			if (mIILIdx[i] < 0) mIILIdx[i] = 0;
		}
	}
	return true;
}


bool
AosQuery::constructBody(
		OmnString &contents, 
		OmnString &errmsg) 
{
	// The docs found are stored in mDocsFound. The number of
	// docs found is mNumDocsFound. This function prepares
	// the response. 
	// Found the IIL. Return one page
	char qtype = mQuery->getAttrChar("qtype", 'n');
	switch (qtype)
	{
	case 'a':
		 return form1Body(contents, errmsg);

	case 'f':
		 return form2Body(contents, errmsg);

	case 'n':
	default:
		 return form3Body(contents, errmsg);
	}	

	OmnShouldNeverComeHere;
	return false;
}


bool
AosQuery::form1Body(
		OmnString &contents, 
		OmnString &errmsg) 
{
	// It is to retrieve some attributes from each matched
	// doc and return the results in the form:
	// 	<records num="xxx">
	// 		<record dataid="xxx" tnail="xxx" name="xxx"/>
	// 		...
	// 		<record dataid="xxx" tnail="xxx" name="xxx"/>
	// 	</records>
	//
	// 	The system already ensures that there are no special
	// 	characters in 'dataid' and 'tnail'. This function will
	// 	ensure 'name' does not contain special characters.
	AosXmlDoc doc;
	u64 docId;
	for (i64 i=0; i<mNumDocsFound; i++)
	{
		docId = mDocsFound[i];	
		if (!doc.readDoc(docId))
		{
			OmnAlarm << "Failed to retrieve the Doc Header "
				<< "for the Doc: " << docId << enderr;
			continue;
		}
		contents << "<record dataid=\"" << docId << "\" tnail=\""
			<< doc.getDocName()
			<< "\" name=\""
			<< doc.getThmnail()
			<< "\"/>";
	}
	contents << "</records>";

	return true;
}

#endif
