////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2015/03/11 Created by gavin	
////////////////////////////////////////////////////////////////////////////
#include "IILClient/Tester3/IILCacheTester.h"

#include "AppMgr/App.h"
#include "QueryRslt/QueryRslt.h"
#include "IILClient/IILClient.h"
#include "IILUtil/IILUtil.h"
#include "BitmapTrans/Ptrs.h"
#include "BitmapTrans/TransBitmapQuery.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "Util/UtUtil.h"
#include "Util/Opr.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/QueryContextObj.h"

AosIILCacheTester::AosIILCacheTester()
{
	mZeroStr = "00000000000000000000";
	u64 iilid = 0;

	mAdd_times = 0;
	mRemove_times = 0;
	mQuery_times = 0;
	mDirty = false;
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
    
    u64 mIIL_num = 10000;//总数
    u64 mIIL_flag = 0;//计数器
    
	//各类IIL上限计数器
	mOneEntry = 0; //1条Entry 45%
    mTenEntry = 0; //2-10条Entry 20%
    mHundredEntry = 0;//11-100条Entry 15%
    mThousandEntry = 0; //101-1000条Entry 10% 
    mOverThousandEntry = 0;//大IIL：10% 


    mI64IILName="_zt44_i64iil";
	mD64IILName="_zt44_d64iil";
	mU64IILName="_zt44_u64iil";
	mStrIILName="_zt44_striil";
	
	mI64Lists = OmnNew vector<I64Entry>*[10000];
	memset(mI64Lists,0,10000*sizeof(vector<I64Entry>*));
	
	mD64Lists = OmnNew vector<D64Entry>*[10000];
	memset(mD64Lists,0,10000*sizeof(vector<D64Entry>*));
	
	mU64Lists = OmnNew vector<U64Entry>*[10000];
	memset(mU64Lists,0,10000*sizeof(vector<U64Entry>*));
	
	mStrLists = OmnNew vector<StrEntry>*[10000];
	memset(mStrLists,0,10000*sizeof(vector<StrEntry>*));

	u64 * mIILTypes;
    mIIL_flag=0;

    u64 * mIILMaxSize;
    int nn;

    while(mIIL_flag < mIIL_num)
  {
  	OmnString Name_add;
  	Name_add << mIIL_flag;
    switch(rand() % 4 +1)
    {
    	case 1:
    	mI64IILName_now = mI64IILName + Name_add;
    	AosIILClientObj::getIILClient()->createIILPublic(mI64IILName_now, iilid, eAosIILType_BigI64, mRundata);
    	mIILTypes[mIIL_flag] = eAosIILType_BigI64;
    	mI64Lists[mIIL_flag] = OmnNew vector<I64Entry>;
    	break;

	    case 2:			magic number
		mD64IILName_now = mD64IILName + Name_add;
		AosIILClientObj::getIILClient()->createIILPublic(mD64IILName_now, iilid, eAosIILType_BigD64, mRundata);
	    mIILTypes[mIIL_flag] = eAosIILType_BigD64;
	    mD64Lists[mIIL_flag] = OmnNew vector<D64Entry>;
    	break;

	    case 3:
		mU64IILName_now = mU64IILName + Name_add;
		AosIILClientObj::getIILClient()->createIILPublic(mU64IILName_now, iilid, eAosIILType_BigU64, mRundata);
	    mIILTypes[mIIL_flag] = eAosIILType_BigU64;
	    mU64Lists[mIIL_flag] = OmnNew vector<U64Entry>;
    	break;

	    case 4:
		mStrIILName_now = Name_add+mStrIILName;
		AosIILClientObj::getIILClient()->createIILPublic(mStrIILName_now, iilid, eAosIILType_BigStr, mRundata);
	    mIILTypes[mIIL_flag] = eAosIILType_BigStr;
	    mStrLists[mIIL_flag] = OmnNew vector<StrEntry>;
    	break;
    }

	// set max
	nn = OmnRandom::intByRange(
	1, 1, 45, 
	10, 10, 20, 
	100, 100, 15,
	1000,1000,10,
	1638400,1638400,10);
	 
	mIILMaxSize[mIIL_flag] = nn;
	mIIL_flag++;
	
  }
}

AosIILCacheTester::~AosIILCacheTester()
{
}


bool      	
AosIILCacheTester::start()
{	
	OmnScreen << " IIL Cache Test start ..." << endl;
	srand(1005);
	for(int k = 0; k<10000000;)
	{
		int opr1 = rand() % 3 + 1;
		switch(opr1)
		{
			case 1:
				OmnScreen << "=====================" << endl;
				OmnScreen << "Round: " << k++ << " Add Entries" << endl; 
				OmnScreen << "=====================" << endl;
				addEntries();
				break;
			case 2:
				OmnScreen << "=====================" << endl;
				OmnScreen << "Round: " << k++ << " Remove Entries" << endl; 
				OmnScreen << "=====================" << endl;
				removeEntries();
				break;
			case 3:
				OmnScreen << "=====================" << endl;
				OmnScreen << "Round: " << k++ << "Cache Query" << endl; 
				OmnScreen << "=====================" << endl;
				query();
				break;
			default:
				break;
		}
	}
	return true;
}//配置测试次数！

