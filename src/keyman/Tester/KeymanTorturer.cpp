////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: KeymanTorturer.cpp
//// Description:
////   
////
//// Modification History:
//// 11/21/2006      Created by Harry Long
////
//////////////////////////////////////////////////////////////////////////////
//
#include "KeymanTorturer.h"
#include "keyman.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tracer/Tracer.h"
#include "Util/Random.h"
#include "Util/RandomSelector.h"


AosKeymanTorturer::AosKeymanTorturer()
:
mNumKeys(0)
{
}


bool AosKeymanTorturer::start()
{
	OmnBeginTest << "Begin the Keyman Testing";
	mTcNameRoot = "Keyman_Torturer";

	testKeyManagement();
	testGenerateMac();
	testDataEncDec();
	testAdd_Get();
	return true;
}


static int sgOprIndexKM[] = {0, 1, 2};
static u16 sgOprWeightsKM[] = {45, 30, 25};


bool 
AosKeymanTorturer::testKeyManagement()
{
	int tries = 100;


	AosRandomSelector oprSelector(sgOprIndexKM, sgOprWeightsKM, 3);

	bool rslt;
	while (tries)
	{
		// Determine the operations: add, delete, or get
		int opr = oprSelector.next();
		switch (opr)
		{
		case 0:
			 // Add a key
			 rslt = addKey(); 
			 OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << "Failed to add key" << endtc;
			 if (!rslt) return false;
			 break;

		case 1:
		     // Remove a key
			 rslt = removeKey(); 
			 OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << "Failed to remove key" << endtc;
			 if (!rslt) return false;

			 break;

	    case 2:
		     // Get a key
			 rslt = getKey(); 
			 OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << "Failed to get key" << endtc;
			 if (!rslt) return false;
			 break;

	    default:
		     OmnAlarm << "Unrecognized operation: " << opr << enderr;
			 return false;
		}

		tries--;
	}

	return true;
}


bool
AosKeymanTorturer::addKey()
{
	char name[33];
	int namelen = 32;
	char key[33];
	int keylen = 32;

	if (mNumKeys >= eMaxKeys)
	{
		return true;
	}

	OmnRandom::nextStr(name, 1, 32);
	int len = strlen(name);
	for(int i =0; i<len; i++)
	{
		if(name[i] == ' ') name[i] = 'a';
	}
	OmnRandom::nextStr(key, 1, 32);

	// We need to check whether the name is already there
	int ret = aos_keymagmt_add_key(name, key);
	if (keyExist(name)>=0) 
	{
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret<0)) << endtc;	 
	}
	else
	{
		addKey(name,key);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}

	return true;
}


bool
AosKeymanTorturer::addKey(char *name, char *key)
{
	if (mNumKeys >= eMaxKeys)
	{
		OmnAlarm << "Too many keys: " << mNumKeys << enderr;
		return false;
	}

	mNames[mNumKeys] = name;
	mKeys[mNumKeys] = key;
	mNumKeys++;
	return true;
}


bool
AosKeymanTorturer::removeKey()
{
	// Determine whether to remove a valid key or invalid key

	if (mNumKeys == 0)
	{
		return true;
	}
	
	int valid = OmnRandom::nextInt(0, 3);
	if (valid)
	{
		int index = OmnRandom::nextInt(0, mNumKeys-1);
		int ret = aos_keymagmt_remove_key((char *)mNames[index].data());
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		
		mNames[index] = mNames[mNumKeys-1];
		mKeys[index] = mKeys[mNumKeys-1];
		mNumKeys--;
	}
	else
	{
		char name[33];
		OmnRandom::nextStr(name, 1, 32);
		int len = strlen(name);
		for(int i =0; i<len; i++)
		{
			if(name[i] == ' ') name[i] = 'a';
		}

		if (keyExist(name)) return true;

		int ret = aos_keymagmt_remove_key(name);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret<0)) << endtc;
	}

	return true;
}


