////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/StringTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Random/random_util.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool OmnStringTester::start()
{
	cout << "    Start OmnString Tester ...";
	// u32 tries = 1000;
	// testDefaultConstructor();
	// testConstructorB();
	// testTrimLastWord();
	// miscTest();
	// testRemoveLine();
	// testGetLine(tries);
	// testReplace(tries);
	// testParsingValues(tries);
	testFindSubString();

	// Test escapeFnameWS()
	// OmnString fname = "chending (13).pst";
	// fname.escapeFnameWS();
	return true;
}


bool OmnStringTester::miscTest()
{
	// 
	// Test OmnString(const char *d);
	//
	OmnBeginTest << "Test OmnString(const char *d)";
	mTcNameRoot = "String-CTR_C";
	OmnString c1("");
	OmnCreateTc << (c1.length() == 0) << endtc;
	OmnCreateTc << (c1 == "") << endtc;
		
	OmnString c2(".");
	OmnCreateTc << (c2.length() == 1) << endtc;
	OmnCreateTc << (c2 == ".") << endtc;

	OmnString c3("@*");
	OmnCreateTc << (c3.length() == 2) << endtc;
	OmnCreateTc << (c3 == "@*") << endtc;

	
	// 
	// Test OmnString(const char *d, const int size);
	//
	OmnBeginTest << "OmnString(const char *d, const int size)";
	mTcNameRoot = "String-CTR_D";
	OmnString d1("",1);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(d1.length())) << endtc;
	OmnCreateTc << (d1 == "") << "Actual: " << endtc;

	OmnString d2(".",1);
	OmnCreateTc << (d2.length() == 1) << "Actual: " << d2.length() << endtc;
	OmnCreateTc << (d2 == ".") << "Actual: " << d2 << endtc;

	// chen ding verified on 10/20/04
	OmnString d3(".",-1);
	OmnCreateTc << (d3.length() == 1) << endtc;
	OmnCreateTc << (d3 == ".") << endtc;

	// 
	// Test OmnString(const OmnString &rhs);
	//
	OmnBeginTest << "OmnString(const OmnString &rhs)";
	mTcNameRoot = "String-CTR_E";
	OmnString ee1("");
	OmnString e1(ee1);
	OmnCreateTc << (e1.length() == 0) << endtc;
	OmnCreateTc << (e1 == "") << endtc;

	OmnString ee2("&");
	OmnString e2(ee2);
	OmnCreateTc << (e2.length() == 1) << endtc;
	OmnCreateTc << (e2 == "&") << endtc;

	// 
	// Test operator const char *() const {return mDataPtr;}
	//
	OmnBeginTest << "operator const char *() const {return mDataPtr;}";
	mTcNameRoot = "String-CTR_F";
	OmnString f1("");
	OmnCreateTc << (strlen(f1.getBuffer())==0) << endtc;

	OmnString f2("&");
	OmnCreateTc << (strlen(f2.getBuffer())==1) << endtc;

	// 
	// Test int length() const {return mLen;}
	//
	OmnBeginTest << "int length() const {return mLen;}";
	mTcNameRoot = "String-CTR_H";
	OmnString h1(12345678, '*', true);
	OmnCreateTc << (h1.length()==12345678) << endtc;

	// 
	// Test void reset()
	//
	OmnBeginTest << "void reset()";
	mTcNameRoot = "String-CTR_I";
	OmnString i1(150, '*', true);
	// i1.reset();
	OmnCreateTc << (i1.length() == 0) << endtc;
	OmnCreateTc << (i1 == "") << endtc;

	// 
	// Test const char * data() const
	// Test char * getBuffer() const
	//
	OmnBeginTest << "const char * data() const\nchar* getBuffer() const ";
	mTcNameRoot = "String-CTR_J";
	OmnString j1(150, '*', true);
	OmnString jj1(150, '*', true);
	OmnCreateTc << (strcmp(j1.data(),jj1.data())==0) << endtc;
	OmnCreateTc << (strcmp(j1.getBuffer(),jj1.getBuffer())==0) << endtc;
	OmnString jj2(150, '&', true);
	OmnCreateTc << (strcmp(j1.data(),jj2.data())!=0) << endtc;
	OmnCreateTc << (strcmp(j1.getBuffer(),jj2.getBuffer())!=0) << endtc;

	// 
	// Test void toLower() & toUpper()
	//
	OmnBeginTest << "void toLower() & toUpper()";
	mTcNameRoot = "String-CTR_K";
	OmnString k1("Test&Buff*123PK");
	k1.toLower();
	OmnCreateTc << (k1 == "test&buff*123pk") << endtc;
	k1.toUpper();
	OmnCreateTc << (k1 == "TEST&BUFF*123PK") << endtc;

	// 
	// Test bool retrieveInt(const int startIndex, int &length, int &value)
	//
	OmnBeginTest << "bool retrieveInt(const int startIndex, int &length, int &value)";
	mTcNameRoot = "String-CTR_L";
	OmnString l1("ABC12345WWW");
	int value = 0; u32 length = 0;
	OmnCreateTc << (l1.retrieveInt(3,length,value)) << endtc;
	OmnCreateTc << (length==5) << endtc;
	OmnCreateTc << (value==12345) << endtc;
	OmnCreateTc << !(l1.retrieveInt(2,length,value)) << endtc;

	// 
	// Test void replace(const uint index1, const uint length, const OmnString &str)
	//
	OmnBeginTest << "void replace(const uint index1, const uint length, const OmnString &str)";
	mTcNameRoot = "String-CTR_M";
	OmnString m1("ABC12345WWW");
	m1.replace(1,7,"ZHENG");
	OmnCreateTc << (m1.length() == 9) << endtc;

	OmnCreateTc << (m1=="AZHENGWWW") << "Actual: " << m1 << endtc;
	m1.replace(0,1,"ABXY");
	OmnCreateTc << (m1=="ABXYZHENGWWW") << "Actual: " << m1 << endtc;
	
	OmnString m2(110, 'c', true);
	m2.replace(1, 100, "ddd");
	OmnCreateTc << (m2 == "cdddccccccccc") << "Actual: " << m2 << endtc;

	// 
	// Test OmnString & operator = (const OmnString &rhs)
	//
	OmnBeginTest << "OmnString & operator = (const OmnString &rhs)";
	mTcNameRoot = "String-CTR_N";
	OmnString n1("!@#$%^&*()_+=-|\?/>.<,;:~`");
	OmnString n2("");
	n2 = n1;
	OmnCreateTc << (n2.length() == 25) << endtc;
	OmnCreateTc << (n2 == "!@#$%^&*()_+=-|\?/>.<,;:~`") << endtc;

	OmnString n3(120,'.', true);
	n2 = n3;
	OmnCreateTc << (n2.length() == 120) << endtc;
	n2 = n1;
	OmnCreateTc << (n2.length() == 25) << endtc;

	// 
	// Test OmnString & operator += (const char *rhs)
	// Test OmnString & operator += (const OmnString &rhs)
	//
	OmnBeginTest << "OmnString & operator += (const char *rhs)";
	mTcNameRoot = "String-CTR_O";
	OmnString o1("!@#$%^&*()_+=-|\?/>.<,;:~`");
	OmnString o2("0123456789");
	o2 += o1;
	OmnCreateTc << (o2.length() == 35) << endtc;
	OmnCreateTc << (o2 == "0123456789!@#$%^&*()_+=-|\?/>.<,;:~`") << endtc;
	o2 += "0123456789";
	OmnCreateTc << (o2.length() == 45) << endtc;
	OmnCreateTc << (o2 == "0123456789!@#$%^&*()_+=-|\?/>.<,;:~`0123456789") << endtc;

	// 
	// Test OmnString & operator << (const char rhs)
	// Test OmnString & operator << (const char *rhs)
	// Test OmnString & operator << (const OmnString &rhs)
	// Test OmnString & operator << (const float rhs)
	// Test OmnString & operator << (const long rhs)
	// Test OmnString & operator << (const OmnUint8 &rhs)
	// Test OmnString & operator << (const int rhs)
	// Test OmnString & operator << (const unsigned int rhs)
	// Test OmnString & operator << (const OmnLL &rhs)
	// Test OmnString & operator << (const OmnULL &rhs)
	//
	OmnBeginTest << "OmnString & operator << ()";
	mTcNameRoot = "String-CTR_P";
	OmnString p1("");
	p1 << 'A';
	OmnCreateTc << (p1.length() == 1) << endtc;
	OmnCreateTc << (p1 == "A") << endtc;
	p1 << "123";
	OmnCreateTc << (p1.length() == 4) << endtc;
	OmnCreateTc << (p1 == "A123") << endtc;
	OmnString p2(101, '#', true);
    p1 << p2;
	OmnCreateTc << (p1.length() == 105) << endtc;
    float pp1 = (float)12.12;
	p1 << pp1;
	OmnCreateTc << (p1.length() == 114) << "Actural: " << p1 << endtc;
	long pp2 = 1234567890;
	p1 << pp2;
	OmnCreateTc << (p1.length() == 124) << "Actural: " << p1 << endtc;

	int pp4 = 1234567890;
    p1 << pp4;
	OmnCreateTc << (p1.length() == 13) << "Actural: " << p1 << endtc;

	unsigned int pp5 = 1234;
	p1 << pp5;
	OmnCreateTc << (p1.length() == 17) << "Actural: " << p1 << endtc;