bool
AosIILCacheTester::addEntries()
{
	//1. pick a iil
	//2. randomly create a number to add
	//3. base on existing size and mIILMaxSize, to fix the add num
	//4. for(num)  create data
	// 4.1 one way to create data is pure random
	// 4.2 another way is pick a existing value from the list, (  list->func, get a value, if the list is empty, get a value randomly)
	// 5. sort
	// 6. prepare buff
	// 7. addentries(    (user the correct iil name ))
	
	
	mAdd_times++;
	OmnScreen << "BatchAdd:" << mAdd_times << endl; //显示第多少次执行batchadd	
	
	u64 num_create;//一次存多少，看随机！
	
	u64 cur_docid;//即将存入的Docid

    i64 cur_i64_val;//即将存入的Value,以下皆是
	vector<I64Entry> new_entries1;//（Docid,value）
	
	vector<D64Entry> new_entries2;
	d64 cur_d64_val;
	
	vector<U64Entry> new_entries3;
	u64 cur_u64_val;

	vector<StrEntry> new_entries4;
	u64 cur_str_val;

//这里想用多线程。暂时一次只加一个IIL。

    int rand_iil=rand() % mIIL_num;//pick iil
    num_create = randnum(eMinGroupSize, eMaxGroupSize);
    
    if(mIILTypes[rand_iil] == eAosIILType_BigI64)
    {
    	mI64IILName_now = mI64IILName;
    	mI64IILName_now << rand_iil;
    	aos_assert_r(mI64Lists[rand_iil],0);

    	if(num_create > mIILMaxSize-mI64Lists[rand_iil]->size)
    	{
    		num_create = mIILMaxSize-mI64Lists[rand_iil]->size;
    	}

    	for(i64 i = 0; i < num_create; i++)
			{
				cur_i64_val = getI64Val(rand_iil);
				cur_docid = rand();

				new_entries1.push_back(I64Entry(cur_i64_val, cur_docid));
				(*mI64Lists[rand_iil]).push_back(I64Entry(cur_i64_val, cur_docid));
			}

			sort(new_entries1.begin(), new_entries1.end());
			sort((*mI64Lists[rand_iil]).begin(), (*mI64Lists[rand_iil]).end());

		    // create mBuff for i64 IIL
			mI64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			mI64Buff->ensureSpace(num_create * 2 * sizeof(u64));
			mI64Buff->setDataLen(num_create * 2 * sizeof(u64));
			i64* cur_pos1 = (i64*)mI64Buff->data();
			for (int i = 0;i < new_entries1.size();i++)
			{
				*cur_pos1 = new_entries1[i].value;
				cur_pos1 ++;
				*((u64*)cur_pos1) = new_entries1[i].docid;
				cur_pos1 ++;
			}

			AosIILClientObj::getIILClient()->BatchAdd(mI64IILName_now, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
			mI64Buff = 0;
    }
    if(mIILTypes[rand_iil] == eAosIILType_BigD64)
    {
    	mD64IILName_now = mD64IILName;
    	mD64IILName_now << rand_iil;
    	aos_assert_r(mD64Lists[rand_iil],0);

    	if(num_create > mIILMaxSize-mD64Lists[rand_iil]->size)
    	{
    		num_create = mIILMaxSize-mD64Lists[rand_iil]->size;
    	}

		    //create d64 entries
			for(i64 i = 0; i < num_create; i++)
			{
				cur_d64_val = getD64Val(rand_iil);
				cur_docid = rand();
			
				new_entries2.push_back(D64Entry(cur_d64_val, cur_docid));
				mD64List.push_back(D64Entry(cur_d64_val, cur_docid));
			}
			sort(new_entries2.begin(), new_entries2.end());
			sort(mD64List.begin(), mD64List.end());

			// create mBuff for d64 IIL
			mD64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			mD64Buff->ensureSpace(num_create * 2 * sizeof(u64));
			mD64Buff->setDataLen(num_create * 2 * sizeof(u64));
			d64* cur_pos2 = (d64*)mD64Buff->data();
			for (int i = 0;i < new_entries2.size();i++)
			{
				*cur_pos2 = new_entries2[i].value;
				cur_pos2 ++;
				*((u64*)cur_pos2) = new_entries2[i].docid;
				cur_pos2 ++;
			}

			AosIILClientObj::getIILClient()->BatchAdd(mD64IILName, eAosIILType_BigD64, 16, mD64Buff, 0, 0, 1, mRundata);
			mD64Buff = 0;
    }
 
        }
        else if(IILList[rand_IIL==3])
        {
        	mU64IILName_now=(rand_IIL)toString+mU64IILName;
		    //create u64 entries
			for(i64 i = 0; i < num_create; i++)
			{
				cur_u64_val = getU64Val();
				cur_docid = rand();
			
				new_entries3.push_back(U64Entry(cur_u64_val, cur_docid));
				mU64List.push_back(U64Entry(cur_u64_val, cur_docid));
			}
			sort(new_entries3.begin(), new_entries3.end());
			sort(mU64List.begin(), mU64List.end());

			// create mBuff for u64 IIL
			mU64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			mU64Buff->ensureSpace(num_create * 2 * sizeof(u64));
			mU64Buff->setDataLen(num_create * 2 * sizeof(u64));
			u64* cur_pos3 = (u64*)mU64Buff->data();
			for (int i = 0;i < new_entries3.size();i++)
			{
				*cur_pos3 = new_entries3[i].value;
				cur_pos3 ++;
				*((u64*)cur_pos3) = new_entries3[i].docid;
				cur_pos3 ++;
			}

			AosIILClientObj::getIILClient()->BatchAdd(mU64IILName, eAosIILType_BigU64, 16, mU64Buff, 0, 0, 1, mRundata);
			mU64Buff = 0;

        }
        else if (IILList[rand_IIL]==4)
        {
        	mStrIILName_now=(rand_IIL)toString+mStrIILName;
		    //create str entries
			for(i64 i = 0; i < num_create; i++)
			{
				cur_str_val = getStrVal();
				cur_docid = rand();
			
				new_entries4.push_back(StrEntry(cur_str_val, cur_docid));
				mStrList.push_back(StrEntry(cur_str_val, cur_docid));
			}
			sort(new_entries4.begin(), new_entries4.end());
			sort(mStrList.begin(), mStrList.end());
			
			// create mBuff for str IIL
			mStrBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			mStrBuff->ensureSpace(num_create * (21+sizeof(u64)));
			mStrBuff->setDataLen(num_create * (21+sizeof(u64)));
			char* cur_pos4 = mStrBuff->data();
			for (int i = 0;i < new_entries4.size();i++)
			{
				OmnString val = toString(new_entries4[i].value);
				strcpy(cur_pos4,val.data());
				cur_pos4 += 21;
				*((u64*)cur_pos4) = new_entries4[i].docid;
				cur_pos4 += 8;
			}

			AosIILClientObj::getIILClient()->BatchAdd(mStrIILName, eAosIILType_BigStr, 29, mStrBuff, 0, 0, 1, mRundata);
			mStrBuff = 0;

        }

    	OneEntry_IIL++;
    }
    else if(2<=num_create<=10 && TenEntry_IIL <= Total_IIL*0.2)
    {
    	rand_IIL=rand()%Total_IIL+1;//随机某个IIL
		if(IILList[rand_IIL]==1)
		        {
		        	mI64IILName_now=(rand_IIL)toString+mI64IILName;
				        	//create i64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_i64_val = getI64Val();
						cur_docid = rand();

						new_entries1.push_back(I64Entry(cur_i64_val, cur_docid));
						mI64List.push_back(I64Entry(cur_i64_val, cur_docid));
					}

					sort(new_entries1.begin(), new_entries1.end());
					sort(mI64List.begin(), mI64List.end());

				    // create mBuff for i64 IIL
					mI64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mI64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mI64Buff->setDataLen(num_create * 2 * sizeof(u64));
					i64* cur_pos1 = (i64*)mI64Buff->data();
					for (int i = 0;i < new_entries1.size();i++)
					{
						*cur_pos1 = new_entries1[i].value;
						cur_pos1 ++;
						*((u64*)cur_pos1) = new_entries1[i].docid;
						cur_pos1 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mI64IILName_now, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
					mI64Buff = 0;
		        }
		        else if (IILList[rand_IIL]==2)
		        {
		        	mD64IILName_now=(rand_IIL)toString+mD64IILName;
				    //create d64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_d64_val = getD64Val();
						cur_docid = rand();
					
						new_entries2.push_back(D64Entry(cur_d64_val, cur_docid));
						mD64List.push_back(D64Entry(cur_d64_val, cur_docid));
					}
					sort(new_entries2.begin(), new_entries2.end());
					sort(mD64List.begin(), mD64List.end());

					// create mBuff for d64 IIL
					mD64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mD64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mD64Buff->setDataLen(num_create * 2 * sizeof(u64));
					d64* cur_pos2 = (d64*)mD64Buff->data();
					for (int i = 0;i < new_entries2.size();i++)
					{
						*cur_pos2 = new_entries2[i].value;
						cur_pos2 ++;
						*((u64*)cur_pos2) = new_entries2[i].docid;
						cur_pos2 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mD64IILName, eAosIILType_BigD64, 16, mD64Buff, 0, 0, 1, mRundata);
					mD64Buff = 0;
		        }
		        else if(IILList[rand_IIL==3])
		        {
		        	mU64IILName_now=(rand_IIL)toString+mU64IILName;
				    //create u64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_u64_val = getU64Val();
						cur_docid = rand();
					
						new_entries3.push_back(U64Entry(cur_u64_val, cur_docid));
						mU64List.push_back(U64Entry(cur_u64_val, cur_docid));
					}
					sort(new_entries3.begin(), new_entries3.end());
					sort(mU64List.begin(), mU64List.end());

					// create mBuff for u64 IIL
					mU64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mU64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mU64Buff->setDataLen(num_create * 2 * sizeof(u64));
					u64* cur_pos3 = (u64*)mU64Buff->data();
					for (int i = 0;i < new_entries3.size();i++)
					{
						*cur_pos3 = new_entries3[i].value;
						cur_pos3 ++;
						*((u64*)cur_pos3) = new_entries3[i].docid;
						cur_pos3 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mU64IILName, eAosIILType_BigU64, 16, mU64Buff, 0, 0, 1, mRundata);
					mU64Buff = 0;

		        }
		        else if (IILList[rand_IIL]==4)
		        {
		        	mStrIILName_now=(rand_IIL)toString+mStrIILName;
				    //create str entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_str_val = getStrVal();
						cur_docid = rand();
					
						new_entries4.push_back(StrEntry(cur_str_val, cur_docid));
						mStrList.push_back(StrEntry(cur_str_val, cur_docid));
					}
					sort(new_entries4.begin(), new_entries4.end());
					sort(mStrList.begin(), mStrList.end());
					
					// create mBuff for str IIL
					mStrBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mStrBuff->ensureSpace(num_create * (21+sizeof(u64)));
					mStrBuff->setDataLen(num_create * (21+sizeof(u64)));
					char* cur_pos4 = mStrBuff->data();
					for (int i = 0;i < new_entries4.size();i++)
					{
						OmnString val = toString(new_entries4[i].value);
						strcpy(cur_pos4,val.data());
						cur_pos4 += 21;
						*((u64*)cur_pos4) = new_entries4[i].docid;
						cur_pos4 += 8;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mStrIILName, eAosIILType_BigStr, 29, mStrBuff, 0, 0, 1, mRundata);
					mStrBuff = 0;

		        }

    	TenEntry_IIL++;
    }
    else if(11<=num_create<=100 && HundredEntry_IIL <= Total_IIL*0.15)
    {
    	rand_IIL=rand()%Total_IIL+1;//随机某个IIL
		if(IILList[rand_IIL]==1)
		        {
		        	mI64IILName_now=(rand_IIL)toString+mI64IILName;
				        	//create i64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_i64_val = getI64Val();
						cur_docid = rand();

						new_entries1.push_back(I64Entry(cur_i64_val, cur_docid));
						mI64List.push_back(I64Entry(cur_i64_val, cur_docid));
					}

					sort(new_entries1.begin(), new_entries1.end());
					sort(mI64List.begin(), mI64List.end());

				    // create mBuff for i64 IIL
					mI64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mI64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mI64Buff->setDataLen(num_create * 2 * sizeof(u64));
					i64* cur_pos1 = (i64*)mI64Buff->data();
					for (int i = 0;i < new_entries1.size();i++)
					{
						*cur_pos1 = new_entries1[i].value;
						cur_pos1 ++;
						*((u64*)cur_pos1) = new_entries1[i].docid;
						cur_pos1 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mI64IILName_now, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
					mI64Buff = 0;
		        }
		        else if (IILList[rand_IIL]==2)
		        {
		        	mD64IILName_now=(rand_IIL)toString+mD64IILName;
				    //create d64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_d64_val = getD64Val();
						cur_docid = rand();
					
						new_entries2.push_back(D64Entry(cur_d64_val, cur_docid));
						mD64List.push_back(D64Entry(cur_d64_val, cur_docid));
					}
					sort(new_entries2.begin(), new_entries2.end());
					sort(mD64List.begin(), mD64List.end());

					// create mBuff for d64 IIL
					mD64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mD64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mD64Buff->setDataLen(num_create * 2 * sizeof(u64));
					d64* cur_pos2 = (d64*)mD64Buff->data();
					for (int i = 0;i < new_entries2.size();i++)
					{
						*cur_pos2 = new_entries2[i].value;
						cur_pos2 ++;
						*((u64*)cur_pos2) = new_entries2[i].docid;
						cur_pos2 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mD64IILName, eAosIILType_BigD64, 16, mD64Buff, 0, 0, 1, mRundata);
					mD64Buff = 0;
		        }
		        else if(IILList[rand_IIL==3])
		        {
		        	mU64IILName_now=(rand_IIL)toString+mU64IILName;
				    //create u64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_u64_val = getU64Val();
						cur_docid = rand();
					
						new_entries3.push_back(U64Entry(cur_u64_val, cur_docid));
						mU64List.push_back(U64Entry(cur_u64_val, cur_docid));
					}
					sort(new_entries3.begin(), new_entries3.end());
					sort(mU64List.begin(), mU64List.end());

					// create mBuff for u64 IIL
					mU64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mU64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mU64Buff->setDataLen(num_create * 2 * sizeof(u64));
					u64* cur_pos3 = (u64*)mU64Buff->data();
					for (int i = 0;i < new_entries3.size();i++)
					{
						*cur_pos3 = new_entries3[i].value;
						cur_pos3 ++;
						*((u64*)cur_pos3) = new_entries3[i].docid;
						cur_pos3 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mU64IILName, eAosIILType_BigU64, 16, mU64Buff, 0, 0, 1, mRundata);
					mU64Buff = 0;

		        }
		        else if (IILList[rand_IIL]==4)
		        {
		        	mStrIILName_now=(rand_IIL)toString+mStrIILName;
				    //create str entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_str_val = getStrVal();
						cur_docid = rand();
					
						new_entries4.push_back(StrEntry(cur_str_val, cur_docid));
						mStrList.push_back(StrEntry(cur_str_val, cur_docid));
					}
					sort(new_entries4.begin(), new_entries4.end());
					sort(mStrList.begin(), mStrList.end());
					
					// create mBuff for str IIL
					mStrBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mStrBuff->ensureSpace(num_create * (21+sizeof(u64)));
					mStrBuff->setDataLen(num_create * (21+sizeof(u64)));
					char* cur_pos4 = mStrBuff->data();
					for (int i = 0;i < new_entries4.size();i++)
					{
						OmnString val = toString(new_entries4[i].value);
						strcpy(cur_pos4,val.data());
						cur_pos4 += 21;
						*((u64*)cur_pos4) = new_entries4[i].docid;
						cur_pos4 += 8;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mStrIILName, eAosIILType_BigStr, 29, mStrBuff, 0, 0, 1, mRundata);
					mStrBuff = 0;

		        }
    	HundredEntry_IIL++;
    }
    else if(101<=num_create<=1000 && ThousandEntry_IIL <= Total_IIL*0.1)
    {
    	rand_IIL=rand()%Total_IIL+1;//随机某个IIL
        if(IILList[rand_IIL]==1)
        {
        	mI64IILName_now=(rand_IIL)toString+mI64IILName;
        }
        else if (IILList[rand_IIL]==2)
        {
        	mD64IILName_now=(rand_IIL)toString+mD64IILName;
        }
        else if(IILList[rand_IIL==3])
        {
        	mU64IILName_now=(rand_IIL)toString+mU64IILName;
        }
        else if (IILList[rand_IIL]==4)
        {
        	mStrIILName_now=(rand_IIL)toString+mStrIILName;
        }
    	ThousandEntry_IIL++;
    }
    else if(num_create>=1000 && Big_IIL <= Total_IIL*0.1)
    {
    	rand_IIL=rand()%Total_IIL+1;//随机某个IIL
		if(IILList[rand_IIL]==1)
		        {
		        	mI64IILName_now=(rand_IIL)toString+mI64IILName;
				        	//create i64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_i64_val = getI64Val();
						cur_docid = rand();

						new_entries1.push_back(I64Entry(cur_i64_val, cur_docid));
						mI64List.push_back(I64Entry(cur_i64_val, cur_docid));
					}

					sort(new_entries1.begin(), new_entries1.end());
					sort(mI64List.begin(), mI64List.end());

				    // create mBuff for i64 IIL
					mI64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mI64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mI64Buff->setDataLen(num_create * 2 * sizeof(u64));
					i64* cur_pos1 = (i64*)mI64Buff->data();
					for (int i = 0;i < new_entries1.size();i++)
					{
						*cur_pos1 = new_entries1[i].value;
						cur_pos1 ++;
						*((u64*)cur_pos1) = new_entries1[i].docid;
						cur_pos1 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mI64IILName_now, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
					mI64Buff = 0;
		        }
		        else if (IILList[rand_IIL]==2)
		        {
		        	mD64IILName_now=(rand_IIL)toString+mD64IILName;
				    //create d64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_d64_val = getD64Val();
						cur_docid = rand();
					
						new_entries2.push_back(D64Entry(cur_d64_val, cur_docid));
						mD64List.push_back(D64Entry(cur_d64_val, cur_docid));
					}
					sort(new_entries2.begin(), new_entries2.end());
					sort(mD64List.begin(), mD64List.end());

					// create mBuff for d64 IIL
					mD64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mD64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mD64Buff->setDataLen(num_create * 2 * sizeof(u64));
					d64* cur_pos2 = (d64*)mD64Buff->data();
					for (int i = 0;i < new_entries2.size();i++)
					{
						*cur_pos2 = new_entries2[i].value;
						cur_pos2 ++;
						*((u64*)cur_pos2) = new_entries2[i].docid;
						cur_pos2 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mD64IILName, eAosIILType_BigD64, 16, mD64Buff, 0, 0, 1, mRundata);
					mD64Buff = 0;
		        }
		        else if(IILList[rand_IIL==3])
		        {
		        	mU64IILName_now=(rand_IIL)toString+mU64IILName;
				    //create u64 entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_u64_val = getU64Val();
						cur_docid = rand();
					
						new_entries3.push_back(U64Entry(cur_u64_val, cur_docid));
						mU64List.push_back(U64Entry(cur_u64_val, cur_docid));
					}
					sort(new_entries3.begin(), new_entries3.end());
					sort(mU64List.begin(), mU64List.end());

					// create mBuff for u64 IIL
					mU64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mU64Buff->ensureSpace(num_create * 2 * sizeof(u64));
					mU64Buff->setDataLen(num_create * 2 * sizeof(u64));
					u64* cur_pos3 = (u64*)mU64Buff->data();
					for (int i = 0;i < new_entries3.size();i++)
					{
						*cur_pos3 = new_entries3[i].value;
						cur_pos3 ++;
						*((u64*)cur_pos3) = new_entries3[i].docid;
						cur_pos3 ++;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mU64IILName, eAosIILType_BigU64, 16, mU64Buff, 0, 0, 1, mRundata);
					mU64Buff = 0;

		        }
		        else if (IILList[rand_IIL]==4)
		        {
		        	mStrIILName_now=(rand_IIL)toString+mStrIILName;
				    //create str entries
					for(i64 i = 0; i < num_create; i++)
					{
						cur_str_val = getStrVal();
						cur_docid = rand();
					
						new_entries4.push_back(StrEntry(cur_str_val, cur_docid));
						mStrList.push_back(StrEntry(cur_str_val, cur_docid));
					}
					sort(new_entries4.begin(), new_entries4.end());
					sort(mStrList.begin(), mStrList.end());
					
					// create mBuff for str IIL
					mStrBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
					mStrBuff->ensureSpace(num_create * (21+sizeof(u64)));
					mStrBuff->setDataLen(num_create * (21+sizeof(u64)));
					char* cur_pos4 = mStrBuff->data();
					for (int i = 0;i < new_entries4.size();i++)
					{
						OmnString val = toString(new_entries4[i].value);
						strcpy(cur_pos4,val.data());
						cur_pos4 += 21;
						*((u64*)cur_pos4) = new_entries4[i].docid;
						cur_pos4 += 8;
					}

					AosIILClientObj::getIILClient()->BatchAdd(mStrIILName, eAosIILType_BigStr, 29, mStrBuff, 0, 0, 1, mRundata);
					mStrBuff = 0;

		        }
        Big_IIL++;
    }
	return true;
}


