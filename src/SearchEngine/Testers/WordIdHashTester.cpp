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
// 2009/10/09	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/Testers/WordIdHashTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "IdGen/IdGen.h"
#include "SearchEngine/WordIdHash.h"
#include "SearchEngine/WordMgr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

#if 0

// OmnString tmpWords[10000];
// u64		  tmpWordIds[10000];
// u64		  tmpPtrs[10000];

bool AosWordIdHashTester::start()
{
	cout << "    Start OmnString Tester ...";
	// basicTest();
	mDirname = "/AOS/Data";
	mHashFname = "testwordid";
	mWordIdHashTsize = 1023;

	// Torturing Strength Setting
	// 1. The number of queries before generating a new word.
	mNumQueries = 100;
					
	prepareEnv();
	torture();
	return true;
}


bool 
AosWordIdHashTester::prepareEnv()
{
	// Empty the 'mHashFname' file
	OmnString cmd = "cp ";
	cmd << mDirname << "/EmptyFile " << mDirname << "/"
		<< mHashFname;
	system(cmd);
	OmnCreateTc << (AosWordMgrSelf->start(mDirname, mHashFname, 
			mWordIdHashTsize)) << endtc;

	// Clear 'wordhashinfo' table
	OmnString stmt = "update wordhashinfo set filesize=0 where name='";
	stmt << mHashFname<< "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	store->runSQL(stmt);

	// Clear the records in 'wordid'
	stmt = "delete from ";
	stmt << mHashFname;
	store->runSQL(stmt);

	// Create the hash table.
	OmnCreateTc << (AosWordMgrSelf->createHashtable()) << endtc;
	return true;
}


bool AosWordIdHashTester::basicTest()
{
	// 
	// Test OmnString(const char *d);
	//
	// Need to prepare an empty environment.
	OmnString cmd = "cp ";
	cmd << mDirname << "/EmptyFile " << mDirname << "/"
		<< mHashFname;
	system(cmd);

	OmnCreateTc << (AosWordMgrSelf->start(mDirname,
		mHashFname, mWordIdHashTsize)) << endtc;

	u64 wordId = AosWordMgrSelf->getWordId((const u8 *)"chen ding", 
		strlen("chen ding"), false);
	OmnCreateTc << (wordId == AOS_INVWID) << endtc; 
	wordId = AosWordMgrSelf->getWordId((const u8 *)"chen ding", 
		strlen("chen ding"), true);
	OmnCreateTc << (wordId != AOS_INVWID) << endtc;

	u64 wordId1 = AosWordMgrSelf->getWordId((const u8 *)"chen ding", 
		strlen("chen ding"), true);
	OmnCreateTc << (wordId1 != AOS_INVWID) << endtc;
	OmnCreateTc << (wordId == wordId1) << endtc;
	return true;
}


const int max_word_len = 30;
bool AosWordIdHashTester::torture()
{
	// This function randomly generates eNumWords number of 
	// words and save it into the hash table. During doing
	// so, it will randomly query the hash table for the
	// words that are no in the table. 


	u64 wordId, wordId1;
	int totalTries = mTestMgr->getTries();
	if (totalTries <= 0) totalTries = 100;
	mWordsRetrieved = 0;
	for (int i=0; i<totalTries; i++)
	{
		if ((i % 100) == 0) cout << "Trying: " << i << endl;

		// 1. Generate a word
		OmnString word = generateNewWord();

		// 2. Query the table for a number of times before 
		//    adding the word
		queryTable(i);

		// 3. Query the new word. It should not find it.
		wordId = AosWordMgrSelf->getWordId(
				(const u8 *)word.data(), word.length(), false);
		OmnCreateTc << (wordId == AOS_INVWID) << endtc; 

		// 4. Insert the word
		wordId = AosWordMgrSelf->getWordId(
				(const u8 *)word.data(), word.length(), true);
		OmnCreateTc << (wordId1 != AOS_INVWID) << endtc; 

		// 5. Set the ptr
		u64 value = rand();
		u64 ptr = (value << (rand() % 20));
		OmnCreateTc << (AosWordMgrSelf->setPtr1(wordId, ptr)) << endtc;

		// 6. Reget the word and ptr
		wordId1 = AosWordMgrSelf->getWordId(
			(const u8 *)word.data(), word.length(), true); 
		OmnCreateTc << (wordId1 != AOS_INVWID) << endtc; 
		OmnCreateTc << (wordId1 == wordId) << endtc; 

		bool rslt = AosWordMgrSelf->getPtr(wordId, value);
		OmnCreateTc << (rslt) << endtc;
		OmnCreateTc << (value == ptr) << endtc;

		// 7. Insert the word into db
		OmnCreateTc << (mWordIds.add(word, wordId)) << endtc;
		OmnCreateTc << (mPtrs.add(word, ptr)) << endtc;
		if (mWordsRetrieved < eWordSize)
		{
			mWords[mWordsRetrieved++] = word;
		}
		else
		{
			int ii = rand() % mWordsRetrieved;
			mWords[ii] = word;
		}

		// 8. verify the database and 
		verifyDb();
	}

	return true;
}