/* Chen Ding, 11/03/2005
	OmnLL pp6 = 1234567890;
	p1.reset();
	p1 << pp6;
	OmnCreateTc << (p1.length() == 10) << "Actural: " << p1 << endtc;

	OmnULL pp7 = 123;
	p1.reset();
	p1 << pp7;
	OmnCreateTc << (p1.length() == 3) << "Actural: " << p1 << endtc;

	// 
	// Test char & 	operator [] (const uint pos)
	//
	OmnBeginTest << "char & operator [] (const uint pos)";
	mTcNameRoot = "String-CTR_Q";
	OmnString q1("Test&Buff*123PK");
	OmnCreateTc << (q1[4]=='&') << endtc;
	OmnCreateTc << (q1[9]=='*') << endtc;

	// 
	// Test bool operator != (const OmnString &rhs) const
	// Test bool operator != (const char *rhs) const
	//
	OmnBeginTest << "bool operator != (const OmnString &rhs) const";
	mTcNameRoot = "String-CTR_R";
	OmnString rr1(110,'&', true);
	OmnString r1("Test&Buff*123PK");
	OmnCreateTc << (r1 != rr1) << endtc;
	OmnCreateTc << (rr1 != "Test&Buff*123PK") << endtc;

	// 
	// Test bool operator == (const OmnString &rhs) const
	// Test bool operator == (const char *rhs) const 
	//
	OmnBeginTest << "bool operator == (const char *rhs) const ";
	mTcNameRoot = "String-CTR_S";
	OmnString sss1(110,'&', true);
	OmnString sss2(110,'&', true);
	char * ss1 = sss1.getBuffer();
	OmnCreateTc << (sss1 == sss2) << endtc;
	OmnCreateTc << (sss1 == ss1) << endtc;
	OmnString sss3("Test&Buff*123PK");
	OmnCreateTc << (sss3 == "Test&Buff*123PK") << endtc;

	// 
	// Test bool isNull() const { return mLen == 0; }
	//
	OmnBeginTest << "bool operator == (const char *rhs) const ";
	mTcNameRoot = "String-CTR_T";
	OmnString t1(110,'&', true);
	OmnCreateTc << (!(t1.isNull())) << endtc;
	OmnString t2("");
	OmnCreateTc << (t2.isNull()) << endtc;

	// 
	// Test OmnString & assign(const char *data, const uint len)
	// Test OmnString & assign(const uint numChars, const char c)
	//
	OmnBeginTest << "OmnString & assign(const char *data, const uint len)";
	mTcNameRoot = "String-CTR_U";
	OmnString u1("");
	u1.assign("testtest",6);
	OmnCreateTc << (u1.length() == 6) << endtc;
	u1.assign(128,'*');
	OmnCreateTc << (u1.length() == 128) << endtc;

	// 
	// Test void hexStringToStr(const char *data,const int len)
	//
	OmnBeginTest << "void hexStringToStr(const char *data,const int len)";
	mTcNameRoot = "String-CTR_V";
	char data[] = "1234567890";
	OmnString v1("");
	v1.hexStringToStr(data,10);
	OmnCreateTc << (v1.length() == 29) <<v1.length()<< endtc;
	OmnTrace << v1 << endl;

	// 
	// Test OmnString &	append(const char *data, const int length)
	//
	OmnBeginTest << "OmnString & append(const char *data, const int length)";
	mTcNameRoot = "String-CTR_W";
	OmnString w1("ABC");
	w1.append(data,8);
	OmnCreateTc << (w1.length() == 11) << endtc;

	// 
	// Test unsigned int getHashKey() const
	//
	OmnBeginTest << "unsigned int getHashKey() const";
	mTcNameRoot = "String-CTR_X";
	OmnString x1("ABC");
	OmnTrace << x1.getHashKey() << endl;
	OmnCreateTc << (x1.getHashKey() == (unsigned int)-85899326) << endtc;

	// 
	// Test OmnString toString() const
	//
	OmnBeginTest << "OmnString toString() const";
	mTcNameRoot = "String-CTR_Y";
	OmnString y1("ABC");
	OmnCreateTc << (y1.toString() == "ABC") << endtc;

	// 
	// Test bool remove(const int starting, const int length)
	// Test OmnString operator - (const OmnString &rhs) const
	//
	OmnBeginTest << "OmnString operator - (const OmnString &rhs) const";
	mTcNameRoot = "String-CTR_Z";
	OmnString zzz1("ABCabcABCDabcdABCDEabcde");
	OmnCreateTc << (zzz1.remove(0,3)) << endtc;
	OmnCreateTc << (zzz1 == "abcABCDabcdABCDEabcde") << endtc;
	OmnCreateTc << (zzz1.remove(3,4)) << endtc;
	OmnCreateTc << (zzz1 == "abcabcdABCDEabcde") << endtc;

	OmnString z1("ABCabcABCDabcdABCDEabcde");
	OmnString zz1("abcd");
	OmnTrace << z1 - zz1 << endl;
	OmnCreateTc << (z1 - zz1 == "ABCabcABCDabcdABCDEabcde") << endtc;
	OmnString zz2("ABCabc");
	OmnTrace << z1 - zz2 << endl;
	OmnCreateTc << (z1 - zz2 == "ABCDabcdABCDEabcde") << endtc;

	// 
	// Test OmnString addSlashes() const
	//
	OmnBeginTest << "OmnString addSlashes() const";
	mTcNameRoot = "String-CTR-AA";
	OmnString aa1("ABC\0abc\\ABCD\\abcd\\ABCDE\\abcde", 29);
//	aa1("ABC\0abc\\ABCD\\abcd\\ABCDE\\abcde");
	OmnString aa2 = aa1.addSlashes();
	OmnTrace << "aa2=" << aa2 << endl;

	// 
	// Make sure cancatinate an empty string works correctly.
	//
	{
		OmnString a("AAAAA");
		OmnString b("");
		OmnString c("BBBBB");
		OmnString d;
		d << a << b << c;
		OmnCreateTc << (d == "AAAAABBBBB") << endtc;
	}
*/

	// 
	// Test OmnLL 
	// 
	{
		// 
		// Create an OmnLL, convert it into string, then convert back to OmnLL.
		// Compare the results.
		//
/*		OmnLL v1;
		v1 = OmnTime::getCrtSec();
		v1 = (v1 << 32) + OmnTime::getCrtUsec();
		OmnString s1;
		s1 << v1;
		OmnLL v2 = OmnAtoLL(s1.getBuffer());
		OmnCreateTc << (v1 == v2) << endtc;

		//
		// Create an OmnULL, convert it into string, then back to OmnULL.
		// Then compare.
		//
		OmnULL v3;
		v3 = OmnTime::getCrtSec();
		v3 = (v3 << 32) + OmnTime::getCrtUsec();
		OmnString s2;
		s2 << v3;
		OmnULL v4 = OmnAtoLL(s2.getBuffer());
		OmnCreateTc << (v3 == v4) << endtc; */
	}

	cout << " Done. Total Testcase: " << mNumTcs << ". Failed: " << mNumTcsFailed << endl;

	return true;
}