bool
AosIILCacheTester::removeEntries()
{
	mRemove_times++;
	OmnScreen << "BatchRemove:" << mRemove_times << endl;

	u64 docid;
	u64 pos_remove = 0;
	u64 num_remove = randnum(0, mI64List.size());	

    int remove_IIL=rand()%OriTotal_IIL;
    
    if(IILList[remove_IIL]==1)
    {
    	mI64IILName_now=(remove_IIL)toString+mI64IILName;
	    	if(mI64List.size() != 0)
		{
			//remove entries
			vector<I64Entry> remove_entries1;
			remove_entries1.reserve(num_remove);
			u64 entry_size = 0;	
			
			//create remove_flag array
			u64 size1 = mI64List.size();
			bool* flag = OmnNew bool[size1];
			memset(flag, 0, size1);

			for(int i = 0; i < num_remove; i++)
			{
				pos_remove = randnum(0, mI64List.size()-1);
				flag[pos_remove] = true;
			}

			u64 a = 0;
			u64 b = 0;
			for(;b < mI64List.size();b++)
			{
				if(flag[b])
				{
					remove_entries1.resize(entry_size+1);
					remove_entries1[entry_size++] = mI64List[b];
				}
				else
				{
					if(a != b)
					{
						mI64List[a] = mI64List[b];
					}
					a++;
				}
			}
			delete [] flag;
			OmnScreen << "I64 remove " << remove_entries1.size() << " entries" << endl;
			
			mI64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			u64 num_real_remove = remove_entries1.size();
			mI64Buff->ensureSpace(num_real_remove * 2 * sizeof(u64));
			mI64Buff->setDataLen(num_real_remove * 2 * sizeof(u64));
			i64* cur_pos1 = (i64*)mI64Buff->data();
			for(int i = 0; i < remove_entries1.size(); i++)
			{
				*cur_pos1 = remove_entries1[i].value;
				cur_pos1 ++;
				*((u64*)cur_pos1) = remove_entries1[i].docid;
				cur_pos1 ++;
			}

			AosIILClientObj::getIILClient()->BatchDel(mI64IILName_now, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
			mI64Buff = 0;
		    mI64List.resize(a);
	}

    }
    else if(IILList[remove_IIL]==2)
    {
    	mD64IILName_now=(remove_IIL)toString+mD64IILName;
	    	
		if(mD64List.size() != 0)
		{
			//remove entries
			vector<D64Entry> remove_entries1;
			remove_entries1.reserve(num_remove);
			u64 entry_size = 0;
			
			//create remove_flag array
			u64 size1 = mD64List.size();
			bool* flag = OmnNew bool[size1];
			memset(flag, 0, size1);

			for(int i = 0; i < num_remove; i++)
			{
				pos_remove = randnum(0, mD64List.size()-1);
				flag[pos_remove] = true;
			}

			u64 a = 0;
			u64 b = 0;
			for(;b < mD64List.size();b++)
			{
				if(flag[b])
				{
					//put it into remove list
					//remove_entries1[remove_entries1.size()] =  mD64List[b];
					remove_entries1.resize(entry_size+1);
					remove_entries1[entry_size++] = mD64List[b];
				}
				else
				{
					//copy entry if needs
					if(a != b)
					{
						mD64List[a] = mD64List[b];
					}
					a++;
				}
			}
			delete []flag;
			OmnScreen << "D64 remove " << remove_entries1.size() << " entries" << endl;
			mD64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			u64 num_real_remove = remove_entries1.size();
			mD64Buff->ensureSpace(num_real_remove * 2 * sizeof(u64));
			mD64Buff->setDataLen(num_real_remove * 2 * sizeof(u64));
			d64* cur_pos1 = (d64*)mD64Buff->data();
			for(int i = 0; i < remove_entries1.size(); i++)
			{
					*cur_pos1 = remove_entries1[i].value;
					cur_pos1 ++;
					*((u64*)cur_pos1) = remove_entries1[i].docid;
					cur_pos1 ++;
			}

			AosIILClientObj::getIILClient()->BatchDel(mD64IILName_now, eAosIILType_BigD64, 16, mD64Buff, 0, 0, 1, mRundata);
			mD64Buff = 0;

			mD64List.resize(a);
	}
    }
    else if(IILList[remove_IIL]==3)
    {
    	mU64IILName_now=(remove_IIL)toString+mU64IILName;
	    
	    	if(mU64List.size() != 0)
		{
			//remove entries
			vector<U64Entry> remove_entries3;
			remove_entries3.reserve(num_remove);
			u64 entry_size = 0;
			
			//create remove_flag array
			u64 size1 = mU64List.size();
			bool* flag = OmnNew bool[size1];
			memset(flag, 0, size1);

			for(int i = 0; i < num_remove; i++)
			{k
				pos_remove = randnum(0, mU64List.size()-1);
				flag[pos_remove] = true;
			}

			u64 a = 0;
			u64 b = 0;
			for(;b < mU64List.size();b++)
			{
				if(flag[b])
				{
					//put it into remove list
					//remove_entries1[remove_entries1.size()] =  mD64List[b];
					remove_entries3.resize(entry_size+1);
					remove_entries3[entry_size++] = mU64List[b];
				}
				else
				{
					//copy entry if needs
					if(a != b)
					{
						mU64List[a] = mU64List[b];
					}
					a++;
				}
			}
			delete []flag;
			OmnScreen << "U64 remove " << remove_entries3.size() << " entries" << endl;
			mU64Buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			u64 num_real_remove = remove_entries3.size();
			mU64Buff->ensureSpace(num_real_remove * 2 * sizeof(u64));
			mU64Buff->setDataLen(num_real_remove * 2 * sizeof(u64));
			u64* cur_pos1 = (u64*)mU64Buff->data();
			for(int i = 0; i < remove_entries3.size(); i++)
			{
					*cur_pos1 = remove_entries3[i].value;
					cur_pos1 ++;
					*((u64*)cur_pos1) = remove_entries3[i].docid;
					cur_pos1 ++;
			}

			AosIILClientObj::getIILClient()->BatchDel(mU64IILName_now, eAosIILType_BigU64, 16, mU64Buff, 0, 0, 1, mRundata);
			mU64Buff = 0;

			mU64List.resize(a);
	}
	
    }
    else if(IILList[remove_IIL]==4)
    {
    	mU64IILName_now=(remove_IIL)toString+mU64IILName;
	    if(mStrList.size() != 0)
		{
			//remove entries
			vector<StrEntry> remove_entries4;
			remove_entries4.reserve(num_remove);
			u64 entry_size = 0;
			
			//create remove_flag array
			u64 size1 = mStrList.size();
			bool* flag = OmnNew bool[size1];
			memset(flag, 0, size1);

			for(int i = 0; i < num_remove; i++)
			{
				pos_remove = randnum(0, mStrList.size()-1);
				flag[pos_remove] = true;
			}

			u64 a = 0;
			u64 b = 0;
			for(;b < mStrList.size();b++)
			{
				if(flag[b])
				{
					//put it into remove list
					//remove_entries1[remove_entries1.size()] =  mD64List[b];
					remove_entries4.resize(entry_size+1);
					remove_entries4[entry_size++] = mStrList[b];
				}
				else
				{
					//copy entry if needs
					if(a != b)
					{
						mStrList[a] = mStrList[b];
					}
					a++;
				}
			}
			delete []flag;
			OmnScreen << "Str remove " << remove_entries4.size() << " entries" << endl;
			mStrBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			u64 num_real_remove = remove_entries4.size();
			mStrBuff->ensureSpace(num_real_remove * (21+sizeof(u64)));
			mStrBuff->setDataLen(num_real_remove * (21+sizeof(u64)));
			char* cur_pos1 = mStrBuff->data();
			for(int i = 0; i < remove_entries4.size(); i++)
			{
				OmnString val = toString(remove_entries4[i].value);
				strcpy(cur_pos1,val.data());
				cur_pos1 += 21;
				*((u64*)cur_pos1) = remove_entries4[i].docid;
				cur_pos1 += 8;
			}

			AosIILClientObj::getIILClient()->BatchDel(mStrIILName, eAosIILType_BigStr, 29, mStrBuff, 0, 0, 1, mRundata);
			mStrBuff = 0;

			//remove data from mStrList
			mStrList.resize(a);
		}
    }
	
	}
		query();				
	return true;
}