bool
AosWordIdHashTester::queryTable(const int numWords)
{
	// It randomly generates a word, check whether it should/should not
	// be in the table.
	u64 wordId, wordId1;

	for (int i=0; i<mNumQueries; i++)
	{
		int vv = rand() % 100;
		if (vv < 20 && numWords)
		{
			// It should check an existing word
			if (mWordsRetrieved == 0) continue;
			int idx = rand() % mWordsRetrieved;
			bool createFlag = (idx % 15) < 8;
			wordId = AosWordMgrSelf->getWordId(
				(const u8 *)mWords[idx].data(), 
				mWords[idx].length(), createFlag);
			wordId1 = AosWordMgrSelf->getWordId(
				(const u8 *)mWords[idx].data(), 
				mWords[idx].length(), !createFlag);
			OmnCreateTc << (wordId != AOS_INVWID) << endtc; 
			OmnCreateTc << (wordId == wordId1) << endtc; 
			continue;
		}

		if (vv < 40 && numWords)
		{
			// It should check a non-existing but similar word
			if (mWordsRetrieved == 0) continue;
			int idx = rand() % mWordsRetrieved;
			OmnString ww = mWords[idx];

			int nn = idx % 4;
			int wlen = ww.length();
			char *data = (char *)ww.data();
			for (int m=0; m<nn; m++)
			{
				idx = rand() % wlen;
				char c = data[idx];
				if (idx % 2)
				{
					if (c > 2) c--;
					else c++;
				}
				else
				{
					if (c < 125) c++;
					else c--;
				}
				if (c == 39) c = 'A';		// single quote
				else if (c == 92) c = 'B';	// back slash
				else if (c < 32) c = 'C';
				else if (c > 126) c = 'D';
				if (c >= 'a' && c <= 'z') c = 'A' + (c - 'a');

				// The first and last char should not be a space
				if (idx == 0 && c == ' ') c = 'E';
				if (idx == wlen-1 && c == ' ') c = 'F';
				data[idx] = c;
			}

			idx = rand() % wlen;
			data[idx] = '{';

			wordId = AosWordMgrSelf->getWordId((const u8 *)ww.data(), 
					ww.length(), false);
			OmnCreateTc << (wordId == AOS_INVWID) << endtc; 
			continue;
		}

		if (vv < 50)
		{
			// Generate a new word. Make sure the word is not in the table
			OmnString ww = generateNewWord();
			if (ww != "")
			{
				wordId = AosWordMgrSelf->getWordId((const u8 *)ww.data(), 
					ww.length(), false);
				OmnCreateTc << (wordId == AOS_INVWID) << endtc; 
			}
			continue;
		}

		if (vv < 60 && numWords)
		{
			// It should modify a ptr
			if (mWordsRetrieved == 0) continue;
			int idx = rand() % mWordsRetrieved;
			u64 ptr = rand();
			ptr = (ptr << (rand() % 10));
			mPtrs.set(mWords[idx], ptr);
			wordId = mWordIds.get(mWords[idx], 0);
			OmnCreateTc << (wordId != AOS_INVWID) << endtc;
			OmnCreateTc << (AosWordMgrSelf->setPtr1(wordId, ptr)) << endtc;
			continue;
		}

		if (vv < 80 && numWords)
		{
			// It should verify ptrs
			if (mWordsRetrieved == 0) continue;
			int numModifies = rand() % 40;
			for (int k=0; k<numModifies; k++)
			{
				// Pick a word to modify
				int idx = rand() % mWordsRetrieved;
				u64 ptr;
				wordId = mWordIds.get(mWords[idx], AOS_INVWID);
				OmnCreateTc << (wordId != AOS_INVWID) << endtc;
				bool rslt = AosWordMgrSelf->getPtr(wordId, ptr);
				OmnCreateTc << (rslt) << endtc;
				u64 ptr1 = mPtrs.get(mWords[idx], AOS_INVWID);
				OmnCreateTc << (ptr == ptr1) << endtc;
			}
			continue;
		}

		// Verify words. Pick a word, retrieve its wordId. 
		// The wordId should not be null. Use the wordId
		// to retrieve the word. Then compare the two words.
		if (mWordsRetrieved == 0) continue;
		int idx = rand() % mWordsRetrieved;
		OmnString ww = mWords[idx];
		u64 wordId = AosWordMgrSelf->getWordId(
				(const u8 *)ww.data(), ww.length(), false);
		OmnCreateTc << (wordId != AOS_INVWID) << endtc;
		u32 wordLen;
		char *data = AosWordMgrSelf->getWord(wordId, wordLen);
		OmnString word(data, wordLen);
		OmnCreateTc << (word != 0) << endtc;
		OmnCreateTc << (ww == word) << endtc;
	}
	return true;
}