bool
OmnStringTester::testDefaultConstructor()
{
	// 
	// Test default constructor
	//
	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "String-DFT-CTR";
	OmnString s1;

	OmnString s2;
	OmnCreateTc << (s1 == s2) << endtc;			// String-DFT-CTR-1
	OmnCreateTc << (s1.length() == 0) << endtc;
	OmnCreateTc << (s1 == "") << endtc;

	return true;
}
 

bool
OmnStringTester::testConstructorB()
{
	// 
	// Test the constructor:
	//		"OmnString(const int n, const char c, bool);"
	// The third parameter is not used. 
	// 
	OmnBeginTest << "Test OmnString(const int n, const char c)";
	mTcNameRoot = "String-CTR_B";
	OmnString s3(0, 'A');

	OmnCreateTc << (s3.length() == 0) << endtc;
	OmnCreateTc << (s3 == "") << endtc;

	OmnString s4(1, '*', true);
	OmnCreateTc << (s4.length() == 1) << endtc;
	OmnCreateTc << (s4 == "*") << endtc;

	OmnString s5(2, '|', true);
	OmnCreateTc << (s5.length() == 2) << endtc;
	OmnCreateTc << (s5 == "||") << endtc;

	OmnString s6(100, '&', true);
	OmnCreateTc << (s6.length() == 100) << endtc;

	OmnString s7(101, '^', true);
	OmnCreateTc << (s7.length() == 101) << endtc;

	OmnString s8(151, '.', true);
	OmnCreateTc << (s8.length() == 151) << endtc;

	OmnString s9(-1, '%', true);
	OmnCreateTc << (s9.length() == 0) << endtc;

	OmnString testMaxStringLength(OmnString::getMaxStringLength(), 'a', true);
	OmnTC(OmnExpected<int>(testMaxStringLength.length()), 
		OmnActual<int>(OmnString::getMaxStringLength())) << endtc;

	return true;
}