bool
AosIILCacheTester::query()
{
	mQuery_times++;
	OmnScreen << "Query:" << mQuery_times << endl;
	
	int query_IIL=rand()%OriTotal_IIL;
	if(IILList[query_IIL]==1)
	{
		queryforI64(query_IIL);
	}
	else if (IILList[query_IIL]==2)
	{
		queryforD64(query_IIL);
	}
	else if(IILList[query_IIL]==3)
	{
		queryforU64(query_IIL);
	}
	else if(IILList[query_IIL]==4)
	{
		queryforStr(query_IIL);
	}
	return true;
}

u64
AosIILCacheTester::randnum(const u64 min, const u64 max)
{
	u64 rand_num = min + rand()%(max - min + 1);
	return rand_num;
}


i64
AosIILCacheTester::getI64Val(u64 rand_iil)
{
	aos_assert_r(mI64Lists[rand_iil],0);
	int r = randnum(0,100);
	if(r< 48)
	{
		//from existing value
		if(mI64Lists[rand_iil]->size() > 0)
		{
			int pos = rand()%mI64Lists[rand_iil]->size();
			return (*mI64Lists[rand_iil])[pos].value;
		}
	}
	
	if(r < 96)
	{
		i64 aa;
		aa = (i64)((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}

	return 0;
}

i64
AosIILCacheTester::getI64Val2()
{
	int r = randnum(0,100);
	if(r< 90)
	{
		//from existing value
		if(mI64List.size() > 0)
		{
			int pos = rand()%mI64List.size();
			return mI64List[pos].value;
		}
	}
	
	if(r < 98)
	{
		i64 aa;
		aa = (i64)((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}

	return 0;
}

d64
AosIILCacheTester::getD64Val(u64 rand_iil)
{
	aos_assert_r(mI64Lists[rand_iil],0);
	int r = randnum(0,100);
	if(r< 48)
	{
		//from existing value
		if(mD64Lists[rand_iil]->size() > 0)
		{
			int pos = rand()%mD64Lists[rand_iil]->size();
			return (*mD64List[rand_iil])[pos].value;
		}
	}
	
	if(r < 96)
	{
		d64 aa;
		aa = (d64)((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}

	return 0;
}

d64
AosIILCacheTester::getD64Val2()
{
	int r = randnum(0,100);
	if(r< 90)
	{
		//from existing value
		if(mD64List.size() > 0)
		{
			int pos = rand()%mD64List.size();
			return mD64List[pos].value;
		}
	}
	
	if(r < 98)
	{
		d64 aa;
		aa = (d64)((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}

	return 0;
}

u64
AosIILCacheTester::getU64Val()
{
	int r = randnum(0,100);
	if(r< 48)
	{
		//from existing value
		if(mU64List.size() > 0)
		{
			int pos = rand()%mU64List.size();
			return mU64List[pos].value;
		}
	}
	
	if(r < 96)
	{
		d64 aa;
		aa = ((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}
	return 0;
}

u64
AosIILCacheTester::getU64Val2()
{
	int r = randnum(0,100);
	if(r< 90)
	{
		//from existing value
		if(mU64List.size() > 0)
		{
			int pos = rand()%mU64List.size();
			return mU64List[pos].value;
		}
	}
	
	if(r < 98)
	{
		d64 aa;
		aa = ((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}
	return 0;
}

u64
AosIILCacheTester::getStrVal()
{
	int r = randnum(0,100);
	if(r< 48)
	{
		//from existing value
		if(mStrList.size() > 0)
		{
			int pos = rand()%mStrList.size();
			return mStrList[pos].value;
		}
	}
	
	if(r < 96)
	{
		d64 aa;
		aa = ((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}

	return 0;
}

u64
AosIILCacheTester::getStrVal2()
{
	int r = randnum(0,100);
	if(r< 90)
	{
		//from existing value
		if(mStrList.size() > 0)
		{
			int pos = rand()%mStrList.size();
			return mStrList[pos].value;
		}
	}
	
	if(r < 98)
	{
		d64 aa;
		aa = ((((u64)rand())<<33) |(((u64)rand()) << 2) | (((u64)rand()) >> 29));
		return aa;
	}

	return 0;
}

bool 
AosIILCacheTester::checkRslt(const u64 i)
{
	//OmnSleep(1000);
	bool finished = false;
	bool in_range = false;
	if(i == 1)
	{
		OmnScreen << "====================================" << endl;
		//1. print the mOpr
		OmnScreen << "mOpr:" << AosOpr_toStr(mOpr) << endl;
		//2. print the conditions
		OmnScreen << "Conditions:" << mI64_val1 << ":" << mI64_val2 << endl;
		OmnScreen << "====================================" << endl;
		//3. print the orig list, if it is small enough
		OmnScreen << "All Orig_datas:" << endl;
		u64 count = 0;
		for(int i = 0; i < mI64List.size(); i++)
		{
			count ++;
			if(count > 1000)
			{
				OmnScreen << "more than 1000 entries" << endl;
				break;
			}
			OmnScreen << mI64List[i].value << "\t:\t" << mI64List[i].docid << endl;
		}
		OmnScreen << count << " Orig_datas" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "Orig_data in range:" << endl;
		count = 0;
		for(int i = 0; i < mI64List.size(); i++)
		{
			in_range = AosIILUtil::valueMatch(mI64List[i].value, mOpr, mI64_val1, mI64_val2);
			if(in_range)
			{	
				count ++;
				OmnScreen << mI64List[i].value << "\t:\t" << mI64List[i].docid << endl;
			}
		}
		OmnScreen << count << " in range in Orig_data" << endl;
		OmnScreen << "====================================" << endl;

		OmnScreen << "QueryRslt data:" << endl;
		mQuery_rslt1->reset();
		OmnScreen << "Query_rslt1_Docids:" << endl;
		count = 0;
		if(mQuery_rslt1->getNumDocs()==0)
		{
			OmnScreen << "NULL" << endl;
		}
		for(int j = 0; j < mQuery_rslt1->getNumDocs(); j++)
		{
			count ++;
			if(count >= 200)
			{
				OmnScreen << "More....." << endl;
				break;
			}
			OmnScreen << mQuery_rslt1->nextDocid(finished) << endl;
		}
		OmnScreen << mQuery_rslt1->getNumDocs() << " in QueryRslt" << endl;
		OmnScreen << "====================================" << endl;
	}
	else if(i == 2)
	{
		//1. print the mOpr
		OmnScreen << "mOpr:" << AosOpr_toStr(mOpr) << endl;
		//2. print the conditions
		//OmnScreen << "Conditions:" << md64_val1 << ":" << md64_val2 << endl;
		OmnScreen << "====================================" << endl;
		//3. print the orig list, if it is small enough
		OmnScreen << "All Orig_datas:" << endl;
		u64 count = 0;
		for(int i = 0; i < mD64List.size(); i++)
		{
			count ++;
			OmnScreen << mD64List[i].value << "\t:\t" << mD64List[i].docid << endl;
		}
		OmnScreen << count << " in Orig_datas" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "Orig_data in range:" << endl;
		count = 0;
		for(int i = 0; i < mD64List.size(); i++)
		{
			in_range = AosIILUtil::valueMatch(mD64List[i].value, mOpr, mD64_val1, mD64_val2);
			if(in_range)
			{	
				count ++;
				OmnScreen << mD64List[i].value << "\t:\t" << mD64List[i].docid << endl;
			}
		}
		OmnScreen << count << " in range in Orig_data" << endl;
		OmnScreen << "====================================" << endl;

		OmnScreen << "QueryRslt data:" << endl;
		mQuery_rslt2->reset();
		count = 0;
		OmnScreen << "Query_rslt2_Docids:" << endl;
		if(mQuery_rslt2->getNumDocs()==0)
		{
			OmnScreen << "NULL" << endl;
		}
		for(int j = 0; j < mQuery_rslt2->getNumDocs(); j++)
		{
			if(count >= 200)
			{
				OmnScreen << "More....." << endl;
				break;
			}
			OmnScreen << mQuery_rslt2->nextDocid(finished) << endl;
			count++;
		}
		OmnScreen << mQuery_rslt2->getNumDocs() << " in QueryRslt2" << endl;
		OmnScreen << "====================================" << endl;
	}
	else if(i == 3)
	{
		//1. print the mOpr
		OmnScreen << "mOpr:" << AosOpr_toStr(mOpr) << endl;
		//2. print the conditions
		OmnScreen << "Conditions:" << mU64_val1 << ":" << mU64_val2 << endl;
		OmnScreen << "====================================" << endl;
		//3. print the orig list, if it is small enough
		OmnScreen << "All Orig_datas:" << endl;
		u64 count = 0;
		for(int i = 0; i < mU64List.size(); i++)
		{
			count ++;
			OmnScreen << mU64List[i].value << "\t:\t" << mU64List[i].docid << endl;
		}
		OmnScreen << count << " in Orig_datas" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "Orig_data in range:" << endl;
		count = 0;
		for(int i = 0; i < mU64List.size(); i++)
		{
			in_range = AosIILUtil::valueMatch(mU64List[i].value, mOpr, mU64_val1, mU64_val2);
			if(in_range)
			{	
				count ++;
				OmnScreen << mU64List[i].value << "\t:\t" << mU64List[i].docid << endl;
			}
		}
		OmnScreen << count << " in range in Orig_data" << endl;
		OmnScreen << "====================================" << endl;

		OmnScreen << "QueryRslt data:" << endl;
		mQuery_rslt3->reset();
		count = 0;
		OmnScreen << "Query_rslt3_Docids:" << endl;
		if(mQuery_rslt3->getNumDocs()==0)
		{
			OmnScreen << "NULL" << endl;
		}
		for(int j = 0; j < mQuery_rslt3->getNumDocs(); j++)
		{
			if(count >= 200)
			{
				OmnScreen << "More....." << endl;
				break;
			}
			OmnScreen << mQuery_rslt3->nextDocid(finished) << endl;
			count++;
		}
		OmnScreen << mQuery_rslt3->getNumDocs() << " in QueryRslt3" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "=============U64=======================" << endl;
	}
	else if(i == 4)
	{
		//1. print the mOpr
		OmnScreen << "mOpr:" << AosOpr_toStr(mOpr) << endl;
		//2. print the conditions
		OmnScreen << "Conditions:" << mStr_val1 << ":" << mStr_val2 << endl;
		OmnScreen << "====================================" << endl;
		//3. print the orig list, if it is small enough
		OmnScreen << "All Orig_datas:" << endl;
		u64 count = 0;
		for(int i = 0; i < mStrList.size(); i++)
		{
			count ++;
			OmnScreen << mStrList[i].value << "\t:\t" << mStrList[i].docid << endl;
		}
		OmnScreen << count << " in Orig_datas" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "Orig_data in range:" << endl;
		count = 0;
		for(int i = 0; i < mStrList.size(); i++)
		{
			in_range = AosIILUtil::valueMatch(mStrList[i].value, mOpr, mStr_val1, mStr_val2);
			if(in_range)
			{	
				count ++;
				OmnScreen << mStrList[i].value << "\t:\t" << mStrList[i].docid << endl;
			}
		}
		OmnScreen << count << " in range in Orig_data" << endl;
		OmnScreen << "====================================" << endl;

		OmnScreen << "QueryRslt data:" << endl;
		mQuery_rslt4->reset();
		count = 0;
		OmnScreen << "Query_rslt4_Docids:" << endl;
		if(mQuery_rslt4->getNumDocs()==0)
		{
			OmnScreen << "NULL" << endl;
		}
		for(int j = 0; j < mQuery_rslt4->getNumDocs(); j++)
		{
			if(count >= 200)
			{
				OmnScreen << "More....." << endl;
				break;
			}
			OmnScreen << mQuery_rslt4->nextDocid(finished) << endl;
			count++;
		}
		OmnScreen << mQuery_rslt4->getNumDocs() << " in QueryRslt4" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "=================Str===================" << endl;
		
	}
	else
	{
		// print the orig list, if it is small enough
		OmnScreen << "All Orig_datas:" << endl;
		u64 count = 0;
		for(int i = 0; i < mHitList.size(); i++)
		{
			count ++;
			OmnScreen << mHitList[i] << endl;
		}
		OmnScreen << count << " in range in Orig_data" << endl;
		OmnScreen << "====================================" << endl;

		OmnScreen << "QueryRslt data:" << endl;
		mQuery_rslt5->reset();
		count = 0;
		OmnScreen << "Query_rslt5_Docids:" << endl;
		if(mQuery_rslt5->getNumDocs()==0)
		{
			OmnScreen << "NULL" << endl;
		}
		for(int j = 0; j < mQuery_rslt5->getNumDocs(); j++)
		{
			if(count >= 200)
			{
				OmnScreen << "More....." << endl;
				break;
			}
			OmnScreen << mQuery_rslt5->nextDocid(finished) << endl;
			count++;
		}
		OmnScreen << mQuery_rslt5->getNumDocs() << " in QueryRslt5" << endl;
		OmnScreen << "====================================" << endl;
		OmnScreen << "=============Hit=======================" << endl;
	}
}

u64
AosIILCacheTester::random_block_size(const u64 &remain)
{
	if(remain == 0)
	{
		return 0;
	}

	// case 1. return 0;
	// case 2. return 10;
	// case 3. return 1-size
	// case 4. over size
	int r = randnum(0,100);
	if(r< 25)
	{
		if(remain < eMaxBlockSizes)
		{
			return 0;
		}
	}
	
	if(r < 50)
	{
		return 10;
	}
	
	if(r < 75)
	{
		u64 max = remain;
		if(max > eMaxBlockSizes)
		{
			max = eMaxBlockSizes;
		}
		return randnum(1,max);
	}
	
	return randnum(1,eMaxBlockSizes);
}

OmnString
AosIILCacheTester::toString(u64 orig_val)
{
	OmnString orig_str;
	orig_str << orig_val;
	OmnString rslt_str;
	rslt_str.setLength(20);
	u64 n = orig_str.length();
	rslt_str.replace(0, 20, mZeroStr);
	rslt_str.replace(20-n, n, orig_str);

	//strcpy(rslt_str, mZeroStr);
	//strcpy(rslt_str+(20-n), orig_str);
	
	return rslt_str;
}

bool
AosIILCacheTester::queryforI64(u64 IILList)
{
	mI64IILName_now=(IILList)toString+mI64IILName;

	mI64_val1 = getI64Val();
	mI64_val2 = getI64Val();
	if(mI64_val1 > mI64_val2)
	{
		i64 tmp = mI64_val1;
		mI64_val1 = mI64_val2;
		mI64_val2 = tmp;
	}

	vector<i64> i64_inlist;
	bool is_in = rand()%2;

	selectOpr();
	u64 inlist_size = 0;	
	OmnString v;
	// Prepare params for i64 query
	AosQueryContextObjPtr 	query_context1 = AosQueryContextObj::createQueryContextStatic();
	query_context1->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		inlist_size = rand()%(mI64List.size()+1)+1;
		if(percent(80))
		{
			inlist_size = rand()%10+1;
		}
		for(int i = 0; i < inlist_size; i++)
		{
			// check if it is in i64_inlist
			bool found = false;
			i64 cur_v = getI64Val2();
			for(int j = 0;j < i64_inlist.size();j++)
			{
				if(cur_v == i64_inlist[j])
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				i64_inlist.push_back(cur_v);
			}
		}
		sort(i64_inlist.begin(),i64_inlist.end());

		//set query_context
		if(is_in)
		{
			for(int i = 0;i < i64_inlist.size();i++)
			{
				v << "eq,"<<i64_inlist[i];
				if(i < i64_inlist.size()-1)
				{	
					v << ";";
				}
			}
		}
		else // not in
		{
			int size = i64_inlist.size();
			aos_assert_r(size > 0,false);
			v << "lt," << i64_inlist[0] << ";";
			for(int i = 0;i < size-1;i++)
			{
				v << "r4," << i64_inlist[i] << "," << i64_inlist[i+1] << ";";
			}

			v << "gt," << i64_inlist[size-1];

		}
		query_context1->setStrValue(v);
	}
	else
	{
		query_context1->setStrValue(OmnStrUtil::lltoa(mI64_val1));
		query_context1->setStrValue2(OmnStrUtil::lltoa(mI64_val2));
	}
	AosQueryContextObjPtr b_context1 = query_context1->clone();

	//I64
	i64 cur_i64_pos = 0;
	u64 cur_rslt = 0;
	u64 cur_rslt2 = 0;
	bool found = false;
	bool finished = false;
	u64 remained_size = mI64List.size();
	u64 cur_block_size = 0;
	bool in_range = false;
    OmnScreen << "Query for I64" << endl;
	while(1)
	{
		//Query for a block
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context1->setBlockSize(cur_block_size);
		b_context1->setBlockSize(cur_block_size);
		mQuery_rslt1 = AosQueryRsltObj::getQueryRsltStatic();
		mParaQuery_rslt1 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mI64IILName_now,
			mQuery_rslt1,
			0,
			query_context1,
			mRundata);
		
		aos_assert_r(rslt,false);								
		mQuery_rslt1->reset();
		cur_rslt = mQuery_rslt1->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_i64_pos < mI64List.size())
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < i64_inlist.size(); i++)
					{
						if(mI64List[cur_i64_pos].value == i64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
					in_range = AosIILUtil::valueMatch(mI64List[cur_i64_pos].value, mOpr, mI64_val1, mI64_val2);
				}
				if(in_range)
				{
					found = true;
					if(cur_rslt != mI64List[cur_i64_pos].docid)
					{
						// not match
						checkRslt(1);
						OmnAlarm << "I64 Query Error:"
									"QueryRslt  not euqal to mI64List" << enderr;
					}
					// equal
					cur_i64_pos ++;
					break;
				}
				cur_i64_pos ++;
			}
			if(!found)
			{
				checkRslt(1);
				OmnAlarm << "I64 Query Error:"
							"mI64List finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt1->nextDocid(finished); 
		}

		if(query_context1->finished())
		{
			break;
		}
	}
	
	for(;cur_i64_pos < mI64List.size();cur_i64_pos++)
	{
		if(mOpr == eAosOpr_in)
		{
			bool found = false;
			for(int i = 0; i < i64_inlist.size(); i++)
			{
				if(mI64List[cur_i64_pos].value == i64_inlist[i])
				{
					found = true;
					break;
				}
			}
			in_range = !(found ^ is_in);
		}
		else
		{
			in_range = AosIILUtil::valueMatch(mI64List[cur_i64_pos].value, mOpr, mI64_val1, mI64_val2);
		}
		if(in_range)
		{
			checkRslt(1);
			OmnAlarm << "I64 Query Error:"
						"mI64List has more valid data than QueryRslt:" << 
						cur_i64_pos << " : " << mI64List[cur_i64_pos].value << " : " <<  mI64List[cur_i64_pos].docid  << enderr;
		}		
	}
	
	//////////////////
	//reverse query
	//////////////////
	
	// Prepare params for i64  reverse query
	query_context1 = AosQueryContextObj::createQueryContextStatic();
	query_context1->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		query_context1->setStrValue(v);
	}
	else
	{
		query_context1->setStrValue(OmnStrUtil::lltoa(mI64_val1));
		query_context1->setStrValue2(OmnStrUtil::lltoa(mI64_val2));
	}
	query_context1->setReverse(true);

	
	cur_i64_pos = mI64List.size()-1;
	cur_rslt = 0;
	found = false;
	remained_size = mI64List.size();
	cur_block_size = 0;
	in_range = false;
    OmnScreen << "Reverse query for I64" << endl;

	while(1)
	{
	
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context1->setBlockSize(cur_block_size);
		mQuery_rslt1 = AosQueryRsltObj::getQueryRsltStatic();
		mParaQuery_rslt1 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mI64IILName_now,
			mQuery_rslt1,
			0,
			query_context1,
			mRundata);


		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt1->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_i64_pos >= 0)
			{
				if(mOpr == eAosOpr_in)
				{
					bool found = false;
					for(int i = i64_inlist.size()-1; i >=0; i--)
					{
						if(mI64List[cur_i64_pos].value == i64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
					in_range = AosIILUtil::valueMatch(mI64List[cur_i64_pos].value, mOpr, mI64_val1, mI64_val2);
				}
				if(in_range)
				{
					found = true;
					if(cur_rslt != mI64List[cur_i64_pos].docid)
					{
						// not match
						checkRslt(1);
						OmnAlarm << "I64 Reverse Query Error:"
									"QueryRslt  not euqal to mI64List" << enderr;
					}
					// equal
					cur_i64_pos --;
					break;
				}
				cur_i64_pos --;
			}
			if(!found)
			{
				checkRslt(1);
				OmnAlarm << "I64 Reverse Query Error:"
							"mI64List finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt1->nextDocid(finished); 
		}

		if(query_context1->finished())
		{
			break;
		}
	}
	
	for(;cur_i64_pos >= 0; cur_i64_pos--)
	{
		if(mOpr == eAosOpr_in)
		{
			bool found = false;
			for(int i = i64_inlist.size()-1; i >=0; i--)
			{
				if(mI64List[cur_i64_pos].value == i64_inlist[i])
				{
					found = true;
					break;
				}
			}
			in_range = !(found ^ is_in);
		}
		else
		{
			in_range = AosIILUtil::valueMatch(mI64List[cur_i64_pos].value, mOpr, mI64_val1, mI64_val2);
		}
		if(in_range)
		{
			checkRslt(1);
			OmnAlarm << "I64 Reverse Query Error:"
						"mI64List has more valid data than QueryRslt:" << 
						cur_i64_pos << " : " << mI64List[cur_i64_pos].value << " : " <<  mI64List[cur_i64_pos].docid  << enderr;
		}		
	}
	return true;
}

bool
AosIILCacheTester::queryforD64(u64 IILList)
{	
	mD64IILName_now=(IILList)toString+mD64IILName;
    mD64_val1 = getD64Val();
	mD64_val2 = getD64Val();
	if(mD64_val1 > mD64_val2)
	{
		d64 tmp = mD64_val1;
		mD64_val1 = mD64_val2;
		mD64_val2 = tmp;
	}

	vector<d64> d64_inlist;
	bool is_in = rand()%2;

	selectOpr();
	u64 inlist_size = 0;	
	OmnString v;
	// Prepare params for i64 query
	AosQueryContextObjPtr 	query_context2 = AosQueryContextObj::createQueryContextStatic();
	query_context2->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		inlist_size = rand()%(mD64List.size()+1)+1;
		if(percent(80))
		{
			inlist_size = rand()%10+1;
		}
		for(int i = 0; i < inlist_size; i++)
		{
			// check if it is in d64_inlist
			bool found = false;
			d64 cur_v = getD64Val2();
			for(int j = 0;j < d64_inlist.size();j++)
			{
				if(cur_v == d64_inlist[j])
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				d64_inlist.push_back(cur_v);
			}
		}
		sort(d64_inlist.begin(),d64_inlist.end());

		//set query_context
		if(is_in)
		{
			for(int i = 0;i < d64_inlist.size();i++)
			{
				v << "eq,"<<d64_inlist[i];
				if(i < d64_inlist.size()-1)
				{	
					v << ";";
				}
			}
		}
		else // not in
		{
			int size = d64_inlist.size();
			aos_assert_r(size > 0,false);
			v << "lt," << d64_inlist[0] << ";";
			for(int i = 0;i < size-1;i++)
			{
				v << "r4," << d64_inlist[i] << "," << d64_inlist[i+1] << ";";
			}

			v << "gt," << d64_inlist[size-1];

		}
		query_context2->setStrValue(v);
	}
	else
	{
		query_context2->setStrValue(OmnStrUtil::dtoa(mD64_val1));
		query_context2->setStrValue2(OmnStrUtil::dtoa(mD64_val2));
	}
    AosQueryContextObjPtr b_context2 = query_context2->clone();
	//D64
	i64 cur_d64_pos = 0;
	u64 cur_rslt = 0;
	u64 cur_rslt2 = 0;
	u64 cur_para_rslt = 0;
	bool found = false;
	bool finished = false;
	u64 remained_size = mD64List.size();
	u64 cur_block_size = 0;
	bool in_range = false;
    OmnScreen << "Query for D64" << endl;

	while(1)
	{
		//Query for a block
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context2->setBlockSize(cur_block_size);
		b_context2->setBlockSize(cur_block_size);
		mQuery_rslt2 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mD64IILName_now,
			mQuery_rslt2,
			0,
			query_context2,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt2->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_d64_pos < mD64List.size())
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < d64_inlist.size(); i++)
					{
						if(mD64List[cur_d64_pos].value == d64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
					in_range = AosIILUtil::valueMatch(mD64List[cur_d64_pos].value, mOpr, mD64_val1, mD64_val2);
			    }
				if(in_range)
				{
					found = true;
					if(cur_rslt != mD64List[cur_d64_pos].docid)
					{
						// not match
						checkRslt(2);
						OmnAlarm << "D64 Query Error:"
									"QueryRslt  not euqal to mD64List" << enderr;
					}
					// equal
					cur_d64_pos ++;
					break;
				}
				cur_d64_pos ++;
			}
			if(!found)
			{
				checkRslt(2);
				OmnAlarm << "D64 Query Error:"
							"mD64List finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt2->nextDocid(finished); 
		}

		if(query_context2->finished())
		{
			break;
		}
	}
	
	for(;cur_d64_pos < mD64List.size();cur_d64_pos++)
	{
		if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < d64_inlist.size(); i++)
					{
						if(mD64List[cur_d64_pos].value == d64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
		            in_range = AosIILUtil::valueMatch(mD64List[cur_d64_pos].value, mOpr, mD64_val1, mD64_val2);
	            }
		if(in_range)
		{
			checkRslt(2);
			OmnAlarm << "D64 Query Error:"
						"mD64List has more valid data than QueryRslt:" << 
						cur_d64_pos << " : " << mD64List[cur_d64_pos].value << " : " <<  mD64List[cur_d64_pos].docid  << enderr;
		}		
	}
	
	//////////////////
	//reverse query
	//////////////////
	
	// Prepare params for d64 query
	query_context2 = AosQueryContextObj::createQueryContextStatic();
	query_context2->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		query_context2->setStrValue(v);
	}
	else
	{
		query_context2->setStrValue(OmnStrUtil::dtoa(mD64_val1));
		query_context2->setStrValue2(OmnStrUtil::dtoa(mD64_val2));
	}
	query_context2->setReverse(true);

	cur_d64_pos = mD64List.size()-1;
	cur_rslt = 0;
	found = false;
	remained_size = mD64List.size();
	cur_block_size = 0;
	in_range = false;
OmnScreen << "Reverse query for D64" << endl;
	while(1)
	{
		//Query for a block
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context2->setBlockSize(cur_block_size);
		mQuery_rslt2 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mD64IILName_now,
			mQuery_rslt2,
			0,
			query_context2,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt2->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_d64_pos >= 0)
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < d64_inlist.size(); i++)
					{
						if(mD64List[cur_d64_pos].value == d64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
				    in_range = AosIILUtil::valueMatch(mD64List[cur_d64_pos].value, mOpr, mD64_val1, mD64_val2);
			    }
				if(in_range)
				{
					found = true;
					if(cur_rslt != mD64List[cur_d64_pos].docid)
					{
						// not match
						checkRslt(2);
						OmnAlarm << "D64 Reverse Query Error:"
									"QueryRslt  not euqal to mD64List" << enderr;
					}
					// equal
					cur_d64_pos --;
					break;
				}
				cur_d64_pos --;
			}
			if(!found)
			{
				checkRslt(2);
				OmnAlarm << "D64 Reverse Query Error:"
							"mD64List finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt2->nextDocid(finished); 
		}

		if(query_context2->finished())
		{
			break;
		}
	}
	
	for(;cur_d64_pos >= 0; cur_d64_pos--)
	{
		if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < d64_inlist.size(); i++)
					{
						if(mD64List[cur_d64_pos].value == d64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else{
		                in_range = AosIILUtil::valueMatch(mD64List[cur_d64_pos].value, mOpr, mD64_val1, mD64_val2);
	                }
		if(in_range)
		{
			checkRslt(2);
			OmnAlarm << "D64 Reverse Query Error:"
						"mD64List has more valid data than QueryRslt:" << 
						cur_d64_pos << " : " << mD64List[cur_d64_pos].value << " : " <<  mD64List[cur_d64_pos].docid  << enderr;
		}		
	}
	return true;
}

bool
AosIILCacheTester::queryforU64(u64 IILList)
{	
	mU64IILName_now=(IILList)toString+mU64IILName;
    mU64_val1 = getU64Val();
	mU64_val2 = getU64Val();
	if(mU64_val1 > mU64_val2)
	{
		u64 tmp = mU64_val1;
		mU64_val1 = mU64_val2;
		mU64_val2 = tmp;
	}

	vector<u64> u64_inlist;
	bool is_in = rand()%2;

	selectOpr();
	u64 inlist_size = 0;	
	OmnString v;
	// Prepare params for u64 query
	AosQueryContextObjPtr 	query_context3 = AosQueryContextObj::createQueryContextStatic();
	query_context3->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		inlist_size = rand()%(mU64List.size()+1)+1;
		if(percent(80))
		{
			inlist_size = rand()%10+1;
		}
		for(int i = 0; i < inlist_size; i++)
		{
			// check if it is in i64_inlist
			bool found = false;
			u64 cur_v = getU64Val();
			for(int j = 0;j < u64_inlist.size();j++)
			{
				if(cur_v == u64_inlist[j])
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				u64_inlist.push_back(cur_v);
			}
		}
		sort(u64_inlist.begin(),u64_inlist.end());

		//set query_context
		if(is_in)
		{
			for(int i = 0;i < u64_inlist.size();i++)
			{
				v << "eq,"<<u64_inlist[i];
				if(i < u64_inlist.size()-1)
				{	
					v << ";";
				}
			}
		}
		else // not in
		{
			int size = u64_inlist.size();
			aos_assert_r(size > 0,false);
			v << "lt," << u64_inlist[0] << ";";
			for(int i = 0;i < size-1;i++)
			{
				v << "r4," << u64_inlist[i] << "," << u64_inlist[i+1] << ";";
			}

			v << "gt," << u64_inlist[size-1];

		}
		query_context3->setStrValue(v);
	}
	else
	{
		query_context3->setStrValue(OmnStrUtil::lltoa(mU64_val1));
		query_context3->setStrValue2(OmnStrUtil::lltoa(mU64_val2));
	}
	AosQueryContextObjPtr b_context3 = query_context3->clone();

	//I64
	i64 cur_u64_pos = 0;
	u64 cur_rslt = 0;
	u64 cur_rslt2 = 0;
	u64 cur_para_rslt = 0;
	bool found = false;
	bool finished = false;
	u64 remained_size = mU64List.size();
	u64 cur_block_size = 0;
	bool in_range = false;
OmnScreen << "Query for U64" << endl;

	while(1)
	{
		//Query for a block
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		b_context3->setBlockSize(cur_block_size);
		query_context3->setBlockSize(cur_block_size);
		mQuery_rslt3 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mU64IILName_now,
			mQuery_rslt3,
			0,
			query_context3,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt3->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_u64_pos < mU64List.size())
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < u64_inlist.size(); i++)
					{
						if(mU64List[cur_u64_pos].value == u64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
				   in_range = AosIILUtil::valueMatch(mU64List[cur_u64_pos].value, mOpr, mU64_val1, mU64_val2);
			    }
				if(in_range)
				{
					found = true;
					if(cur_rslt != mU64List[cur_u64_pos].docid)
					{
						// not match
						checkRslt(3);
						OmnAlarm << "U64 Query Error:"
									"QueryRslt  not euqal to mU64List" << enderr;
					}
					// equal
					cur_u64_pos ++;
					break;
				}
				cur_u64_pos ++;
			}
			if(!found)
			{
				checkRslt(3);
				OmnAlarm << "U64 Query Error:"
							"mU64List finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt3->nextDocid(finished); 
		}

		if(query_context3->finished())
		{
			break;
		}
	}
	
	for( ; cur_u64_pos < mU64List.size(); cur_u64_pos++)
	{
		if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < u64_inlist.size(); i++)
					{
						if(mU64List[cur_u64_pos].value == u64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
		            in_range = AosIILUtil::valueMatch(mU64List[cur_u64_pos].value, mOpr, mU64_val1, mU64_val2);
	            }
		if(in_range)
		{
			checkRslt(3);
			OmnAlarm << "U64 Query Error:"
						"mU64List has more valid data than QueryRslt:" << 
						cur_u64_pos << " : " << mU64List[cur_u64_pos].value << " : " <<  mU64List[cur_u64_pos].docid  << enderr;
		}		
	}
	
	//////////////////
	//reverse query
	//////////////////
	
	// Prepare params for u64 query
	query_context3 = AosQueryContextObj::createQueryContextStatic();
	query_context3->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		query_context3->setStrValue(v);
	}
	else
	{
		query_context3->setStrValue(OmnStrUtil::lltoa(mU64_val1));
		query_context3->setStrValue2(OmnStrUtil::lltoa(mU64_val2));
	}
	query_context3->setReverse(true);
	//U64 
	cur_u64_pos = mU64List.size()-1;
	cur_rslt = 0;
	found = false;
	remained_size = mU64List.size();
	cur_block_size = 0;
	in_range = false;
OmnScreen << "Reverse query for U64" << endl;
	while(1)
	{
		//Query for a block
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context3->setBlockSize(cur_block_size);
		mQuery_rslt3 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mU64IILName_now,
			mQuery_rslt3,
			0,
			query_context3,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt3->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_u64_pos >= 0)
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < u64_inlist.size(); i++)
					{
						if(mU64List[cur_u64_pos].value == u64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
				    in_range = AosIILUtil::valueMatch(mU64List[cur_u64_pos].value, mOpr, mU64_val1, mU64_val2);
			    }
				if(in_range)
				{
					found = true;
					if(cur_rslt != mU64List[cur_u64_pos].docid)
					{
						// not match
						checkRslt(3);
						OmnAlarm << "U64 Reverse Query Error:"
									"QueryRslt not euqal to mU64List" << enderr;
					}
					// equal
					cur_u64_pos --;
					break;
				}
				cur_u64_pos --;
			}
			if(!found)
			{
				checkRslt(3);
				OmnAlarm << "U64 Reverse Query Error:"
							"mU64List finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt3->nextDocid(finished); 
		}

		if(query_context3->finished())
		{
			break;
		}
	}
	
	for( ; cur_u64_pos >= 0; cur_u64_pos--)
	{
		if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < u64_inlist.size(); i++)
					{
						if(mU64List[cur_u64_pos].value == u64_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
							in_range = AosIILUtil::valueMatch(mU64List[cur_u64_pos].value, mOpr, mU64_val1, mU64_val2);
				}
		if(in_range)
		{
			checkRslt(3);
			OmnAlarm << "U64 Reverse Query Error:"
						"mU64List has more valid data than QueryRslt:" << 
						cur_u64_pos << " : " << mU64List[cur_u64_pos].value << " : " <<  mU64List[cur_u64_pos].docid  << enderr;
		}		
	}
}

