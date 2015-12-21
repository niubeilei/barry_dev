////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/ValueRslt.h"

const int sgNumElems = 1000 * 1000 * 10;		// 10 Millions

class AosTestValue
{
public:
};

class AosTestValueU64 : public AosTestValue
{
private:
	u64 mValue;

public:
	AosTestValueU64(const u64 &vv)
	:
	mValue(vv)
	{
	}
};


class AosTestValueStr : public AosTestValue
{
private:
	char mData[100];
	char *mDataPtr;

public:
	AosTestValueStr(const char *data)
	{
		int len = strlen(data);
		if (len > 15)
		{
			mDataPtr = new char[len+1];
		}
		else
		{
		 	mDataPtr = mData;
		}
		strcpy(mDataPtr, data);
	}

	~AosTestValueStr()
	{
		if (mDataPtr != mData)
		{
			delete [] mDataPtr;
		}
		mDataPtr = 0;
	}
};


class AosTestValueWrap
{
private:
	AosTestValue *mMember;

public:
	AosTestValueWrap(const u64 &vv)
	{
		mMember = new AosTestValueU64(vv);
	}

	~AosTestValueWrap()
	{
		delete mMember;
		mMember = 0;
	}
};

void AosValueRsltTesterCreateStrings()
{
	AosValueRslt ** array1 = new AosValueRslt*[sgNumElems];
	u64 time1 = OmnGetTimestamp();
	cout << "sizeof AosValueRslt: " << sizeof(AosValueRslt) << endl;
	for (int i=0; i<sgNumElems; i++)
	{
		array1[i] = new AosValueRslt("1234567890");
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million AosValueRslt Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;

	u64 time3 = OmnGetTimestamp();
	for (int i=0; i<sgNumElems; i++)
	{
		delete array1[i];
	}
	u64 time4 = OmnGetTimestamp();

	delete [] array1;
	cout << "10 million AosValueRslt Delete: " 
		<< (time4 - time3) / 1000 << "ms" << endl;
}


void AosValueRsltTesterReadStrings()
{
	AosValueRslt rslt;
	u64 time1 = OmnGetTimestamp();
	OmnString str = "123567890123556677";
	const char *data = str.data();
	for (int i=0; i<sgNumElems; i++)
	{
		rslt.setCharStr1(data, 10, false);
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million setValue Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;
}


void AosValueRsltTesterCreateChar()
{
	// Create char *
	u64 time1 = OmnGetTimestamp();
	char **array2 = new char*[sgNumElems];
	for (int i=0; i<sgNumElems; i++)
	{
		array2[i] = new char[10];
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million char * Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;

	u64 time3 = OmnGetTimestamp();
	for (int i=0; i<sgNumElems; i++)
	{
		delete array2[i];
	}
	delete [] array2;
	u64 time4 = OmnGetTimestamp();

	cout << "10 million AosValueRslt Delete: " 
		<< (time4 - time3) / 1000 << "ms" << endl;
}


void AosValueRsltTesterCreateU64()
{
	// Create u64
	u64 time1 = OmnGetTimestamp();
	AosTestValueU64 **array3 = new AosTestValueU64*[sgNumElems];
	for (int i=0; i<sgNumElems; i++)
	{
		array3[i] = new AosTestValueU64(i);
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million U64 Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;

	u64 time3 = OmnGetTimestamp();
	for (int i=0; i<sgNumElems; i++)
	{
		delete array3[i];
	}
	delete [] array3;
	u64 time4 = OmnGetTimestamp();
	cout << "10 million U64 Deletion: " 
		<< (time4 - time3) / 1000 << "ms" << endl;
}


void AosValueRsltTesterCreateStr()
{
	// Create u64
	u64 time1 = OmnGetTimestamp();
	AosTestValueStr **array3 = new AosTestValueStr*[sgNumElems];
	for (int i=0; i<sgNumElems; i++)
	{
		if (i % 5 == 0)
		{
			array3[i] = new AosTestValueStr("12345678901234567890");
		}
		else
		{
			array3[i] = new AosTestValueStr("1234567890");
		}
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million String Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;

	u64 time3 = OmnGetTimestamp();
	for (int i=0; i<sgNumElems; i++)
	{
		delete array3[i];
	}
	delete [] array3;
	u64 time4 = OmnGetTimestamp();
	cout << "10 million String Deletion: " 
		<< (time4 - time3) / 1000 << "ms" << endl;
}


void AosValueRsltTesterCreateRawU64()
{
	// Create u64
	u64 time1 = OmnGetTimestamp();
	u64 **array = new u64*[sgNumElems];
	for (int i=0; i<sgNumElems; i++)
	{
		array[i] = new u64;
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million Raw U64 Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;

	u64 time3 = OmnGetTimestamp();
	for (int i=0; i<sgNumElems; i++)
	{
		delete array[i];
	}
	delete [] array;
	u64 time4 = OmnGetTimestamp();
	cout << "10 million Raw U64 Deletion: " 
		<< (time4 - time3) / 1000 << "ms" << endl;
}


void AosValueRsltTesterValueWrapU64()
{
	// Create u64
	u64 time1 = OmnGetTimestamp();
	AosTestValueWrap **array = new AosTestValueWrap*[sgNumElems];
	for (int i=0; i<sgNumElems; i++)
	{
		array[i] = new AosTestValueWrap((u64)i);
	}
	u64 time2 = OmnGetTimestamp();
	cout << "10 million ValueTestU64 Time used: " 
		<< (time2 - time1) / 1000 << "ms" << endl;

	u64 time3 = OmnGetTimestamp();
	for (int i=0; i<sgNumElems; i++)
	{
		delete array[i];
	}
	delete [] array;
	u64 time4 = OmnGetTimestamp();
	cout << "10 million ValueTestU64 Deletion: " 
		<< (time4 - time3) / 1000 << "ms" << endl;
}

void AosTestValueRslt()
{

	// Create Strings
	cout << "sizeof AosValueRslt: " << sizeof(AosValueRslt) << endl;
	cout << "sizeof AosTestValue: " << sizeof(AosTestValue) << endl;
	cout << "sizeof AosTestValueU64: " << sizeof(AosTestValueU64) << endl;
	AosValueRsltTesterReadStrings();
	// AosValueRsltTesterCreateStrings();
	// AosValueRsltTesterCreateChar();
	// AosValueRsltTesterCreateU64();
	// AosValueRsltTesterCreateStr();
	// AosValueRsltTesterCreateRawU64();
	// AosValueRsltTesterValueWrapU64();
}

#endif
