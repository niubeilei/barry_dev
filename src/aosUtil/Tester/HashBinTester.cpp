////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashBinTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/HashBinTester.h"

#include "aosUtil/HashBin.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Random.h"
#include "aosUtil/ReturnCode.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"

#include "aosUtil/Tracer.h"

static u32 sgIndex = 0;

AosHashBinTester::AosHashBinTester()
{
	mName = "AosHashBinTester";
}


AosHashBinTester::~AosHashBinTester()
{
}
 
 
bool 
AosHashBinTester::start()
{
	OmnBeginTest << "AosHashBin Tester";

	normalTests();
	return true;
}


bool
AosHashBinTester::normalTests()
{
	// 
	// It tests the normal functions.
	//
	int ret;
	struct AosHashBin *hash;
	
	// 
	// Test Constructor
	//
	ret = AosHashBin_constructor(&hash, 999+1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(hash->buckets)) << endtc;	
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(hash->slab)) << endtc;	
	OmnTC(OmnExpected<int>(1023), OmnActual<int>(hash->tableSize)) << endtc;	
	OmnTC(OmnExpected<int>(0), OmnActual<int>(hash->errorBlockOverflow)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(hash->errorTableOverflow)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(hash->errorBlockAllocError)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(hash->errorGetEntryFail)) << endtc;	

	// 
	// Test Add. 
	//
	const int num_entries = 10;
	char **keys = (char **)aos_malloc(num_entries*4);
	u32 *keyLen = (u32 *)aos_malloc(num_entries*4);
	int *values = (int*)aos_malloc(num_entries*4);
	void *value = 0;
	OmnTrace << "To test HashBin: " << num_entries << endl;
	int i;
	for (i=0; i<num_entries; i++)
	{
		if ((++sgIndex & 0xfff) == 0) cout << sgIndex << endl;

		u32 key_len = (i & 0x0f);
		key_len += 4;

		keys[i] = (char *)aos_malloc(key_len);
		keyLen[i] = key_len;
		AosRandom_get(keys[i], key_len-1);
		keys[i][key_len-1] = i;

		values[i] = i;

		// 
		// Check whether it is already in the list
		//
		ret = AosHashBin_get(hash, keys[i], key_len, &value, 0);
		if (ret == 0)
		{
			// 
			// It is in the list. Check whether it should be in the list
			//
			for (int j=0; j<i; j++)
			{
				if (memcmp(keys[j], keys[i], keyLen[j]) == 0 &&
					keyLen[j] == key_len)
				{
					//
					// It is indeed in the list. Do not add it.
					//
					i--;
					continue;
				}
			}
		}

		// 
		// The key is not in the list. Add it to the hash table.
		//
		ret = AosHashBin_add(hash, keys[i], key_len, (void *)values[i], 0);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
		if (ret)
		{
			aos_trace_hex("Add failed", keys[i], key_len);
			AosHashBin_dump(hash, keys[i], key_len);
			exit(0);
		}

		// 
		// Retrieve it. It should be in the list. The retrieve should be successful
		//
		ret = AosHashBin_get(hash, keys[i], key_len, &value, 0);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
		OmnTC(OmnExpected<int>(i), OmnActual<int>((int)value)) << "i = " << i << endtc;	
		if (ret)
		{
			// 
			// This is an error.
			//
			aos_trace_hex("Retrieve failed", keys[i], key_len);
			exit(0);
		}

		ret = AosHashBin_get(hash, keys[i], key_len, &value, 0);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
		OmnTC(OmnExpected<int>(i), OmnActual<int>((int)value)) << "i = " << i << endtc;	
	}

	// 
	// Check all the keys are on the list
	//
	aos_list_head *entry;
	struct AosHashBin_entry *ee;
	bool found;
	for (i=0; i<num_entries; i++)
	{
		found = false;
		aos_list_for_each(entry, &hash->list)
		{
			ee = (struct AosHashBin_entry *)entry;
			if (ee->key_len == keyLen[i] &&
				memcmp(ee->key1, keys[i], keyLen[i]) == 0)
			{
				found = true;
				break;
			}
		}

		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(found)) << i << endtc;
	}

	// 
	// Print all the keys
	//
	// aos_list_for_each(entry, &hash->list)
	// {
	// 	ee = (struct AosHashBin_entry *)entry;
	// 	aos_trace_hex("Key", (char *)ee->key, ee->key_len);
	// }

	// 
	// Remove all the entries
	//
	for (i=0; i<num_entries; i++)
	{
		ret = AosHashBin_get(hash, keys[i], keyLen[i], &value, 1);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) 
			<< (int)keyLen[i] << ":" << i << endtc;	
		OmnTC(OmnExpected<int>(i), OmnActual<int>((int)value)) 
			<< "i = " << i << endtc;	
		if (ret)
		{
			aos_trace_hex("Remove Key", keys[i], keyLen[i]);
		}

	}

	OmnTC(OmnExpected<int>(1), OmnActual<int>(aos_list_empty(&hash->list))) << endtc;

	// 
	// Make sure these entries are not in the list 
	//
	for (i=0; i<num_entries; i++)
    {
        ret = AosHashBin_get(hash, keys[i], keyLen[i], &value, 1);
        OmnTC(OmnExpected<int>(-eAosRc_EntryNotFound), OmnActual<int>(ret)) << endtc;
    }

	// 
	// Free the memory
	//
	for (i=0; i<num_entries; i++)
	{
		aos_free(keys[i]);
	}
	
	aos_free(keys);
	aos_free(keyLen);
	aos_free(values);
	return true;
}
  

