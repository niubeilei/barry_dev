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
//
// Modification History:
// 2009/10/19	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/Testers/DocServerTester.h"

#include "alarm_c/alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGen.h"
#include "Porting/Sleep.h"
#include "SearchEngine/WordIdHash.h"
#include "SearchEngine/IIL.h"
#include "SearchEngine/IILDocid.h"
#include "SearchEngine/IILMgr.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "SearchEngine/WordMgr.h"
#include "SearchEngine/DocServer.h"
#include "SearchEngine/Testers/Ptrs.h"
#include "SearchEngine/Testers/TestXmlDoc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"



AosDocServerTester::AosDocServerTester()
{
	mName = "Tester";
	mNumDocs = 0;
	mNumDocsSaved = 0;
	memset(mDocIds, 0, sizeof(mDocIds));
	memset(mWordIds, 0, sizeof(mWordIds));
}


bool AosDocServerTester::start()
{
	cout << "    Start Tester ...";
	mDataDirname = "/AOS/Data";
	mWordIdHashTsize = 100000;
	prepareEnv();
	torturer();
	return true;
}


bool 
AosDocServerTester::prepareEnv()
{
    return true;
}


bool 
AosDocServerTester::basicTest()
{
	/*
	OmnString data = 
		"<request>"
    		"<item name='username'><![CDATA[chending]]></item>"
    		"<item name='appname'><![CDATA[testapp]]></item>"
    		"<xmlobj>"
				"<pinfo firstname='Chen' lastname='Ding'/>"
			"</xmlobj>"
		"</request>";

	OmnString data1 = "<p fname='chen'><![CDATA[wwww1 hhh2]]></p>";
	OmnConnBuffPtr buff = OmnNew OmnConnBuff(data1.data(), data1.length());
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(buff, "");
	xml->resetGetWords();
	u8 *bbb;
	u32 wl;
	while ((bbb = xml->nextWord(wl)))
	{
		char ppp[100];
		strcpy(ppp, (char *)bbb);
		cout << "Found: " << ppp << ":" << wl<< endl;
	}

	// OmnConnBuffPtr buff = OmnNew OmnConnBuff(data.data(), data.length());
	AosWebRequestPtr req = OmnNew AosWebRequest(0, buff);

	OmnString contents;
	AosXmlRc errcode;
	OmnString errmsg;
	AosXmlDocPtr header;
	AosXmlDocPtr headerptr(&header, false);
	AosDocServerCbPtr thisPtr(this, false);
	AosDocServerSelf->createDoc(req, contents, errcode, errmsg,
		headerptr, thisPtr, 0);

	while (AosDocServerSelf->getNumRequests() > 0)
	{
		OmnSleep(1);
	}

	OmnSleep(1);

	// It should have the following words:
	//	xmlobj, pinfo, firstname, Chen, lastname, Ding
	u32 wordLen;
	u64 wordId = AosWordMgrSelf->getWordId((u8 *)"pinfo", false);
	char *word = AosWordMgrSelf->getWord(wordId, wordLen);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;
	OmnCreateTc << (word != 0 && wordLen == strlen("pinfo")) << endtc;
	
	wordId = AosWordMgrSelf->getWordId((u8 *)"xmlobj", false);
	word = AosWordMgrSelf->getWord(wordId, wordLen);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;
	OmnCreateTc << (word != 0 && wordLen == strlen("xmlobj")) << endtc;
	
	wordId = AosWordMgrSelf->getWordId((u8 *)"firstname", false);
	word = AosWordMgrSelf->getWord(wordId, wordLen);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;
	OmnCreateTc << (word != 0 && wordLen == strlen("firstname")) << endtc;
	
	wordId = AosWordMgrSelf->getWordId((u8 *)"Chen", false);
	word = AosWordMgrSelf->getWord(wordId, wordLen);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;
	OmnCreateTc << (word != 0 && wordLen == strlen("Chen")) << endtc;
	
	wordId = AosWordMgrSelf->getWordId((u8 *)"lastname", false);
	word = AosWordMgrSelf->getWord(wordId, wordLen);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;
	OmnCreateTc << (word != 0 && wordLen == strlen("lastname")) << endtc;
	
	wordId = AosWordMgrSelf->getWordId((u8 *)"Ding", false);
	word = AosWordMgrSelf->getWord(wordId, wordLen);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;
	OmnCreateTc << (word != 0 && wordLen == strlen("Ding")) << endtc;
	*/
	
	return true;
}


