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
// 2014/07/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapMgr/Testers/BitmapTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "AppMgr/App.h"
#include "BitmapMgr/Ptrs.h"
#include "BitmapMgr/Bitmap.h"
#include "BitmapMgr/Bitmap.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Util/File.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"

int print_flag = 0;
extern i64 sgNumber;
extern i64 sgTime;
extern i64 sgMaxRoundTime;
extern i64 sgMaxMemory;
extern i64 sgAlarmFlag;

AosBitmapTester::AosBitmapTester()
:
mNumAppends(0)
{
	aos_assert(config());
}


AosBitmapTester::~AosBitmapTester()
{
}

bool 
AosBitmapTester::config()
{
	mTries = 1000000;
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	if (!conf) return true;

	AosXmlTagPtr tag = conf->getFirstChild("bitmap_tester");
	if (!tag) return true;

	mTries = tag->getAttrU32("tries", 1000);
	if (mTries <= 0) mTries = 1000;

	return true;
}

bool 
AosBitmapTester::start()
{
	bool rslt = config();
	aos_assert_r(rslt, false);

	aos_assert_r(basicTest(), false);
	return true;
}


bool
AosBitmapTester::basicTest()
{
	srand(1005);                             
//	mBitmapSize = OmnRandom::intByRange(
//			0, 16383, 10,
//			16384, 10*16384-1, 20,
//			10*16384, 10*100*16384-1, 50);
	mBitmapSize = 1638400;
	aos_assert_r(mBitmapSize > 0, false);
	mArray1 = new char[mBitmapSize];
	mArray2 = new char[mBitmapSize];
	mBitmap1 = new AosBitmap();
	mBitmap2 = new AosBitmap();
	memset(mArray1, 0, mBitmapSize);
	memset(mArray2, 0, mBitmapSize);
	mRound = 0;
	mNumber = sgNumber;
	mTime = sgTime;
	mMaxRoundTime = sgMaxRoundTime;
	mMaxMemory = sgMaxMemory;
	mAlarmFlag = sgAlarmFlag;

	system("mkdir /tmp/torturer/");
	if(mAlarmFlag) system("touch /tmp/torturer/alarm_flag_file");
	OmnString state_file_str = "/tmp/torturer/bitmaptorturer_states_file";
	mStateFile = OmnNew OmnFile(state_file_str, OmnFile::eCreate AosMemoryCheckerArgs);
		
	time_t total_time_dur = 0;
	time_t round_time_dur = 0;
	time_t round_start_time = 0;
	time_t start_time = time(NULL);
	//u64 start_time = OmnGetTimestamp();
	//AosTimestampToHumanRead(cost)
	time_t end_time = start_time;
	int k = 0;

	if(mNumber>0)
	{
		for(int k=0; k<mNumber; k++)
		{
			mStateFile->openFile(OmnFile::eAppend);
			round_start_time = time(NULL);
		//	OmnString start_str = "Round start time : ";
		//	start_str << AosTimestampToHumanRead(OmnGetTimestamp()) << "\n";
		//	mStateFile->append(start_str);
		//	mStateFile->closeFile();
			
			bitmapTorturer();
		
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
		//	mStateFile->append("Round start at:");
		//	mStateFile->closeFile();
		
			bitmapTorturer();
			
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
	return true;
}


bool
AosBitmapTester::bitmapTorturer()
{
	// This function uses two arrays to keep track of bits.
	// It randomly generates bits, remove bits, and other
	// bitmap related operations.
	if(!(mBitmap1->getNumBits()))
	{
		OmnScreen << "===============================" << endl;
		OmnScreen << "==========bitmap empty=========" << endl;
		OmnScreen << "===============================" << endl;
	}

	OmnString round_str = "\nRound : ";
	round_str << mRound++;
	
	OmnScreen << round_str << endl;
	
	mStateFile->append(round_str);
	switch (rand() % 30)
	{
		case 0: 
			//				OmnScreen << "To set bits1 " << endl;
			//				if(rand()%20 == 0)
			//				break;

		case 1:
			OmnScreen << "To set bits bitmap1 and bitmap2" << endl;
			mStateFile->append(" setBits\n");
			setBits(mBitmap1, mArray1); 
			setBits(mBitmap2, mArray2);
			break;
		case 2:
			OmnScreen << "To test bits" << endl;
			mStateFile->append(" testBits\n");
			testBits();
			break;
		case 3:
			OmnScreen << "To test And" << endl;
			mStateFile->append(" testAND\n");
			testAND();
			break;
		case 4:
			OmnScreen << "To test Or" << endl;
			mStateFile->append(" testOR\n");
			testOR();
			break;
		case 5:
			OmnScreen << "To test Save" << endl;
			mStateFile->append(" testSave\n");
			testSave();
			testSaveNew();
			break;	
		case 6:
			OmnScreen << "To test Empty" << endl;
			mStateFile->append(" testIsEmpty\n");
			testIsEmpty();      
			break;
		case 7:
			OmnScreen << "To test Clear" << endl;
			mStateFile->append(" testClear\n");
			if(rand()%20==0)
			{
				testClear();                                                                      
			}
			break;
		case 8:
			OmnScreen << "To test Nextdocid and Prevdocid" << endl;
			mStateFile->append(" testNextandPreDocid\n");
			testNextandPreDocid(mArray1, mBitmapSize, mBitmap1);
			break;
		case 9:
			OmnScreen << "To test Check and Append" << endl;
			mStateFile->append(" testCheckandAppend\n");
			testCheckandAppend();                                                                      
			break;
		case 10:
			OmnScreen << "To test count And A not B" << endl;
			mStateFile->append(" testcountAndAnotB\n");
			testcountAndAnotB();
			break;
		case 11:
			OmnScreen << "To test nextDocid" << endl;
			mStateFile->append(" testnextDocid\n");
			testnextDocid(mArray1,mBitmap1);                                       
			break;
		case 12:
			OmnScreen << "To test include Bitmap" << endl;
			mStateFile->append(" testincludeBitmap\n");
			testincludeBitmap(); 
			break;
		case 13:
			OmnScreen << "To test append Bitmap" << endl;
			mStateFile->append(" testappendBitmap\n");
			testappendBitmap();                                                                      
			break;
		case 14:
			OmnScreen << "To test remove Bits" << endl;
			mStateFile->append(" testremoveBits\n");
			testremoveBits();                                                                   
			break;
		case 15:
			OmnScreen << "To test exchange Content" << endl;
			mStateFile->append(" testexchangeContent\n");
			testexchangeContent();                                                                       
			break;	
		case 16:
			OmnScreen << "To test get Number Bits" << endl;
			mStateFile->append(" testgetNumBits\n");
			testgetNumBits();
			break;
		case 17:
			OmnScreen<<"To test reset"<<endl;
			mStateFile->append(" testreset\n");
			if(rand()%10==0)
			{
				testreset();
			}
			break;
		case 18:
			OmnScreen<<"to test splitBitmap"<<endl;
			mStateFile->append(" testSplitBitmap\n");
			if(rand()%5 == 0)
				testSplitBitmap();
			break;
		case 19:
			OmnScreen<<"to test splitBitmapByValues"<<endl;
			mStateFile->append(" testSplitBitmapByValues\n");
			testSplitBitmapByValues();
			break;
		case 20:
			OmnScreen<<"to test cloneFrom"<<endl;
			mStateFile->append(" testCloneFrom\n");
			testCloneFrom();
			break;
		case 21:
			OmnScreen<<"to test AppendDocids2"<<endl;
			mStateFile->append(" testAppendDocids2\n");
			testAppendDocids2(mBitmap1, mArray1);
			testAppendDocids2(mBitmap2, mArray2);
			testnextDocid(mArray1,mBitmap1);                                       
			testnextDocid(mArray2,mBitmap2);                                       
			break;
		case 22:
			OmnScreen<<"to test firstDoc"<<endl;
			mStateFile->append(" testFirstDoc\n");
			testFirstDoc();
			break;
		case 23:
			OmnScreen<<"to test lastDoc"<<endl;
			mStateFile->append(" testLastDoc\n");
			testLastDoc();
			break;
		case 24:
			OmnScreen<<"to test getCountDocids"<<endl;
			mStateFile->append(" testGetCountDocids\n");
			testGetCountDocids();
			break;

		default:
			OmnScreen<<"to test getPage(moveTo)"<<endl;
			mStateFile->append(" testGetPage\n");
			//				testGetPage();
			break;
	}
	return true;
}
/*

   bool
   AosBitmapTester::setBits(
   const AosBitmapObjPtr &bitmap, 
   char *array)
   {
// 1. Determine the number of changes.
if (rand() % 2 == 1)
{
u64 idx = rand() % mBitmapSize;	
bitmap->appendDocid(idx);	
array[idx] = 1;
mNumAppends++;
return true;
}

int nn = OmnRandom::intByRange(
0, 1, 10, 
2, 10, 50, 
11, 100, 100);
if (nn == 0) return true;

u64 *idxs = new u64[nn];
for (int i = 0; i < nn; i++)
{
idxs[i] = rand() % mBitmapSize;
}
bitmap->appendDocids(idxs, nn);
for (int i = 0; i < nn; i++)
{
int idx = idxs[i];
array[idx] = 1;
}
mNumAppends++;
delete []idxs;
return true;
}  */
///
bool
AosBitmapTester::setBits( 
		const AosBitmapObjPtr &bitmap, 
		char *array)
{
	// 1. Determine the number of changes.
	int nn = OmnRandom::intByRange(
			1, 2, 10, 
			3, 10, 50, 
			11, 100, 100);   
	int type;
	u64 bitmap_bit_pos = 0; //起始位置
	while (nn--)
	{
		type = rand()%9+1;
		switch(type)
		{
			case 1:
				// 0 bit
				break;
			case 2:
				// 1 bit
				{  //一个bitmap长为16384为，把其中的一位置1
					u64 idx = ((u64)(rand())<<32 | (u64)(rand()))%16384 + bitmap_bit_pos;//rand()出来的是一个32位数，而U64为64位
					bitmap->appendDocid(idx);  //所以左移32位后低32位就为0了，再跟一个32位做活运算就能填满低32位了。最后再%16384
					array[idx] = 1; //就能保证不会超过一个bitmap的长度。第301行：就能把bitmap中的idx个位置置为1
				}
				break;
			case 3://随机选中一个U64，随机地把其中的num_bits(随机)位置1
				// 1 u64
				{
					u64 num_bits=rand()%61+2; //一个U64中要置1的个数，排除0，1，64的情况

					bool* flag = new bool[64];//与被选中的U64对应
					memset(flag,0,sizeof(bool)*64);
					u64 idx_start = bitmap_bit_pos + rand()%256 * 64;//rand()%256从256个U64中选出一个，比如选中10，则10*64表示
					u64 idx = 0;  //在整个bitmap中的第640个位置（整个bitmap为16384），bitmap_bit_pos开始时为0，所以idx_start就是
					for(int i=0; i<num_bits; ) //在bitmap中的第640个位置开始
					{
						idx = rand()%64 + idx_start; //rand()%64用于在被选中的U64中随机抽取被置1的位置，加上idx_start就表示其在
						if(flag[idx-idx_start] != 1)  //整个bitmap中的位置。flag与被选中的U64对应
						{
							bitmap->appendDocid(idx); //将给bitmap的第idx个位置置1
							array[idx] = 1;  //array与bitmap对应，flag与被选中的U64对应
							flag[idx-idx_start] = 1;
							i++;
						}
					}

					flag = 0;
					delete[] flag;
				}
				break;
			case 4: //被选中的U64全部置0，其他的U64每个U64中都随机抽取一些位数置1
				// n u64(blank node > 52)
				{
					if(mBitmapSize/16384==0)
						break;

					u64 blank_node_num = rand()%203 + 53;//在53到256个U64中随机抽取blank_node_num个U64做测试（放0）

					bool* blank_node_pos = new bool[256];//与256个U64对应，用于记录该U64是否被选中，被选中blank_node_pos就置1
					memset(blank_node_pos,0,sizeof(bool)*blank_node_num);
					int i=0;
					int pos = 0;
					while(i<blank_node_num) //遍历256个U64
					{
						pos = rand()%256; //在256个位置中随机取一个，比如取到第10个，这第10个U64就全为0
						if(blank_node_pos[pos]!=1) //blank_node_pos[10]!=1表示第10个U64还未被选中
						{ 
							blank_node_pos[pos]=1; //选中第10个U64
							i++;
						}
					}

					u64 idx_start = bitmap_bit_pos; //初始位置为0
					for(int i=0; i<256; i++) //从第0个U64开始查找，如果遇到标识为ieblank_node_pos为1则说明该U64被选中，idx_start
					{   //就加上该U64的长度64，跳过该U64，不放1，让他全为0
						if(blank_node_pos[i]==1)//等于1表示该U64被选中，该U64就不放1
						{
							idx_start+=64;
							continue; //结束了本次循环，本次循环中的361行到380行都不执行，继续下一次循环
						}

						u64 num_bits=rand()%61+2; //没被选中的U64中被置1的位数，比如num_bits=7，则该U64中有7个位置要置1
						bool* flag = new bool[64]; //flag与被选中的U64对应
						memset(flag,0,sizeof(bool)*64);
						u64 idx = 0;
						for(int i=0; i<num_bits; )
						{
							idx = rand()%64 + idx_start;
							if(flag[idx-idx_start] != 1)
							{
								bitmap->appendDocid(idx);
								array[idx] = 1;
								flag[idx-idx_start]=1;
								i++;
							}
						}

						flag=0;
						delete[] flag;
						
						idx_start+=64;
					}
				}
				break;
			case 5://选中所有的U64，在每个U64中随机抽取一些位置置1
				// all u64
				{
					if(mBitmapSize/16384==0)
						break;
					u64 idx_start = bitmap_bit_pos;//idx_start初始值为0，即从第0个U64开始
					for(int i=0; i<256; i++)
					{
						u64 num_bits=rand()%61+2;


						bool* flag = new bool[64];
						memset(flag,0,sizeof(bool)*64);

						u64 idx = 0;
						for(int k=0; k<num_bits; )  //U64里随机选一些位置置1，总共置num_bits个
						{
							idx = rand()%64 + idx_start;
							if(flag[idx-idx_start] != 1)
							{
								bitmap->appendDocid(idx);
								array[idx] = 1;
								flag[idx-idx_start] = 1;
								k++;
							}
						}
						flag=0;
						delete[] flag;
						idx_start+=64;//跳到下一个U64
					}
				}
				break;
			case 6://从256-52个U64中随机选full_node_num个U64，被选中的U64全部填满1，没被选中的则随机抽取一些位置填1
				// n not full(full node >52)
				{
					if(mBitmapSize/16384==0)
						break;

					u64 full_node_num = rand()%203 + 53;//抽取要被选中的U64的个数

					bool* full_node_pos = new bool[256];
					memset(full_node_pos,0,sizeof(bool)*full_node_num);
					int i=0;
					int pos = 0;
					while(i<full_node_num) //随机抽取full_node_num个U64,并将其标志位full_node_pos置1
					{
						pos = rand()%256;
						if(full_node_pos[pos]!=1)
						{
							full_node_pos[pos]=1;
							i++;
						}
					}

					u64 idx_start = bitmap_bit_pos;//idx_start从0开始
					for(int i=0; i<256; i++)
					{
						if(full_node_pos[i]==1) //如果full_node_pos为1，说明该U64被选中
						{
							for(int j=0; j<64; j++)//将该U64全部填满1
							{
								bitmap->appendDocid(idx_start);
								array[idx_start++] = 1;
							}
						}
						else
						{
							u64 num_bits=rand()%61+2;
							bool* flag = new bool[64];
							memset(flag,0,sizeof(bool)*64);
							u64 idx = 0;
							for(int j=0; j<num_bits; )
							{
								idx = rand()%64 + idx_start;
								if(flag[idx-idx_start] != 1)
								{
									bitmap->appendDocid(idx);
									array[idx] = 1;
									flag[idx-idx_start] = 1;
									j++;
								}
							}
							idx_start+=64;
						}
					}
				}
				break;
			case 7://从256个U64中随机选出1个U64，在这个U64里面随机取一些位置放 1，其余没被选中的U64则全部填满 1
				// 1 not full
				{
					if(mBitmapSize/16384==0)
						break;
					u64 idx_start = bitmap_bit_pos;
					u64 not_full_pos = rand()%256;
					for(u64 i=0; i<256; i++)
					{
						if(i==not_full_pos)
						{
							u64 num_bits=rand()%61+2;
							bool* flag = new bool[64];
							memset(flag,0,sizeof(bool)*64);
							u64 idx = 0;
							for(int j=0; j<num_bits; )
							{
								idx = rand()%64 + idx_start;
								if(flag[idx-idx_start] != 1)//如果flag！=1说明bitmap此处不是1，于是就把它填上1
								{
									bitmap->appendDocid(idx);
									array[idx] = 1;
									flag[idx-idx_start] = 1; //已经填过1了，就把flag置为1
									j++;
								}
							}
							flag = 0;
							delete[] flag;
							idx_start+=64;//跳到下一个U64
							continue;
						}
						for(u64 j=0; j<64; j++)
						{
							bitmap->appendDocid(idx_start);
							array[idx_start++] = 1;
						}
					}
				}
				break;
			case 8: //在16384个bit里选中一个bit让它为0,其余全置为1
				// 1 bit is 0
				{
					if(mBitmapSize/16384==0) 
						break;
					u64 idx_start = bitmap_bit_pos;
					u64 not_full_pos = rand()%16384 + idx_start;//比如idx_start从0开始，not_full_pos选中1
					for(u64 i=0; i<256; i++) //i，j都=0时第一次循环，idx_start=0，不等于not_full_pos，语句不执行，执行526，527行
					{                        //bitmap的第0为置为1，array对应位置为1，idx_start自加后变为1.内层循环for开始第二次循环
						for(u64 j=0; j<64; j++) //j=1，此时idx_start==not_full_pos==1，执行if，idx_start自加变为2，continue跳出本次
						{                      //循环，bitmap对应的第1个位置就没被置1(即为0).继续循环idx_start都不会再等于not_full_pos
							if(idx_start == not_full_pos) //因此除了第1位外其余的16383位全为1
							{
								idx_start++;
								continue;
							}
							bitmap->appendDocid(idx_start);
							array[idx_start++] = 1;
						}
					}
				}
				break;
			case 9: //16384个位置全部置1
				// all full
				{
					if(mBitmapSize/16384==0)
						break;
					u64 idx_start = bitmap_bit_pos;
					for(u64 i=0; i<256; i++)
					{
						for(u64 j=0; j<64; j++)
						{
							bitmap->appendDocid(idx_start);
							array[idx_start++] = 1;
						}
					}
				}
				break;
		}
		bitmap_bit_pos+=16384;	
	}
	return true;

}

bool
AosBitmapTester::testAppendDocids2(const AosBitmapObjPtr &bitmap, char *array)
{

	int nn = OmnRandom::intByRange(
		   1, 2, 10,
		   3, 10, 50,
		   11, 100, 100);
	int type = 0;
	u64 bitmap_bit_pos = 0;
	u64 docid_bit_pos = 0;
	vector<u64> docids;
	//u64 idx = 0;

	u64 num = 0;

	type = rand()%9+1;
	if(mBitmapSize/16384)
	{
		bitmap_bit_pos = (rand()%(mBitmapSize/16384))*16384;
	}
	switch(type)
	{
		case 1:
			//0 docid
			break;
		case 2:
			// 1 docid
			//随机选一个位置里面放数，其余的不放
			{
				u64 idx = 0;
				idx = rand()%16384+bitmap_bit_pos;
				docids.push_back(idx);
				array[idx] = 1;
				num++;
			}
			break;
		case 3:
			// 1 u64
			{
				u64 num_bits=rand()%61+2; //一个U64中要置1的个数，排除0，1，64的情况
				bool* flag = new bool[64];//与被选中的U64对应
				memset(flag,0,sizeof(bool)*64);
				u64 idx_start = bitmap_bit_pos + rand()%256 * 64;//rand()%256从256个U64中选出一个，比如选中10，则10*64表示
				u64 idx = 0;  //在整个bitmap中的第640个位置（整个bitmap为16384），bitmap_bit_pos开始时为0，所以idx_start就是
				for(int i=0; i<num_bits; ) //在bitmap中的第640个位置开始
				{
					idx = rand()%64 + idx_start; //rand()%64用于在被选中的U64中随机抽取被置1的位置，加上idx_start就表示其在
					if(flag[idx-idx_start] != 1)  //整个bitmap中的位置。flag与被选中的U64对应
					{
						docids.push_back(idx);
						num++;
						//	bitmap->appendDocid(idx); //将给bitmap的第idx个位置置1
						array[idx] = 1;  //array与bitmap对应，flag与被选中的U64对应
						flag[idx-idx_start] = 1;
						i++;
					}
				}

				flag = 0;
				delete[] flag;
			}
			break;
		case 4:
			//n u64
			//被选中的u64全被置0，未被选中的随机抽取一些位置置1
			{
				if(mBitmapSize/16384==0)
					break;

				u64 blank_node_num = rand()%203 + 53;//在53到256个U64中随机抽取blank_node_num个U64做测试（放0）

				bool* blank_node_pos = new bool[256];//与256个U64对应，用于记录该U64是否被选中，被选中blank_node_pos就置1
				memset(blank_node_pos,0,sizeof(bool)*blank_node_num);
				int i=0;
				int pos = 0;
				while(i<blank_node_num) //遍历256个U64
				{
					pos = rand()%256; //在256个位置中随机取一个，比如取到第10个，这第10个U64就全为0
					if(blank_node_pos[pos]!=1) //blank_node_pos[10]!=1表示第10个U64还未被选中
					{ 
						blank_node_pos[pos]=1; //选中第10个U64
						i++;
					}
				}

				u64 idx_start = bitmap_bit_pos; //初始位置为0
				for(int i=0; i<256; i++) //从第0个U64开始查找，如果遇到标识为ieblank_node_pos为1则说明该U64被选中，idx_start
				{   //就加上该U64的长度64，跳过该U64，不放1，让他全为0
					if(blank_node_pos[i]==1)//等于1表示该U64被选中，该U64就不放1
					{
						idx_start+=64;
						continue; //结束了本次循环，本次循环中的361行到380行都不执行，继续下一次循环
					}

					u64 num_bits=rand()%61+2; //没被选中的U64中被置1的位数，比如num_bits=7，则该U64中有7个位置要置1
					bool* flag = new bool[64]; //flag与被选中的U64对应
					memset(flag,0,sizeof(bool)*64);
					u64 idx = 0;
					for(int i=0; i<num_bits; )
					{
						idx = rand()%64 + idx_start;
						if(flag[idx-idx_start] != 1)
						{
							//				bitmap->appendDocid(idx);
							docids.push_back(idx);
							num++;
							array[idx] = 1;
							flag[idx-idx_start]=1;
							i++;
						}
					}

					flag=0;
					delete[] flag;

					idx_start+=64;
				}
			}
			break;
		case 5:
			{
				if(mBitmapSize/16384==0)
					break;
				u64 idx_start = bitmap_bit_pos;//idx_start初始值为0，即从第0个U64开始
				for(int i=0; i<256; i++)
				{
					u64 num_bits=rand()%61+2;


					bool* flag = new bool[64];
					memset(flag,0,sizeof(bool)*64);

					u64 idx = 0;
					for(int k=0; k<num_bits; )  //U64里随机选一些位置置1，总共置num_bits个
					{
						idx = rand()%64 + idx_start;
						if(flag[idx-idx_start] != 1)
						{
							//				bitmap->appendDocid(idx);
							docids.push_back(idx);
							num++;
							array[idx] = 1;
							flag[idx-idx_start] = 1;
							k++;
						}
					}
					flag=0;
					delete[] flag;
					idx_start+=64;//跳到下一个U64
				}
			}
			break;
		case 6:
			{
				if(mBitmapSize/16384==0)
					break;

				u64 full_node_num = rand()%203 + 53;//抽取要被选中的U64的个数

				bool* full_node_pos = new bool[256];
				memset(full_node_pos,0,sizeof(bool)*full_node_num);
				int i=0;
				int pos = 0;
				while(i<full_node_num) //随机抽取full_node_num个U64,并将其标志位full_node_pos置1
				{
					pos = rand()%256;
					if(full_node_pos[pos]!=1)
					{
						full_node_pos[pos]=1;
						i++;
					}
				}

				u64 idx_start = bitmap_bit_pos;//idx_start从0开始
				for(int i=0; i<256; i++)
				{
					if(full_node_pos[i]==1) //如果full_node_pos为1，说明该U64被选中
					{
						for(int j=0; j<64; j++)//将该U64全部填满1
						{
							bitmap->appendDocid(idx_start);
							array[idx_start++] = 1;
						}
					}
					else
					{
						u64 num_bits=rand()%61+2;
						bool* flag = new bool[64];
						memset(flag,0,sizeof(bool)*64);
						u64 idx = 0;
						for(int j=0; j<num_bits; )
						{
							idx = rand()%64 + idx_start;
							if(flag[idx-idx_start] != 1)
							{
								//	bitmap->appendDocid(idx);
								docids.push_back(idx);
								num++;
								array[idx] = 1;
								flag[idx-idx_start] = 1;
								j++;
							}
						}
						idx_start+=64;
					}
				}
			}
			break;
		case 7:
			{
				if(mBitmapSize/16384==0)
					break;
				u64 idx_start = bitmap_bit_pos;
				u64 not_full_pos = rand()%256;
				for(u64 i=0; i<256; i++)
				{
					if(i==not_full_pos)
					{
						u64 num_bits=rand()%61+2;
						bool* flag = new bool[64];
						memset(flag,0,sizeof(bool)*64);
						u64 idx = 0;
						for(int j=0; j<num_bits; )
						{
							idx = rand()%64 + idx_start;
							if(flag[idx-idx_start] != 1)//如果flag！=1说明bitmap此处不是1，于是就把它填上1
							{
								//bitmap->appendDocid(idx);
								docids.push_back(idx);
								num++;
								array[idx] = 1;
								flag[idx-idx_start] = 1; //已经填过1了，就把flag置为1
								j++;
							}
						}
						flag = 0;
						delete[] flag;
						idx_start+=64;//跳到下一个U64
						continue;
					}
					for(u64 j=0; j<64; j++)
					{
						docids.push_back(idx_start);
						num++;
						//	bitmap->appendDocid(idx_start);
						array[idx_start++] = 1;
					}
				}
			}
			break;
		case 8:
			//1 bit is 0
			//有一个字节是0其余全是1
			{
				if(mBitmapSize/16384==0) 
					break;
				u64 idx_start = bitmap_bit_pos;
				u64 not_full_pos = rand()%16384 + idx_start;//比如idx_start从0开始，not_full_pos选中1
				for(u64 i=0; i<256; i++) //i，j都=0时第一次循环，idx_start=0，不等于not_full_pos，语句不执行，执行526，527行
				{                        //bitmap的第0为置为1，array对应位置为1，idx_start自加后变为1.内层循环for开始第二次循环
					for(u64 j=0; j<64; j++) //j=1，此时idx_start==not_full_pos==1，执行if，idx_start自加变为2，continue跳出本次
					{                      //循环，bitmap对应的第1个位置就没被置1(即为0).继续循环idx_start都不会再等于not_full_pos
						if(idx_start == not_full_pos) //因此除了第1位外其余的16383位全为1
						{
							idx_start++;
							continue;
						}
						//	bitmap->appendDocid(idx_start);
						docids.push_back(idx_start);
						num++;
						array[idx_start++] = 1;
					}
				}
			}
			break;
		case 9:
			//all full
			//所有的位置全部置1
			{
				if(mBitmapSize/16384==0)
					break;
				u64 idx_start = bitmap_bit_pos;
				for(u64 i=0; i<256; i++)
				{
					for(u64 j=0; j<64; j++)
					{
						//	bitmap->appendDocid(idx_start);
						docids.push_back(idx_start);
						num++;
						array[idx_start++] = 1;
					}
				}
			}
			break;
	}
	bitmap->appendDocids(&docids[0],num);
	return true;
}

/*
   bool
   AosBitmapTester::testBits()
   {
   int nn = OmnRandom::intByRange(
   0, 1, 10, 
   2, 10, 50, 
   11, 100, 100);

   while (nn--)
   {
   aos_assert_r(mBitmapSize > 0, false);
   u64 idx = rand() % (mBitmapSize -1)+1; 
   bool rslt = mBitmap1->checkDoc(idx);
   if (rslt) 
   {
   aos_assert_r(mArray1[idx], false);
   }
   else 
   {
   aos_assert_r(!mArray1[idx], false);
   }

   rslt = mBitmap2->checkDoc(idx);
   if (rslt) 
   {
   aos_assert_r(mArray2[idx], false);
   }
   else 
   {
   aos_assert_r(!mArray2[idx], false);
   }
   }

	return true;
}
*/

bool
AosBitmapTester::testAppendDocids()
{
    // prepare parameters
    const u64 count = 200000;
    u64 tmpDocidCount = 0;
    u64 location;
    vector<u64> docids;
    u64 idx = 0;
    u64 filter = 0;
    
    while (idx < count)
    {
        if(percent(70))//百分之70的可能
        {
            //70% 1 docid
            // create one docid
            docids.push_back(randomU64());//随便找个随机数放到docid里面
            idx++;
        }
        else
        {
            //30% 2-10 docids
            // create docids
            tmpDocidCount = randomBetweenAAndB(2ULL, 10ULL);//产生2到10的随机数
            filter = randomU64() & 0xffffffffffffc000;//
            for(int k = 0;k < tmpDocidCount;k++)
            {
                //1. create a docid and append it
                docids.push_back(filter + random()%16384);
                //2. exchange location
                if (idx != 0)//?
                {
                    location = random()%idx; //随机一个0~idx的数
                    changeValue(docids[location], docids[idx]); //1交换两个数
                }
                idx++;
            }
        }
    }
    docids.resize(count); //改变docid的大小为count
    
    AosBitmapPtr bitmap;
    // apppend data to bitmap
    OmnScreen << "start appending docids." << endl;
   // for(int round = 0;round < 100;round++)
    //{
        bitmap = new AosBitmap();
        bitmap->appendDocids(&(docids[0]),count);
    //}
    OmnScreen << "finish appending docids." << endl;
    
    // check bitmap
    for(int round = 0;round < count;round++)
    {
        aos_assert_r(bitmap->checkDoc(docids[round]),false);
    }
    
    
    OmnScreen << "start getting sections." << endl;
    for(int i = 0;i < 10;i++)
    {
        AosBitmap::map_t bitmaps;
        bitmap->getSections(mRundata,bitmaps);
    }
    OmnScreen << "finish getting sections." << endl;
    
    return true;
}


bool
AosBitmapTester::testBits()
{
	int nn = OmnRandom::intByRange(
			0, 1, 10, 
			2, 10, 50, 
			11, 100, 100);
	OmnScreen<<"nn  		"<<nn<<endl;
	while (nn--)
	{
		aos_assert_r(mBitmapSize > 0, false);
		/*u64 cc = rand() % mBitmapSize;
		u64 idx=0;
		if(cc<= 20000)
		{
			idx=0;
		}
		else
		{
			idx=cc;
		}
		OmnScreen<<"cc  		"<<cc<<"     idx  		"<<idx<<endl;
		*/
		u64 idx = rand() % mBitmapSize; //idx的范围是0~16384，即在bitmap中的位置
		if(idx!=0)//这个if语句的作用是增加idx等于0的概率，提升到十分之一的概率
		{
			if(rand()%10==0)
			{
				idx=0;
			}
		}
		//OmnScreen<<"     idx  		"<<idx<<endl;
		bool rslt = mBitmap1->checkDoc(idx);
		if (rslt) 
		{
			aos_assert_r(mArray1[idx], false);
		}
		else 
		{
			aos_assert_r(!mArray1[idx], false);
		}

		rslt = mBitmap2->checkDoc(idx);
		if (rslt) 
		{
			aos_assert_r(mArray2[idx], false);
		}
		else 
		{
			aos_assert_r(!mArray2[idx], false);
		}
	}
	return true;
}

bool
AosBitmapTester::testAND()
{

	//	static int lsTries = 0;

	//  OmnScreen << "Test AND: " << lsTries++ << endl;

	if(rand()%2==0) //二分之一的几率
	{
		mBitmap1->pack();//将bitmap进行压缩
	}
	else
	{
		mBitmap1->expand();//将bitmap进行展开
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}

	AosBitmapObjPtr bmp = AosBitmap::countAnd(mBitmap1, mBitmap2);
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		if (bmp->checkDoc(docid))
		{
			aos_assert_r(mArray1[docid] && mArray2[docid], false);
		}
		else
		{
			aos_assert_r(!mArray1[docid] || !mArray2[docid], false);
		}
	}

	// OmnScreen << "1" << endl;
	mBitmap1->andBitmap(mBitmap2);
	//	OmnScreen << "1" << endl;
	//AosBitmapObjPtr curbmp;
	//curbmp=mBitmap1->clone(mRundata);
	//curbmp->andBitmap(mBitmap2);
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		if (mBitmap1->checkDoc(docid))
		{				
			aos_assert_r(mArray1[docid] && mArray2[docid], false);
		}
		else
		{
			aos_assert_r(!mArray1[docid] || !mArray2[docid], false);
			mArray1[docid] = false;
		}
	}
	//	OmnScreen << "1" << endl;
	return true;

}