bool
AosHashBinTester::abnormalTests()
{
	return true;
}


bool
AosHashBinTester::addSameKey()
{
	int ret;
	struct AosHashBin *hash;
	
	// 
	// Test Constructor
	//
	ret = AosHashBin_constructor(&hash, 999+1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	// 
	// Test Add. 
	//
	void *value;

	int value1 = 10;
	char key1[] = {0x3a, 0xeb, 0x47, 0x71, 0xee, 0x90, 0x00, 0x00, 0x00, 0x06};
	ret = AosHashBin_add(hash, key1, 10, (void *)value1, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	ret = AosHashBin_get(hash, key1, 10, &value, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
	OmnTC(OmnExpected<int>(value1), OmnActual<int>((int)value)) << endtc;	

	int value2 = 10;
	char key2[] = {0xe5, 0xf4, 0x4a, 0x06, 0x7a, 0x80, 0x77, 0x00, 0x00, 0x00, 0x07};
	ret = AosHashBin_add(hash, key2, 11, (void *)value2, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	ret = AosHashBin_get(hash, key2, 11, &value, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
	OmnTC(OmnExpected<int>(value2), OmnActual<int>((int)value)) << endtc;	

	ret = AosHashBin_get(hash, key1, 10, &value, 1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	ret = AosHashBin_get(hash, key2, 11, &value, 1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	return true;
}
  

bool
AosHashBinTester::special1()
{
#if 0
	int ret;
	struct AosHashBin *hash;
	int value;
	
	// 
	// Test Constructor
	//
	ret = AosHashBin_constructor(&hash, 999+1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	ret = AosHashBin_constructor(&hash, 999+1);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

    // char key0[] = {0xa0, 0x82, 0xa3, 0x99, 0x6d, 0xdb, 0xf6, 0x44, 0x00};
    // ret = AosHashBin_add(hash, (void *)key0, 9, (void *)value, 0);

    char key1[] = {0x33, 0x0d, 0x86, 0x4b, 0x75, 0x9d, 0xe7, 0x39, 0x2f, 0x0b, 0x02, 0xc0, 0xf7, 0xfd, 0xbf, 0xef, 0x11, 0x6e};
    ret = AosHashBin_add(hash, (void *)key1, 18, (void *)value, 0);
    char key2[] = {0x78, 0xd0, 0xb6, 0x74, 0xd7, 0xb5, 0x6d, 0x1b, 0xb9, 0xae, 0x2b, 0xc8};
    ret = AosHashBin_add(hash, (void *)key2, 12, (void *)value, 0);
    char key3[] = {0x8f, 0x9c, 0xa9, 0x36, 0xbd, 0xef, 0x3b, 0xce, 0x81, 0xa0, 0xa8, 0x58};
    ret = AosHashBin_add(hash, (void *)key3, 12, (void *)value, 0);
    char key4[] = {0x97, 0x0c, 0x85, 0x37, 0x9a, 0x66, 0x19, 0x46, 0xd2, 0xf4, 0x7d, 0xdf, 0xd6, 0xb5, 0x6d, 0x1b, 0x2d};
    ret = AosHashBin_add(hash, (void *)key4, 17, (void *)value, 0);
    char key5[] = {0xf4, 0x3d, 0x52, 0x43, 0x66, 0xd9, 0x36, 0x8d, 0xe5, 0x39, 0x8e, 0xa3, 0xdf, 0x6a};
    ret = AosHashBin_add(hash, (void *)key5, 14, (void *)value, 0);
    char key6[] = {0x33, 0x6d, 0x1e, 0x4b, 0x80, 0x20, 0x08, 0x82, 0x1f, 0x47, 0x07};
    ret = AosHashBin_add(hash, (void *)key6, 11, (void *)value, 0);
    char key7[] = {0x84, 0x02, 0x03, 0x95, 0xff, 0xbf, 0x2f, 0xcb, 0x2f, 0xcb, 0xf2, 0xbc, 0xc8, 0x32, 0x4c, 0x1c};
    ret = AosHashBin_add(hash, (void *)key7, 16, (void *)value, 0);
    char key8[] = {0xb6, 0xce, 0xf6, 0x5b, 0x4f, 0x13, 0x04, 0x01, 0x62, 0xd8, 0xf6, 0xbd, 0x8d, 0xaa};
    ret = AosHashBin_add(hash, (void *)key8, 14, (void *)value, 0);
    char key9[] = {0x43, 0x61, 0xdb, 0x8d, 0xe9, 0xba, 0xae, 0xeb, 0x14, 0x45, 0x91, 0x24, 0x50, 0xba};
    ret = AosHashBin_add(hash, (void *)key9, 14, (void *)value, 0);
    char key10[] = {0x6c, 0x94, 0xe7, 0xb2, 0x67, 0x99, 0x66, 0x19, 0xd9, 0x76, 0x5d, 0x97, 0x29, 0x0a, 0x42, 0x50, 0xdb, 0x36, 0x6f};
    ret = AosHashBin_add(hash, (void *)key10, 19, (void *)value, 0);
    char key11[] = {0xdb, 0x83, 0xe3, 0xa0, 0x8f, 0x23, 0xc8, 0xb2, 0xc9, 0x32, 0x0c, 0xc3, 0x49};
    ret = AosHashBin_add(hash, (void *)key11, 13, (void *)value, 0);
    char key12[] = {0x96, 0xca, 0x75, 0x17, 0xd3, 0xb4, 0xad, 0x6b, 0x33, 0x0c, 0xc3, 0xf0, 0x06, 0x81, 0x60, 0x7c};
    ret = AosHashBin_add(hash, (void *)key12, 16, (void *)value, 0);
    char key13[] = {0xb9, 0x50, 0x56, 0x7c, 0x28, 0x0a, 0x82, 0xa0, 0x34, 0x4d, 0x53, 0xd4, 0xe6, 0xf9, 0xbe, 0x6c};
    ret = AosHashBin_add(hash, (void *)key13, 16, (void *)value, 0);
    char key14[] = {0x8c, 0x16, 0x48, 0xd6, 0x78, 0x9e, 0x67, 0x99, 0x1e, 0xc7, 0x31, 0xcc, 0xbe, 0xea};
    ret = AosHashBin_add(hash, (void *)key14, 14, (void *)value, 0);
    char key15[] = {0xa5, 0x5a, 0x59, 0x19, 0xc1, 0xb0, 0xec, 0xbb, 0xfa, 0x7e, 0x5f, 0x17, 0xa9};
    ret = AosHashBin_add(hash, (void *)key15, 13, (void *)value, 0);
    char key16[] = {0xc6, 0x30, 0x4e, 0x7d, 0x3c, 0x4f, 0xd3, 0x74, 0x98, 0x26, 0x89, 0xa2, 0xe1, 0xf8, 0x4b};
    ret = AosHashBin_add(hash, (void *)key16, 15, (void *)value, 0);
    char key17[] = {0x0b, 0x8f, 0x26, 0x20};
    ret = AosHashBin_add(hash, (void *)key17, 4, (void *)value, 0);
    char key18[] = {0x07, 0x57, 0xd8, 0xe5, 0xf5, 0x3d, 0x0f, 0xc3, 0x21, 0x08, 0x42, 0x90, 0x83, 0xe0, 0x38, 0x4e, 0xcf, 0x8e};
    ret = AosHashBin_add(hash, (void *)key18, 18, (void *)value, 0);
    char key19[] = {0xd6, 0x81, 0xa3, 0x7f, 0x4e, 0x13, 0xc4, 0xf1, 0x88, 0xe2, 0xa7};
    ret = AosHashBin_add(hash, (void *)key19, 11, (void *)value, 0);
    char key20[] = {0x38, 0x0b, 0x85, 0x2c, 0x77, 0xdd, 0xf7, 0x3d, 0xf2, 0xbc, 0xaf, 0x2b, 0xce, 0x33, 0x9b};
    ret = AosHashBin_add(hash, (void *)key20, 15, (void *)value, 0);
    char key21[] = {0xc9, 0xd2, 0xb7, 0x9e, 0x2b, 0x0a, 0x42, 0x10, 0xf4, 0x7d, 0x9f, 0x27, 0x73, 0x5c, 0xd7, 0x8c};
    ret = AosHashBin_add(hash, (void *)key21, 16, (void *)value, 0);
    char key22[] = {0xca, 0x26, 0x4c, 0xde, 0xcd, 0x73, 0x5c, 0x57, 0xf0, 0xbc, 0x6f, 0xb8};
    ret = AosHashBin_add(hash, (void *)key22, 12, (void *)value, 0);
    char key23[] = {0xeb, 0xc1, 0x73, 0x82, 0x4f, 0xd3, 0x74, 0x5d, 0x46, 0x91, 0x24, 0x09, 0xca, 0x32, 0xcc, 0x73, 0x4d};
    ret = AosHashBin_add(hash, (void *)key23, 17, (void *)value, 0);
    char key24[] = {0xbd, 0x04, 0xc3, 0xbc, 0xbb, 0xee, 0xfb, 0x3e, 0x46, 0x91, 0xa7};
    ret = AosHashBin_add(hash, (void *)key24, 11, (void *)value, 0);
    char key25[] = {0x53, 0x48, 0x94, 0xef, 0x17, 0x45, 0x51, 0x94, 0x69, 0xda, 0x76, 0x9d, 0x41, 0x90, 0x24, 0x89, 0xed};
    ret = AosHashBin_add(hash, (void *)key25, 17, (void *)value, 0);
    char key26[] = {0xe7, 0xdb, 0x79, 0x21, 0xfd, 0x7f, 0x1f, 0x07, 0x09, 0x02, 0x80, 0xe0, 0xf1, 0x3c, 0x0f, 0x43, 0x8a, 0xee};
    ret = AosHashBin_add(hash, (void *)key26, 18, (void *)value, 0);
    char key27[] = {0x33, 0x47, 0xd4, 0xeb, 0x32, 0x4c, 0x53, 0xd4, 0xe6, 0xb9, 0x2e, 0x8b, 0x40, 0xea};
    ret = AosHashBin_add(hash, (void *)key27, 14, (void *)value, 0);
    char key28[] = {0x91, 0x0a, 0x05, 0x40};
    ret = AosHashBin_add(hash, (void *)key28, 4, (void *)value, 0);
    char key29[] = {0x81, 0x86, 0xa4, 0xd5, 0xc8, 0xf2, 0x7c, 0x1f, 0xb7, 0x6d, 0xa7};
    ret = AosHashBin_add(hash, (void *)key29, 11, (void *)value, 0);
    char key30[] = {0x5c, 0x80, 0xa2, 0x70, 0x69, 0x9a, 0xa6, 0x29, 0x2b, 0x0a, 0x42, 0x90, 0xff, 0x7f, 0x9f, 0xfc};
    ret = AosHashBin_add(hash, (void *)key30, 16, (void *)value, 0);
    char key31[] = {0x8b, 0x72, 0x9f, 0x96, 0xa6, 0x29, 0x8a, 0x62, 0x51, 0x14, 0x45, 0xd1, 0xe4, 0x79, 0xdb};
    ret = AosHashBin_add(hash, (void *)key31, 15, (void *)value, 0);
    char key32[] = {0x44, 0x4b, 0x55, 0x2d, 0xfa, 0xfe, 0x3f, 0xcf, 0x47, 0x91, 0xe4, 0xf9, 0x4f, 0x13, 0x84, 0x61, 0x0d};
    ret = AosHashBin_add(hash, (void *)key32, 17, (void *)value, 0);
    char key33[] = {0x94, 0x08, 0x04, 0x40};
    ret = AosHashBin_add(hash, (void *)key33, 4, (void *)value, 0);
    char key34[] = {0x10, 0xdf, 0xfa, 0x67, 0x37, 0x0d, 0x83, 0x20, 0x47, 0xd1, 0xf4, 0xfd, 0x04, 0x81, 0xdb};
    ret = AosHashBin_add(hash, (void *)key34, 15, (void *)value, 0);
    char key35[] = {0xb8, 0x62, 0xdb, 0x9c, 0x59, 0x56, 0x95, 0xa5, 0x85, 0x61, 0xd8, 0xa8};
    ret = AosHashBin_add(hash, (void *)key35, 12, (void *)value, 0);
    char key36[] = {0x74, 0x3e, 0x92, 0x53, 0xf8, 0xfe, 0xbf, 0x6f, 0x90, 0x64, 0x59, 0xd8};
    ret = AosHashBin_add(hash, (void *)key36, 12, (void *)value, 0);
    char key37[] = {0x07, 0x37, 0xcf, 0xe5, 0x88, 0xe2, 0xb8, 0xae, 0x75, 0x9d, 0x27, 0x49, 0xa3, 0xa8, 0x6a, 0x1a, 0x0d};
    ret = AosHashBin_add(hash, (void *)key37, 17, (void *)value, 0);
    char key38[] = {0xaf, 0xd0, 0x75, 0x7a, 0xd7, 0xb5, 0x6d, 0x1b, 0xb9, 0xae, 0xb7};
    ret = AosHashBin_add(hash, (void *)key38, 11, (void *)value, 0);
    char key39[] = {0x26, 0x47, 0x53, 0xe9, 0x32, 0x4c, 0x53, 0xd4, 0xe6, 0xb9, 0x2e, 0x8b, 0x40, 0x10, 0x84, 0xe1, 0xa7, 0x5e};
    ret = AosHashBin_add(hash, (void *)key39, 18, (void *)value, 0);
    char key40[] = {0x1d, 0xed, 0x3d, 0x48, 0x51, 0x14, 0x85, 0x61, 0xc3, 0x70, 0xdc, 0xf7, 0xc7, 0x71, 0x5c, 0x6c};
    ret = AosHashBin_add(hash, (void *)key40, 16, (void *)value, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
    char key41[] = {0x1e, 0xf9, 0x41, 0x08, 0x15, 0x05, 0x81, 0x20, 0x05};
    ret = AosHashBin_add(hash, (void *)key41, 9, (void *)value, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
    char key42[] = {0x86, 0x92, 0xa6, 0x95, 0x7e, 0x5f, 0x97, 0xa5, 0x0c, 0xc3, 0xf0, 0x7c, 0x79};
    ret = AosHashBin_add(hash, (void *)key42, 13, (void *)value, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	

	char key43[] = {0xa0,0x82,0xa3,0x99,0x6d,0xdb,0xf6,0x44,0x00}; 

	void *v1;
	ret = AosHashBin_get(hash, (void *)key43, 9, &v1, 0);
OmnTrace << "Ret = " << ret << endl;

    ret = AosHashBin_add(hash, (void *)key43, 9, (void *)value, 0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;	
#endif

	return true;
}