bool 
AosDocServerTester::torturerXmlDocCreation()
{
	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 100;
	for (int i=0; i<tries; i++)
	{
		if (i % 100 == 0) cout << "Trying: " << i << endl;
		int depth = rand() % eDepth;
		AosTestXmlDocPtr doc = OmnNew AosTestXmlDoc(depth);
		aos_assert_r(doc, false);
		OmnCreateTc << (saveDoc(doc, 0)) << endtc;
	}
	return true;
}


bool 
AosDocServerTester::torturer()
{
	/*
	// for (int i=0; i<eMaxDocs; i++)
	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 100;

	mStartTime = OmnTime::getCrtSecond();
	mTotalCreated = 0;
	mTotalProcessed = 0;

	for (int i=0; i<tries; i++)
	{
		// Add a new doc
		int depth = rand() % eDepth;
		AosTestXmlDocPtr doc = OmnNew AosTestXmlDoc(depth);
		aos_assert_r(doc, false);
		mTotalCreated++;

		// Need to construct a request, which is in the form:
		//	<request>
    	//		<item name='username'>chending</item>
    	//		<item name='appname'>testapp</item>
		//		<xmlobj>
		//			the xml generate above
		//		</xmlobj>
		//	</request>

		// Find a slot to store the doc
		int idx = -1;

		while (idx == -1)
		{
			for (u32 k=0; k<eMaxDocs; k++)
			{
				if (!mDocs[k])
				{
					mDocs[k] = doc;
					idx = k;
					break;
				}
			}

			if (idx == -1)
			{
				OmnSleep(1);
			}
		}

		// Convert the generated XML into a string
		OmnString docstr;
		doc->composeData(docstr);

		// Add the header to the generated xml
		OmnString hh = 
			"<request><cmd><item name='zky_uname'>chending";
		hh << i << "</item><item name='appname'>testapp"
			<< i << "</item><item name='zky_siteid'>site_"
			<< i << "</item><item name='containers'>container_"
			<< i << "</item>"
			<< "</cmd><xmlobj>";
		OmnString tail = "</xmlobj></request>";

		// Construct the buff
		int totalLen = hh.length() + docstr.length() + tail.length();
		OmnConnBuffPtr buff = OmnNew OmnConnBuff(totalLen + 10);
		char *data = buff->getBuffer();
		int headerlen = hh.length();
		memcpy(data, hh.data(), headerlen);
		int doclen = docstr.length();
		memcpy(&data[headerlen], docstr.data(), doclen);
		int taillen = tail.length();
		memcpy(&data[headerlen + doclen], tail.data(), taillen);
		data[totalLen] = 0;
		buff->setDataLength(totalLen);

		// Add metadata
		addMetadata(buff);
		// Create the request
		AosWebRequestPtr req = OmnNew AosWebRequest(0, buff);

		// Create the document
		AosDocServerCbPtr thisPtr(this, false);
		AosXmlRc errcode;
		OmnString errmsg;
		AosXmlDocPtr header;
		AosXmlParser parser;
		AosXmlTagPtr root = parser.parse(req->getDataBuff(), "");
		AosXmlTagPtr child = root->getFirstChild();
		aos_assert_r(child, false);
		AosDocServerSelf->createDoc(req, child, header, 
			thisPtr, (void *)idx, errcode, errmsg);
		OmnCreateTc << (errcode == eAosXmlInt_Ok) << endtc;
		OmnCreateTc << (header) << endtc;
		mDocIds[idx] = header->getDocid();
		saveHeader(header);

		OmnString username = "chending";
		username << i;
		OmnCreateTc << (header->getModifier() == username) << endtc;
		OmnCreateTc << (header->getCreator() == username) << endtc;
		OmnCreateTc << (header->getDocName() == mDocname) << endtc;
		OmnCreateTc << (header->getThmnail() == mThmnail) << endtc;
		OmnCreateTc << (header->getDocType() == mDoctype) << endtc;
		OmnCreateTc << (header->getSubType() == mSubtype) << endtc;
		OmnCreateTc << (header->getAppname() == mAppname) << endtc;

		// Verify the environment
		verify();
	}

	OmnScreen << "Finished!" << endl;
	while (mTotalProcessed < mTotalCreated)
	{
		OmnSleep(10);
		u64 tt = OmnTime::getCrtSecond();
		cout << "Statistics: " << tt - mStartTime 
			<< ":" << AosWordMgrSelf->getTotalWords()
			<< ":" << AosWordMgrSelf->getWordHashSize()
			<< ":" << AosWordMgrSelf->getMaxConflictNum()
			<< ":" << AosWordMgrSelf->getMaxNumElems()
			<< ":" << AosWordMgrSelf->getMaxBucketLen() << endl;
	}
	*/

	return true;
}