bool
AosIILCacheTester::queryforStr(u64 IILList)
{	
	mStrIILName_now=(IILList)toString+mStrIILName;
    mStr_val1 = getStrVal();
	mStr_val2 = getStrVal();
	if(mStr_val1 > mStr_val2)
	{
		u64 tmp = mStr_val1;
		mStr_val1 = mStr_val2;
		mStr_val2 = tmp;
	}

	vector<u64> str_inlist;
	bool is_in = rand()%2;

	selectOpr();
	u64 inlist_size = 0;	
	OmnString v;
	// Prepare params for i64 query
	AosQueryContextObjPtr 	query_context4 = AosQueryContextObj::createQueryContextStatic();
	query_context4->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		inlist_size = rand()%(mStrList.size()+1)+1;
		if(percent(80))
		{
			inlist_size = rand()%10+1;
		}
		for(int i = 0; i < inlist_size; i++)
		{
			// check if it is in i64_inlist
			bool found = false;
			u64 cur_v = getStrVal();
			for(int j = 0;j < str_inlist.size();j++)
			{
				if(cur_v == str_inlist[j])
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				str_inlist.push_back(cur_v);
			}
		}
		sort(str_inlist.begin(),str_inlist.end());

		//set query_context
		if(is_in)
		{
			for(int i = 0;i < str_inlist.size();i++)
			{
				v << "eq,"<<toString(str_inlist[i]);
				if(i < str_inlist.size()-1)
				{	
					v << ";";
				}
			}
		}
		else // not in
		{
			int size = str_inlist.size();
			aos_assert_r(size > 0,false);
			v << "lt," << toString(str_inlist[0]) << ";";
			for(int i = 0;i < size-1;i++)
			{
				v << "r4," << toString(str_inlist[i]) << "," << toString(str_inlist[i+1]) << ";";
			}

			v << "gt," << toString(str_inlist[size-1]);

		}
		query_context4->setStrValue(v);
	}
	else
	{
		query_context4->setStrValue(toString(mStr_val1));
		query_context4->setStrValue2(toString(mStr_val2));
	}
	AosQueryContextObjPtr b_context1 = query_context4->clone();

	//I64
	i64 cur_str_pos = 0;
	u64 cur_rslt = 0;
	u64 cur_rslt2 = 0;
	u64 cur_para_rslt = 0;
	bool found = false;
	bool finished = false;
	u64 remained_size = mStrList.size();
	u64 cur_block_size = 0;
	bool in_range = false;
