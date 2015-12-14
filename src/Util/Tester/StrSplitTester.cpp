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
// 08/06/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/StrSplitTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/StrSplit.h"


bool AosStrSplitTester::start()
{
	// 
	// Test default constructor
	//
	cout << "    Start AosStrSplit Tester...";

	basicTest();
	testSplitByChar();
	testSplitBySubstr();
	return true;
}


bool 
AosStrSplitTester::basicTest()
{
	OmnBeginTest << "Test";

	vector<OmnString> containers;
	vector<OmnString> members;
	OmnString str = "aaaaaaa|$|bbbbbbbb|$|ccccccccc|$|ddddddddd|$|fffffffffffffffffff|$|ggggggggggggggg";
	u32 start_time = OmnGetSecond();
	for (int i=0; i<1000000000; i++)
	{
		AosStrSplit::splitStrBySubstr(str, "|$|", containers, members, 4);
		if (i % 1000000 == 0) 
		{
			u32 delta = OmnGetSecond() - start_time;
			if (delta)
			{
				OmnScreen << "Tried: " << i << ". Speed: " << i / delta << endl;
			}
		}

		// for (u32 i=0; i<containers.size(); i++)
		// {
		// 	OmnScreen << "Containers: " << containers[i] << endl;
		// }

		// for (u32 i=0; i<members.size(); i++)
		// {
		// 	OmnScreen << "Members: " << members[i] << endl;
		// }

	}

	const char *contents = "tag1|$|tag2|$|tag3|$|tag4";
	OmnString parts[10];
	bool finished;
	AosStrSplit split(contents, "|$|", parts, 10, finished);

	OmnCreateTc << (split.entries() == 4) << endtc;
	OmnCreateTc << (split.getEntry(0) == "tag1") << endtc;
	OmnCreateTc << (split.getEntry(1) == "tag2") << endtc;
	OmnCreateTc << (split.getEntry(2) == "tag3") << endtc;
	OmnCreateTc << (split.getEntry(3) == "tag4") << endtc;
	return true;
}


bool 
AosStrSplitTester::testSplitByChar()
{
	OmnBeginTest << "Test";

	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 10000000;
	OmnString words[eMaxNumWords+1];
	while (tries--)
	{
		// Create separator
		OmnString seps = createSeparator();
		int sep_len = seps.length();
		
		// Create the string
		int num_words = rand() % eMaxNumWords;
		OmnString contents;
		OmnString word;
		int total_words = 0;
		for (int i=0; i<num_words; i++)
		{
			int minlen = rand() % eMaxWordLen;
			int maxlen = rand() % eMaxWordLen;
			if (minlen > maxlen)
			{
				int tmp = minlen;
				minlen = maxlen;
				maxlen = tmp;
			}
			word = OmnRandom::word(minlen, maxlen);
			if (word == "")
			{
				i--;
				continue;
			}
			if (i != 0 || rand() % 1)
			{
				if (i == 0) 
				{
					words[total_words++] = "";
				}
				char ss[eMaxSepLength];
				int m = rand() % 5;
				if (m == 0) m = 1;
				for (int k=0; k<m; k++)
				{
					ss[k] = seps.data()[(rand() % sep_len)];
				}
				ss[m] = 0;
				contents << ss;
			}
			
			contents << word;
			words[total_words++] = word;
		}

		if (rand() % 1)
		{
			// Will add a separator to the end
			char ss[eMaxSepLength];
			int m = rand() % 5;
			if (m == 0) m = 1;
			for (int k=0; k<m; k++)
			{
				ss[k] = seps.data()[rand() % sep_len];
			}
			ss[m] = 0;
			contents << ss;
			words[total_words++] = "";
		}
		num_words = total_words;

		vector<OmnString> substrs;
		int mm = AosStrSplit::splitStrByChar(contents, seps.data(), substrs, num_words);
		if (mm != num_words)
		{
			return false;
		}
		//aos_assert_r(mm == num_words, false);
		for (int i=0; i<mm; i++)
		{
			aos_assert_r(substrs[i] == words[i], false);
		}
	}
	return true;
}


OmnString
AosStrSplitTester::createSeparator()
{
	// Create separator
	static char sep_map[] = "!@#$%^&*,.' ()+~";
	char seps[eMaxSepLength];
	int sep_len = rand() % eMaxSepLength;
	if (sep_len == 0) sep_len = 1;
	int idx;
	for (int i=0; i<sep_len; i++)
	{
		seps[i] = sep_map[0];
		idx = rand() % sizeof(sep_map);
		if (idx == 0 || idx == 1)
		{
			idx = 2;
		}
		seps[i] = sep_map[idx-2];
		if (seps[i] == 0)
		{
			OmnMark;
		}
	}
	seps[sep_len] = 0;
	return seps;
}


bool 
AosStrSplitTester::testSplitBySubstr()
{
	int tries = mTestMgr->getTries();
	if (tries <= 0) tries = 10000000;
	OmnString words[eMaxNumWords+1];
	while (tries--)
	{
		// Create substr 
		OmnString substr = createSeparator();
		
		// Create the string
		int num_words = 3;//rand() % eMaxNumWords;
		OmnString contents;
		OmnString word;
		int total_words = 0;
		for (int i=0; i<num_words; i++)
		{
			int minlen = rand() % eMaxWordLen;
			int maxlen = rand() % eMaxWordLen;
			if (minlen > maxlen)
			{
				int tmp = minlen;
				minlen = maxlen;
				maxlen = tmp;
			}
			word = OmnRandom::word(minlen, maxlen);
			if (i != 0 || rand() % 1)
			{
				if (i == 0) 
				{
					words[total_words++] = "";
				}
				contents << substr;
				words[total_words++] = "";
				contents << substr;
			}
			
			contents << word;
			words[total_words++] = word;
		}

		if (rand() % 1)
		{
			// Will add a separator to the end
			contents << substr;
			words[total_words++] = "";
		}
		num_words = total_words;

		vector<OmnString> substrs;
		bool finished = false;
		int mm = AosStrSplit::splitStrBySubstr(contents, substr.data(), substrs, num_words);
		aos_assert_r(finished, false);
		if (mm != num_words)
		{
			return false;
		}

		//aos_assert_r(mm == num_words, false);
		for (int i=0; i<mm; i++)
		{
			aos_assert_r(substrs[i] == words[i], false);
		}
	}
	return true;
}