bool OmnStringTester::testTrimLastWord()
{
	OmnString str1 = "www aaa bbb";
	OmnString str2 = str1;
	str2 << " ccc";
	OmnString str3 = str2.trimLastWord();
	OmnTC(OmnExpected<OmnString>(str1), OmnActual<OmnString>(str3)) << endtc;

	return true;
}


bool
OmnStringTester::testRemoveLine()
{
	OmnString str0 = "1111111111\n2222222\n\r33333333\r44444444";
	OmnString str1 = "1111111111\n2222222\n\r33333333\r44444444";
	OmnString str2 = "2222222\n\r33333333\r44444444";
	OmnString str3 = "33333333\r44444444";
	OmnString str4 = "44444444";

	str1.removeLine(0);
	OmnTCBool(true, str1 == str2) << str1 << endtc;

	str1.removeLine(0);
	OmnTCBool(true, str1 == str3) << str1 << endtc;

	str1.removeLine(0);
	OmnTCBool(true, str1 == str4) << str1 << endtc;

	str1.removeLine(0);
	OmnTCBool(true, str1 == "") << str1 << endtc;

	str1 = str0;
	OmnString str5 = "11112222222\n\r33333333\r44444444";
	str1.removeLine(4);
	OmnTCBool(true, str1 == str5) << str1 << endtc;

	return true;
}


