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
#include "IILClient/Tester2/IILI64D64Tester.h"

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

extern i64 sgNumber;
extern i64 sgTime;
extern i64 sgMaxRoundTime;
extern i64 sgMaxMemory;
extern i64 sgAlarmFlag;

AosIILI64D64Tester::AosIILI64D64Tester()
{
	mZeroStr = "00000000000000000000";
	u64 iilid = 0;

	mRound = 0;
	mNumber = sgNumber;
	mTime = sgTime;
	mMaxRoundTime = sgMaxRoundTime;
	mMaxMemory = sgMaxMemory;
	mAlarmFlag = sgAlarmFlag;
	
	mAdd_times = 0;
	mRemove_times = 0;
	mQuery_times = 0;
	mDirty = false;
	mI64IILName = "_zt44_i64iil";	
//	mI64ParaIILName1 = "";	
//	mI64ParaIILName2 = "";	
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(200);

//	AosIILClientObj::getIILClient()->deleteIIL(mI64IILName, true, mRundata);
	AosIILClientObj::getIILClient()->createIILPublic(mI64IILName, iilid, eAosIILType_BigI64, mRundata);
//	AosIILClientObj::getIILClient()->createIILPublic(mI64ParaIILName1, iilid, eAosIILType_BigI64, mRundata);
//	AosIILClientObj::getIILClient()->createIILPublic(mI64ParaIILName2, iilid, eAosIILType_BigI64, mRundata);

	mD64IILName = "_zt44_d64iil";
//	AosIILClientObj::getIILClient()->deleteIIL(mD64IILName, true, mRundata);
	AosIILClientObj::getIILClient()->createIILPublic(mD64IILName, iilid, eAosIILType_BigD64, mRundata);

	mU64IILName = "_zt44_u64iil";
//	AosIILClientObj::getIILClient()->deleteIIL(mU64IILName, true, mRundata);
	AosIILClientObj::getIILClient()->createIILPublic(mU64IILName, iilid, eAosIILType_BigU64, mRundata);

	mStrIILName = "_zt44_striil";
//	AosIILClientObj::getIILClient()->deleteIIL(mStrIILName, true, mRundata);
	AosIILClientObj::getIILClient()->createIILPublic(mStrIILName, iilid, eAosIILType_BigStr, mRundata);

	mHitIILName = "_zt44_hitiil";
	AosIILClientObj::getIILClient()->createIILPublic(mHitIILName, iilid, eAosIILType_BigHit, mRundata);//eAosIILType_BigHit huang
}


AosIILI64D64Tester::~AosIILI64D64Tester()
{
}

bool
AosIILI64D64Tester::start()
{
	srand(1005);

	system("mkdir /tmp/torturer/");
	if(mAlarmFlag) system("touch /tmp/torturer/alarm_flag_file");
	OmnString state_file_str = "/tmp/torturer/iiltorturer_states_file";
	mStateFile = OmnNew OmnFile(state_file_str, OmnFile::eCreate AosMemoryCheckerArgs);
	time_t total_time_dur = 0;
	time_t round_time_dur = 0;
	time_t round_start_time = 0;
	time_t start_time = time(NULL);
	time_t end_time = start_time;
	int k = 0;
	
	if(mNumber>0)
	{
		for(int k=0; k<mNumber; k++)
		{
			mStateFile->openFile(OmnFile::eAppend);
			round_start_time = time(NULL);
			
			iilTorturer(k);
			
			end_time = time(NULL);
			total_time_dur = (end_time-start_time);
			round_time_dur = (end_time-round_start_time);
			OmnString time_str = "total time : ";
			time_str << total_time_dur << "s\n";
			time_str << "current round take time : " << round_time_dur << "s\n";
			mStateFile->append(time_str);
			mStateFile->closeFile();
		}
	}
	else
	{
		while(total_time_dur/60 < mTime)
		{
			mStateFile->openFile(OmnFile::eAppend);
			round_start_time = time(NULL);
			
			iilTorturer(k++);
			
			end_time = time(NULL);
			total_time_dur = (end_time-start_time);
			round_time_dur = (end_time-round_start_time);
			OmnString time_str = "total time : ";
			time_str << total_time_dur << "s\n";
			time_str << "current round take time : " << round_time_dur << "s\n";
			mStateFile->append(time_str);
			mStateFile->closeFile();
		}
	}
}