void
AosDocServerTester::procFinished(
	const bool status, 
	const u64 &docId,
	const OmnString &errmsg, 
	const void *userdata, 
	const int numWords)
{
	int idx = (int)userdata;
	u64 tt = OmnTime::getCrtSecond();
	mTotalProcessed++;
	cout << "Total: " << mTotalProcessed
		<< ":" << mTotalCreated << ":" << numWords 
		<< " tm: " << tt - mStartTime 
		<< " tw: " << AosWordMgrSelf->getTotalWords()
		<< " hsz: " << AosWordMgrSelf->getWordHashSize()
		<< " cnum: " << AosWordMgrSelf->getMaxConflictNum()
		<< " mlen: " << AosWordMgrSelf->getMaxNumElems()
		<< " mbklen: " << AosWordMgrSelf->getMaxBucketLen() << endl;

	OmnCreateTc << (status) << "Failed to process the document: "
		<< idx << endtc;
	aos_assert(idx >= 0 && (u32)idx < eMaxDocs);

	OmnCreateTc << (mDocs[idx]) << endtc;
	saveDoc(mDocs[idx], docId);
	mDocs[idx] = 0;
}


bool
AosDocServerTester::saveDoc(
		const AosTestXmlDocPtr &doc, 
		const u64 &docId)
{
	// The original doc is stored in two versions. One is all the
	// words contained in the doc and the other is the doc itself. 
	// The words are stored in 'XmlDocWords' files, and the original
	// doc is stored in '/XmlRawDoc' files. 
	
	// 1. Store the words
	if (!mCrtDocFile)
	{
		OmnString fname = mDataDirname;
		fname << "/XmlDocWords_0";
		mCrtDocFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mCrtDocFile, false);
		mCrtFileIdx = 0;
		mCrtFilesize = 0;
	}

	if (mCrtFilesize >= eMaxFileSize)
	{
		mCrtFileIdx++;
		OmnString fname = mDataDirname;
		fname << "/XmlDocWords_" << mCrtFileIdx;
		mCrtDocFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mCrtDocFile, false);
		mCrtFilesize = 0;
	}

	// Write all words into the doc. The file is in the format:
	// 		total words:	four bytes
	//		word length:	four bytes	(first word)
	//		word:			vary
	//		word length:	four bytes	(second word)
	//		word:			vary
	//		...
	int startPos = mCrtFilesize;
	int numWords = mAllWords.entries();
	mCrtDocFile->append((char *)&numWords, 4);
	mCrtFilesize += 4;
	mAllWords.clear();
	doc->getAllWords(mAllWords);
	int totalWords = mAllWords.entries();
	for (int i=0; i<totalWords; i++)
	{
		OmnString str = mAllWords[i];
		int len = str.length();
		aos_assert_r((u32)len < eMaxWordLen, false);
		mCrtDocFile->append((char *)&len, 4);
		mCrtDocFile->append(str.data(), len, i == totalWords-1);
		mCrtFilesize += 4 + len;
	}

	// Store the index record into the database
	OmnString stmt = "insert into testdocindex (seqno, docid, fileno, offset) values (";
	stmt << mNumDocsSaved << ", "
		<< docId << ", " 
		<< mCrtFileIdx << ", " 
		<< startPos << ")";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store->runSQL(stmt), false);	

	// 2. Store the raw doc
	// 1. Store the words
	if (!mCrtRawDocFile)
	{
		OmnString fname = mDataDirname;
		fname << "/XmlRawDoc_0";
		mCrtRawDocFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mCrtRawDocFile, false);
		mCrtRawFileIdx = 0;
		mCrtRawFilesize = 0;
	}

	if (mCrtRawFilesize >= eMaxFileSize)
	{
		mCrtRawFileIdx++;
		OmnString fname = mDataDirname;
		fname << "/XmlRawDoc_" << mCrtRawFileIdx;
		mCrtRawDocFile = OmnNew OmnFile(fname, OmnFile::eCreate);
		aos_assert_r(mCrtRawDocFile, false);
		mCrtRawFilesize = 0;
	}

	// The format is: length (four bytes) + the data.
	
	startPos = mCrtRawFilesize;
	OmnString docstr;
	doc->composeData(docstr);
	int len = docstr.length();
	mCrtRawDocFile->append((char *)&len, 4);
	mCrtRawDocFile->append(docstr.data(), len, true);
	stmt = "insert into testrawdocindex (seqno, docid, fileno, offset) values (";
	stmt << mNumDocsSaved << ", "
		<< docId << ", " 
		<< mCrtRawFileIdx << ", " 
		<< startPos << ")";
	aos_assert_r(store->runSQL(stmt), false);	

	mNumDocsSaved++;
	return true;
}


