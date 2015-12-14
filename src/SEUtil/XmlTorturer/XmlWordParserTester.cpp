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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/XmlTorturer/XmlWordParserTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlTag.h"
#include "SearchEngine/Ptrs.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "SEUtil/SeXmlParser.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



AosXmlWordParserTester::AosXmlWordParserTester()
{
	mName = "XmlWordParserTester";
}


		bool 
AosXmlWordParserTester::start()
{
	cout << "Start AosXmlWordParser Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosXmlWordParserTester::basicTest()
{
	int tries = 1000000;
	for (int i=0; i<tries; i++)
	{
		vector<OmnString> wordlist; 
		OmnString strs[50] = {"one","tow","three","four","five","six", "seven", "eight", "night","ten",
				"eleven","twelve", "thriteen", "fourteen", "fifteen", "sixteen", "seventeen","eighteen",
				"nighteen", "twenty","thrity", "forty", "fifty","sixty","senventy","eighty","nighty",
				"hundred","thousand", "million", "billion","monday", "tuesday", "wednesday", "thursday",
				"friday", "saturday", "sunday", "january", "februry","marth", "april", "may", "jun", "july",
				"august","september","obtober","november","december"};

		for (int i = 0; i < 50; i++)
			wordlist.push_back(strs[i]);
		
		vector<OmnString> attrlist;
		vector<OmnString> wordlist1;

		AosWordParserPtr wordparser = OmnNew AosWordParser();
		char buff[100];
		int len = 0;
		AosXmlTagPtr root;

		int k=0;
		while(1)
		{
			//cout << "===========================" << endl;
			wordlist1 = wordlist;
			attrlist.clear();
			if (k%100 ==0)
			{
				cout << "Testing " << k << endl;
			}
			k++;

			root =	createTag(wordlist1, attrlist);
			//cout << root->toString() <<endl;
			//cout << "Word1 list: " ;
			//copy(wordlist1.begin(),wordlist1.end(), ostream_iterator<OmnString> (cout ,","));
			//cout << endl ;

			OmnString attr;
			if(attrlist.size() != 0)
			{
				int m = rand()%attrlist.size(); 
				//cout << "Disable Attr: " ;

				for (int h = 0; h <=m ; h++)
				{
					int idx = rand() % attrlist.size();
					root->disableAttr(attrlist[idx]);
					//cout << attrlist[idx] << "=" << *(wordlist1.begin()+idx) << ",";
					attr << attrlist[idx] << "=" << *(wordlist1.begin()+idx) << ",";
					wordlist1.erase(wordlist1.begin()+idx);
					attrlist.erase(attrlist.begin()+idx);
				}
				//cout << endl;
			}

			//cout << "Word2 list: " ;
			//copy(wordlist1.begin(), wordlist1.end(), ostream_iterator<OmnString> (cout ,","));
			//cout << endl ;

			root->resetGetWords();
			OmnString error = "Invalid word:\t ";
			bool find = false;
			bool hasWord = false;
			while(root->nextWord(wordparser, buff, 100, len) )
			{
				OmnString word(buff, len);	

				hasWord = true;
				for (int h = 0; h<(int)wordlist1.size(); h++)
				{
					if (word == wordlist1[h])
					{
						//cout << "match:" << *(wordlist1.begin()+h) << endl;
						wordlist1.erase(wordlist1.begin()+h);	
						find = true;
						break;
					}
				}
				if (!find) error << word << ",";
			}

			if (!find && hasWord)
			{
				cout << root->toString() << endl;
				cout << "Disable attr:]\t " << attr << endl;
				cout << error << endl; 
				cout << endl;
			}

			if (wordlist1.size() > 0)
			{
				cout << root->toString() << endl;
				cout << "Disable attr:\t " << attr << endl;
				cout << "Miss    word:\t " ;
				copy(wordlist1.begin(), wordlist1.end(), ostream_iterator<OmnString> (cout ,","));
				cout << endl;
				cout << endl;
			}
			//cout << endl;
		}
	}
	cout << endl;
	return true;
}


AosXmlTagPtr 
AosXmlWordParserTester::createTag(vector<OmnString> wordlist, vector<OmnString> &disable_attr)
{
	// 1. get vector 
	// 2. define how deep the xml is (deepest is about 4)
	// 3. random create attribute, attr name like: attr1, attr2, attr3...
	// 4. tagname, attrvalue, nodetext, those three parts are one part of nextWord
	
	deque<OmnString> endlist;

	struct timeval tv;

	//root tag
	OmnString tagstr;

	int i = 0, j = 0, k = 0;
	bool hastext = false;
	bool tagend = false;
	bool root_attr = true;
	bool closed = false;

	tagstr << "<tag" << k << " ";
	OmnString tmp = "</tag";
	tmp << k++ << ">";
	endlist.push_front(tmp);
	endlist.push_front(">");

	while(endlist.size())
	{
		if (!closed)
		{
			//atttr or end
			gettimeofday(&tv, NULL);
			srand(tv.tv_usec);
			if (rand()%10<5 && i<(int)wordlist.size())
			{
				//attr
				tagstr << "attr" << j << "=\"" << wordlist[i++] << "\" ";
				OmnString disattr = "";
				disattr << "attr" << j++;

				vector<OmnString>::iterator it = find(disable_attr.begin(), disable_attr.end(), disattr);
				if (root_attr && it == disable_attr.end())
				{
					disable_attr.push_back(disattr);
				}
				continue;
			}
			else
			{
				//end
				tagstr << endlist[0];
				endlist.pop_front();
				closed = true;
				hastext = false;
				tagend = false;
				continue;
			}
		}
		else
		{
			//tag text end tag
			gettimeofday(&tv, NULL);
			srand(tv.tv_usec);
			int rd =  rand();
	
			if (i==(int)wordlist.size())
				tagend=true;

			if (rd%3 ==0)
				//&& quanrd < 40)
			{
				//text
				if (!hastext && i<(int)wordlist.size() && !root_attr)
				{
					tagstr << wordlist[i++];
					hastext = true;
					tagend= true;
				}
				continue;
			}
			else if (rd %3==1 && i<(int)wordlist.size() && !tagend)
			{
				//tag
				tagstr << "<tag" << k << " ";
				OmnString tmp = "</tag";
				tmp << k++ << ">";
				endlist.push_front(tmp);
				endlist.push_front(">");
				closed = false;
				tagend = true;
				hastext = true;
				root_attr = false;
			}
			else if(tagend)
			{
				//end tag
				if (endlist.size()>1)
				{
					tagstr << endlist[0];
					endlist.pop_front();
					tagend= false;
					hastext = true;
				}
				else if (i == (int)wordlist.size())
				{
					tagstr << endlist[0];
					endlist.pop_front();
					tagend= false;
				}
			}
		}
	}

	AosXmlParser parser;
	AosXmlTagPtr ctag = parser.parse(tagstr, "");
	if (!ctag)
	{
		cout << "parse tagstr error!" << endl;
		return NULL;
	}
	return ctag;
}