bool
AosBitmapTester::testOR()
{ 

	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}


	AosBitmapObjPtr bmp = AosBitmap::countOr(mBitmap1, mBitmap2);
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		if( bmp->checkDoc(docid))
		{
			aos_assert_r(mArray1[docid] || mArray2[docid], false);
		}
		else
		{
			aos_assert_r(!mArray1[docid] && !mArray2[docid], false);
		}
	}
	AosBitmapObjPtr bitmap1 = mBitmap1->clone(mRundata);
	if(rand()%2==0)
	{
		bitmap1->pack();
	}
	else
	{
		bitmap1->expand();
	}
	bitmap1->orBitmap(mBitmap2);
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		if (bitmap1->checkDoc(docid))
		{
			aos_assert_r(mArray1[docid] || mArray2[docid], false);
		}
		else
		{
			aos_assert_r(!mArray1[docid] && !mArray2[docid], false);
		}
	}
	return true;

}

bool
AosBitmapTester::testSave()
{
	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}

	AosBuffPtr buff = new AosBuff(AosMemoryCheckerArgsBegin);
	mBitmap1->saveToBuff(buff); //将mBitmap1中的东西存到buff中

	AosBitmapPtr bmp = new AosBitmap();
	bmp->loadFromBuff(buff); //将buff中的东西放大到bmp中

	bool rslt = mBitmap1->compareBitmap(bmp);
	aos_assert_r(rslt, false);
	rslt = AosBitmap::compare(mBitmap1,bmp);
	aos_assert_r(rslt, false);
	return true;                                                                                                                 
}