bool
AosDocServerTester::verify()
{
	/*
	if (mNumDocsSaved <= 0) return true;

	int numVerifies = rand() % eNumVerifies;
	for (int i=0; i<numVerifies; i++)
	{
		// Determine which doc to verify
		int idx = rand() % mNumDocsSaved;
		OmnCreateTc << (verifyOneDoc(idx)) << endtc;
	}
	*/
	return true;
}


bool
AosDocServerTester::verifyOneDoc(const int idx)
{
	// Read in the index record
	OmnString stmt = "select fileno, offset, docid from testdocindex where seqno=";
	stmt << idx;
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnDbRecordPtr record;
	aos_assert_r(store->query(stmt, record), false);	
	aos_assert_r(record, false);
	OmnRslt rslt;
	int fileno = record->getInt(0, -1, rslt);
	int offset = record->getInt(1, -1, rslt);
	mDocid = record->getU64(1, AOS_INVDID, rslt);
	OmnCreateTc << (fileno >= 0 && offset >= 0) << endtc;
	OmnCreateTc << (mDocid != AOS_INVDID) << endtc;

	// Open the "XmlDocs_" file
	OmnString fname = mDataDirname;
	fname << "/XmlDocs_" << mCrtFileIdx;
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	aos_assert_r(ff && ff->isGood(), false);
	aos_assert_r(ff->seek(offset), false);
	int numWords;
	int bytesRead = ff->readToBuff(offset, 4, (char *)&numWords);
	aos_assert_r(bytesRead == 4, false);
	aos_assert_r(numWords >= 0, false);
	offset += 4;
	char buff[eMaxWordLen];
	mAllWords.clear();
	for (int i=0; i<numWords; i++)
	{
		int len;
		bytesRead = ff->readToBuff(offset, 4, (char *)&len);
		aos_assert_r(bytesRead == 4, false);
		aos_assert_r(len >= 0, false);
		offset += 4;
		bytesRead = ff->readToBuff(offset, len, buff);
		aos_assert_r(bytesRead == len, false);
		offset += len;
		mAllWords.append(OmnString(buff, len));
	}

	OmnCreateTc << (verifyAllWords()) << endtc;
	OmnCreateTc << (verifyDoc(mDocid)) << endtc;
	OmnCreateTc << (verifyRawDoc(mDocid)) << endtc;
	return true;
}


