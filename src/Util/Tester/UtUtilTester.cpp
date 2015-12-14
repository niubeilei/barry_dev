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
// 01/01/2011:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/UtUtilTester.h"

#include "Debug/Debug.h"
#include "IILUtil/IILUtil.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/StrHash.h"


bool AosUtUtilTester::start()
{
	cout << "    Start AosUtUtil Tester...";
	basic();
	return true;
}


bool 
AosUtUtilTester::basic()
{
	OmnScreen << "Start testing ..." << endl;
	
	//aos_assert_r(testAosGetStrDiff(), false);
	aos_assert_r(testAosFindFirstIndex(), false);

	OmnScreen << "End testing ..." << endl;
	return true;
}


bool
AosUtUtilTester::testAosGetStrDiff()
{
	int tries = 100;
	const int num_words = 1000;
	OmnString words[num_words];
	for (int i=0; i<num_words; i++)
	{
		words[i] = "asdf_";
		words[i] << i;
	}

	int idx1[20];
	int idx2[20];
	for (int i=0; i<tries; i++)
	{
OmnScreen << "tries: "<< i << endl;
		// Randomly generate two strings: str1 and str2, separated by ',' and ' '
		OmnString str1; 
		int nn1 = rand() % 20;
		for (int k=0; k<nn1; k++)
		{
			if (k != 0) str1 << ", ";
			idx1[k] = rand() % num_words;
			str1 << words[idx1[k]];
		}

		OmnString str2; 
		int nn2 = rand() % 20;
		for (int k=0; k<nn2; k++)
		{
			if (k != 0) str2 << ", ";
			idx2[k] = rand() % num_words;
			str2 << words[idx2[k]];
		}

		list<OmnString> list1, list2, list3;
		aos_assert_r(AosGetStrDiff(str1, str2, ", ", list1, list2, list3), false);

		for (int k=0; k<nn1; k++)
		{
			int idx = idx1[k];

			// Check whether ww is in str2
			bool exist = false;
			for (int m=0; m<nn2; m++)
			{
				if (idx == idx2[m])
				{
					exist = true;
					break;
				}
			}

			OmnString ww = words[idx];
			if (exist)
			{
				// ww exists in str2, which means ww should not be in list1 and list2
				// but should be in list3.
				list<OmnString>::iterator itr;
				for (itr = list1.begin(); itr != list1.end(); itr++)
				{
if (*itr == ww)
	OmnMark;
					aos_assert_r(*itr != ww, false);
				}

				for (itr = list2.begin(); itr != list2.end(); itr++)
				{
					aos_assert_r(*itr != ww, false);
				}

				int found = 0;
				for (itr = list3.begin(); itr != list3.end(); itr++)
				{
					if (ww == *itr) found++;
				}

				aos_assert_r(found == 1, false);
			}
			else
			{
				// ww does not exist in str2, which means that ww should be in list1
				// but not in list2 and list3
				list<OmnString>::iterator itr;
				int found = 0;
				for (itr = list1.begin(); itr != list1.end(); itr++)
				{
					if (*itr == ww) found++;
				}
				aos_assert_r(found == 1, false);

				for (itr = list2.begin(); itr != list2.end(); itr++)
				{
					aos_assert_r(*itr != ww, false);
				}

				for (itr = list3.begin(); itr != list3.end(); itr++)
				{
					aos_assert_r(*itr != ww, false);
				}
			}
		}
	}
	return true;
}


bool
AosUtUtilTester::testAosFindFirstIndex()
{
	const int sgTotalNums = 10000;
	const int sgTotalTries = 10000000;
	const int sgMaxStrLen = 20;		
	const bool sgIsNumAlpha = false;

	OmnScreen << "Create data start." << endl;

	char * values[sgTotalNums];
	for(int i=0; i<sgTotalNums; i++)
	{
		values[i] = new char[sgMaxStrLen + 2];
	}

	int num = 0;
	int len = rand() % sgMaxStrLen + 1;
	char data[len + 1];
	AosRandomLetterStr(len, data);
	strcpy(values[0], data);
	num++;

	int idx, rslt;
	OmnString value;
	for(num = 1; num<sgTotalNums; num++)
	{
		len = rand() % sgMaxStrLen + 1;
		char d[len + 1];
		AosRandomLetterStr(len, d);
		value = d;
		
		idx = AosFindFirstIndex((const char **)&values, (u32)num, eAosOpr_ge, value, sgIsNumAlpha);
		if(idx == -5) idx = num;

		int i = 0;
		for(i=0; i<num; i++)
		{
			rslt = AosIILUtil::valueMatch(values[i], value, sgIsNumAlpha);
			if(rslt >= 0) break;
		}
		aos_assert_r(idx == i, false);
		
		for(int j=i; j<num; j++)
		{
			strcpy(values[j + 1], values[j]);
		}
		strcpy(values[i], value.data());
	}

	OmnScreen << "Create data finished." << endl;
	OmnScreen << "Test start." << endl;
	
	for(int s=0; s<sgTotalTries; s++)
	{
		if(s % 1000 == 0) OmnScreen << "total tries : " << s << endl;

		if(rand() % 100 <= 50)
		{
			value = values[rand() % num];
		}
		else
		{
			len = rand() % sgMaxStrLen + 1;
			char d[len + 1];
			AosRandomLetterStr(len, d);
			value = d;
		}
	
		AosOpr opr;
		switch(rand() % 6)
		{
		case 0 : opr = eAosOpr_gt; break;
		case 1 : opr = eAosOpr_ge; break;
		case 2 : opr = eAosOpr_lt; break;
		case 3 : opr = eAosOpr_le; break;
		case 4 : opr = eAosOpr_eq; break;
		case 5 : opr = eAosOpr_ne; break;
		//case 6 : opr = eAosOpr_an; break;
		//case 7 : opr = eAosOpr_prefix; break;
		default : return false;
		}
	
		idx = AosFindFirstIndex((const char **)&values, (u32)num, opr, value, sgIsNumAlpha);

		bool find = false;
		int find_idx = -1;
		for(int m=0; m<num; m++)
		{
		/*	if(opr == eAosOpr_an)
			{
				find = true;
				find_idx = m;
				break;
			}
			
			if(opr == eAosOpr_prefix)
			{
				rslt = strncmp(values[m], value.data(), value.length());
				if(rslt == 0)
				{
					find = true;
					find_idx = m;
					break;
				}
				continue;
			}
		*/	
			rslt = AosIILUtil::valueMatch(values[m], value, sgIsNumAlpha);
			switch(opr)
			{
			case eAosOpr_eq : if(rslt == 0) { find = true; find_idx = m; }; break;
			case eAosOpr_ne : if(rslt != 0) { find = true; find_idx = m; }; break;
			case eAosOpr_gt : if(rslt >  0) { find = true; find_idx = m; }; break;
			case eAosOpr_ge : if(rslt >= 0) { find = true; find_idx = m; }; break;
			case eAosOpr_lt : if(rslt <  0) { find = true; find_idx = m; }; break;
			case eAosOpr_le : if(rslt <= 0) { find = true; find_idx = m; }; break;
			default: return false;
			}
			if(find) { break; }
		}
		if(idx == -5) 
		{
			aos_assert_r(!find, false);
		}
		else
		{
			aos_assert_r(find && find_idx == idx, false);
		}
	}
	OmnScreen << "Test finished." << endl;
	return true;
}