int
AosKeymanTorturer::keyExist(const char *name)
{
	for (int i=0; i<mNumKeys; i++)
	{
		if (strcmp(mNames[i].data(),name) == 0) return i;
	}

	return -1;
}


bool
AosKeymanTorturer::getKey()
{	
	if (mNumKeys == 0)
	{
		return true;
	}
	
	int valid = OmnRandom::nextInt(0,4);
	if(valid)
	{
		int index = OmnRandom::nextInt(0, mNumKeys-1);
		char* key = new char[33];
		int ret = aos_keymagmt_get_key((char *)mNames[index].data(), key, 33);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		return true;
	}
	else
	{
		char name[33];
		OmnRandom::nextStr(name, 1, 32);
		int len = strlen(name);
		for(int i =0; i<len; i++)
		{
			if(name[i] == ' ') name[i] = 'a';
		}

		if (keyExist(name)) return true;
		char* key = new char[17];
		int ret = aos_keymagmt_get_key(name, key, 16);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret<0)) << endtc;
		return true;
	}
}

static int sgOprIndexEN[] = {0, 1 ,2};
static u16 sgOprWeightsEN[] = {30, 30 ,40};

bool
AosKeymanTorturer::testDataEncDec()
{
	int tries = 500;

	AosRandomSelector oprSelector(sgOprIndexEN, sgOprWeightsEN, 3);

	bool rslt;
	while (tries)
	{
		// Determine the operations: add, delete, or get
		int opr = oprSelector.next();
		switch (opr)
		{
		case 0:
			rslt = encodeData();
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << "Failed to encrypt" << endtc;
			 break;

		case 1:
		    rslt = decodeData();
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << "Failed to decrypt" << endtc;
			 break;
		case 2:
			 //Compare if the data decrypted matches the original data;
			 rslt = compareData();
			 OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rslt)) << "Data no matches!" << endtc;
			 break;
	    default:
		     OmnAlarm << "Unrecognized operation: " << opr << enderr;
			 return false;
		}

		tries--;
	}

	return true;
}
	

bool
AosKeymanTorturer::encodeData()
{
	if (mNumKeys == 0)
	{
		return true;
	}
	
	int valid = OmnRandom::nextInt(0,4);
	if(valid)
	{
		int index = OmnRandom::nextInt(0, mNumKeys-1);
		char data[20001];
		int datalen = OmnRandom::nextBinStr(data, 10000, 20000);
		char encryptdata[20001];
		int ret = aos_keymagmt_encrypt(data, datalen, encryptdata , 
			AOS_KEYMGMT_ENC_ALGO_3DES, (char*)mNames[index].data());
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}
	else
	{
		char name[33];
		OmnRandom::nextStr(name, 1, 32);
		int len = strlen(name);
		for(int i =0; i<len; i++)
		{
			if(name[i] == ' ') name[i] = 'a';
		}
		if (keyExist(name)) return true;

		char data[20001];
		int datalen = OmnRandom::nextBinStr(data, 10000, 20000);
		char encryptdata[20001];
		int ret = aos_keymagmt_encrypt(data, datalen, encryptdata , 
			AOS_KEYMGMT_ENC_ALGO_3DES, name);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret<0)) << endtc;
	}
	return true;
}


bool
AosKeymanTorturer::decodeData()
{
	if (mNumKeys == 0)
	{
		return true;
	}
	
	int valid = OmnRandom::nextInt(0,4);
	if(valid)
	{
		int index = OmnRandom::nextInt(0, mNumKeys-1);
		char data[20001];
		char encryptdata[20001];
		int datalen = OmnRandom::nextBinStr(encryptdata, 10000, 20000);
		int ret = aos_keymagmt_encrypt(data, datalen, encryptdata , 
			AOS_KEYMGMT_ENC_ALGO_3DES, (char*)mNames[index].data());
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}
	else
	{
		char name[33];
		OmnRandom::nextStr(name, 1, 32);
		int len = strlen(name);
		for(int i =0; i<len; i++)
		{
			if(name[i] == ' ') name[i] = 'a';
		}
		if (keyExist(name)) return true;
		char data[20001];
		char encryptdata[20001];
		int datalen = OmnRandom::nextBinStr(encryptdata, 10000, 20000);
		int ret = aos_keymagmt_encrypt(data, datalen, encryptdata , 
			AOS_KEYMGMT_ENC_ALGO_3DES, name);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ret<0)) << endtc;
	}
	return true;
}

