////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemMtr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/MemMtr.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <string.h>


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Thread/Mutex.h"
#include "Util1/Time.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "NMS/Nms.h"

static OmnMutex		sgNewLock;
static OmnMutex		sgDeleteLock;
static int			sgLocId = 0;
static int 			sgSnapshotId = 1;
static bool 		sgInitStat = false;

OmnSingletonImpl(OmnMemMtrSingleton,
				 OmnMemMtr,
				 OmnMemMtrSelf,
				"OmnMemMtr");


int64_t			OmnMemMtr:: mLastWriteTick = 0;
int				OmnMemMtr::mTotalNew = 0;	
int				OmnMemMtr::mTotalNewArray = 0;	
int				OmnMemMtr::mTotalDel = 0;	
int				OmnMemMtr::mTotalDelArray = 0;	
Entry			OmnMemMtr::mEntry[OmnMemMtr::eEntrySize][OmnMemMtr::eMaxColumnSize];


OmnMemMtr::OmnMemMtr()
{
	
}

OmnMemMtr::~OmnMemMtr()
{
	if(sgInitStat == false)
    {
        OmnMemMtr::memInitStat();
    }
}

void OmnMemMtr::memInitStat()
{
	for (int i=0; i<eEntrySize; i++)
    {
        for (int j=0; j<eMaxColumnSize; j++)
        {
            mEntry[i][j].mLocId 	= i*eMaxColumnSize + j;
			mEntry[i][j].mNewNum 	= 0;
			mEntry[i][j].mDelNum 	= 0;	
			mEntry[i][j].mIsGood    = false;
        }
    }
    sgLocId 			= eEntrySize * eMaxColumnSize;
	sgInitStat			= true;
}

int
OmnMemMtr::memCreated(const char 	*file, 
					  const int 	line,
					  const size_t 	size)
{
	if(sgInitStat == false)
	{
		OmnMemMtr::memInitStat();
	}
	mTotalNew ++;
	
	int locId = getLocId(file, line,size);

	if (locId < 0 || locId >= sgLocId)
	{
		std::cout<< "81++++ memCreated() Invalid local ID" << std::endl;
		return 0;
	}
	return locId;
}


int
OmnMemMtr::getLocId(const char 		*file, 
					const int 		line,
					const size_t 	size)
{
	int key = line%eEntrySize;
	int index = 0;

	sgNewLock.lock();
	for(index = 0; index < eMaxColumnSize; index++)
	{
		if(mEntry[key][index].mIsGood == false)
		{
			break;
		}
		if((mEntry[key][index].mLine != (uint)line)||
		   (mEntry[key][index].mSize != size))
			
		{
			continue;
        }
		if(strcmp(mEntry[key][index].mFile ,file) == 0)
		{
			mEntry[key][index].mNewNum++;
			sgNewLock.unlock();
			return mEntry[key][index].mLocId;
		}
    }
	/*alloc a new local id*/
	if(index < eMaxColumnSize)
	{
		strcpy(mEntry[key][index].mFile,file);
		mEntry[key][index].mLine   = line;
		mEntry[key][index].mSize   = size;
		mEntry[key][index].mIsGood = true;
	}
	else 
	{
		key   = 0;
		index = 0;
	}
	mEntry[key][index].mNewNum++;
	sgNewLock.unlock();
	return mEntry[key][index].mLocId;
}

	
void		
OmnMemMtr::memDeleted(const char * file, 
					  const int line,
					  const int locId)
{
	
	int tmp,row, column;
	
	if(sgInitStat == false)
	{
		std::cout << "82++++ Not contral delete " << line <<":" << locId << std::endl;
		OmnMemMtr::memInitStat();
	}
	mTotalDel++;
	
	tmp = locId;
	if (tmp < 0 || tmp >= sgLocId)
	{
		OmnAlarm << "Invalid line number" << enderr;
		tmp = 0;
	}
	column  =  tmp % eMaxColumnSize;
	row 	=  tmp / eMaxColumnSize;

	sgDeleteLock.lock();
	mEntry[row][column].mDelNum++;
	sgDeleteLock.unlock();
}



bool

OmnMemMtr::start()
{
	OmnNmsSelf->addHouseKeepObj(this);
	return true;
}



void		
OmnMemMtr::procHouseKeeping(const int64_t &tick)
{

	if (!OmnNms::isPrimary())
	{
		return ;
	}
	if ((tick - mLastWriteTick) < eWriteFreq)
	{
		return;
	}
	mLastWriteTick = tick;
	writeToDb();
}


void
OmnMemMtr::writeToDb()
{
	//
	// It is the time to write to database
	//
	char tempBuf[1000];
	// OmnTrace << "Memory static : SnapshotId =" << sgSnapshotId 
	// 		 <<" New="<< mTotalNew << " NewArray=" << mTotalNewArray
	// 		 <<" Del="<< mTotalDel << " DelArray=" << mTotalDelArray<<endl;
	sgNewLock.lock();
	sgDeleteLock.lock();
	
	for (int i=0; i<eEntrySize; i++)
    {
        for (int j=0; j<eMaxColumnSize; j++)
        {
			if(mEntry[i][j].mIsGood == true)
			{
				int diff = mEntry[i][j].mNewNum - mEntry[i][j].mDelNum;
				if((mEntry[i][j].mNewNum <= 2)||
				   (diff == 0))
				{
					continue;
				}

				sprintf(tempBuf,
						"%5d  %6d  %6d %6d  %6d %4d  %d:%s",
						sgSnapshotId,
						mEntry[i][j].mLocId,
						diff,
						mEntry[i][j].mNewNum,
						mEntry[i][j].mDelNum,
						mEntry[i][j].mSize,
						mEntry[i][j].mLine,
						mEntry[i][j].mFile);
				OmnTrace << tempBuf<<endl;
			}
        }


    }
    mEntry[0][0].mIsGood = true;
	
	sgSnapshotId++;
	sgNewLock.unlock();
	sgDeleteLock.unlock();
}