bool
AosBitmapTester::testSaveNew()
{
/*
	AosBuffPtr buff = new AosBuff(AosMemoryCheckerArgsBegin);
	vector<AosBitmapPtr> bitmaps;
	int nn = 1;
	while(nn--)
	{
		char * array = new char[mBitmapSize];
		AosBitmapPtr bmp1 = OmnNew AosBitmap();
		testAppendDocids2(bmp1, array);
		if(rand()%2==0)
		{
			bmp1->pack();
		}
		else
		{
			bmp1->expand();
		}
		bitmaps.push_back(bmp1);
		bmp1->saveToBuff(buff, false);
		delete[] array;
	}
	
	nn=1;
	while(nn--)
	{
		AosBitmapPtr bmp2 = OmnNew AosBitmap();
		bmp2->loadFromBuff(buff,false);
		bool rslt = bitmaps[0-nn]->compareBitmap(bmp2);
		aos_assert_r(rslt, false);
	}
	return true;
*/
}


/*
bool
AosBitmapTester::testCheckandAppend()
{
	AosBitmapObjPtr cur_bitmap = OmnNew AosBitmap();
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		u64 vv = random();
		cur_bitmap->appendDocid(vv);
		bool rslt= cur_bitmap->checkDoc(vv);
		aos_assert_r(rslt, false);
	}
	return true;
}
*/