bool
AosDocServerTester::verifyAllWords()
{
	// All the words in mAllWords that are not ignored
	// should be in the system. Note that words need to be
	// normalized before checking. 
	//
	// Issues: if a word is ignored, it will not be checked.
	// Whether a word is ignored or not is determined by
	// the DocServer. If the DocServer has flaws in determining
	// the ignored words, this torturer may fail. Need to 
	// have a torturer to verify ignoring words is done
	// correctly. 
	//
	// There is the same issue about normalizing the words. 
	//
	
	/*
	// 1. Either a word is ignored or it should be in 
	//    the system.
	OmnString word;
	for (u32 i=0; i<(u32)mAllWords.entries(); i++)
	{
		word = mAllWords[i];
		bool isIgnored = false;
		u64 wordId = AosDocServerSelf->checkWord(word, isIgnored);
		OmnCreateTc << ((isIgnored && wordId == AOS_INVWID) 
			|| wordId != AOS_INVWID) << endtc;
		if (i < eMaxWords) mWordIds[i] = wordId;
	}

	// All words are in the system. Now need to check:
	// 1. No two wordIDs should be the same
	// 2. All normalized words should be different. 
	OmnString word1, word2;
	char *buff1, *buff2;
	u64 wordId1, wordId2;
	u32 wordLen1, wordLen2;
	bool ignored1, ignored2;
	int wordsToCheck = mAllWords.entries();
	if ((u32)wordsToCheck > eMaxWords) wordsToCheck = eMaxWords;
	for (int i=0; i<wordsToCheck; i++)
	{
		for (int k=0; k<wordsToCheck; k++)
		{
			if (k == i) continue;
			word1 = mAllWords[i];
			word2 = mAllWords[k];
			wordLen1 = word1.length();
			wordLen2 = word2.length();
			wordId1 = AosDocServerSelf->checkWord(word1, ignored1);
			wordId2 = AosDocServerSelf->checkWord(word2, ignored2);
			if (!ignored1 && !ignored2)
			{
				OmnString ww1 = word1;
				OmnString ww2 = word2;
				buff1 = (char *)AosDocServerSelf->normalizeWord(
					(char *)ww1.data(), wordLen1);
				buff2 = (char *)AosDocServerSelf->normalizeWord(
					(char *)ww2.data(), wordLen2);

				OmnCreateTc << (wordId1 == mWordIds[i]) << endtc;
				OmnCreateTc << (wordId2 == mWordIds[k]) << endtc;
				OmnCreateTc << (
					(wordId1 == wordId2 && wordLen1 == wordLen2 &&
				 	 strncmp(buff1, buff2, wordLen1) == 0) ||
					 wordLen1 != wordLen2 ||
					 strncmp(buff1, buff2, wordLen1) != 0) << endtc;
			}
		}
	}

	// 2. If a word is not ignored, its ptr should not be null. 
	for (int i=0; i<wordsToCheck; i++)
	{
		word1 = mAllWords[i];
		wordId1 = AosDocServerSelf->checkWord(word1, ignored1);
		if (!ignored1)
		{
			OmnCreateTc << (wordId1 != AOS_INVWID) << endtc;
			wordId2 = AosWordMgrSelf->getWordId(word1, false);
			OmnCreateTc << (wordId1 == wordId2) << endtc;
			u64 ptr = 0;
			OmnCreateTc << (AosWordMgrSelf->getPtr(wordId1, ptr)) << endtc;
			OmnCreateTc << (ptr != 0) << endtc;
		}
	}
	*/
	return true;
}


