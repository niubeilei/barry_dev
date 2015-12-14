#include "torture.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include <time.h>
#include <cstring>
#include <stdlib.h>
#include <iostream>


AOSTorture::AOSTorture()
{
	mJoinList[0] = ",";
	mJoinList[1] = "join";
	mJoinList[2] = "leftjoin";
	mJoinList[3] = "rightjoin";
	mJoinList[4] = "innerjoin";
}


bool
AOSTorture::initBracketList()
{
	mBracket.clear();
	vector<int> tem;
	for(int i =0; i<= mNumTables; i++ )//initial the two dimension vector
			                           //the first value of each demension is the num of left brackets
                                       //the second value of each demension is the num of right brackets
    {
	    tem.clear();
	    int temvalue =0;
	    tem.push_back(temvalue);
	    tem.push_back(temvalue);
	    mBracket.push_back(tem);
	}
}


bool
AOSTorture::config(AosRundata *rdata, const AosXmlTagPtr &conf)
{
	// The configuration is in the form:
	// 	<tag max_tables="ddd" .../>
	mMaxTables = conf->getAttrInt("max_tables", -1);
	aos_assert_rr(mMaxTables > 0, rdata, false);

	return true;
}


bool
AOSTorture::generateSQLCase()
{
	initBracketList();
	generateTableSequence();
	generateJoinSequence();
	generateBracketSequence(0,mNumTables);
	printSQLCase();
	return true;
}


bool
AOSTorture::generateTableSequence()
{
	// This function randomly determines how many tables to use
	// for this round of testing. It then randomly picks those
	// tables from the preconfigured table list. The results are
	// stored in 'mTableList'.
	
	int cnt = 0;// table that has been selected
	vector<bool> flag;
	for (int i=0; i<mMaxTables; i++) flag.push_back(false);

    srand(time(NULL));
	mNumTables  = OmnRandom::intByRange(
			        1, 2, 60,
					3, 5, 40,
					6, 10, 5,
					11, 50, 1);

    while (cnt < mNumTables)//generate table sequence
    {
		int temid = rand()%(mMaxTables - 1) +1;
		if(flag[temid] == false)
		{
			flag[temid] = true;
			mTableList.push_back(temid);//save table id sequence
		}
	}
	return true;
}


bool
AOSTorture::generateJoinSequence()
{
	// This function randomly picks a join type for each table
	// in 'mTableList'.
	mJoinSequence.clear();

	srand(time(NULL));
	int cnt = 0;
	while(cnt < mNumTables - 1)
	{
		int joinid = rand()%5;
		mJoinSequence.push_back(joinid);
	}
	return true;
}


bool
AOSTorture::generateBracketSequence(int low, int high)
{
	// This function randomly inserts brackets into 'mTableList'. 
	// 
	//
	if(high - low < 2)//this list is too short to add bracket
		return true;

	srand(time(NULL));
	int cnt = 0;
	
	while(cnt + low + 2 <= high)
	{
		int value = rand()%10;
		int len = 1;
		if(value < 3)//30% odds to add a left bracket
		{
			mBracket[cnt+ low][0]++;
			len = rand()% (high - cnt - low -1) + 2;
			mBracket[cnt + low + len][1]++;
			generateBracketSequence(cnt + low + 1 , cnt + low + len);
		}
		cnt += len;
	}
	return true;
}


bool
AOSTorture::printSQLCase()
{
	
	for(int i = 0; i< mBracket[0][0]; i++)
		std::cout<<"(";
	int cnt = 1; 
	std::cout<<"select * \n from ";
	while(cnt <= mNumTables)
	{
		std::cout<<"t"<<mTableList[cnt]<<" ";
		if(mBracket[cnt][0]== 0 && mBracket[cnt][1] == 0)
		{
			std::cout<<mJoinList[mJoinSequence[cnt]]<<" ";
		}
		if(mBracket[cnt][1] != 0)
		{	
			for(int i = 0; i< mBracket[0][1]; i++)
				std::cout<<")";
			if(mJoinSequence[cnt] != -1)
			{   
				std::cout<<mJoinList[mJoinSequence[cnt]]<<" ";
		    }
		}
		if(mBracket[cnt][0] != 0)
		{
			if( mJoinSequence[cnt] != -1)
			{
				std::cout<<mJoinList[mJoinSequence[cnt]]<<" ";
			}
			for(int i = 0; i< mBracket[cnt][0]; i++)
				std::cout<<"(";
		}
		cnt++;
	}
	std::cout<<std::endl;
	return true;
}


AOSTorture::~AOSTorture()
{
    //dtor
}