bool
AosBitmapTester::testCheckandAppend()
{
	AosBitmapObjPtr cur_bitmap = OmnNew AosBitmap();
	u64 vv;
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		
		if(rand()%100<10)
		{
			vv=0;
		}
		else
		{
			vv=rand() % (mBitmapSize-1)+1;
		}
		
		cur_bitmap->appendDocid(vv);
		bool rslt= cur_bitmap->checkDoc(vv);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosBitmapTester::testIsEmpty()
{
	vector<int> docids;
	AosBitmapObjPtr bmp = mBitmap1->clone(mRundata);

	if(rand()%2==0)
	{
		bmp->pack();
	}
	else
	{
		bmp->expand();
	}
	
	//u64 tt1 = OmnGetTimestamp();
	for (u32 i=0; i<mBitmapSize; i++)
	{
		if (mArray1[i]) docids.push_back(i);

	}
	//u64 tt2 = OmnGetTimestamp();
	//OmnScreen << "Time1: " << tt2 - tt1 << ":" << mBitmapSize << endl;

	//char *flags = new char[mBitmapSize];
	//memcpy(flags, mArray1, mBitmapSize);
	//test
	//tt1 = OmnGetTimestamp();
	//u64 remove_time = 0;
	//u64 erase_time = 0;

	u32 num_docids = docids.size();
	while (num_docids != 0)
	{
		//u64 ttt1 = OmnGetTimestamp();
		int idx = rand() % num_docids;
		// int idx=docids.size()-1;
		bmp->removeDocid(docids[idx]);
		//u64 ttt2 = OmnGetTimestamp();
		//remove_time += ttt2 - ttt1;

		//aos_assert_r(flags[docids[idx]], false);
		//flags[docids[idx]] = 0;
		docids[idx] = docids[num_docids-1];
		//u64 ttt3 = OmnGetTimestamp();
		//erase_time += ttt3 - ttt2;
		num_docids--;
	}
	//OmnScreen << "remove time is: " << remove_time << ":" << mBitmapSize << endl;
	//OmnScreen << "erase  time is: " << erase_time  << ":" << mBitmapSize << endl;
	//tt2 = OmnGetTimestamp();
	//OmnScreen << "Time2: " << tt2 - tt1 << ":" << mBitmapSize << endl;
	aos_assert_r(bmp->isEmpty(), false);
	// 'flags' should be empty, too.
//	for (u32 i=0; i < mBitmapSize; i++)
//	{
//		aos_assert_r(flags[i] == 0, false);
//	}
    u64 Number =bmp ->   getNumBits();
	aos_assert_r(Number == 0,false); 

	return true;
}


bool
AosBitmapTester::testClear()
{
	/*	vector<int> docids;
		for (int i=0; i<mBitmapSize; i++)
		{
		if (mArray1[i]) docids.push_back(i);
		}*/
	AosBitmapObjPtr bmp = mBitmap1->clone(mRundata);
	/*  char *flags = new char[mBitmapSize];
		memcpy(flags, mArray1, mBitmapSize);
		while (docids.size() != 0)
		{
		int idx = rand() % docids.size();
		bmp->removeDocid(docids[idx]);
		aos_assert_r(flags[docids[idx]], false);
		flags[docids[idx]] = 0;
		docids[idx] = docids[docids.size()-1];
		docids.erase(docids.begin()+idx);                                                                           
		} */ 
	if(rand()%2==0)
	{
		bmp->pack();
	}
	else
	{
		bmp->expand();
	}
	bmp->clear();	
	aos_assert_r(bmp->isEmpty(), false);
	return true;                                                                                          
} 

/*
bool
AosBitmapTester::testNextandPreDocid(
		char *array, 
		const int array_size,
		const AosBitmapObjPtr &bitmap)
{
	// array[0, 0, 0, 1, 0, 0, 1]
	bitmap->reset(); 
	u64 idx = 0;
	u64 docid;
	bool rslt = false;
	while (1)
	{
		rslt = bitmap->nextDocid(docid);
		if (!rslt)
		{
			while (idx < (u64)array_size)
			{	
				aos_assert_r(array[idx] == 0, false);
				idx++;
			}
			break;
		}

		while (idx < docid)
		{
			aos_assert_r(idx < (u64)array_size, false);
			aos_assert_r(array[idx] == 0, false);
			idx++;
		}

		// There are no more docids. 
		aos_assert_r(array[docid], false);
		idx++;
	}
	/*
	   idx = array_size-1;
	   while (1)
	   {
	   u64 docid = bitmap->prevDocid();
	   if (docid == 0)
	   {
	   while (idx >= 0)
	   {
	   aos_assert_r(array[idx] == 0, false);
	   idx--;
	   }
	   break;
	   }

	   while (idx > docid)
	   {
	   aos_assert_r(idx < array_size, false);
	   aos_assert_r(array[idx] == 0, false);
	   idx--;
	   }

	// There are no more docids. 
	aos_assert_r(array[docid], false);
	idx--;
	}
	*/
//	return true;
//} 


bool
AosBitmapTester::testNextandPreDocid(
		char *array, 
		const int array_size,
		const AosBitmapObjPtr &bitmap)
{
	// array[0, 0, 0, 1, 0, 0, 1]
	if(rand()%2==0)
	{
		bitmap->pack();
	}
	else
	{
		bitmap->expand();
	}
	bitmap->reset(); 
	i64 idx = 0;
	while (1)
	{
		u64 docid = 0;
		bool rslt = false;
		rslt = bitmap->nextDocid(docid);
										
		
		if (!rslt)
		{
			while (idx < (u64)array_size)
			{	
				aos_assert_r(array[idx] == 0, false);
				idx++;
			}
			break;
		}

		while ( idx < docid)
		{
			aos_assert_r(idx < (u64)array_size, false);
			aos_assert_r(array[idx] == 0, false);
			idx++;
		}

		// There are no more docids. 
		aos_assert_r(array[docid], false);
		idx++;
	}
	
	
	
	idx = array_size-1;

	bitmap->reset(); 

	while (1)
	{
		u64 docid = 0;
		bool rslt = false;
		rslt = bitmap->prevDocid(docid);
		if(!rslt)
		{
			while (idx >= 0)
			{
				aos_assert_r(array[idx] == 0, false);
				if(idx == 0)
				{
					break;
				}
				idx--;
			}
			break;
		}

	   while (idx > docid)
	   {
	   aos_assert_r(idx < array_size, false);
	   aos_assert_r(array[idx] == 0, false);
	   idx--;
	   }

	// There are no more docids. 
	aos_assert_r(array[docid], false);
	idx--;
	}
	return true;
}

	bool
AosBitmapTester::testcountAndAnotB()
{ 
	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}
	
	AosBitmapObjPtr bmp = AosBitmap::countAndAnotB(mBitmap1, mBitmap2);
	if(rand()%2==0)
	{
		bmp->pack();
	}
	else
	{
		bmp->expand();
	}
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		if( bmp->checkDoc(docid))
		{
			aos_assert_r((mArray1[docid] && !mArray2[docid]), false);
		}
		else
		{
			aos_assert_r(!(mArray1[docid] && !mArray2[docid]), false);
		}
	}
	return true;
}