OmnScreen << "Query for Str" << endl;
u64	times = 0;

	while(1)
	{
		//Query for a block
		times++;
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context4->setBlockSize(cur_block_size);
		mQuery_rslt4 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mStrIILName_now,
			mQuery_rslt4,
			0,
			query_context4,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt4->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_str_pos < mStrList.size())
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < str_inlist.size(); i++)
					{
						if(mStrList[cur_str_pos].value == str_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
				    in_range = AosIILUtil::valueMatch(mStrList[cur_str_pos].value, mOpr, mStr_val1, mStr_val2);
			    }
				if(in_range)
				{
					found = true;
					if(cur_rslt != mStrList[cur_str_pos].docid)
					{
						// not match
						checkRslt(4);
						OmnAlarm << "Str Query Error:"
									"QueryRslt  not euqal to mStrList" << enderr;
					}
					// equal
					cur_str_pos ++;
					break;
				}
				cur_str_pos ++;
			}
			if(!found)
			{
				checkRslt(4);
				OmnAlarm << "Str Query Error:"
							"mStrList finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt4->nextDocid(finished); 
		}

		if(query_context4->finished())
		{
			break;
		}
	}
	
	for( ; cur_str_pos < mStrList.size(); cur_str_pos++)
	{
		if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < str_inlist.size(); i++)
					{
						if(mStrList[cur_str_pos].value == str_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
					in_range = AosIILUtil::valueMatch(mStrList[cur_str_pos].value, mOpr, mStr_val1, mStr_val2);
				}
		if(in_range)
		{
			checkRslt(4);
			OmnAlarm << "Str Query Error:"
						"mStrList has more valid data than QueryRslt:" << 
						cur_str_pos << " : " << mStrList[cur_str_pos].value << " : " <<  mStrList[cur_str_pos].docid  << enderr;
		}		
	}
	
	//////////////////
	//reverse query
	//////////////////
	
	// Prepare params for str reverse query
	query_context4 = AosQueryContextObj::createQueryContextStatic();
	query_context4->setOpr(mOpr);
	if(mOpr == eAosOpr_in)
	{
		query_context4->setStrValue(v);
	}
	else
	{
		query_context4->setStrValue(toString(mStr_val1));
		query_context4->setStrValue2(toString(mStr_val2));
	}
	query_context4->setReverse(true);
	cur_str_pos = mStrList.size()-1;
	cur_rslt = 0;
	found = false;
	remained_size = mStrList.size();
	cur_block_size = 0;
	in_range = false;