bool      	
AosIILI64D64Tester::iilTorturer(const int k)
{	
	OmnScreen << " IIL I64/D64/U64/Str/Hit Test start ..." << endl;
	//case 1 : add entry
	//case 2 : add entries
	//case 3 : remove entry
	//case 4 : remove entries
	//case 5 : query
	//	switch query mOpr
	//		1. eq
	//		2. gt
	//		3. ge
	//		4. lt
	//		5. le
	//		6. ne
	//		7. in(optional)
	//		8. gt_lt
	//		9. gt_le
	//		10. ge_lt
	//		11. ge_le
	
	//	mi64iil = AosIIL(i64);
	int opr1 = rand() % 6 + 1;
	
	OmnString round_str = "\nRound : ";
	round_str << mRound++;
	mStateFile->append(round_str);

	switch(opr1)
	{
		case 1:
			mStateFile->append(" addEntry\n");
			//	addEntry();
			break;
		case 2:
			OmnScreen << "=====================" << endl;
			OmnScreen << "Round: " << k << " Add Entries" << endl; 
			OmnScreen << "		current i64 entries: " << mI64List.size() << endl; 
			OmnScreen << "		current d64 entries: " << mD64List.size() << endl; 
			OmnScreen << "		current u64 entries: " << mU64List.size() << endl; 
			OmnScreen << "		current str entries: " << mStrList.size() << endl; 
			OmnScreen << "		current hit entries: " << mHitList.size() << endl; 
			OmnScreen << "=====================" << endl;
			
			mStateFile->append(" addEntries\n");
			
			addEntries();
			break;
		case 3:
			//	removeEntry();
			mStateFile->append(" removeEntry\n");
			break;
		case 4:
			OmnScreen << "=====================" << endl;
			OmnScreen << "Round: " << k << " Remove Entries" << endl; 
			OmnScreen << "		current i64 entries: " << mI64List.size() << endl; 
			OmnScreen << "		current d64 entries: " << mD64List.size() << endl; 
			OmnScreen << "		current U64 entries: " << mU64List.size() << endl; 
			OmnScreen << "		current str entries: " << mStrList.size() << endl; 
			OmnScreen << "		current hit entries: " << mHitList.size() << endl;
			OmnScreen << "=====================" << endl;
			mStateFile->append(" removeEntries\n");
			removeEntries();
			break;
		case 5:
			OmnScreen << "=====================" << endl;
			OmnScreen << "Round: " << k << " Query" << endl; 
			OmnScreen << "		current i64 entries: " << mI64List.size() << endl; 
			OmnScreen << "		current d64 entries: " << mD64List.size() << endl; 
			OmnScreen << "		current u64 entries: " << mU64List.size() << endl; 
			OmnScreen << "		current str entries: " << mStrList.size() << endl; 
			OmnScreen << "		current hit entries: " << mHitList.size() << endl;
			OmnScreen << "=====================" << endl;
			mStateFile->append(" query\n");
			query();
			break;
		case 6:
			mStateFile->append(" splitIIL\n");
			splitStrIIL();
			splitU64IIL();
			splitI64IIL();
			splitD64IIL();
			//				splitHitIIL();
			break;
		default:
			break;
	}
	return true;
}
/*
   bool
   AosIILI64D64Tester::addEntry()
   {
// add entry
//
mDirty = true;
i64 i64_value = getI64Val();
d64 d64_value = getD64Val();
u64 u64_value = getU64Val();
u64 str_value = getStrVal();

OmnString str_value1;
OmnString str_value2;
OmnString str_value3;
OmnString str_value4 = toString(str_value);

str_value1 << i64_value;
str_value2 << d64_value;
str_value3 << u64_value;

u64 docid = rand();

AosIILClientObj::getIILClient()->addValueDoc(mI64IILName, eAosIILType_BigI64, str_value1, docid, false, false, mRundata);
AosIILClientObj::getIILClient()->addValueDoc(mD64IILName, eAosIILType_BigD64, str_value2, docid, false, false, mRundata);
AosIILClientObj::getIILClient()->addValueDoc(mU64IILName, eAosIILType_BigU64, str_value3, docid, false, false, mRundata);
AosIILClientObj::getIILClient()->addValueDoc(mStrIILName, eAosIILType_BigStr, str_value4, docid, false, false, mRundata);
// find the pos
//
I64Entry new_entry1(i64_value,docid);
D64Entry new_entry2(d64_value,docid);
U64Entry new_entry3(u64_value,docid);
StrEntry new_entry4(str_value,docid);

mI64List.push_back(new_entry1);
mD64List.push_back(new_entry2);
mU64List.push_back(new_entry3);
mStrList.push_back(new_entry4);

sort(mI64List.begin(),mI64List.end());
sort(mD64List.begin(),mD64List.end());
sort(mU64List.begin(),mU64List.end());
sort(mStrList.begin(),mStrList.end());

return true;
}
*/
bool
AosIILI64D64Tester::addEntries()
{
	// 1. How many entries we should add into the list(random from eMinGroupSize to eMaxGroupSize
	//assert(min <= max)
	//random(min,max)			min + random()%(max-min)
	
	// 2. Random create entries, add into a list (ordered)
	//
	// 3. Create mBuff
	//
	// 4. Call BatchAdd to add entries into IIL
	//
	// 5. Insert all entries into I64List
	//
	
//	mDirty = true;
	mAdd_times++;
	OmnScreen << "BatchAdd...:" << mAdd_times << endl; 	
	

	u64 num_create = randnum(eMinGroupSize, eMaxGroupSize);
	OmnScreen << "num_create:" << num_create << endl;

	vector<I64Entry> new_entries1;
	vector<I64Entry> para_entries1;
	vector<I64Entry> para_entries2;
	i64 cur_i64_val;
	u64 cur_docid;

	vector<D64Entry> new_entries2;
	d64 cur_d64_val;
	
	vector<U64Entry> new_entries3;
	u64 cur_u64_val;

	vector<StrEntry> new_entries4;
	u64 cur_str_val;

	vector<u64> new_entries5;
	u64 cur_hit_val;

	int ii;

	//create i64 entries
	for(i64 i = 0; i < num_create; i++)
	{
		cur_i64_val = getI64Val();
		cur_docid = rand();

		if(cur_i64_val%2==0)	
		{
			para_entries1.push_back(I64Entry(cur_i64_val, cur_docid));
		}
		else
		{
			para_entries2.push_back(I64Entry(cur_i64_val, cur_docid));
		}
		new_entries1.push_back(I64Entry(cur_i64_val, cur_docid));
		mI64List.push_back(I64Entry(cur_i64_val, cur_docid));
	}
	sort(para_entries1.begin(),para_entries1.end());
	sort(para_entries2.begin(),para_entries2.end());
	sort(new_entries1.begin(), new_entries1.end());
	sort(mI64List.begin(), mI64List.end());

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

	//create hit entries
	for(i64 i = 0; i < num_create; i++)
	{
		cur_docid = rand();
		new_entries5.push_back(cur_docid);
		mHitList.push_back(cur_docid);
	}
	sort(new_entries5.begin(), new_entries5.end());
	sort(mHitList.begin(), mHitList.end());	
	


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

	mI64ParaBuff1 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	int para_size1 = para_entries1.size();
	mI64ParaBuff1->ensureSpace(para_size1*2*sizeof(u64));
	mI64ParaBuff1->setDataLen(para_size1*2*sizeof(u64));
	i64* cur_para_pos1 = (i64*)mI64ParaBuff1->data();
	for(int i = 0;i < para_size1;i++)
	{
		*cur_para_pos1 = para_entries1[i].value;
		cur_para_pos1 ++;
		*((u64*)cur_para_pos1) = para_entries1[i].docid;
		cur_para_pos1 ++;
	}

	mI64ParaBuff2 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	int para_size2 = para_entries2.size();
	mI64ParaBuff2->ensureSpace(para_size2*2*sizeof(u64));
	mI64ParaBuff2->setDataLen(para_size2*2*sizeof(u64));
	i64* cur_para_pos2 = (i64*)mI64ParaBuff2->data();
	for(int i = 0;i < para_size2;i++)
	{
		*cur_para_pos2 = para_entries2[i].value;
		cur_para_pos2 ++;
		*((u64*)cur_para_pos2) = para_entries2[i].docid;
		cur_para_pos2 ++;
	}


	AosIILClientObj::getIILClient()->BatchAdd(mI64IILName, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
	mI64Buff = 0;

//	AosIILClientObj::getIILClient()->BatchAdd(mI64ParaIILName1, eAosIILType_BigI64, 16, mI64ParaBuff1, 0, 0, 1, mRundata);
//	mI64ParaBuff1 = 0;

//	AosIILClientObj::getIILClient()->BatchAdd(mI64ParaIILName2, eAosIILType_BigI64, 16, mI64ParaBuff2, 0, 0, 1, mRundata);
//	mI64ParaBuff2 = 0;
	
	
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

	// create mBuff for hit IIL
	mHitBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	mHitBuff->ensureSpace(num_create * sizeof(u64));
	mHitBuff->setDataLen(num_create  * sizeof(u64));
	u64* cur_pos5 = (u64*)mHitBuff->data();
	for (int i = 0;i < new_entries5.size();i++)
	{
		*cur_pos5 = new_entries5[i];
		cur_pos5 ++;
	}

	AosIILClientObj::getIILClient()->BatchAdd(mHitIILName, eAosIILType_BigHit, 8, mHitBuff, 0, 0, 1, mRundata);//eAosIILType_BigHit huang
	mHitBuff = 0;
	
	

	return true;
}

/*
bool
AosIILI64D64Tester::removeEntry()
{
	mDirty = true;
	if(mI64List.size()!=0)
	{
		int size = mI64List.size();
		int pos = rand() % size;

		i64 cur_value = mI64List[pos].value;

		OmnString cur_str_val;
		cur_str_val << cur_value;
		u64 cur_docid = mI64List[pos].docid;
	
		for(int i = pos; i < size-1; i++)
		{
			mI64List[i] = mI64List[i+1];
		}
		mI64List.resize(size-1);

		//	//从IIL中删除
		AosIILClientObj::getIILClient()->removeValueDoc(mI64IILName, eAosIILType_BigI64, cur_str_val, cur_docid, mRundata);
	}
	
	if(mD64List.size()!=0)
	{
		int size = mD64List.size();
		int pos = rand() % size;
		
		d64 cur_value = mD64List[pos].value;
		OmnString cur_str_val;
		cur_str_val << cur_value;

		u64 cur_docid = mD64List[pos].docid;
		
		for(int i = pos; i < size-1; i++)
		{
			mD64List[i] = mD64List[i+1];
		}
		mD64List.resize(size-1);
		//从IIL中删除
		AosIILClientObj::getIILClient()->removeValueDoc(mD64IILName, eAosIILType_BigD64, cur_str_val, cur_docid, mRundata);
	}
	
	if(mU64List.size()!=0)
	{
		int size = mU64List.size();
		int pos = rand() % size;
		
		u64 cur_value = mU64List[pos].value;
		OmnString cur_str_val;
		cur_str_val << cur_value;

		u64 cur_docid = mU64List[pos].docid;
		
		for(int i = pos; i < size-1; i++)
		{
			mU64List[i] = mU64List[i+1];
		}
		mU64List.resize(size-1);
		//从IIL中删除
		AosIILClientObj::getIILClient()->removeValueDoc(mU64IILName, eAosIILType_BigU64, cur_str_val, cur_docid, mRundata);
	}
	
	if(mStrList.size()!=0)
	{
		int size = mStrList.size();
		int pos = rand() % size;
		
		u64 cur_str_val = mStrList[pos].value;

		u64 cur_docid = mStrList[pos].docid;
		
		for(int i = pos; i < size-1; i++)
		{
			mStrList[i] = mStrList[i+1];
		}
		mStrList.resize(size-1);
		//从IIL中删除
		AosIILClientObj::getIILClient()->removeValueDoc(mStrIILName, eAosIILType_BigStr, toString(cur_str_val), cur_docid, mRundata);
	}
	
	return true;
}
*/

bool
AosIILI64D64Tester::removeEntries()
{
	//mDirty = true;
	mRemove_times++;
	OmnScreen << "BatchRemove...:" << mRemove_times << endl;
	
	u64 docid;
	u64 pos_remove = 0;
	u64 para_pos_remove1 = 0;
	u64 para_pos_remove2 = 0;
	
	u64 num_remove = randnum(0, mI64List.size());
	//OmnScreen << "num_remove:" << num_remove << endl;

	
	if(mI64List.size() != 0)
	{
		//remove entries
		vector<I64Entry> remove_entries1;
		remove_entries1.reserve(num_remove);
		vector<I64Entry> para_remove_entries1;
		para_remove_entries1.reserve(num_remove);
		vector<I64Entry> para_remove_entries2;
		para_remove_entries2.reserve(num_remove);
		
		u64 entry_size = 0;	
		u64	para_entry_size1 = 0;	
		u64 para_entry_size2 = 0;	
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
				//put it into remove list
				//remove_entries1.push_back(mI64List[b]);
				//remove_entries1[remove_entries1.size()] = mI64List[b];
				remove_entries1.resize(entry_size+1);
				remove_entries1[entry_size++] = mI64List[b];
				if(mI64List[b].value%2 == 0)
				{
					para_remove_entries1.resize(para_entry_size1+1);
					para_remove_entries1[para_entry_size1++] = mI64List[b];	
				}
				else
				{
					para_remove_entries2.resize(para_entry_size2+1);
					para_remove_entries2[para_entry_size2++] = mI64List[b];
				}
			}
			else
			{
				//copy entry if needs
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

		mI64ParaBuff1 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		num_real_remove = para_remove_entries1.size();
		mI64ParaBuff1->ensureSpace(num_real_remove * 2 * sizeof(u64));
		mI64ParaBuff1->setDataLen(num_real_remove * 2 * sizeof(u64));
		cur_pos1 = (i64*)mI64ParaBuff1->data();
		for(int i = 0; i < para_remove_entries1.size(); i++)
		{
			*cur_pos1 = para_remove_entries1[i].value;
			cur_pos1 ++;
			*((u64*)cur_pos1) = para_remove_entries1[i].docid;
			cur_pos1 ++;
		}

		mI64ParaBuff2 = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		num_real_remove = para_remove_entries2.size();
		mI64ParaBuff2->ensureSpace(num_real_remove * 2 * sizeof(u64));
		mI64ParaBuff2->setDataLen(num_real_remove * 2 * sizeof(u64));
		cur_pos1 = (i64*)mI64ParaBuff2->data();
		for(int i = 0; i < para_remove_entries2.size(); i++)
		{
			*cur_pos1 = para_remove_entries2[i].value;
			cur_pos1 ++;
			*((u64*)cur_pos1) = para_remove_entries2[i].docid;
			cur_pos1 ++;
		}

		AosIILClientObj::getIILClient()->BatchDel(mI64IILName, eAosIILType_BigI64, 16, mI64Buff, 0, 0, 1, mRundata);
		mI64Buff = 0;

//		AosIILClientObj::getIILClient()->BatchDel(mI64ParaIILName1, eAosIILType_BigI64, 16, mI64ParaBuff1, 0, 0, 1, mRundata);
//		mI64ParaBuff1 = 0;

//		AosIILClientObj::getIILClient()->BatchDel(mI64ParaIILName2, eAosIILType_BigI64, 16, mI64ParaBuff2, 0, 0, 1, mRundata);
//		mI64ParaBuff2 = 0;

		//remove data from mI64List
		mI64List.resize(a);
	}

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

		AosIILClientObj::getIILClient()->BatchDel(mD64IILName, eAosIILType_BigD64, 16, mD64Buff, 0, 0, 1, mRundata);
		mD64Buff = 0;

		//remove data from mI64List
		mD64List.resize(a);
	}
	
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
		{
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

		AosIILClientObj::getIILClient()->BatchDel(mU64IILName, eAosIILType_BigU64, 16, mU64Buff, 0, 0, 1, mRundata);
		mU64Buff = 0;

		//remove data from mI64List
		mU64List.resize(a);
	}
	
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

	if(mHitList.size() != 0)
	{
		//remove entries
		vector<u64> remove_entries5;
		remove_entries5.reserve(num_remove);
		u64 entry_size = 0;
		
		//create remove_flag array
		u64 size1 = mHitList.size();
		bool* flag = OmnNew bool[size1];
		memset(flag, 0, size1);

		for(int i = 0; i < num_remove; i++)
		{
			pos_remove = randnum(0, mHitList.size()-1);
			flag[pos_remove] = true;
		}

		u64 a = 0;
		u64 b = 0;
		for(;b < mHitList.size();b++)
		{
			if(flag[b])
			{
				//put it into remove list
				//remove_entries1[remove_entries1.size()] =  mD64List[b];
				remove_entries5.push_back(mHitList[b]);
			}
			else
			{
				//copy entry if needs
				if(a != b)
				{
					mHitList[a] = mHitList[b];
				}
				a++;
			}
		}
		delete []flag;
		OmnScreen << "Hit remove " << remove_entries5.size() << " entries" << endl;
		mHitBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		u64 num_real_remove = remove_entries5.size();
		mHitBuff->ensureSpace(num_real_remove * sizeof(u64));
		mHitBuff->setDataLen(num_real_remove  * sizeof(u64));
		u64* cur_pos1 = (u64*)mHitBuff->data();
		for(int i = 0; i < remove_entries5.size(); i++)
		{
				*cur_pos1 = remove_entries5[i];
				cur_pos1 ++;
		}

		AosIILClientObj::getIILClient()->BatchDel(mHitIILName, eAosIILType_BigHit, 8, mHitBuff, 0, 0, 1, mRundata);//eAosIILType_BigHit huang
		mHitBuff = 0;

		//remove data from mI64List
		mHitList.resize(a);
	}
query();	
	
	return true;
}