bool
AosWordIdHashTester::wordExist(
		const OmnString &word, 
		u64 &wordId)
{
	/*
	// This function checks whether the word is defined
	OmnString stmt = "select wordid from torturer_seng_words where word='";
	stmt << word << "'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	OmnDbTablePtr table;
	OmnRslt rslt = store->query(stmt, table);
	OmnCreateTc << (rslt) << endtc;
	if (table->entries() == 0) return false;
	OmnCreateTc << (table->entries() == 1) << endtc;
	table->reset();
	OmnDbRecordPtr record = table->next();
	wordId = record->getU64(0, 0, rslt);
	return true;
	*/
	wordId = mWordIds.get(word, 0);
	if (wordId == 0) return false;
	return true;
}


OmnString
AosWordIdHashTester::generateNewWord()
{
	// This function generates a new word that is guaranteed
	// not the same as any of the ones that were generated.
	while (1)
	{
		char word[max_word_len+1];
		int len = rand() % max_word_len;
		if (len == 0) len = 5;

		for (int k=0; k<len; k++)
		{
			char c = (rand() % 94) + 32;
			if (c == 39) c = 'A';			// The single quote (39)
			else if (c == 92) c = 'B';		// the backslash (92)

			// Currently the database is case insensitive.
			// Convert all lower case to upper case
			if (c >= 'a' && c <= 'z') c = 'A' + (c - 'a');
			if (c == '{') c = 'G';
			word[k] = c;
		}

		// The first and last char should not be a space
		if (word[0] == ' ') word[0] = 'E';
		if (word[len-1] == ' ') word[len-1] = 'F';
		word[len] = 0;
		if (!mWordIds.exist(word)) return word;
	}
	return "";
}


bool
AosWordIdHashTester::verifyDb()
{
	int tries = rand() % 50;
	for (int i=0; i<tries; i++)
	{
		int tablesize = AosWordMgrSelf->getTablesize();
		int bktid = rand() % tablesize;
		bool rslt = AosWordMgrSelf->verifyDb(bktid);
		if (!rslt)
		{
			OmnAlarm << "Failed db checking!" << enderr;
		}
	}
	return true;
}

#endif