OmnScreen << "Reverse query for Str" << endl;
	while(1)
	{
		//Query for a block
		cur_block_size = random_block_size(remained_size);
		if(cur_block_size == 0)
		{
			remained_size = 0;
		}
		else
		{
			remained_size -= cur_block_size;
		}
		query_context4->setBlockSize(cur_block_size);
		mQuery_rslt4 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mStrIILName_now,
			mQuery_rslt4,
			0,
			query_context4,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;											
		cur_rslt = mQuery_rslt4->nextDocid(finished); 

		while(!finished)
		{
			found = false;
			// find 1 docid in real rslt
			while(cur_str_pos >= 0)
			{
				if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < str_inlist.size(); i++)
					{
						if(mStrList[cur_str_pos].value == str_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
					in_range = AosIILUtil::valueMatch(mStrList[cur_str_pos].value, mOpr, mStr_val1, mStr_val2);
				}
				if(in_range)
				{
					found = true;
					if(cur_rslt != mStrList[cur_str_pos].docid)
					{
						// not match
						checkRslt(4);
						OmnAlarm << "Str Query Error:"
									"QueryRslt  not euqal to mStrList" << enderr;
					}
					// equal
					cur_str_pos --;
					break;
				}
				cur_str_pos --;
			}
			if(!found)
			{
				checkRslt(4);
				OmnAlarm << "Str Query Error:"
							"mStrList finished before QueryRslt, docid not found in list:" << cur_rslt << enderr;
			}
			cur_rslt = mQuery_rslt4->nextDocid(finished); 
		}

		if(query_context4->finished())
		{
			break;
		}
	}
	
	for( ; cur_str_pos >= 0; cur_str_pos--)
	{
			if(mOpr == eAosOpr_in)
				{
					// in  or  not in
					bool found = false;
					for(int i = 0; i < str_inlist.size(); i++)
					{
						if(mStrList[cur_str_pos].value == str_inlist[i])
						{
							found = true;
							break;
						}
					}
					in_range = !(found ^ is_in);
				}
				else
				{
					in_range = AosIILUtil::valueMatch(mStrList[cur_str_pos].value, mOpr, mStr_val1, mStr_val2);
				}
		if(in_range)
		{
			checkRslt(4);
			OmnAlarm << "Str Query Error:"
						"mStrList has more valid data than QueryRslt:" << 
						cur_str_pos << " : " << mStrList[cur_str_pos].value << " : " <<  mStrList[cur_str_pos].docid  << enderr;
		}		
	}
	return true;
}



bool
AosIILCacheTester::selectOpr()
{
	int opr_sel = rand()%11+1;
	switch(opr_sel)
	{
		case 1:	//eq
			mOpr = eAosOpr_eq;
			break;
		case 2:	//gt
			mOpr = eAosOpr_gt;
			break;
		case 3:	//lt
			mOpr = eAosOpr_lt;
			break;
		case 4:	//ge
			mOpr = eAosOpr_ge;
			break;
		case 5:	//le
			mOpr = eAosOpr_le;
			break;
		case 6:	//ne
			mOpr = eAosOpr_ne;
			break;
		case 7:	//gt_lt
			mOpr = eAosOpr_range_gt_lt;
			if(mI64_val1 == mI64_val2)mI64_val2+=2048;
			if(mD64_val1 == mD64_val2)mD64_val2+=2048;
			if(mU64_val1 == mU64_val2)mU64_val2+=2048;
			if(mStr_val1 == mStr_val2)mStr_val2+=2048;
			break;
		case 8:	//ge_lt
			mOpr = eAosOpr_range_ge_lt;
			if(mI64_val1 == mI64_val2)mI64_val2+=2048;
			if(mD64_val1 == mD64_val2)mD64_val2+=2048;
			if(mU64_val1 == mU64_val2)mU64_val2+=2048;
			if(mStr_val1 == mStr_val2)mStr_val2+=2048;
			break;
		case 9:	//ge_le
			mOpr = eAosOpr_range_ge_le;
			break;
		case 10:	//gt_le
			mOpr = eAosOpr_range_gt_le;
			if(mI64_val1 == mI64_val2)mI64_val2+=2048;
			if(mD64_val1 == mD64_val2)mD64_val2+=2048;
			if(mU64_val1 == mU64_val2)mU64_val2+=2048;
			if(mStr_val1 == mStr_val2)mStr_val2+=2048;
			break;
		case 11:
			mOpr = eAosOpr_in;
			break;
		default:
			OmnAlarm << "====================picking opr error======================"<< enderr;
			return false;
	}
	OmnScreen << "mOpr:" << AosOpr_toStr(mOpr) << endl;
	return true;
}

