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
// 2009/10/16	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/Testers/XmlParserTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SearchEngine/WordMgr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "Util1/Time.h"

#if 0

bool AosXmlParserTester::start()
{
	cout << "    Start OmnString Tester ...";
	basicTest();
	return true;
}


bool AosXmlParserTester::basicTest()
{
	// 
	// Test OmnString(const char *d);
	//
	OmnBeginTest << "Test OmnString(const char *d)";
	mTcNameRoot = "String-CTR_C";

	// OmnString xml = "<chending firstname='chen' lastname='ding'>"
	// 	"12345"
	// 	"<second a='one'/>"
	// 	"</chending>";
	// OmnString xml = "<embedobj tags=\"chentag1,chentag2\" __name=\"Student Container\" __pctnrs=\"pcontainer1\" object_id=\"\" objid=\"objtable_b00000006\" />";
	//
	//
OmnString xml="<embedobj category=\"简介\" date=\"2009-11-5\" abstract=\"娄葑第一中心小学是娄葑镇在规划教育过程中，由娄葑实小和娄葑中心小学合并而成，于2003年投资二千多万异地新建的一所高标准的花园式学校\" author=\"娄葑一小\" subtitle=\"\" __name=\"学校简介\" objid=\"objtable_b0000002B\" />";
	AosXmlParser parser;
	OmnConnBuffPtr buff = OmnNew OmnConnBuff(xml.data(), xml.length());
	AosXmlTagPtr tag = parser.parse(buff, "");	
	tag = tag->getFirstChild();

	return true;
}


/*
bool
AosXmlParserTester::torturer()
{
	// 1. It randomly generate a new XML document, then 
	//    parse it. Save the doc into the database.
	// 2. For each doc generated, verify the following:
	//    a. The words contained in it.
	//    b. The tags contained in it.
	//    c. The attributes contained in it.
	//    d. The texts contained in it.
	
	// 1. Generate a doc.
	OmnString doc;
	const int maxWords = 100000;
	const int maxTags = 10000;
	const int numChecks = 10000;
	OmnString words[maxWords];
	u8 wordMarks[maxWords];
	TagInfo tags[maxTags];
	int numWords, numTags;
	int tries = 100000;
	AosXmlParser parser;
	for (int i=0; i<tries; i++)
	{
		if ((i % 100) == 0) cout << Trying i;
		OmnCreateTc << 
			(generateDoc(doc, words, numWords, tags, numTags)) << endtc;

		// Parse the doc
		AosXmlTagPtr root = parser.parse(doc.data(), doc.length(), "");
		OmnCreateTc << (root) << endtc;

		AosXmlTagPtr childtag = root->getChild();
		OmnCreateTc << (childtag) << endtc;

		int nn = rand() % numChecks;
		for (int k=0; k<nn; k++)
		{
			int vv = rand() % 100;
			if (nn < 20)
			{
				// Check the words
				memset(wordMars, 0, sizeof(wordMarks));
				childtag->resetWordLoop();
				char *buff;
				int len;
				while ((buff = childtag->nextWord(len)))
				{
					OmnCreateTc << (checkWord(numWords, words, 
						wordMarks, buff, len)) 
						<< "Length: " << len
						<< "Word: " << buff << endtc;
				}

				// Check whether all the words being marked
				for (int i=0; i<numWords; i++)
					OmnCreateTc << (wordMarks[i]) << endtc;
				continue;
			}

			if (nn < 40)
			{
				// Check the tags
				for (int i=0; i<numTags; i++)
				{
					AosXmlTagPtr tag = root->getFirstChild(tags[i].tagname);
					OmnCreateTc << (tag) << "Tag name: "
						<< tags[i].tagname << endtc;
				}

				// Check tag names
				root->resetTagNameLoop();
				char *buff;
				int len;
				while ((buff = root->nextTagName(len)))
				{

				}
				continue;
			}

			if (nn < 60)
			{
				// Check the attributes
				continue;
			}

			// Check the texts
		}
	}

	return true;
}


bool
AosXmlParserTester::checkWord(
		const int numWords,
		OmnString *words, 
		u8 * wordMarks, 
		const char *word, 
		const int wordlen)
{
	// Check whether it is in the list
	int left = 0;
	int right = numWords; 
	int idx;
	int rslt;
	while (left <= right)
	{
		idx = ((right - left) << 1) + left;
		rslt = strncmp(words[idx].data(), buff, wordlen);
		if (rslt == 0)
		{
			// Check whether the lengths are the same.
			// If not, need to search left and right.
			if (wordlen == words[idx].length())
			{
				// Found the word.
				wordMarks[idx] = 1;
				found = true;
				return true;
			}

			// Search the left side first.
			int crtIdx = idx-1;
			while (crtIdx >= 0)
			{
				rslt = strncmp(words[crtIdx].data(), buff, wordlen);
				if (rslt != 0) break;
				if (wordlen == words[crtIdx].length())
				{
					// Found the word
					wordMarks[crtIdx] = 1;
					return true;
				}
				crtIdx--;
			}

			if (found) break;

			// Search the right side
			crtIdx = idx+1;
			while (crtIdx < numWrods)
			{
				rslt = strncmp(words[crtIdx].data(), buff, wordlen);
				if (rslt != 0) break;
				if (wordlen == words[crtIdx].length())
				{
					wordMarks[crtIdx] = 1;
					return true;
				}
				crtIdx++;
			}
			return false;
		}

		if (rslt < 0)
		{
			// words[idx] is smaller than buff. 
			left = idx+1;
		}
		else
		{
			// words[idx] is larger than buff.
			right = idx-1;
		}
	}

	return false;
}
*/
#endif