bool
AosDocServerTester::verifyDoc(const u64 &docId)
{
	// All the words in mAllWords that are not ignored
	// should be in an IIL and the doc should be in 
	// that IIL. Otherwise, it is an error. 
	//
	// This function checks all the docs to make sure
	// the above is correct.
	//
	
	/*
	// 1. Either a word is ignored or it should be in 
	//    the system.
	OmnString word;
	for (u32 i=0; i<(u32)mAllWords.entries(); i++)
	{
		word = mAllWords[i];
		bool isIgnored = false;
		u64 wordId = AosDocServerSelf->checkWord(word, isIgnored);
		if (!isIgnored)
		{
			AosIILPtr iil = AosIILMgrSelf->getIIL(word);
			OmnCreateTc << (wordId != AOS_INVWID) << endtc;
			OmnCreateTc << (iil.getPtr() != 0) << endtc;
			OmnCreateTc << (iil->docExist(docId)) << endtc;
			iil->removeRefCount();
		}
	}
	*/

	return true;
}


bool
AosDocServerTester::verifyRawDoc(const u64 docid)
{
	// A doc is stored in two place: Doc Header and Doc Body.
	// The doc header is stored in 'se_docheader' table. 
	
	/*
	// 1. Check the header
	AosXmlDoc header1, header2; 
	OmnCreateTc << (readXmlDocFromDb(docid, header1)) << endtc;
	OmnCreateTc << (header2.loadHeaderFromFile(docid)) << endtc;
	OmnCreateTc << (header1 == header2) << endtc;

	// 2. Check the body
	OmnString body1, body2;
	OmnCreateTc << (readDocBodyFromFile(header1, body1)) << endtc;
	AosXmlTagPtr doc = AosDocServerSelf->getDoc(docid);
	OmnCreateTc << (doc) << endtc;
	body2.assign((char *)doc->getData(), doc->getDataLength());
	OmnCreateTc << (body1 == body2) << endtc;
	*/
	return true;
}


bool
AosDocServerTester::readXmlDocFromDb(
		const u64 &docid, 
		AosXmlDoc &header)
{
	// Doc header is stored in table 'se_docheader'. 
	return true;
}


bool
AosDocServerTester::readDocBodyFromFile(
		const AosXmlDoc &header,
		OmnString &body)
{
	// Open the file
	OmnString fname = mDataDirname;
	fname << "/XmlRawDoc_" << header.getSeqno();
	OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eReadOnly);
	OmnCreateTc << (ff && ff->isGood()) << endtc;
	OmnCreateTc << (ff->seek(header.getOffset())) << endtc;

	u64 offset = header.getOffset();
	int len = ff->readInt(offset, 0); offset += 4;
	OmnCreateTc << (len > 0) << endtc;

	OmnString status = ff->readStr(offset, AosXmlDoc::eStatusLen, "");
	OmnCreateTc << (status.length() == AosXmlDoc::eStatusLen) << endtc;
	char *dd = (char *)status.data();
	OmnCreateTc << (dd[0] == 'A') << endtc; 
	offset += AosXmlDoc::eStatusLen;

	OmnString rawdoc(len+1, 'c', true);
	char *data = (char *)rawdoc.data();
	int bytesRead = ff->readToBuff(offset, len, data);
	OmnCreateTc << (bytesRead == len) << endtc;
	return true;
}