bool
AosIILCacheTester::splitStrIIL()
{
	int num_split;

	if(mStrList.size() == 0) return true;
	num_split = rand()%9 + 2;

	//split iil
	AosQueryRsltObjPtr	query_rslt;
	AosQueryContextObjPtr context1;
	vector<AosQueryContextObjPtr> contexts1;
	AosIILClientObj::getIILClient()->getSplitValue(mStrIILName, context1, num_split, contexts1, mRundata);
	//compare the rslt and list
	OmnString split_val[num_split];
	
	int size = contexts1.size();
	aos_assert_r(size > 1,false);

	// check opr/value
	aos_assert_r(contexts1[0]->getOpr() == eAosOpr_lt,false);
	OmnString cur_str_value = contexts1[0]->getStrValue();
	int cur_pos = 0;
	split_val[cur_pos++] = cur_str_value;
	for(int i = 1;i < size-1;i++)
	{
		OmnString v1 = contexts1[i]->getStrValue();
		aos_assert_r(v1 == cur_str_value,false);
		OmnString v2 = contexts1[i]->getStrValue2();
		cur_str_value = v2;
		split_val[cur_pos++] = cur_str_value;
		aos_assert_r(contexts1[i]->getOpr() == eAosOpr_range_ge_lt,false);
	}
	aos_assert_r(contexts1[size-1]->getOpr() == eAosOpr_ge,false);
	aos_assert_r(contexts1[size-1]->getStrValue() == cur_str_value, false);
	

	int cur_rslt_pos = 0;
	int cur_list_pos = 0;
	int	split_idx[size];
	for(; cur_rslt_pos < contexts1.size()-1; cur_rslt_pos++)
	{
		for(; cur_list_pos < mStrList.size(); cur_list_pos++)
		{
			if(toString(mStrList[cur_list_pos].value) == split_val[cur_rslt_pos])
			{
				split_idx[cur_rslt_pos] = cur_list_pos;
				break;
			}
		}
		if(cur_list_pos >= mStrList.size())
		{
			OmnAlarm << "split error" << enderr;
		}
	}
	split_idx[cur_rslt_pos] = mStrList.size();
/*	
	u64 max = split_idx[0];
	u64 min = split_idx[0];

	for(int i = size-1;i >0 ;i--)
	{
		split_idx[i] -= split_idx[i-1];
		if(split_idx[i] > max) max = split_idx[i];
		if(split_idx[i] < min) min = split_idx[i];
	}

	float split_limit = 0.05;
	if( max-min>500 && ((float)(max-min))/max > split_limit)
	{
		OmnAlarm << "split differences are too large" << enderr;
	}
*/
	return true;
}

bool
AosIILCacheTester::splitU64IIL()
{
	int num_split;

	if(mU64List.size() == 0) return true;
	num_split = rand()%9 + 2;

	//split iil
	AosQueryRsltObjPtr	query_rslt;
	AosQueryContextObjPtr context1;
	vector<AosQueryContextObjPtr> contexts1;
	AosIILClientObj::getIILClient()->getSplitValue(mU64IILName, context1, num_split, contexts1, mRundata);
	//compare the rslt and list
	u64 split_val[num_split];
	
	int size = contexts1.size();
	aos_assert_r(size > 1,false);

	// check opr/value
	aos_assert_r(contexts1[0]->getOpr() == eAosOpr_lt,false);
	u64 cur_u64_value = contexts1[0]->getStrValue().toU64(0);
	int cur_pos = 0;
	split_val[cur_pos++] = cur_u64_value;
	for(int i = 1;i < size-1;i++)
	{
		u64 v1 = contexts1[i]->getStrValue().toU64(0);
		aos_assert_r(v1 == cur_u64_value,false);
		u64 v2 = contexts1[i]->getStrValue2().toU64(0);
		cur_u64_value = v2;
		split_val[cur_pos++] = cur_u64_value;
		aos_assert_r(contexts1[i]->getOpr() == eAosOpr_range_ge_lt,false);
	}
	aos_assert_r(contexts1[size-1]->getOpr() == eAosOpr_ge,false);
	aos_assert_r(contexts1[size-1]->getStrValue().toU64(0) == cur_u64_value, false);
	

	int cur_rslt_pos = 0;
	int cur_list_pos = 0;
	int	split_idx[size];
	for(; cur_rslt_pos < contexts1.size()-1; cur_rslt_pos++)
	{
		for(; cur_list_pos < mU64List.size(); cur_list_pos++)
		{
			if(mU64List[cur_list_pos].value == split_val[cur_rslt_pos])
			{
				split_idx[cur_rslt_pos] = cur_list_pos;
				break;
			}
		}
		if(cur_list_pos >= mU64List.size())
		{
			OmnAlarm << "split error" << enderr;
		}
	}
	split_idx[cur_rslt_pos] = mU64List.size();
/*	
	u64 max = split_idx[0];
	u64 min = split_idx[0];

	for(int i = size-1;i >0 ;i--)
	{
		split_idx[i] -= split_idx[i-1];
		if(split_idx[i] > max) max = split_idx[i];
		if(split_idx[i] < min) min = split_idx[i];
	}

	float split_limit = 0.05;
	if( max-min>500 && ((float)(max-min))/max > split_limit)
	{
		OmnAlarm << "split differences are too large" << enderr;
	}
*/
	return true;
}

bool
AosIILCacheTester::splitI64IIL()
{
	int num_split;

	if(mI64List.size() == 0) return true;
	num_split = rand()%9 + 2;

	//split iil
	AosQueryRsltObjPtr	query_rslt;
	AosQueryContextObjPtr context1;
	vector<AosQueryContextObjPtr> contexts1;
	AosIILClientObj::getIILClient()->getSplitValue(mI64IILName, context1, num_split, contexts1, mRundata);
	//compare the rslt and list
	i64 split_val[num_split];
	
	int size = contexts1.size();
	aos_assert_r(size > 1,false);

	// check opr/value
	aos_assert_r(contexts1[0]->getOpr() == eAosOpr_lt,false);
	i64 cur_i64_value = contexts1[0]->getStrValue().toI64(0);
	int cur_pos = 0;
	split_val[cur_pos++] = cur_i64_value;
	for(int i = 1;i < size-1;i++)
	{
		i64 v1 = contexts1[i]->getStrValue().toI64(0);
		aos_assert_r(v1 == cur_i64_value,false);
		i64 v2 = contexts1[i]->getStrValue2().toI64(0);
		cur_i64_value = v2;
		split_val[cur_pos++] = cur_i64_value;
		aos_assert_r(contexts1[i]->getOpr() == eAosOpr_range_ge_lt,false);
	}
	aos_assert_r(contexts1[size-1]->getOpr() == eAosOpr_ge,false);
	aos_assert_r(contexts1[size-1]->getStrValue().toI64(0) == cur_i64_value, false);
	

	int cur_rslt_pos = 0;
	int cur_list_pos = 0;
	int	split_idx[size];
	for(; cur_rslt_pos < contexts1.size()-1; cur_rslt_pos++)
	{
		for(; cur_list_pos < mI64List.size(); cur_list_pos++)
		{
			if(mI64List[cur_list_pos].value == split_val[cur_rslt_pos])
			{
				split_idx[cur_rslt_pos] = cur_list_pos;
				break;
			}
		}
		if(cur_list_pos >= mI64List.size())
		{
			OmnAlarm << "split error" << enderr;
		}
	}
	split_idx[cur_rslt_pos] = mI64List.size();
/*	
	i64 max = split_idx[0];
	i64 min = split_idx[0];

	for(int i = size-1;i >0 ;i--)
	{
		split_idx[i] -= split_idx[i-1];
		if(split_idx[i] > max) max = split_idx[i];
		if(split_idx[i] < min) min = split_idx[i];
	}

	float split_limit = 0.05;
	if( max-min>500 && ((float)(max-min))/max > split_limit)
	{
		OmnAlarm << "split differences are too large" << enderr;
	}
*/
	return true;
}

bool
AosIILCacheTester::splitD64IIL()
{
	int num_split;

	if(mD64List.size() == 0) return true;
	num_split = rand()%9 + 2;

	//split iil
	AosQueryRsltObjPtr	query_rslt;
	AosQueryContextObjPtr context1;
	vector<AosQueryContextObjPtr> contexts1;
	AosIILClientObj::getIILClient()->getSplitValue(mD64IILName, context1, num_split, contexts1, mRundata);
	//compare the rslt and list
	d64 split_val[num_split];
	
	int size = contexts1.size();
	aos_assert_r(size > 1,false);

	// check opr/value
	aos_assert_r(contexts1[0]->getOpr() == eAosOpr_lt,false);
	d64 cur_d64_value = contexts1[0]->getStrValue().toD64(0);
	int cur_pos = 0;
	split_val[cur_pos++] = cur_d64_value;
	for(int i = 1;i < size-1;i++)
	{
		d64 v1 = contexts1[i]->getStrValue().toD64(0);
		aos_assert_r(v1 == cur_d64_value,false);
		d64 v2 = contexts1[i]->getStrValue2().toD64(0);
		cur_d64_value = v2;
		split_val[cur_pos++] = cur_d64_value;
		aos_assert_r(contexts1[i]->getOpr() == eAosOpr_range_ge_lt,false);
	}
	aos_assert_r(contexts1[size-1]->getOpr() == eAosOpr_ge,false);
	aos_assert_r(contexts1[size-1]->getStrValue().toD64(0) == cur_d64_value, false);
	

	int cur_rslt_pos = 0;
	int cur_list_pos = 0;
	int	split_idx[size];
	for(; cur_rslt_pos < contexts1.size()-1; cur_rslt_pos++)
	{
		for(; cur_list_pos < mD64List.size(); cur_list_pos++)
		{
			if(mD64List[cur_list_pos].value == split_val[cur_rslt_pos])
			{
				split_idx[cur_rslt_pos] = cur_list_pos;
				break;
			}
		}
		if(cur_list_pos >= mD64List.size())
		{
			OmnAlarm << "split error" << enderr;
		}
	}
	split_idx[cur_rslt_pos] = mD64List.size();
/*	
	d64 max = split_idx[0];
	d64 min = split_idx[0];

	for(int i = size-1;i >0 ;i--)
	{
		split_idx[i] -= split_idx[i-1];
		if(split_idx[i] > max) max = split_idx[i];
		if(split_idx[i] < min) min = split_idx[i];
	}

	float split_limit = 0.05;
	if( max-min>500 && ((float)(max-min))/max > split_limit)
	{
		OmnAlarm << "split differences are too large" << enderr;
	}
*/
	return true;
}

xxxxxx
random(u64 a, u64 b)
{
	aos_assert_r(a <= b,0);
	return rand()%(b - a+1) + a;
}
