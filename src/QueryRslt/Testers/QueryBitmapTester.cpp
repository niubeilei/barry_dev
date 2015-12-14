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
// 09/21/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "QueryRslt/Testers/BitmapTester.h"

#include "Debug/Debug.h"
#include "BitmapMgr/Bitmap.h"
#include "BitmapMgr/BitmapMgr.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BitmapObj.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Queue.h"
#include "Util/OmnNew.h"
#include "Util/HashUtil.h"
#include "Porting/Sleep.h"
#include "Porting/TimeOfDay.h"

#define AOS_MBITS_NUM       10000   //1000000 //to use the macro to define number

AosBitmapTester::AosBitmapTester()
:
mNumBits(0),
mNumBits2(0),
find_count(0)	
{
}


AosBitmapTester::~AosBitmapTester()
{
}

bool AosBitmapTester::start()
{
	mBits 		= new u64[AOS_MBITS_NUM];
	mBitExist 	= new bool[AOS_MBITS_NUM];
	mBits2 		= new u64[AOS_MBITS_NUM];
	mBitExist2 	= new bool[AOS_MBITS_NUM];

	mBits3 		= new u64[AOS_MBITS_NUM];
	mBitExist3 	= new bool[AOS_MBITS_NUM];
	mBits4 		= new u64[AOS_MBITS_NUM];
	mBitExist4 	= new bool[AOS_MBITS_NUM];

#if 1    //modified by wanghaijun for test 
	for(int i = 0;i<AOS_MBITS_NUM;i++)
	{
		if(i % 100 == 0 && i > 0) cout << "round " <<  i << endl;
//		testOneBitmap(false);
		testCtrlBitmap(false);
	}
 		
	for(int i = 0;i <AOS_MBITS_NUM;i++)
	{
		if(i % 100 == 0) cout << "round " <<  i << endl;
//		testOneBitmap(true);		
		testCtrlBitmap(true);
		//cout << "=========================" << endl;
	}	

#else	
    testCtrlBitmap(false);	
//	testCtrlBitmap(true);
#endif
	OmnScreen << "AosBitmapTester,find_count = "<< getFindCount() << endl;
	return true;
}


bool
AosBitmapTester::basicTest(const bool show_info)
{
	AosBitmapObjPtr bitmap = AosBitmapMgr::getSelf()->getBitmap();
	u64 *bit = new u64[AOS_MBITS_NUM];
	memset(bit,0,AOS_MBITS_NUM*sizeof(u64) );
	u32 num = rand()%500;
//num = 4;
	
//	cout << "create data" << endl;
//	u64 max_value = 0x400000000000ULL; // 2 ^ 46
	for(u32 i = 0;i < num;i++)
	{
		u64 value = (((u64)rand()) << 32) | (rand());
		bit[i] = value;
		bitmap->appendDocid(value);
		
	}

//	cout << "create data ok, check it." << endl;
		
	for(u32 i = 0;i < num;i++)
	{
		if(! bitmap->checkDoc(bit[i]))
		{
			// show the original info
			for(u32 j = 0;j < num;j++)
			{
				u64 value = bit[j];
				u64 value1 = value >> 6;
				cout << (int)(((u8*)(&value1))[7]) << "\t"
					 << (int)(((u8*)(&value1))[6]) << "\t"
					 << (int)(((u8*)(&value1))[5]) << "\t"
					 << (int)(((u8*)(&value1))[4]) << "\t"
					 << (int)(((u8*)(&value1))[3]) << "\t"
					 << (int)(((u8*)(&value1))[2]) << "\t"
					 << (int)(((u8*)(&value1))[1]) << "\t"
					 << (int)(((u8*)(&value1))[0]) << "\t"
					 << (int)(value & 0x3f) << endl;
			}
			cout << "num: " << i << " value : " << bit[i] << "   not found" << endl;
			OmnSleep(1000000);
		}
//		aos_assert_r(bitmap->checkDoc(bit[i]),false);
	}
/*
	// save to buff
	AosBuffPtr buff = OmnNew AosBuff(10000,10000*AosMemoryCheckerArgs);
	bitmap->saveToBuff(buff);
	
	// load from buff
	AosBitmapObjPtr bitmap2 = AosBitmapMgr::getSelf()->getBitmap();
	bitmap2->loadFromBuff(buff);

	// check bitmap
	for(u32 i = 0;i < num;i++)
	{
		aos_assert_r(bitmap2->checkDoc(bit[i]),false);
	}
//	cout << "check ok." << endl;
*/
	AosBitmapMgr::getSelf()->returnBitmap(bitmap);
//	AosQueryRsltMgr::getSelf()->returnBitmap(bitmap2);

	delete []bit;
	return true;
}