bool
AosBitmapTester::testnextDocid(
		char *array, 
		const AosBitmapObjPtr &bitmap)
{
	if(rand()%2==0)
	{
    	bitmap->pack();
	}
	else
	{
		bitmap->expand();
	}
	bitmap->reset();
	// new a list, copy list
	u64 docid = 0;
    char *flags = new char[mBitmapSize];
	memset(flags,0,mBitmapSize*sizeof(char)); 
    memcpy(flags, array, mBitmapSize);
	bool rslt = true;  
	//	count = bitmap -> getDocidCount(); 
	while(rslt)
	{
		rslt = bitmap->nextDocid(docid);
		if(rslt)
		{
	    	aos_assert_r( flags[docid]  > 0 ,false) ;
	    	aos_assert_r( flags[docid]< (i64)mBitmapSize ,false);	
			aos_assert_r(flags[docid] == 1,false);
			flags[docid] = 0;
	        docid++;
		}		
	}
	for(u64 i = 0;i < mBitmapSize;i++)
	{
		if(flags[i])
		{
			OmnAlarm << "something wrong " <<enderr;
			// something wrong
			for(int  aaaaaa = 0;aaaaaa < 10000; aaaaaa++)
			{
				bitmap->reset();
				rslt = true;
				while(rslt)
				{
					bitmap->nextDocid(docid);
					OmnScreen << docid << endl;
				}
			}
		}

		aos_assert_r(flags[i] == 0,false);
	}
	delete []flags;
	return true;
}	
/*
  i64 idx = 0;
  491     while (1)                                                                                                                    
  492     {
  493         u64 docid = bitmap->nextDocid();
  494         if (docid == 0)
  495         {
  496             while (idx < array_size)
  497             {   
  498                 aos_assert_r(array[idx] == 0, false);
  499                 idx++;
  500             }
  501             break;
  502         }
  503 
  504         while (idx < docid)
  505         {
  506             aos_assert_r(idx < array_size, false);
  507             aos_assert_r(array[idx] == 0, false);
  508             idx++;
  509         }
 
 */

	bool