bool
AosIILI64D64Tester::query()
{
	mQuery_times++;
	OmnScreen << "Query...:" << mQuery_times << endl;
	//query
	//
	//1. random mOpr
	//2. query
	//3. compare between the real result and the list we have

	if(mDirty)
	{
		AosIILClientObj::getIILClient()->rebuildBitmap(mI64IILName, mRundata);
		AosIILClientObj::getIILClient()->rebuildBitmap(mD64IILName, mRundata);
		AosIILClientObj::getIILClient()->rebuildBitmap(mU64IILName, mRundata);
		AosIILClientObj::getIILClient()->rebuildBitmap(mStrIILName, mRundata);
		AosIILClientObj::getIILClient()->rebuildBitmap(mHitIILName, mRundata);
		mDirty = false;
	}
	
	queryforI64();
	queryforD64();
	queryforU64();
	queryforStr();
	queryforHit();

	return true;
}

u64
AosIILI64D64Tester::randnum(const u64 min, const u64 max)
{
	u64 rand_num = min + rand()%(max - min + 1);
	return rand_num;
}


i64
AosIILI64D64Tester::getI64Val()
{
	int r = randnum(0,100);
	if(r< 48)
	{
		//from existing value
		if(mI64List.size() > 0)
		{
			int pos = rand()%mI64List.size();
			return mI64List[pos].value;
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
AosIILI64D64Tester::getI64Val2()
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
AosIILI64D64Tester::getD64Val()
{
	int r = randnum(0,100);
	if(r< 48)
	{
		//from existing value
		if(mD64List.size() > 0)
		{
			int pos = rand()%mD64List.size();
			return mD64List[pos].value;
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
AosIILI64D64Tester::getD64Val2()
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
AosIILI64D64Tester::getU64Val()
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
AosIILI64D64Tester::getU64Val2()
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
AosIILI64D64Tester::getStrVal()
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
AosIILI64D64Tester::getStrVal2()
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
AosIILI64D64Tester::checkRslt(const u64 i)
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
AosIILI64D64Tester::random_block_size(const u64 &remain)
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
AosIILI64D64Tester::toString(u64 orig_val)
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
AosIILI64D64Tester::queryforI64()
{
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
	u64 cur_para_rslt = 0;
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
			mI64IILName,
			mQuery_rslt1,
			0,
			query_context1,
			mRundata);
/*
		rslt = AosIILClientObj::getIILClient()->querySafe(
			mI64ParaIILName1,
			mParaQuery_rslt1,
			0,
			query_context1,
			mRundata);
		aos_assert_r(rslt,false);
*/
		
		// use bitmap query to get a result, compare with mQuery_rslt1
		AosQueryRsltObjPtr brslt1 = AosQueryRsltObj::getQueryRsltStatic();
		AosBitmapObjPtr bmp = AosGetBitmap();
		AosQueryContextObjPtr tmp_context = b_context1->clone();
		//1. bitmap query
		//2. send to bitmap server
		//3. bitmap rslt query
		
		//1. bitmap query
		rslt = AosIILClientObj::getIILClient()->bitmapQueryNewSafe(
			mI64IILName,
			brslt1,
			bmp,
			b_context1,
			mRundata);
		aos_assert_r(rslt,false);

		//2. send to bitmap server
		if(brslt1->getNumDocs() > 0)
		{
			vector<OmnString> iilnames;
			vector<AosBitmapObjPtr> bitmaps;
			bitmaps.push_back(bmp);
			vector<AosQueryRsltObjPtr> idlists;
			idlists.push_back(brslt1);
			vector<u32> empty_list;
			AosTransBitmapQueryPtr trans = OmnNew AosTransBitmapQuery(
					mRundata, 0, AosTransBitmapQuery::eTestTermID, iilnames, idlists,
					bitmaps, empty_list);
			OmnScreen << "To send trans" << endl;
			rslt = AosSendTransAsyncResp(mRundata, trans);
			OmnScreen << "Trans sent" << endl;
			OmnScreen << "Wait........" << endl;
			trans->wait();
			OmnScreen << "Wake up" << endl;
			bmp = trans->getRslt();
			aos_assert_r(rslt,false);
		}
		if(!(bmp->isEmpty()))
		{
			//3. bitmap rslt query
			brslt1 = AosQueryRsltObj::getQueryRsltStatic();
			rslt = AosIILClientObj::getIILClient()->bitmapRsltQuerySafe(
					mI64IILName,
					brslt1,
					bmp,
					tmp_context,
					mRundata);
			aos_assert_r(rslt,false);
		}

		// compare with mQuery_rslt1
		brslt1->reset();
		mQuery_rslt1->reset();
		mParaQuery_rslt1->reset();
		aos_assert_r(brslt1->getNumDocs() == mQuery_rslt1->getNumDocs(),false);
		finished = false;
		while(!finished)
		{
			cur_rslt = mQuery_rslt1->nextDocid(finished); 
			if(finished)break;
			cur_rslt2 = brslt1->nextDocid(finished); 
			aos_assert_r(!finished,false);
			//cur_para_rslt = mParaQuery_rslt1->nextDocid(finished);
			//aos_assert_r(!finished,false);
			aos_assert_r(cur_rslt == cur_rslt2, false);
			//aos_assert_r(cur_rslt == cur_rslt2 && cur_rslt == cur_para_rslt,false);
		}
		
		finished = false;											
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
//	counter1 = 0;
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
		mQuery_rslt1 = AosQueryRsltObj::getQueryRsltStatic();
		mParaQuery_rslt1 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mI64IILName,
			mQuery_rslt1,
			0,
			query_context1,
			mRundata);

//		rslt = AosIILClientObj::getIILClient()->querySafe(
//			mI64ParaIILName1,
//			mParaQuery_rslt1,
//			0,
//			query_context1,
//			mRundata);

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
AosIILI64D64Tester::queryforD64()
{	
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
//*****************************//
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
			mD64IILName,
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
			mD64IILName,
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
AosIILI64D64Tester::queryforU64()
{	
/*	
	mU64_val1 = getU64Val();
	mU64_val2 = getU64Val();
	if(mU64_val1 > mU64_val2)
	{
		u64 tmp = mU64_val1;
		mU64_val1 = mU64_val2;
		mU64_val2 = tmp;
	}
	
	selectOpr();
	
	// Prepare params for u64 query
	AosQueryContextObjPtr 	query_context3 = AosQueryContextObj::createQueryContextStatic();
	query_context3->setOpr(mOpr);
	query_context3->setStrValue(OmnStrUtil::ulltoa(mU64_val1));
	query_context3->setStrValue2(OmnStrUtil::ulltoa(mU64_val2));

	//U64 
	i64 cur_u64_pos = 0;
	u64 cur_rslt = 0;
	bool found = false;
	u64 remained_size = mU64List.size();
	u64 cur_block_size = 0;
	bool in_range = false;
OmnScreen << "Query for U64" << endl;
*/
//******************************************************//
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
//******************************************************//
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
			mU64IILName,
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
			mU64IILName,
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
AosIILI64D64Tester::queryforStr()
{	
/*	
	mStr_val1 = getStrVal();
	mStr_val2 = getStrVal();
	if(mStr_val1 > mStr_val2)
	{
		u64 tmp = mStr_val1;
		mStr_val1 = mStr_val2;
		mStr_val2 = tmp;
	}
	
	selectOpr();
	
	// Prepare params for str query
	AosQueryContextObjPtr 	query_context4 = AosQueryContextObj::createQueryContextStatic();
	query_context4->setOpr(mOpr);
	query_context4->setStrValue(toString(mStr_val1));//liuwei
	query_context4->setStrValue2(toString(mStr_val2));

	//Str 
	i64 cur_str_pos = 0;
	u64 cur_rslt = 0;
	bool found = false;
	u64 remained_size = mStrList.size();
	u64 cur_block_size = 0;
	bool in_range = false;
OmnScreen << "Query for Str" << endl;
*/
//*************************************************************//
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
//*************************************************************//
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
			mStrIILName,
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
			mStrIILName,
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
AosIILI64D64Tester::queryforHit()
{	
	
	AosQueryContextObjPtr query_context5 = AosQueryContextObj::createQueryContextStatic();

	AosQueryContextObjPtr b_context5 = query_context5->clone();

	//Hit
	i64 cur_hit_pos = 0;
	i64 rslt_size=0;
	u64 cur_rslt=0;
	bool finished = false;
	u64 remained_size = mHitList.size();
	u64 cur_block_size = 0;
	u64 rslt_NumDocs=0;
    OmnScreen << "Query for Hit" << endl;

	cur_hit_pos = 0;
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
		b_context5->setBlockSize(cur_block_size);
		query_context5->setBlockSize(cur_block_size);
		
		AosQueryContextObjPtr tmp_context1 = query_context5->clone();
		AosQueryContextObjPtr tmp_context2;

		mQuery_rslt5 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mHitIILName,
			mQuery_rslt5,
			0,
			query_context5,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;	
		mQuery_rslt5->reset();										
		cur_rslt = mQuery_rslt5->nextDocid(finished); 
		while(!finished)
		{
			aos_assert_r(cur_hit_pos < mHitList.size(),false);
			
			if(cur_rslt != mHitList[cur_hit_pos])
			{
				OmnAlarm << "query error" << enderr;
				while(1)
				{
					OmnScreen << "query error" << endl;
					tmp_context2 = tmp_context1->clone();
					rslt = AosIILClientObj::getIILClient()->querySafe(
							mHitIILName,
							mQuery_rslt5,
							0,
							tmp_context2,
							mRundata);
				}

			}
			rslt_size ++;
			
			cur_hit_pos ++;
			
			cur_rslt = mQuery_rslt5->nextDocid(finished); 
		}

        rslt_NumDocs += mQuery_rslt5->getNumDocs();
		if(cur_block_size > 0)
		{
			aos_assert_r(cur_block_size >= mQuery_rslt5->getNumDocs(),false);
		}
		if(query_context5->finished())
		{
			break;
		}
		else
		{
			aos_assert_r(cur_block_size == mQuery_rslt5->getNumDocs(),false);
			// check the result size should equal to block size
		}
	}
	aos_assert_r(rslt_NumDocs == mHitList.size(),false);

	rslt_size=0;
	
	//////////////////
	//reverse query
	//////////////////
	
	// Prepare params for hit query
	query_context5 = AosQueryContextObj::createQueryContextStatic();
	query_context5->setReverse(true);
	//U64 
	cur_hit_pos = mHitList.size()-1;
	cur_rslt = 0;
	remained_size = mHitList.size()-1;
	cur_block_size = 0;
	rslt_NumDocs = 0;
    OmnScreen << "Reverse query for Hit" << endl;
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
		query_context5->setBlockSize(cur_block_size);
		
		AosQueryContextObjPtr tmp_context1 = query_context5->clone();
		AosQueryContextObjPtr tmp_context2;
		
		mQuery_rslt5 = AosQueryRsltObj::getQueryRsltStatic();
		bool rslt = AosIILClientObj::getIILClient()->querySafe(
			mHitIILName,
			mQuery_rslt5,
			0,
			query_context5,
			mRundata);
		aos_assert_r(rslt,false);
		bool finished = false;	
		mQuery_rslt5->reset();										
		cur_rslt = mQuery_rslt5->nextDocid(finished); 
		while(!finished)
		{
			// find 1 docid in real rslt
			aos_assert_r(cur_hit_pos >= 0, false);			
			if(cur_rslt != mHitList[cur_hit_pos])
			{
				OmnAlarm << "query error" << enderr;
				while(1)
				{
					tmp_context2 = tmp_context1->clone();
					rslt = AosIILClientObj::getIILClient()->querySafe(
							mHitIILName,
							mQuery_rslt5,
							0,
							tmp_context2,
							mRundata);
				}
			}
			cur_hit_pos --;
			cur_rslt = mQuery_rslt5->nextDocid(finished); 
		}
		
		rslt_NumDocs += mQuery_rslt5->getNumDocs();

		if(cur_block_size > 0)
		{
			aos_assert_r(cur_block_size >= mQuery_rslt5->getNumDocs(),false);
		}
		
		if(query_context5->finished())
		{
			break;
		}
		else
		{
			aos_assert_r(cur_block_size == mQuery_rslt5->getNumDocs(),false);
			// check the result size should equal to block size
		}
	}
	aos_assert_r(rslt_NumDocs == mHitList.size(),false);

	return true;
}


bool
AosIILI64D64Tester::selectOpr()
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
AosIILI64D64Tester::splitStrIIL()
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
AosIILI64D64Tester::splitU64IIL()
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
AosIILI64D64Tester::splitI64IIL()
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
AosIILI64D64Tester::splitD64IIL()
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