bool
AosBitmapTester::testOneBitmap(const bool show_info)
{
	mBitmap1 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap3 = AosBitmapMgr::getSelf()->getBitmap();
		
	mNumBits = 0;
		
	memset(mBits,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist,1,AOS_MBITS_NUM/8);

	addDeleteBitmap(1,show_info);

    checkAllBitmap(1,show_info);

	checkSaveLoad(1,show_info);

	cleanResource();
	return true;
}

bool
AosBitmapTester::testTwoBitmap(const bool show_info)
{
	mBitmap1 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap2 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap3 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap4 = AosBitmapMgr::getSelf()->getBitmap();

	mNumBits = 0;
	mNumBits2 = 0;
		
	memset(mBits,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist,1,AOS_MBITS_NUM/8);
	memset(mBits2,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist2,1,AOS_MBITS_NUM/8);
	
	addDeleteBitmap(1,show_info);

	if(show_info)
	{
	   cout << "=====================" << endl;
	}
	addDeleteBitmap(2,show_info);


    checkAllBitmap(1,show_info);
	checkAllBitmap(2,show_info);

	checkSaveLoad(1,show_info);
	checkSaveLoad(2,show_info);

	checkNextDocid(1,show_info);
	checkNextDocid(2,show_info);
	checkAndOr(show_info);
	cleanResource();
	return true;
}

bool
AosBitmapTester::testCtrlBitmap(const bool show_info)
{
	mBitmap1 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap2 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap3 = AosBitmapMgr::getSelf()->getBitmap();
	mBitmap4 = AosBitmapMgr::getSelf()->getBitmap();

	mNumBits = 0;
	mNumBits2 = 0;
	mNumBits3 = 0;
	mNumBits4 = 0;
	
	memset(mBits,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist,1,AOS_MBITS_NUM/8);
	memset(mBits2,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist2,1,AOS_MBITS_NUM/8);
	memset(mBits3,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist3,1,AOS_MBITS_NUM/8);
	memset(mBits4,0,AOS_MBITS_NUM*sizeof(u64) );
	memset(mBitExist4,1,AOS_MBITS_NUM/8);

	addDeleteBitmap(1,show_info);

	if(show_info)
	{
	//   cout << "=====================" << endl;
	}
	addDeleteBitmap(2,show_info);
	
	if(show_info)
	{
	  //cout << "=====================" << endl;
	}
    addDeleteBitmap(3,show_info);

	if(show_info)
	{
	   //cout << "=====================" << endl;
	}
    addDeleteBitmap(4,show_info);
 
	checkAndWithCtrlRslt(show_info);
	
    cleanResource();
	return true;
}

bool
AosBitmapTester::checkNextDocid(const int seq,const bool show_info)
{
	AosBitmapObjPtr bitmap;
	u64* bits;
	bool* bit_exist = new bool[20000];
	int	num = mNumBits;

	switch(seq)
	{
		case 1:
			bitmap = mBitmap1;
			bits = mBits;
//			bit_exist = mBitExist;
			memcpy(bit_exist,mBitExist,mNumBits);
			num = mNumBits;
			break;
		case 2:
			bitmap = mBitmap2;
			bits = mBits2;
//			bit_exist = mBitExist2;
			memcpy(bit_exist,mBitExist2,mNumBits2);
			num = mNumBits2;
			break;
		case 3:
			bitmap = mBitmap3;
			bits = mBits3;
//			bit_exist = mBitExist;
			memcpy(bit_exist,mBitExist3,mNumBits3);
			num = mNumBits3;
			break;
		case 4:
			bitmap = mBitmap4;
			bits = mBits4;
//			bit_exist = mBitExist2;
			memcpy(bit_exist,mBitExist4,mNumBits4);
			num = mNumBits4;
			break;
		default:
			return true;
	}
	bitmap->reset();
	u64 cur_did = bitmap->nextDocid();	
	while(cur_did)
	{
		// check cur_did is in the list or not
		bool found = false;         
		for(int i = 0;i < num;i++)
		{
			// for test
			if(bits[i] == cur_did )
			{
				int i = 0;
				i++;
			}
			
			// check bit exist or not
			if(bits[i] == cur_did && bit_exist[i])
			{	
				found = true;				
				bit_exist[i] = false;
			}			
		}
		aos_assert_r(found,false);
		cur_did = bitmap->nextDocid();
	}
	
	// check whether all bit_exist bits have been set to false;
	for(int i = 0;i < num;i++)
	{
		aos_assert_r(!bit_exist[i],false);
	}
	
	if(bit_exist)
	{
		delete []bit_exist;
		bit_exist = 0;
	}	
	return true;
}