bool
OmnStringTester::testGetLine(const u32 tries)
{
	for (u32 i=0; i<tries; i++)
	{
		if (!testGetLine()) return false;
	}

	return true;
}


bool
OmnStringTester::testGetLine()
{
	const u32 arraySize = 10000;
	OmnString strs[arraySize];
	u32 numLines = 0;
	OmnString theStr;
	u32 tries; 
	if (aos_next_pct(50))
	{
		tries = aos_next_int(1, 30);
	}
	else
	{
		tries = aos_next_int(31, 500);
	}

	for (u32 i=0; i<tries; i++)
	{
		if (numLines >= arraySize) return true;
		int min = aos_next_int(1, 2);
		int max = aos_next_int(min+1, 20);
		u32 stop = (u32)aos_next_int(1, 50);
		for (u32 k=0; k<stop; k++)
		{
			// OmnString sss = aos_next_str(eAosStrType_DigitStr, min, max);
			OmnString sss = OmnRandom::digitStr(min, max);
			sss << "\n";
			strs[numLines] = sss;
			theStr << strs[numLines++];
		}

		OmnTCTrue(numLines > 0) << (int)numLines << endtc;

		for (u32 m=0; m<numLines; m++)
		{
			int index;
			int pct = aos_next_int(1, 100);
			if (pct < 50)
			{
				index = 1;
			}
			else
			{
				index = aos_next_int(1, numLines);
			}
			bool remove = aos_next_pct(50);

			OmnString s = theStr.getLine(remove, index);
			OmnTCTrue(index > 0) << index << endtc;

			OmnTCTrue(s == strs[index-1]) << "s = " << s.toHex() 
				<< ". The string: " << strs[index-1].toHex() 
				<< ". The line: " << index 
				<< ". Length1: " << s.length()
				<< ". Length2: " << strs[index-1].length() 
				<< ". numLines: " << (int)numLines 
				<< ". Index: " << index
				<< ". The String: " << theStr.toHex() << endtc;

			if (remove)
			{
				u32 a;
				for (a=index-1; a<numLines-1; a++)
				{
					strs[a] = strs[a+1];
				}
				numLines--;
				aos_assert_r(numLines >= 0, false);

				for (a=0; a<numLines; a++)
				{
					OmnString ss = theStr.getLine(false, a+1);
					OmnTCTrue(ss == strs[a]) << "ss: " << ss.toHex()
						<< ". strs[a]: " << strs[a].toHex()
						<< ". a: " << (int)a << endtc;
				}
			}
		}
	}

	return true;
}