AosBitmapTester::testincludeBitmap()
{    
	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}
	
	AosBitmapObjPtr bmp = AosBitmap::countAnd(mBitmap1, mBitmap2);

	if(rand()%2==0)
	{
		bmp->pack();
	}
	else
	{
		bmp->expand();
	}

	bool rslt = mBitmap1->includeBitmap(bmp);
	aos_assert_r(rslt,false);
	rslt = mBitmap2->includeBitmap(bmp);;
	aos_assert_r(rslt,false);
	return true;
}

bool
AosBitmapTester::testremoveBits()
{   

	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}


	AosBitmapObjPtr bmp = AosBitmap::countOr(mBitmap1, mBitmap2);

	AosBitmapObjPtr bmp2 = AosBitmap::countAnd(mBitmap1, mBitmap2);
	bool rslt = bmp-> removeBits(mBitmap1);
	aos_assert_r(rslt,false);
	AosBitmapObjPtr bmp3 = mBitmap2->clone(mRundata);
	rslt = bmp3->removeBits(bmp2);
	aos_assert_r(rslt,false);
	rslt = AosBitmap::compare(bmp3,bmp);
	aos_assert_r(rslt, false);
	return true;

}


	bool
AosBitmapTester::testappendBitmap()
{ 
/*
	AosBitmapObjPtr bitmap1 = mBitmap1->clone(mRundata);
	bool rslt = bitmap1->appendBitmap(mBitmap2);
   // bool rslt=true;
	aos_assert_r(rslt, false);
	for (u64 docid = 0; docid < mBitmapSize; docid++)
	{
		if (bitmap1->checkDoc(docid))
		{
			aos_assert_r(mArray1[docid] || mArray2[docid], false);
		}
		else
		{
			aos_assert_r(!mArray1[docid] && !mArray2[docid], false);
		}
	}
	return true;
*/
}

    
bool
AosBitmapTester::testexchangeContent()
{
 
	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}
    AosBitmapObjPtr bitmap1 = mBitmap1->clone(mRundata);
	AosBitmapObjPtr bitmap2 = mBitmap2->clone(mRundata);
	AosBitmap::exchangeContent(bitmap1,bitmap2);
    bool rslt = mBitmap1->compareBitmap(bitmap2);
    aos_assert_r(rslt, false);
    rslt = mBitmap2->compareBitmap(bitmap1);
    aos_assert_r(rslt, false);
	return true;

}