bool
AosBitmapTester::checkSaveLoad(const int seq,const bool show_info)
{
	AosBitmapObjPtr bitmap_a;
	AosBitmapObjPtr bitmap_b;
	int load_bitmap_seq = 0;
	if(seq == 1)
	{
		bitmap_a = mBitmap1;
		bitmap_b = mBitmap3;
		load_bitmap_seq = 3;
	}
	else
	{
		bitmap_a = mBitmap2;
		bitmap_b = mBitmap4;
		load_bitmap_seq = 4;
	}
	
	AosBuffPtr buff = OmnNew AosBuff(eBuffInitSize AosMemoryCheckerArgs);
	bitmap_a->saveToBuff(buff);
	bitmap_b->loadFromBuff(buff);
	
	checkAllBitmap(load_bitmap_seq,show_info);
	return true;
}

bool
AosBitmapTester::cleanResource()
{
	mNumBits = 0;
	mNumBits2 = 0;
	if(mBitmap1)
	{		
		AosBitmapMgr::getSelf()->returnBitmap(mBitmap1);
		mBitmap1 = 0;
	}
	if(mBitmap2)
	{
		AosBitmapMgr::getSelf()->returnBitmap(mBitmap2);
		mBitmap2 = 0;
	}
	if(mBitmap3)
	{
		AosBitmapMgr::getSelf()->returnBitmap(mBitmap3);
		mBitmap3 = 0;
	}
	if(mBitmap4)
	{
		AosBitmapMgr::getSelf()->returnBitmap(mBitmap4);
		mBitmap4 = 0;
    }
//	if(mBits)
//	{
//		delete []mBits;
//		mBits = 0;
//	}
//	if(mBits2)
//	{
//		delete []mBits2;
//		mBits2 = 0;
//	}
//	if(mBitExist)
//	{
//		delete []mBitExist;
//		mBitExist = 0;
//	}
//	if(mBitExist2)
//	{
//		delete []mBitExist2;
//		mBitExist2 = 0;
//	}
	return true;
}


bool 
AosBitmapTester::addDeleteBitmap(const int seq, const bool show_info)
{
	u32 num_opr = rand()%3000;  //expand by navy to test more data
	int opr =0;

	//num_opr = 400;   //added by wanghaijun for test
	
	for(u32 i = 0;i < num_opr;i++)
	{
	 	opr = rand()%100;
				
		//add 5% percent to add and remove.
		if(opr < 55)
		{
			// add bitmap			
			addBitmap(seq,show_info);
		}
		else if(opr < 75)
		{			
			// remove bitmap
			removeBitmap(seq,show_info);
		}
		else
		{
			// check bitmap
			checkBitmapRandom(seq,show_info);
		}			
	}	
	return true;
}