bool
OmnStringTester::testReplace(const u32 tries)
{
	OmnString str1 = "aaaabbbbcccdddeeff";
	str1.replace("bbbb", "1111", true);
	OmnTCTrue(str1 == "aaaa1111cccdddeeff") << str1 << endtc;
	return true;
}


bool
OmnStringTester::testParsingValues(const u32)
{
	OmnString str1 = "aaaa1234bbbbbbb";
	int value; 
	u32 len;
	bool rslt = str1.parseInt(4, len, value);
	OmnCreateTc << (rslt) << endtc;
	OmnCreateTc << (value == 1234) << endtc;
	OmnCreateTc << (len == 4) << endtc;

	OmnString str2 = "aaaa+1234bbbbbbb";
	rslt = str2.parseInt(4, len, value);
	OmnCreateTc << (rslt) << endtc;
	OmnCreateTc << (value == 1234) << endtc;
	OmnCreateTc << (len == 5) << endtc;

	str2 = "aaaa-1234bbbbbbb";
	rslt = str2.parseInt(4, len, value);
	OmnCreateTc << (rslt) << endtc;
	OmnCreateTc << (value == -1234) << endtc;
	OmnCreateTc << (len == 5) << endtc;

	str2 = "aaaa1234bbbbbbb";
	u32 uvalue;
	rslt = str2.parseU32(4, len, uvalue);
	OmnCreateTc << (rslt) << endtc;
	OmnCreateTc << (uvalue == 1234) << endtc;
	OmnCreateTc << (len == 4) << endtc;

	return true;
}


bool
OmnStringTester::testFindSubString()
{
	OmnString str1 = "ldkafla k;lda fladkfj l;adkf ladkflakdfddddddddddlkaj dkla dklfadsf";
	OmnString str2 = "dddddddddd";
	u32 start_time = OmnGetSecond();
	for (int i=0; i<1000000000; i++)
	{
		str1.findSubString(str2, 0, false);
		if (i % 10000000 == 0)
		{
			u32 delta = OmnGetSecond() - start_time;
			if (delta)
			{
				OmnScreen << "Tried: " << i << ". Speed: " << i / delta << endl;
			}
		}
	}
	return true;
}


