////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SmartIndexValListTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/SmartIndexValListTester.h"

#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/SmtIdxVList.h"


bool OmnSIVTester::start()
{
	// 
	// Test default constructor
	//
	OmnSIVTE defaultVal;
	defaultVal.mKey = "Default";
	defaultVal.mValue = -1;

	OmnBeginTest << "Test Default Constructor";
	mTcNameRoot = "SmtIdxVList-DFT-CTR";
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Set the default value
	//
	mList.setDefaultValue(defaultVal);
	OmnCreateTc << (mList.get("chen", false) == defaultVal) << endtc;

	// 
	// Test append(...)
	//
	OmnSIVTE entry;
	entry.mKey = "chen";
	entry.mValue = 2;
	mList.append(entry, true);
	mTcNameRoot = "SmtIdxVList-append1";
	OmnCreateTc << (mList.entries() == 1) << endtc;
	OmnCreateTc << (mList.get("chen") == entry) << endtc;
	OmnCreateTc << (mList.get("chen", false) == entry) << endtc;
	OmnCreateTc << (mList.get("chen", true) == entry) << endtc;
	OmnCreateTc << (mList.get("chen", true) == defaultVal) << endtc;
	OmnCreateTc << (mList.get("chen", false) == defaultVal) << endtc;
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Test append(entry, false)
	//
	mTcNameRoot = "SmtIdxVList-append2";
	mList.append(entry, false);
	mList.append(entry, false);	// Now there shall be only one entry with the same key
	OmnCreateTc << (mList.entries() == 1) << endtc;
	OmnCreateTc << (mList.get("chen", false) == entry) << endtc;
	OmnCreateTc << (mList.get("chen", true) == entry) << endtc;
	OmnCreateTc << (mList.get("chen", true) == defaultVal) << endtc;
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Test append(entry, true)
	//
	mList.clear();
	mList.append(entry, false);
	entry.mValue = 100;
	mTcNameRoot = "SmtIdxVList-append3";
	mList.append(entry, true);	// Now there shall be only one entry with the same key
	OmnCreateTc << (mList.entries() == 1) << endtc;
	OmnCreateTc << (mList.get("chen", false) == entry) << endtc;
	OmnCreateTc << (mList.get("chen", true) == entry) << endtc;
	OmnCreateTc << (mList.get("chen", true) == defaultVal) << endtc;
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Test null key
	//
	OmnSIVTE entry1;
	entry1.mKey = "";
	entry1.mValue = 2;
	mList.append(entry1, false);
	mTcNameRoot = "SmtIdxVList-null";
	OmnCreateTc << (mList.entries() == 1) << endtc;
	OmnCreateTc << (mList.get("", false) == entry1) << endtc;
	OmnCreateTc << (mList.get("chen", true) == defaultVal) << endtc;
	OmnCreateTc << (mList.get("", false) == entry1) << endtc;
	OmnCreateTc << (mList.get("", true) == entry1) << endtc;
	OmnCreateTc << (mList.get("", false) == defaultVal) << endtc;
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Test clear
	//
	createList(100);
	mList.clear();
	mTcNameRoot = "SmtIdxVList-clear";
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Test popFront(...)
	//
	int entriesToTest = 500;
	createList(entriesToTest);
	mTcNameRoot = "SmtIdxVList-popFront";
	mList.entries();
	for (int i=0; i<entriesToTest; i++)
	{
		OmnSIVTE en;
		en.mKey << "Key" << i;
		en.mValue = i;
		OmnSIVTE en1 = mList.popFront();
		OmnCreateTc << (en1 == en) 
			<< en1.mKey 
			<< ":" 
			<< 5
			<< en1.mValue 
			<< "   " 
			<< en.mKey 
			<< ":" 
			<< en.mValue 
			<< endtc;
	}

	// 
	// Test looping, using crtValue1()
	// 
	mTcNameRoot = "SmtIdxVList-crtValue";
	mList.resetLoop();
	int k=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Key" << k;
		en1.mValue = k++;
		OmnSIVTE en2 = mList.crtValue();
		mList.next();
		OmnCreateTc << (en1 == en2) << endtc;
	}

	// 
	// Test looping, using crtValue() const
	// 
	mTcNameRoot = "SmtIdxVList-crtValue2";
	mList.resetLoop();
	int j=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Key" << j;
		en1.mValue = j++;
		const OmnSIVTE en2 = mList.crtValue();
		mList.next();
		OmnCreateTc << (en1 == en2) << endtc;
	}

	// 
	// Test looping, using next(), not crtValue()
	// 
	mTcNameRoot = "SmtIdxVList-next";
	mList.resetLoop();
	int m=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Key" << m;
		en1.mValue = m++;
		OmnSIVTE en2 = mList.next();
		OmnCreateTc << (en1 == en2) << endtc;
	}

	// 
	// Test eraseCrt()
	// 
	mTcNameRoot = "SmtIdxVList-eraseCrt";
	mList.resetLoop();
	int n=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Key" << n;
		en1.mValue = n++;
		OmnSIVTE en2 = mList.crtValue();
		OmnCreateTc << (en1 == en2) << endtc;
		OmnCreateTc << ((int)mList.entries() == entriesToTest - n) << endtc;
		mList.eraseCrt();
		OmnCreateTc << ((int)mList.entries() == entriesToTest - n - 1) << endtc;
	}
	OmnCreateTc << (mList.entries() == 0) << endtc;

	// 
	// Test insert(...). We will loop on the entries and insert one for 
	// each entry we looped.
	//
	mTcNameRoot = "SmtIdxVList-insert";
	createList(entriesToTest);
	mList.resetLoop();
	int p=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Key" << p;
		en1.mValue = p;
		OmnSIVTE en2 = mList.crtValue();
		OmnCreateTc << (en1 == en2) << endtc;

		en1.mKey = "Insert";
		en1.mKey << p;
		mList.insert(en1);
		en1 = mList.crtValue();
		OmnCreateTc << (en1 == en2) 
			<< en1.toString() << "---" << en2.toString() << endtc;

		mList.next();
		p++;
	}
	OmnCreateTc << ((int)mList.entries() == entriesToTest * 2) << endtc;

	// 
	// Retrieve all entries using crtValue()
	//
	mTcNameRoot = "SmtIdxVList-retrieve1";
	mList.resetLoop();
	int q=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Insert" << q;
		en1.mValue = q;

		OmnSIVTE en2; 
		en2.mKey << "Key" << q;
		en2.mValue = q++;

		OmnSIVTE en3 = mList.crtValue(); mList.next();
		OmnSIVTE en4 = mList.crtValue(); mList.next();

		OmnCreateTc << (en1 == en3) 
			<< en1.toString() << "---" << en3.toString() << endtc;
		OmnCreateTc << (en2 == en4) 
			<< en2.toString() << "---" << en4.toString() << endtc;
	}
	OmnCreateTc << ((int)mList.entries() == entriesToTest * 2) << endtc;

	// 
	// Retrieve all entries using crtValue() const
	//
	mTcNameRoot = "SmtIdxVList-retrieve2";
	mList.resetLoop();
	int i1=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Insert" << i1;
		en1.mValue = i1;

		OmnSIVTE en2; 
		en2.mKey << "Key" << i1;
		en2.mValue = i1++;

		const OmnSIVTE en3 = mList.crtValue(); mList.next();
		const OmnSIVTE en4 = mList.crtValue(); mList.next();

		OmnCreateTc << (en1 == en3) << endtc;
		OmnCreateTc << (en2 == en4) << endtc;
	}
	OmnCreateTc << ((int)mList.entries() == entriesToTest * 2) << endtc;

	// 
	// Retrieve all entries using next()
	//
	mTcNameRoot = "SmtIdxVList-retrieve3";
	mList.resetLoop();
	int i2=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		en1.mKey << "Insert" << i2;
		en1.mValue = i2;

		OmnSIVTE en2; 
		en2.mKey << "Key" << i2;
		en2.mValue = i2++;

		const OmnSIVTE en3 = mList.next();
		const OmnSIVTE en4 = mList.next();

		OmnCreateTc << (en1 == en3) << endtc;
		OmnCreateTc << (en2 == en4) << endtc;
	}
	OmnCreateTc << ((int)mList.entries() == entriesToTest * 2) << endtc;

	// 
	// Retrieve all entries using get(key, false)
	//
	mTcNameRoot = "SmtIdxVList-retrieve4";
	mList.resetLoop();
	int i3=0;
	while (mList.hasMore())
	{
		OmnSIVTE en1;
		OmnString key1 = "Key"; key1 << i3;
		en1.mKey = key1;
		en1.mValue = i3;

		OmnSIVTE en2; 
		OmnString key2 = "Insert"; key2 << i3;
		en2.mKey = key2;
		en2.mValue = i3++;

		const OmnSIVTE en3 = mList.get(key1); mList.next();
		const OmnSIVTE en4 = mList.get(key2); mList.next();

		OmnCreateTc << (en1 == en3) << endtc;
		OmnCreateTc << (en2 == en4) << endtc;
	}
	OmnCreateTc << ((int)mList.entries() == entriesToTest * 2) << endtc;

	// 
	// Retrieve all entries using get(key, true)
	//
	mTcNameRoot = "SmtIdxVList-retrieve5";
	for (int i4=0; i4<entriesToTest; i4++)
	{
		OmnSIVTE en1;
		OmnString key1 = "Key"; key1 << i4;
		en1.mKey = key1;
		en1.mValue = i4;

		OmnSIVTE en2; 
		OmnString key2 = "Insert"; key2 << i4;
		en2.mKey = key2;
		en2.mValue = i4;

		const OmnSIVTE en3 = mList.get(key1, true); mList.next();
		const OmnSIVTE en4 = mList.get(key2, true); mList.next();

		OmnCreateTc << (en1 == en3) << endtc;
		OmnCreateTc << (en2 == en4) << endtc;
		OmnCreateTc << ((int)mList.entries() == entriesToTest * 2 - (i4+1) * 2) 
			<< "Expected: " 
			<< (int)entriesToTest*2 - (i4+1)*2
			<< (char*)". Actual: " 
			<< (int)mList.entries() 
			<< endtc;
	}
	OmnCreateTc << (mList.entries() == 0) 
		<< "Expected: " << 0
		<< ". Actual: " << (int)mList.entries() << endtc;

	// 
	// Retrieve all entries using get(key, false), shall found none
	//
	mTcNameRoot = "SmtIdxVList-retrieve6";
	mList.resetLoop();
	int i5=0;
	while (mList.hasMore())
	{
		OmnString key1 = "Key"; key1 << i5;
		OmnString key2 = "Insert"; key2 << i5++;

		const OmnSIVTE en3 = mList.get(key1); mList.next();
		const OmnSIVTE en4 = mList.get(key2); mList.next();

		OmnCreateTc << (defaultVal == en3) << endtc;
		OmnCreateTc << (defaultVal == en4) << endtc;
	}
	OmnCreateTc << (mList.entries() == 0) << endtc;

	return true;
}


void
OmnSIVTester::createList(const int howMany)
{
	mList.clear();
	for (int i=0; i<howMany; i++)
	{
		OmnSIVTE en;
		OmnString key = "Key";
		key << i;
		en.mKey = key;
		en.mValue = i;
		mList.append(en, true);
		OmnCreateTc << ((int)mList.entries() == i+1) << endtc;
		OmnCreateTc << (mList.get(key) == en) << endtc;
	}

	mList.entries();
}