bool
AosBitmapTester::testgetNumBits()
{
	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}
	u64 i=0;
    u64 BitNumber1 =0, BitNumber2 =0;
	for(i=0;i<mBitmapSize;i++)
	{ 
		if(mArray1[i]) BitNumber1++;
		if(mArray2[i]) BitNumber2++;
    }
    u64 Number1 = mBitmap1 -> getNumBits();
	u64 Number2 = mBitmap2 -> getNumBits(); 
	aos_assert_r(BitNumber1 == Number1,false);
    aos_assert_r(BitNumber2 == Number2,false);
	
	return true;
}

/*
    virtual bool getDocids(vector<u64> &docids);
 
      virtual u64  getDocidCount();*/
bool
AosBitmapTester::testreset()
{
	mBitmapSize = OmnRandom::intByRange(
			2, 16384, 10,
			16385, 10*16384, 20,
			10*16384+1, 10*100*16384, 50);
	mBitmapSize = 1638400;
/*
	mBitmapSize = OmnRandom::intByRange(
			2, 1000, 10,
			1001, 10000, 20,
			10001, 10*100*1000, 50);
*/

	aos_assert_r(mBitmapSize > 0, false);
	delete []mArray1;
	delete []mArray2;
	mBitmap1->clear();
	mBitmap2->clear();
	mArray1 = new char[mBitmapSize];
	mArray2 = new char[mBitmapSize];
	mBitmap1 = new AosBitmap();
	mBitmap2 = new AosBitmap();
	memset(mArray1, 0, mBitmapSize);
	memset(mArray2, 0, mBitmapSize);
	return true;
}

/*
bool 
AosBitmapTester::testSplitBitmapByValues()
{
	if(mBitmap1->getNumBits() == 0)
	{
		return true;
	}

	
	vector<u64> values;
	vector<AosBitmapObjPtr> bitmaps;
	AosBitmapObjPtr new_bitmap1=mBitmap1->clone(mRundata);
	//保证vector中升序排列，没有重复值
	u64 idx=0;
	while(idx < mBitmapSize)
	{
		int n=rand()%mBitmapSize;
		if(n==0)
		{
			continue;
		}
		idx=n+idx;
		if(idx<mBitmapSize)
		{
			values.push_back(idx);
		}
		else 
		{
			// values.push_back(mBitmapSize);
		}
	}

	int value_num=values.size();
	int num =value_num +1;
	aos_assert_r(num>0, false);

	bool rslt;
	rslt=mBitmap1->splitBitmapByValues(values,bitmaps);
	aos_assert_r(rslt,false);
	int i=0,n=0;
	int new_numbits=0;
	int numbits=mBitmap1->getNumBits();


	for(i=0;i<num;i++)
	{
		new_numbits += bitmaps[i]->getNumBits();
	}
	aos_assert_r(new_numbits==numbits,false);


	u64 start = 0;
	u64 end = 0;
	for(i=0;i<num;i++)
	{
		bitmaps[i]->reset();
		start = end +1;
		if(i != num -1)
		{
			end = values[i] - 1;
		}
		else
		{
			end = mBitmapSize -1;
		}

		i64 counter = 0;
		for(i64 cur_idx = start;cur_idx <= end;cur_idx ++)
		{
			if(cur_idx == 0)
			{
				continue;
			}
			rslt=bitmaps[i]->checkDoc(cur_idx);
			if(rslt)
			{
if(print_flag)cout << cur_idx << ",";
				aos_assert_r(mArray1[cur_idx]== true,false);
				counter ++;
			}
			else
			{
				aos_assert_r(mArray1[cur_idx]== false,false);
			}
		}
if(print_flag){
cout << endl;
bitmaps[i]->reset();
bool rslt=false;
u64 aaa;
rslt = bitmaps[i]->nextDocid(aaa);
while(rslt)
{
	cout << aaa << ",";
	rslt = bitmaps[i]->nextDocid(aaa);
}
cout << endl;
}
		aos_assert_r(bitmaps[i]->getNumBits() == counter,false);

	}

	return true;
}
*/

bool 
AosBitmapTester::testSplitBitmapByValues()
{

	if(mBitmap1->getNumBits() == 0)
	{
		return true;
	}
	vector<u64> values;
	vector<AosBitmapObjPtr> bitmaps; 
	AosBitmapObjPtr new_bitmap1=mBitmap1->clone(mRundata);
	//保证vector中升序排列，没有重复值
	u64 idx=0;
	while(idx < mBitmapSize)
	{
		int n=rand()%mBitmapSize;
		if(n==0)
		{
			continue;
		}
		idx=n+idx;
		if(idx<mBitmapSize)
		{
			values.push_back(idx);
		}
		else 
		{
			// values.push_back(mBitmapSize);
		}
	}
	int value_num=values.size();
	int num =value_num +1;
	aos_assert_r(num>0, false);

	bool rslt;
	rslt=mBitmap1->splitBitmapByValues(values,bitmaps);
	aos_assert_r(rslt,false);
	int i=0,n=0;
	int new_numbits=0;
	int numbits=mBitmap1->getNumBits();
	for(i=0;i<num;i++)
	{
		new_numbits += bitmaps[i]->getNumBits();
	}
	aos_assert_r(new_numbits==numbits,false);
	u64 start = 0;
	u64 end = 0;
	for(i=0;i<num;i++)
	{
		bitmaps[i]->reset();
		start = end +1;
		if(i==0)
		{
			start=0;
		}
		if(i != num -1)
		{
			end = values[i] - 1;
		}
		else
		{
			end = mBitmapSize -1;
		}
                                                                        
		i64 counter = 0;
		for(i64 cur_idx = start;cur_idx <= end;cur_idx ++)
		{
		//	if(cur_idx == 0)
		//	{
		//		continue;
		//	}
		//
			rslt=bitmaps[i]->checkDoc(cur_idx);		
			if(rslt)		
			{
			if(print_flag)cout << cur_idx << ",";
				aos_assert_r(mArray1[cur_idx]== true,false);
				counter ++;
			}
			else
			{
				aos_assert_r(mArray1[cur_idx]== false,false);
			}
		}

//if(print_flag){
//cout << endl;
//bitmaps[i]->reset();
//u64 aaa = bitmaps[i]->nextDocid();
//while(aaa)
//{
//	cout << aaa << ",";
//	aaa = bitmaps[i]->nextDocid();
//}
//cout << endl;
//}

		aos_assert_r(bitmaps[i]->getNumBits() == counter,false);

	}

	return true;
}

/*
bool 
AosBitmapTester::testSplitBitmap()
{
	bool rslt = false;
	int new_numbits;
	u32 num=rand()%mBitmapSize+2;
	vector<AosBitmapObjPtr> bitmaps;
	mBitmap1->reset();
	AosBitmapObjPtr new_bitmap1=mBitmap1->clone(mRundata);
	u64 numbits=new_bitmap1->getNumBits();//get the numbits 
	int fixnumbits=numbits/num;//get the numbits/num
	//test the size==num
	rslt=new_bitmap1->splitBitmap(num,bitmaps);
	aos_assert_r(rslt,false);
	aos_assert_r(num==bitmaps.size(),false);
	//end
	u64 total = 0;
	for(u32 i=0;i<num;i++)
	{
		new_numbits=bitmaps[i]->getNumBits();
		total += new_numbits;
		aos_assert_r(new_numbits==fixnumbits+1 || new_numbits==fixnumbits,false);
	}
	aos_assert_r(total == numbits, false);

	u64 cur_docid = 0;
	u64 bitmap_idx = 0;
	bitmaps[0]->reset();
	for(u64 cur_idx = 1;cur_idx < mBitmapSize;cur_idx++)
	{
		if(!mArray1[cur_idx])
		{
			continue;
		}
		// the bit is true
		rslt = bitmaps[bitmap_idx]->nextDocid(cur_docid);
		while(!rslt)
		{
			// this bitmap is empty, check next bitmap
			aos_assert_r(bitmap_idx < num-1,false);
			bitmap_idx++;
			bitmaps[bitmap_idx]->reset();
			rslt = bitmaps[bitmap_idx]->nextDocid(cur_docid);
		}
		aos_assert_r(cur_docid == cur_idx,false);
	}
	return true;
}
*/