bool
AosKeymanTorturer::compareData()
{
	if(mNumKeys == 0) return true;

	int index = OmnRandom::nextInt(0, mNumKeys-1);
	char origdata[20000];
	char encryptdata[20000];
	char decryptdata[20000];
	int datalen = OmnRandom::nextBinStr(origdata, 1000, 20000);
		
	int ret1 = aos_keymagmt_encrypt(origdata, datalen, encryptdata , 
			AOS_KEYMGMT_ENC_ALGO_RC4, (char*)mNames[index].data());
	
	int ret2 = aos_keymagmt_decrypt(decryptdata, datalen, encryptdata , 
			AOS_KEYMGMT_ENC_ALGO_RC4, (char*)mNames[index].data());

	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret1)) << "error to encrypt!" <<endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret2)) << "error to decrypt!" <<endtc;

	if(memcmp(origdata,decryptdata,datalen) == 0)
	{
		return true;
	}
	else 
	{
		cout<<"the original data is  : "<<origdata<<endl;
		cout<<"the decrypted data is : "<<decryptdata<<endl;
		return false;
	}
}

bool
AosKeymanTorturer::testAdd_Get()
{
	int tries = 100;
	while (tries)
	{
		char name[33];
		int namelen = 32;
		char key[33];
		int keylen = 32;
		char *keyget = new char[33];
		OmnRandom::nextStr(name, 1, namelen);
		int len = strlen(name);
		for(int i =0; i<len; i++)
		{
			if(name[i] == ' ') name[i] = 'a';
		}
		OmnRandom::nextStr(key, 32, 32);

		int ret = aos_keymagmt_add_key(name, key);
		if(ret != 0)
		{	
			cout<<"add failed!"<<endl;
			cout<<"doesn't need get!"<<endl;
			continue;
		}
		ret = aos_keymagmt_get_key(name, keyget, 33);
		if(ret != 0)  cout<<"get failed!"<<endl;
		OmnTC(OmnExpected<int>(strlen(key)), OmnActual<int>(strlen(keyget))) << endtc;
   		if(strlen(key) != strlen(keyget)) cout<<"length no match! "<<strlen(key)<<" : "<<strlen(keyget)<<endl;
		cout<<"add key : keyname = "<<name<<"    key = "<<key<<endl;
		cout<<"get key : keyname = "<<name<<"    key = "<<keyget<<endl;

		tries--;
	}

	return true;
}

bool
AosKeymanTorturer::testGenerateMac()
{
	if(mNumKeys == 0) return true;
	int tries = 500;
	while(tries)
	{
		int algo = OmnRandom::nextInt(0,3);
		int index = OmnRandom::nextInt(0, mNumKeys-1);
		char origdata[256];
		char mac[256];
		OmnRandom::nextStr(origdata, 1, 256);
		int ret = aos_keymagmt_generate_mac( origdata, strlen(origdata),(char*)mNames[index].data(), mac , algo);
		if(algo == AOS_KEYMGMT_MAC_ALGO_MD5 || algo == AOS_KEYMGMT_MAC_ALGO_SHA)
		{
			OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
			cout<<"the mac value is : "<<mac<<endl;
		}
		else
		{
			OmnTC(OmnExpected<int>(-1), OmnActual<int>(ret)) << endtc;
			cout<<"Unrecongnized algo : "<< algo <<endl;
		}
		tries--;
	}
		return true;
}