bool
AosDocServerTester::saveHeader(const AosXmlDocPtr &header)
{
	/*
	OmnString stmt = "insert into se_docheader (";
	stmt << 
		"version,"
		"docid,"
		"seqno,"
		"offset,"
		"docsize,"
		"diskCap,"
		"status,"
		"ctime,"
		"mtime,"
		"cntrd,"
		"cntwt,"
		"cntup,"
		"cntdn,"
		"cntcm,"
		"cntrm,"
		"cntcl,"
		"cntlk,"
		"sflag,"
		"vvpd,"
		"evpd,"
		"doctype,"
		"subtype,"
		"docname,"
		"tnail,"
		"appname,"
		"containers,"
		"siteid,"
		"creator,"
		"modifier) values (";
	stmt << header->getVersion()
		<< ", " << header->getDocid()
		<< ", " << header->getSeqno()
		<< ", " << header->getOffset()
		<< ", " << header->getDocSize()
		<< ", " << header->getDiskCap()
		<< ", '" << header->getStatus()
		<< "', " << header->getCtime()
		<< ", " << header->getMtime()
		<< ", " << header->getCntrd()
		<< ", " << header->getCntwt()
		<< ", " << header->getCntup()
		<< ", " << header->getCntdn()
		<< ", " << header->getCntcm()
		<< ", " << header->getCntrm()
		<< ", " << header->getCntcl()
		<< ", " << header->getCntlk()
		<< ", '" << header->getStorageFlag()
		<< "', '" << header->getVVPD()
		<< "', '" << header->getEVPD()
		<< "', '" << header->getDocType()
		<< "', '" << header->getSubType()
		<< "', '" << header->getDocName()
		<< "', '" << header->getThmnail()
		<< "', '" << header->getAppname()
		<< "', '" << header->getContainers()
		<< "', '" << header->getSiteid()
		<< "', '" << header->getCreator()
		<< "', '" << header->getModifier()
		<< "')";

	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnCreateTc << (store->runSQL(stmt)) << endtc;
	*/
	return true;
}


bool
AosDocServerTester::addMetadata(const OmnConnBuffPtr &buff)
{
	/*
	AosXmlParser parser;
	AosXmlTagPtr xml = parser.parse(buff, "");
	OmnCreateTc << (xml && 
			(xml = xml->getFirstChild()) &&
			(xml = xml->getFirstChild("xmlobj"))) << endtc;

	mDocname = "docname";
	mDocname << rand();
	xml->setAttr(AOSTAG_DOCNAME, mDocname);

	mThmnail = "tnail";
	mThmnail << rand();
	xml->setAttr(AOSTAG_TNAIL, mThmnail);

	mDoctype = "doctype";
	mDoctype << rand();
	xml->setAttr(AOSTAG_DOCTYPE, mDoctype);

	mSubtype = "subtype";
	mSubtype << rand();
	xml->setAttr(AOSTAG_SUBTYPE, mSubtype);

	mAppname = "appname";
	mAppname << rand();
	xml->setAttr(AOSTAG_APPNAME, mAppname);
	*/

	return true;
}


bool
AosDocServerTester::verifyQuery()
{
	// 1. For every attribute name, there is an IIL that keeps
   	//    the attribute values.
	// A query is in the form:
	// 	select <attrnames> where <cond> AND <cond> ... <cond>
	// 		order by <fname> limit <start>, <size>
	// where <attrnames> can be 'xml', which means to select
	// the entire doc. 
	//
	// <cond> can be one of the following:
	// 	1. <attrname> <opr> <value>
	// 	2. <attrname> inrange [(value1, value2)]
	// There can be multiple <cond>. 
	//
	// There is a database table that keeps all the attribute 
	// values:
	// 	[attrname, attrvalue, docid]
	// When generating a <cond>, we can use this table to determine
	// which docs being selected. 
	/*	
	OmnString attrnames = generateAttrnames();
	OmnString conds = generateConditions();

	OmnString query = "select ";
	query << attrnames << " where " << conds;
	*/
	return true;
}