bool
AosBitmapTester::testSplitBitmap()
{
    // 1. params and split
    vector<AosBitmapObjPtr> bitmaps;
    u64 origionBits = mBitmap1->getNumBits();
    if(origionBits==0)
	{
		return true;
	}
	u64 newBits=0;
	u64 spliter = rand()%(mBitmapSize/2+1)+1;
//    u64 spliter = randomBetweenAAndB(2, mBitmapSize+1);
    u64 count = origionBits / spliter;
    AosBitmapObjPtr bitmap1 = new AosBitmap();
    
    aos_assert_r(mBitmap1->splitBitmap(spliter,bitmaps),false);
    
    // 2. judge bits
    u64 tmpBits;
    for (u64 index=0; index<spliter; index++)
    {
        tmpBits = bitmaps[index]->getNumBits();
        newBits += tmpBits;
        aos_assert_r(tmpBits==count||tmpBits==count+1 ,false);
    }
    aos_assert_r(newBits == origionBits,false);
    
    // 3. judge order of every element in bitmaps's bitmap
    bool canContinue;
    bool rslt;
    u64 lastlocation=0;
	u64	currentlocation=0;
    for (u64 index=0; index<spliter; index++)
    {
        bitmaps[index]->reset();
        canContinue = true;
		rslt = bitmaps[index]->nextDocid(currentlocation);
        while(rslt)
        {
            aos_assert_r(mArray1[currentlocation],false);
            aos_assert_r(lastlocation<currentlocation||currentlocation==0,false);
            lastlocation = currentlocation;
			rslt = bitmaps[index]->nextDocid(currentlocation);
        }
    }
    
/*  consider performance, you can annotate step 4 and 5
    // 4. perform or calculation
    for (u64 index=0; index<spliter; index++)
    {
        bitmap1 = AosBitmap::countOr(bitmap1,bitmaps[index]);
    }
    
    // 5. judge bitmap1 and mbitmap1
    aos_assert_r(mBitmap1->getNumBits() == bitmap1->getNumBits(),false);
    aos_assert_r(mBitmap1->includeBitmap(bitmap1),false);
*/    
    return true;
}

/*
bool 
AosBitmapTester::testGetPage()
{
	u64 page_size=rand()%mBitmapSize;
	u64 start_pos=rand()%mBitmapSize;
	bool direct = rand()%2;
	bool rslt = false;
	while(!rslt)
	{
		rslt = testGetPage(page_size,start_pos,direct);
	}
}
*/

/*
bool 
AosBitmapTester::testGetPage(const u64 &page_size,const u64 &sp, const bool direct)
{
	u64 start_pos = sp;
	u64 cur_pos;
	bool rslt = mBitmap1->moveTo(start_pos,cur_pos,direct);
	// check moveto
	while(!mArray1[start_pos])
	{
		if(direct)
		{
			start_pos --;
		}
		else
		{
			start_pos ++;
		}
		if(start_pos <=0 || start_pos >= mBitmapSize)
		{
			start_pos = 0;
			break;
		}
	}
	aos_assert_r(start_pos == cur_pos, false);	

	AosBitmapObjPtr target_bmp = OmnNew AosBitmap();
	u64 last_bit=mBitmap1->getPage(page_size,target_bmp,direct);
	target_bmp->reset();
	if(start_pos == 0)
	{
		aos_assert_r(target_bmp->getNumBits() == 0,false);
		return true;
	}

	u64 total = target_bmp->getNumBits();
	aos_assert_r(total <= page_size,false);
	u64 num_checked = 0;
	u64 prev_id = 0;
	while(num_checked < total)
	{
		if(start_pos <=0 || start_pos >= mBitmapSize)
		{
			aos_assert_r(false,false);
			return false;
		}

		if(mArray1[start_pos])
		{
			prev_id = start_pos;
			aos_assert_r(target_bmp->checkDoc(start_pos),false);
			num_checked++;
		}
		
		if(direct)
		{
			start_pos --;
		}
		else
		{
			start_pos ++;
		}

	}

	if(total < page_size)
	{
		if(start_pos <=0 || start_pos >= mBitmapSize)
		{
			return true;
		}

		aos_assert_r(!mArray1[start_pos],false);
		
		if(direct)
		{
			start_pos --;
		}
		else
		{
			start_pos ++;
		}
		
	}
	return true;
}
*/

bool
AosBitmapTester::testCloneFrom()
{

	if(rand()%2==0)
	{
		mBitmap1->pack();
	}
	else
	{
		mBitmap1->expand();
	}
	if(rand()%2==0)
	{
		mBitmap2->pack();
	}
	else
	{
		mBitmap2->expand();
	}
	bool rslt;
	AosBitmapObjPtr new_bitmap1=mBitmap2->clone(mRundata);
	new_bitmap1->cloneFrom(mBitmap1);
	
	AosBitmapObjPtr new_bitmap2=mBitmap1->clone(mRundata);
	new_bitmap2->cloneFrom(mBitmap2);

	rslt=new_bitmap1->compareBitmap(mBitmap1);
	aos_assert_r(rslt,false);
	rslt=new_bitmap2->compareBitmap(mBitmap2);
	aos_assert_r(rslt,false);
	return true;

}

u64
AosBitmapTester::randomBetweenAAndB(const u64 &a,const u64 &b)
{
    if (a>b) return 0;
    return (random()%(b-a+1))+a;
}

u64
AosBitmapTester::randomU64()
{
    return (((u64)random() << 32) | (u64)random());
}

bool
AosBitmapTester::percent(const int i)
{
    return (rand()%100 < i);
}

void
AosBitmapTester::changeValue(u64 &v1, u64 &v2)
{
    u64 tmp = v2;
    v2 = v1;
    v1 = tmp;
}

bool
AosBitmapTester::testFirstDoc()
{
/*
	// if bitmap is empty, there is no need to continue
	if(mBitmap1->isEmpty()) return true;

    u64 tmp = 0;

    aos_assert_r(mBitmap1->firstDocid(tmp), false);

    for(u64 i=0; i<=tmp; i++)
    {
        if(i == tmp)
        {
            aos_assert_r(mArray1[i], false);
        }
        else
        {
            aos_assert_r(!mArray1[i], false);
        }
    }
*/
    return true;
}

bool
AosBitmapTester::testLastDoc()
{
/*
	if(mBitmap2->isEmpty()) return true;

    mBitmap2->reset();
    u64 tmp = 0;

    aos_assert_r(mBitmap2->lastDocid(tmp), false);

    for(u64 i=tmp; i<mBitmapSize; i++)
    {
        if(i == tmp)
        {
            aos_assert_r(mArray2[i], false);
        }
        else
        {
            aos_assert_r(!mArray2[i], false);
        }
    }
*/
    return true;
}

bool
AosBitmapTester::testGetCountDocids()
{
/*
// 1. GetCountDocids
    bool isDelete = randomBetweenAAndB(0, 1);
    u64 fst_doc,lst_doc;
//    aos_assert_r(mBitmap1->firstDocid(fst_doc), false);
//    aos_assert_r(mBitmap1->lastDocid(lst_doc), false);
	bool b1 = mBitmap1->firstDocid(fst_doc);
	bool b2 = mBitmap1->lastDocid(lst_doc);
	cout<< b1<< " "<< b2<< endl;
    cout<< fst_doc<<" " <<lst_doc <<endl;
	u64 location = randomBetweenAAndB(fst_doc, lst_doc);
//	u64 location = randomBetweenAAndB(1, mBitmapSize);
    AosBitmapObjPtr bmp = OmnNew AosBitmap();
    aos_assert_r(mBitmap1->getCountDocids(location, bmp, false), false);
    
    // 2. compare every element of bmp whether not grand than location and judge these elements whether in mBitmap1
    bmp->reset();
    u64 tmp_doc;
    while (bmp->nextDocid(tmp_doc))
    {
        aos_assert_r(tmp_doc<location, false);
        if (isDelete)
        {
            aos_assert_r(!mBitmap1->checkDoc(tmp_doc), false);
        }
        else
        {
            aos_assert_r(mBitmap1->checkDoc(tmp_doc), false);
        }
    }
*/    
    return true;
}