//Added by navy to check the result of "A and !B " function
bool	
AosBitmapTester::checkNegate(const bool show_info)
{
	u64 cur_value = 0;
	bool expect = false,expect1=false;
	bool checkrslt = false;
	bool is_show = 0;//show_info;

	mBitmapNegate = AosBitmap::countAndAnotB(mBitmap1,mBitmap2);
	
	for(int i = 0;i < mNumBits;i++)
	{
		cur_value = mBits[i];
		checkrslt = mBitmapNegate->checkDoc(cur_value);

		if (is_show)
           OmnScreen << "checkrslt="<<checkrslt << "  value = " << cur_value<<" mBitExist[i]="<< mBitExist[i] << endl;

 		expect = false;
		if(!mBitExist[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits;j++)
			{
				if((mBits[j] == cur_value) && mBitExist[j])
				{
					expect = true;
					break;
				}
			}
			//if this value is not in both of mBitmapNegate and bit1,it's successful
			//so if expect is false, but it's in mBitmapNegate,it must be error.
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}		
       
		//this value is in bit1,it should be not in bit2 
		expect = true;
		for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect = false;
				break;
			}
		}
		if (is_show) OmnScreen << "expect="<< expect << endl;

		checkRslt(cur_value,checkrslt,expect);
	}

	for(int i = 0;i < mNumBits2;i++)
	{
		cur_value = mBits2[i];
		checkrslt = mBitmapNegate->checkDoc(cur_value);

		if (is_show)
		   OmnScreen << "checkrslt="<<checkrslt << "  value = " << cur_value<<" mBitExist2[i]="<< mBitExist2[i] << endl;

		expect = true;
		if(!mBitExist2[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits2;j++)
			{
				if((mBits2[j] == cur_value) && mBitExist2[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == true)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}

		if (is_show)
            OmnScreen << "expect2="<< expect << endl;

		expect1 = false;
		
	    for(int j = 0;j < mNumBits;j++)
	    {
			  if ((mBits[j] == cur_value) && mBitExist[j])
			  {			  	  
				  expect1 = !expect ;  
				  break;
			  }		   			  
		}
		expect = expect1;
		checkRslt(cur_value,checkrslt,expect);
	}

	return true;
}

bool	
AosBitmapTester::checkAndOr(const bool show_info)
{
	checkAnd(show_info);
	checkOr(show_info);	
    checkNegate(show_info);

	if(mBitmapAnd)
	{		
		AosBitmapMgr::getSelf()->returnBitmap(mBitmapAnd);
		mBitmapAnd = 0;
	}
	if(mBitmapOr)
	{
		AosBitmapMgr::getSelf()->returnBitmap(mBitmapOr);
		mBitmapOr = 0;
	}	
    if (mBitmapNegate)	
	{
        AosBitmapMgr::getSelf()->returnBitmap(mBitmapNegate);
		mBitmapNegate = 0 ;
	}
	return true;
}

void
AosBitmapTester::printBitmapFilter(AosBitmapObjPtr a,bool is_move)
{
	aos_assert(a);
    u64 mFilter = 0;
    AosQueryNode* node = a->getNextLeafNode();

	while (node)
	{
       if (is_move)
	   {
           mFilter = (node->mFilter>>14) & AosBitmap::smMasks[0];
	   }
	   else
	   {
		   mFilter = node->mFilter;
	   }
       OmnScreen << "node->mFilter="<<mFilter<<endl;
	   node = a->getNextLeafNode();
	}
	return;
}
//Navy: test AND function with Ctrl Bitmap
//And we use four bitmap to test
bool	
AosBitmapTester::checkAndWithCtrlRslt(const bool show_info)
{	
    u64 t1,t2;	
	t1 = OmnGetTimestamp();	 

    AosBitmapObjPtr bitmap1,bitmap2;
    
	
   bitmap1 = AosBitmap::countAnd(mBitmap1,mBitmap2);  	    
   bitmap2 = AosBitmap::countAnd(mBitmap3,bitmap1);
   mBitmapAnd = AosBitmap::countAnd(mBitmap4,bitmap2);		
   t1 = OmnGetTimestamp()-t1;

   //OmnScreen << " print the old data" << endl;
   //printBitmapFilter(mBitmapAnd,false);

   if (bitmap1)
   {
	  AosBitmapMgr::getSelf()->returnBitmap(bitmap1);
	  bitmap1 = 0; 
   }
   if (bitmap2)
   {
	  AosBitmapMgr::getSelf()->returnBitmap(bitmap2);
	  bitmap2 = 0; 
   }       

   //Calulate the exec time of the new function 
   AosBitmap *a1,*b1,*c1,*d1;

   a1 = (AosBitmap*)mBitmap1.getPtr();
   b1 = (AosBitmap*)mBitmap2.getPtr();

   c1 = (AosBitmap*)mBitmap3.getPtr();
   d1 = (AosBitmap*)mBitmap4.getPtr();

   if (!a1 || !b1 || !c1 || !d1)
	  return false;
   
   if (a1->mCtrlBitmap && b1->mCtrlBitmap && c1->mCtrlBitmap && d1->mCtrlBitmap)
   {
	  t2 = OmnGetTimestamp();	 

	  //A AND B according to Ctrl Bitmap of C, return the new bitmap bitmap1  
	  //bitmap1 = AosBitmap::countAnd(mBitmap1,mBitmap2,c1->mCtrlBitmap);               

	  //C AND The result of A AND B according to Ctrl Bitmap of D,return the new bitmap bitmap2
	  //bitmap2; = AosBitmap::countAnd(mBitmap3,bitmap1,d1->mCtrlBitmap); 	 
	  
	  //D AND bitmap2, return the last result
	  mBitmapAnd = AosBitmap::countAnd(mBitmap4,bitmap2);
	  t2 = OmnGetTimestamp()-t2;

	  //OmnScreen << "mBitmapAnd------------------------"<<endl;
	  //printBitmapFilter(mBitmapAnd,false);
	  
	  if (bitmap1)
	  {
		 AosBitmapMgr::getSelf()->returnBitmap(bitmap1);
		 bitmap1 = 0; 
	  }
	  if (bitmap2)
	  {
		 AosBitmapMgr::getSelf()->returnBitmap(bitmap2);
		 bitmap2 = 0; 
	  }	   
	}			
	
    if (t2 <= t1) setFindCount();

	u64 cur_value = 0;
	bool expect = false,expect1 =false,expect2 =false,expect3 =false ;
	bool checkrslt = false;

	for(int i = 0;i < mNumBits;i++)
	{
		cur_value = mBits[i];
		//cur_value = (cur_value >> 14) & AosQueryNode::mMasks[0];
		checkrslt = mBitmapAnd->checkDoc(cur_value);
	    //OmnScreen << "checkrslt << " << checkrslt << " cur_value="<< cur_value <<endl;
        // A: 0; B:0; C:0; D:1/0  rslt: 0
		// A: 1; B:0; C:0; D:0    rslt: 0

		expect = true;
		if(!mBitExist[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits;j++)
			{
				if((mBits[j] == cur_value) && mBitExist[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}						
		
		//OmnScreen << "check the first bitmap:expect="<< expect <<endl;

		expect1 =false;
	    for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect1 = true;
				break;
			}
		}
	    //OmnScreen << "check the second bitmap:expect="<< expect1 <<endl;

		expect2 = false;
        for(int j = 0;j < mNumBits3;j++)
		{
			if((mBits3[j] == cur_value) && mBitExist3[j])
			{
				expect2= true;
				break;
			}
		}

		//OmnScreen << "check the third bitmap:expect="<< expect2 <<endl;

		expect3 = false;
        for(int j = 0;j < mNumBits4;j++)
		{
			if((mBits4[j] == cur_value) && mBitExist4[j])
			{
				expect3 = true;
				break;
			}
		}

		//OmnScreen << "check the fourth bitmap:expect="<< expect3 <<endl;
		
		expect = expect & expect1 & expect2 & expect3;

		//OmnScreen << "check the result bitmap:expect = "<< expect <<endl;
		checkRslt(cur_value,checkrslt,expect);
	}

    for(int i = 0;i < mNumBits2;i++)
	{
		cur_value = mBits2[i];
		//cur_value = (cur_value >> 14) & AosQueryNode::mMasks[0];
		checkrslt = mBitmapAnd->checkDoc(cur_value);
	    //OmnScreen << "checkrslt << " << checkrslt << " cur_value="<< cur_value <<endl;
        // A: 0; B:0; C:0; D:1/0  rslt: 0
		// A: 1; B:0; C:0; D:0    rslt: 0

		expect = true;
		if(!mBitExist2[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits2;j++)
			{
				if((mBits2[j] == cur_value) && mBitExist2[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}						
		
		//OmnScreen << "check the first bitmap:expect="<< expect <<endl;

		expect1 =false;
	    for(int j = 0;j < mNumBits;j++)
		{
			if((mBits[j] == cur_value) && mBitExist[j])
			{
				expect1 = true;
				break;
			}
		}
	    //OmnScreen << "check the second bitmap:expect="<< expect1 <<endl;

		expect2 = false;
        for(int j = 0;j < mNumBits3;j++)
		{
			if((mBits3[j] == cur_value) && mBitExist3[j])
			{
				expect2= true;
				break;
			}
		}

		//OmnScreen << "check the third bitmap:expect="<< expect2 <<endl;

		expect3 = false;
        for(int j = 0;j < mNumBits4;j++)
		{
			if((mBits4[j] == cur_value) && mBitExist4[j])
			{
				expect3 = true;
				break;
			}
		}

		//OmnScreen << "check the fourth bitmap:expect="<< expect3 <<endl;
		
		expect = expect & expect1 & expect2 & expect3;

		//OmnScreen << "check the result bitmap:expect = "<< expect <<endl;
		checkRslt(cur_value,checkrslt,expect);	
	}

    for(int i = 0;i < mNumBits3;i++)
	{
		cur_value = mBits3[i];
		//cur_value = (cur_value >> 14) & AosQueryNode::mMasks[0];
		checkrslt = mBitmapAnd->checkDoc(cur_value);
	    //OmnScreen << "checkrslt << " << checkrslt << " cur_value="<< cur_value <<endl;
        // A: 0; B:0; C:0; D:1/0  rslt: 0
		// A: 1; B:0; C:0; D:0    rslt: 0

		expect = true;
		if(!mBitExist3[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits3;j++)
			{
				if((mBits3[j] == cur_value) && mBitExist3[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}						
		
		//OmnScreen << "check the first bitmap:expect="<< expect <<endl;

		expect1 =false;
	    for(int j = 0;j < mNumBits;j++)
		{
			if((mBits[j] == cur_value) && mBitExist[j])
			{
				expect1 = true;
				break;
			}
		}
	    //OmnScreen << "check the second bitmap:expect="<< expect1 <<endl;

		expect2 = false;
        for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect2= true;
				break;
			}
		}

		//OmnScreen << "check the third bitmap:expect="<< expect2 <<endl;

		expect3 = false;
        for(int j = 0;j < mNumBits4;j++)
		{
			if((mBits4[j] == cur_value) && mBitExist4[j])
			{
				expect3 = true;
				break;
			}
		}

		//OmnScreen << "check the fourth bitmap:expect="<< expect3 <<endl;
		
		expect = expect & expect1 & expect2 & expect3;

		//OmnScreen << "check the result bitmap:expect = "<< expect <<endl;
		checkRslt(cur_value,checkrslt,expect);	
	}

    for(int i = 0;i < mNumBits4;i++)
	{
		cur_value = mBits4[i];
		//cur_value = (cur_value >> 14) & AosQueryNode::mMasks[0];
		checkrslt = mBitmapAnd->checkDoc(cur_value);
	    //OmnScreen << "checkrslt << " << checkrslt << " cur_value="<< cur_value <<endl;
        // A: 0; B:0; C:0; D:1/0  rslt: 0
		// A: 1; B:0; C:0; D:0    rslt: 0

		expect = true;
		if(!mBitExist4[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits4;j++)
			{
				if((mBits4[j] == cur_value) && mBitExist4[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}						
		
		//OmnScreen << "check the first bitmap:expect="<< expect <<endl;

		expect1 =false;
	    for(int j = 0;j < mNumBits;j++)
		{
			if((mBits[j] == cur_value) && mBitExist[j])
			{
				expect1 = true;
				break;
			}
		}
	    //OmnScreen << "check the second bitmap:expect="<< expect1 <<endl;

		expect2 = false;
        for(int j = 0;j < mNumBits3;j++)
		{
			if((mBits3[j] == cur_value) && mBitExist3[j])
			{
				expect2= true;
				break;
			}
		}

		//OmnScreen << "check the third bitmap:expect="<< expect2 <<endl;

		expect3 = false;
        for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect3 = true;
				break;
			}
		}

		//OmnScreen << "check the fourth bitmap:expect="<< expect3 <<endl;
		
		expect = expect & expect1 & expect2 & expect3;

		//OmnScreen << "check the result bitmap:expect = "<< expect <<endl;
		checkRslt(cur_value,checkrslt,expect);	
	}
	return true;
}

bool	
AosBitmapTester::checkAnd(const bool show_info)
{
//	AosBitmap::countRsltAnd(mBitmap1,mBitmap2);         
    mBitmapAnd = AosBitmap::countAnd(mBitmap1,mBitmap2);  	    
    
	u64 cur_value = 0;
	bool expect = false;
	bool checkrslt = false;

	for(int i = 0;i < mNumBits;i++)
	{
		cur_value = mBits[i];
		//cur_value = (cur_value >> 14) & AosQueryNode::mMasks[0];
		checkrslt = mBitmapAnd->checkDoc(cur_value);
        //OmnScreen << "checkrslt << " << checkrslt << " cur_value="<< cur_value<<endl;

		expect = true;
		if(!mBitExist[i])
		{
			// check whether there is any other value equal to cur_value
			expect = false;
			for(int j = 0;j < mNumBits;j++)
			{
				if((mBits[j] == cur_value) && mBitExist[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}

		expect = false;
		for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect = true;
				break;
			}
		}
		checkRslt(cur_value,checkrslt,expect);
	}

	for(int i = 0;i < mNumBits2;i++)
	{
		cur_value = mBits2[i];
//		cur_value = (cur_value >> 14) & AosQueryNode::mMasks[0];
		checkrslt = mBitmapAnd->checkDoc(cur_value);
		//OmnScreen << " checkrslt22 << " << cur_value<<endl;

		expect = false;
		if(!mBitExist2[i])
		{
			// check whether there is any other value equal to cur_value
			// if not found in B,it's successful
			expect = false;
			for(int j = 0;j < mNumBits2;j++)
			{
				if((mBits2[j] == cur_value) && mBitExist2[j])
				{
					expect = true;
					break;
				}
			}
			if(expect == false)
			{
				checkRslt(cur_value,checkrslt,expect);
				continue;
			}
		}

		expect = false;
		for(int j = 0;j < mNumBits;j++)
		{
			if((mBits[j] == cur_value) && mBitExist[j])
			{
				expect = true;
				break;
			}
		}
		checkRslt(cur_value,checkrslt,expect);
	}

	return true;
}


bool	
AosBitmapTester::checkOr(const bool show_info)
{
//	AosBitmap::countRsltAnd(mBitmap1,mBitmap2);
	mBitmapOr = AosBitmap::countOr(mBitmap1,mBitmap2);
	u64 cur_value = 0;
	bool expect = false;
	bool checkrslt = false;
	for(int i = 0;i < mNumBits;i++)
	{
		cur_value = mBits[i];
		checkrslt = mBitmapOr->checkDoc(cur_value);
		
		expect = true;
		if(mBitExist[i])
		{
			checkRslt(cur_value,checkrslt,expect);
			continue;
		}
		
		// mBitExist == false;
		// check whether there is any other value equal to cur_value
		expect = false;
		for(int j = 0;j < mNumBits;j++)
		{
			if((mBits[j] == cur_value) && mBitExist[j])
			{
				expect = true;
				break;
			}
		}
		if(expect == true)
		{
			checkRslt(cur_value,checkrslt,expect);
			continue;
		}
		
		expect = false;
		for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect = true;
				break;
			}
		}
		checkRslt(cur_value,checkrslt,expect);
	}
		
	for(int i = 0;i < mNumBits2;i++)
	{
		cur_value = mBits2[i];
		checkrslt = mBitmapOr->checkDoc(cur_value);
		
		expect = true;
		if(mBitExist2[i])
		{
			checkRslt(cur_value,checkrslt,expect);
			continue;
		}
		
		// check whether there is any other value equal to cur_value
		expect = false;
		for(int j = 0;j < mNumBits2;j++)
		{
			if((mBits2[j] == cur_value) && mBitExist2[j])
			{
				expect = true;
				break;
			}
		}
		if(expect == true)
		{
			checkRslt(cur_value,checkrslt,expect);
			continue;
		}

		expect = false;
		for(int j = 0;j < mNumBits;j++)
		{
			if((mBits[j] == cur_value) && mBitExist[j])
			{
				expect = true;
				break;
			}
		}
		checkRslt(cur_value,checkrslt,expect);
	}
	return true;
}

bool	
AosBitmapTester::checkRslt(const u64 &expect_doc,const bool a, const bool b)
{
	if(a!= b)
	{
		cout << "check value:" << expect_doc << " failed:" <<(int)a << ":" << (int)b  << endl;
		OmnSleep(10000);
	}
	return true;
}

bool	
AosBitmapTester::addBitmap(const int seq,const bool show_info)
{
	AosBitmapObjPtr bitmap = 0;
	u64* bits;
	bool* bit_exist;
	int	num = mNumBits;
		
	switch(seq)
	{
		case 1:
			bitmap = mBitmap1;
			bits = mBits;
			bit_exist = mBitExist;
			num = mNumBits;
			break;
		case 2:
			bitmap = mBitmap2;
			bits = mBits2;
			bit_exist = mBitExist2;
			num = mNumBits2;
			break;
		case 3:
			bitmap = mBitmap3;
			bits = mBits3;
			bit_exist = mBitExist3;
			num = mNumBits3;
			break;
		case 4:
			bitmap = mBitmap4;
			bits = mBits4;
			bit_exist = mBitExist4;
			num = mNumBits4;
			break;	
		default:
			return false;
	}
	
	// create value
	int a = rand()%100;
	u64 value = (((u64)rand()) << 32) | (rand());
	if(a > 20)// create from existing value:30
	{
		// which list we can get from?		
		if((!mNumBits2) || a > 80) 
		{
			if(mNumBits)
			{
				int index = rand() % mNumBits;				
				value = mBits[index] + rand()%100 - 49;
			}
		}
		else if (!mNumBits3 || a >60 )
		{
			if(mNumBits2)
			{
				int index = rand() % mNumBits2;				
				value = mBits2[index] + rand()%100 - 49;
			}				
		}	
		else if (!mNumBits4 || a > 40)
		{
            if(mNumBits3)
			{
				int index = rand() % mNumBits3;				
				value = mBits3[index] + rand()%100 - 49;
			}
		}
		else
		{
            if(mNumBits4)
			{
				int index = rand() % mNumBits4;				
				value = mBits4[index] + rand()%100 - 49;
			}
		}
	}
	
//	u64 value = (((u64)rand()) << 32) | (rand());

	if(show_info && (value == 202597898456474694)) 
		cout << "add\t" << value << " seq = " << seq << endl;

	bits[num++] = value;
	bitmap->appendDocid(value);

	for(int i = 0;i < num;i++)
	{
		if(bits[i] == bits[num-1])bit_exist[i] = true;		
	}			
	
	switch(seq)
	{
		case 1:
			mNumBits = num;
			break;
		case 2:
			mNumBits2 = num;
			break;
 	    case 3:
			mNumBits3 = num;
			break;
		case 4:
			mNumBits4 = num;
			break;
		default:
			return false;
	}
	return true;
}			

bool	
AosBitmapTester::removeBitmap(const int seq,const bool show_info)
{
	AosBitmapObjPtr bitmap = 0;
	u64* bits;
	bool* bit_exist;
	int	num = 0;
		
	switch(seq)
	{
		case 1:
			bitmap = mBitmap1;
			bits = mBits;
			bit_exist = mBitExist;
			num = mNumBits;
			break;
		case 2:
			bitmap = mBitmap2;
			bits = mBits2;
			bit_exist = mBitExist2;
			num = mNumBits2;
			break;
		case 3:
			bitmap = mBitmap3;
			bits = mBits3;
			bit_exist = mBitExist3;
			num = mNumBits3;
			break;
		case 4:
			bitmap = mBitmap4;
			bits = mBits4;
			bit_exist = mBitExist4;
			num = mNumBits4;
			break;		
		default:
			return false;
	}

	if(num == 0) return true;
	int index = rand() % num;
	
	if(show_info && (bits[index] == 202597898456474694)) 
		cout << "remove\t" << bits[index] << " seq ="  << seq<< endl;

	bitmap->removeDocid(bits[index]);
	bit_exist[index] = false;
	for(int i = 0;i < num;i++)
	{
		if(bits[i] == bits[index])bit_exist[i] = false;
	}			

	switch(seq)
	{
		case 1:
			mNumBits = num;
			break;
		case 2:
			mNumBits2 = num;
			break;
		case 3:
			mNumBits3 = num;
			break;
		case 4:
			mNumBits4 = num;
			break;	
		default:
			return false;
	}

	return true;
}

bool	
AosBitmapTester::checkAllBitmap(const int seq, const bool show_info)
{
	int num = 0;
	switch(seq)
	{
		case 1:
			num = mNumBits;
			break;
		case 2:
			num = mNumBits2;
			break;
		case 3:
			num = mNumBits3;
			break;
		case 4:
			num = mNumBits4;
			break;
		default:
			return false;
	}

	for(int i = 0;i < num;i++)
	{
		checkBitmap(seq,i,show_info);
	}
	return true;
}

bool	
AosBitmapTester::checkBitmapRandom(const int seq,const bool show_info)
{
	int num = 0;
	switch(seq)
	{
		case 1:
			num = mNumBits;
			break;
		case 2:
			num = mNumBits2;
			break;
		case 3:
			num = mNumBits3;
			break;
		case 4:
			num = mNumBits4;
			break;
		default:
			return false;
	}

	if(num == 0) return true;
	int index = rand() % num;
	return checkBitmap(seq,index,show_info);
}

bool	
AosBitmapTester::checkBitmap(const int seq,const int &index,const bool show_info)
{
	AosBitmapObjPtr bitmap;
	int num = 0;
	u64* bits;
	bool* bit_exist;
	switch(seq)
	{
		case 1:
			bitmap = mBitmap1;
			bits = mBits;
			bit_exist = mBitExist;
			num = mNumBits;
			break;
		case 2:
			bitmap = mBitmap2;
			bits = mBits2;
			bit_exist = mBitExist2;
			num = mNumBits2;
			break;
		case 3:
			bitmap = mBitmap3;
			bits = mBits3;
			bit_exist = mBitExist3;
			num = mNumBits3;
			break;
		case 4:
			bitmap = mBitmap4;
			bits = mBits4;
			bit_exist = mBitExist4;
			num = mNumBits4;
			break;
		default:
			return false;
	}

	if(num == 0) return true;

	// check bit[index]
	bool rslt = bitmap->checkDoc(bits[index]);
	if((!rslt) && bit_exist[index])
	{
		// should be there but not exist
		for(int j = 0;j < num;j++)
		{
			u64 value = bits[j];
			u64 value1 = value >> 6;
			cout << (int)(((u8*)(&value1))[7]) << "\t"
				 << (int)(((u8*)(&value1))[6]) << "\t"
				 << (int)(((u8*)(&value1))[5]) << "\t"
				 << (int)(((u8*)(&value1))[4]) << "\t"
				 << (int)(((u8*)(&value1))[3]) << "\t"
				 << (int)(((u8*)(&value1))[2]) << "\t"
				 << (int)(((u8*)(&value1))[1]) << "\t"
				 << (int)(((u8*)(&value1))[0]) << "\t"
				 << (int)(value & 0x3f) 	   << "\t" 
				 << (int)(bit_exist[index])<< endl;
		}
		cout << "should be there but not exist" << endl;
		cout << "num: " << index << " value : " << bits[index] << endl;
		OmnSleep(1000000);
	}

	if( rslt && (!(bit_exist[index])))
	{
		// should be deleted but found
		for(int j = 0;j < num;j++)
		{
			u64 value = bits[j];
			u64 value1 = value >> 6;
			cout << (int)(((u8*)(&value1))[7]) << "\t"
				 << (int)(((u8*)(&value1))[6]) << "\t"
				 << (int)(((u8*)(&value1))[5]) << "\t"
				 << (int)(((u8*)(&value1))[4]) << "\t"
				 << (int)(((u8*)(&value1))[3]) << "\t"
				 << (int)(((u8*)(&value1))[2]) << "\t"
				 << (int)(((u8*)(&value1))[1]) << "\t"
				 << (int)(((u8*)(&value1))[0]) << "\t"
				 << (int)(value & 0x3f) 	   << "\t" 
				 << (int)(bit_exist[index])<< endl;
		}
		cout << "should be deleted but found" << endl;
		cout << "num: " << index << " value : " << bits[index] << endl;
		OmnSleep(1000000);
	}
	return true;
}


bool test()
{
#if 0	
	AosU642Char_t map;
	u64 docid = rand();
	map[docid] = 10;
	AosU322StrItr_t itr = map.find(100);
	if (itr == map.end())
	{
		// Not found
	}
	else
	{
		cout << itr->second;
	}

	AosU322StrItr_t itr = map.find(docid);
	if (itr == map.end())
	{
		// Not found
	}
	else
	{
		cout << itr->second;
		aos_assert_r(itr->second == 10, false);
		aos_assert_r(itr->first == docid, false);
	}
#endif
	return false;
}

			
			
			
			
			
