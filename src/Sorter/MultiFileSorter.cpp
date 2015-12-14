////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/27/2012 Created by Ice Yu
////////////////////////////////////////////////////////////////////////////
#include "Sorter/MultiFileSorter.h"

#include "DataSort/MultiFileCBReq.h"
#include "DataSort/DataSortMgr.h"
#include "Debug/Debug.h"
#include "Sorter/MergeSorter.h"
#include "Sorter/MergeSorterMgr.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Thread/Sem.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "WordMgr/WordIdHash.h"
#include "Porting/Sleep.h"

#include <dirent.h>
#include <math.h>
#include <bitset>

AosMultiFileSorter::AosMultiFileSorter(const bool regflag)
:
mMinSize(0),
mMaxSize(0),
mCellSize(0),
mMaxRate(0),
mTotalMemory(0),
mMaxTotalMemory(0),
mTotalFinished(0),
mTotalMergeSort(0),
mTotalSort(0),
mTotalReadSize(0),
mSeqno(0),
mCrtSeqno(0),
mDiskError(false)
{
}


AosMultiFileSorter::AosMultiFileSorter(
		const int64_t minsize,
		const int64_t maxsize,
		const int64_t buffsize,
		const AosDataColOpr::E opr,
		AosCompareFun * cmp_raw,
		const vector<AosNetFileObjPtr> &files,
		const AosRundataPtr &rdata)
:
mSem(OmnNew OmnSem(0)),
mFinishSem(OmnNew OmnSem(0)),
mLock(OmnNew OmnMutex()),
mDataLock(OmnNew OmnMutex()),
mOpr(opr),
//mCmp(cmp),
mCmpRaw(cmp_raw),
mMinSize(minsize),
mMaxSize(maxsize),
mCellSize(0),
mMaxRate(0),
mTotalMemory(0),
mMaxTotalMemory(maxsize * 2),
mTotalFinished(0),
mTotalMergeSort(0),
mTotalSort(0),
mTotalReadSize(0),
mMemCondVar(OmnNew OmnCondVar()),
mCondVar(OmnNew OmnCondVar()),
mDataCondVar(OmnNew OmnCondVar()),
mSeqno(0),
mCrtSeqno(0),
mIsFinished(false),
mMergedTotal(0),
mDiskError(false)
{
	int buffSize = buffsize;
	buffSize = (buffSize/mCmpRaw->size) * mCmpRaw->size;
	
	mScanner = AosDataScannerObj::createMultiFileScannerStatic(rdata);
	aos_assert(mScanner);

	mScanner->setFiles(files);
	AosMultiFileSorterPtr thisptr(this, false);
	mScanner->setActionsCaller(thisptr);

	aos_assert(files.size()>0);
	if (files.size() == 0)
	{
		OmnAlarm << "file num is 0" << enderr;
	}
	for(u64 i=0; i<files.size(); i++)
	{
		AosMultiFileSorterPtr thisptr(this, false);
		OmnSPtr<DataSource> source = 
			OmnNew DataSource(files[i], buffSize, mCmpRaw, thisptr, i);
		mDataSources.push_back(source);
	}

	if (mDataSources.empty()) return; 

	mCellSize = mMaxSize/mDataSources.size()/10;
	mCellSize = (mCellSize/mCmpRaw->size) * mCmpRaw->size;
	int cellsize = mCellSize;

	mMaxRate = 0;
	while(1)
	{
		mMaxRate+=10;
		cellsize = cellsize << 1;
		if (cellsize > buffSize)
			break;
	}
}


AosMultiFileSorter::~AosMultiFileSorter()
{
	//OmnScreen << "~~~ AosMultiFileSorter Destructor " << endl;
}

bool
AosMultiFileSorter::sanitycheck(
		DataSource *ds,
		AosCompareFun * comp,
		char *crt,
		const int length)
{
return true;
	int record_len = comp->size;
	int entries = length/record_len;
	for (int i=0; i<entries; ++i)
	{
		if (crt[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}

		// 1. check file is sorted
		if (i>0)
		{
			char *precrt = ds->move(crt, (-record_len));
			aos_assert_r(comp->cmp(crt, precrt) >= 0, false);
		}

		// 2. check the entry whether is right
//		bool rslt = checkEntry(crt);
//		aos_assert_r(rslt, false);
		
		//crt += record_len;
		crt = ds->move(crt, record_len);
	}
	return true;
}

bool
AosMultiFileSorter::sanitycheck(
		AosCompareFun * comp,
		char *crt, 
		const int length)
{
return true;
	int record_len = comp->size;
	int entries = length/record_len;
	for (int i=0; i<entries; ++i)
	{
		if (crt[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}

		// 1. check file is sorted
		if (i>0)
		{
			aos_assert_r(comp->cmp(crt, crt-record_len) >= 0, false);
		}

		// 2. check the entry whether is right
//		bool rslt = checkEntry(crt);
//		aos_assert_r(rslt, false);
		
		crt += record_len;
	}
	return true;
}

bool
AosMultiFileSorter::checkEntry(const char* entry)
{
return true;
	// this function is to check the file entries whether is right
	// 1. check the hashkey whether is right
	// 2. check the entry whether is unique
/*	
	if (!entry) return true;
	AosStrSplit split;
	bool finished;
	OmnString pair[3];                                      
	split.splitStr(entry,"_", pair, 3, finished);

	// 1. check haskkey
	u32 raw_hashkey = atoi(pair[0].data());
	int len = pair[2].length();
	OmnString word = pair[1];
	word << "_" << pair[2];
	u32 new_hashkey = AosGetHashKey(word.data(), len+2);
	new_hashkey %= 100000;
	aos_assert_r(new_hashkey == raw_hashkey, false);
	return true;
	*/
}

static int totalbatch = 0;
static int totaltries =0;

bool
AosMultiFileSorter::sort(AosRundata *rdata)
{
	// This function assumes that there are a number of files that
	// were already sorted. It retrieves one block each time from 
	// all the files, sort the contents, and add the block into 
	// a queue. It repeats this process until all the files are
	// processed. 
	
	// Note that when there are too many processed blocks in the
	// cache, it will stop processing until some of the blocks 
	// are retrieved by its consumer. 
	//clock_t c1 = clock();
	//time_t t1 = time(NULL);
	u64 st = OmnGetTimestamp();
	for(u32 i=0; i<mDataSources.size(); i++)
	{
		aos_assert_r(mDataSources[i], false);
		int64_t total_read = mDataSources[i]->getTotalRead();
		mDataSources[i]->loadData(total_read);
	}

	vector<OmnSPtr<Element> > oldElements(mDataSources.size());
	vector<OmnSPtr<Element> > newElements(mDataSources.size());
	while(1)
	{
		mLock->lock();
		if (mTotalMemory > mMaxTotalMemory)
		{
//			OmnScreen << " mTotalMemory : " << mTotalMemory << "  mMaxTotalMemory : " << mMaxTotalMemory << endl;
			mMemCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		mLock->unlock();
		
		int num_elements = getElements(oldElements, newElements);
		if (!num_elements) break;
		oldElements.clear();
		oldElements.resize(newElements.size());
		bool rslt = getOneBatch(oldElements, newElements, rdata);
		aos_assert_r(!oldElements.empty(), false);
		aos_assert_r(rslt, false);

		rslt = mergeSort(oldElements, rdata);
		aos_assert_r(rslt, false);

/*		
static int ii=0;
if (++ii == 1)
{
	OmnScreen << "stat : " << totalbatch << "/" << totaltries << endl;
	OmnScreen << "time :" << time(NULL)-t1 << ", clock: " << clock()-c1 << endl;
	ii=0;
}
*/
	}

	for (int i=0; i<mSeqno; i++)
	{
		mFinishSem->wait();
	}	

	mDataLock->lock();
	mIsFinished = true;
	mDataCondVar->signal();
	mDataLock->unlock();

	OmnScreen << "AosMultiFileSorter All finished, spend : " << AosTimestampToHumanRead(OmnGetTimestamp() - st) << endl;

	return true;
}


bool
AosMultiFileSorter::getOneBatch(
		vector<OmnSPtr<Element> > &oldElements, 
		vector<OmnSPtr<Element> > &newElements, 
		const AosRundataPtr &rdata)
{
	double rate = 10;
	bool success = false;

	OmnSPtr<Element> seed;
	int max = 0;
	OmnSPtr<Element> maxSeed = 0;
	int min = mMaxSize;
	OmnSPtr<Element> minSeed = 0;

	int total;
	int trymin = 0;
	char *bound = calculateBoundary(newElements, rate);
//OmnScreen << "Get One batch!  bound: " << bound << "  =================" << endl;
totalbatch++;
	do
	{
		do
		{
totaltries++;
//OmnScreen << "New try :" << bound << endl;
			// When the control comes to this point, a seed has been selected
			// and a boundary has been determined. We need to use the bound
			// to check all other files.
			success = true;
			total = 0;
			aos_assert_r(bound, false);

			bitset<eMaxFileNum> unKnowIdx;
			for (size_t i=0; i<newElements.size(); i++)
			{
				if(!newElements[i]) continue;
				int size = newElements[i]->getSize(bound);
//OmnScreen << "======  GetSize1 idx: " << newElements[i]->getEventId() << ", size: " << size << endl;
				if (size == (int)DataSource::eNull)
				{
					//this data source doesn't have matched data.
					continue;
				}
				if (size == (int)DataSource::eUnReady)
				{
					//the data source is reading. This file shall be 
					//skipped for this round.
					unKnowIdx.set(newElements[i]->getEventId());
					continue;
				}
				
				aos_assert_r(size > 0, false);

				total += size;
				if (size == (int)DataSource::eNaN)
				{
					// This means the largest entry in memory for the file is smaller
					// than 'bound' and there are enough cached entries for the file.
					total = mMaxSize + 1;  
				}

				if (size > max)
				{
					max = size;
					maxSeed = newElements[i];
				}
				if (size < min)
				{
					min = size;
					minSeed = newElements[i];
				}

				if (total > mMaxSize)
				{
					//total data size is too big
					success = false;		
					max = 0;
					min =mMaxSize;
					if (seed == maxSeed)
					{
						if (rate <= 2)
						{
							//Jozhi 2014-04-10
							//OmnAlarm << "" << enderr;
							//OmnScreen << "may be an error" << endl;
							rate = rate/2;
						}
						else
						{
							rate = rate - 2;
						}
					}
					seed = maxSeed;

					bound = calculateBoundary(newElements, seed, rate);
//OmnScreen << " idx: " << newElements[i]->getEventId() << "  recalculate Boundary : " << bound << endl;
					oldElements.clear();
					oldElements.resize(newElements.size());
					unKnowIdx.reset();
					break;
				}
				int64_t el_totalr = newElements[i]->getTotalRead();
				int64_t el_totals = newElements[i]->getTotalSort();
				int64_t el_remain = newElements[i]->getRemain();
				if (el_totals + el_remain + size < el_totalr)
				{
					OmnAlarm << " error " << enderr;
				}
				
				OmnSPtr<Element> element = OmnNew Element(*newElements[i]);
				oldElements[i] = element;
				sanitycheck(element->getDataSource().getPtr(), 
						mCmpRaw, element->first(), element->size());
			}
//OmnScreen << "Unkown elements " << unKnowIdx.count() << endl;
			while (unKnowIdx.count())
			{
				mSem->wait();

				mLock->lock();
				size_t eventid = mEvents.front();
				mEvents.pop();
				mLock->unlock();
				//OmnScreen << "wait1 idx " << eventid << endl;

				if (!unKnowIdx.test(eventid))
				{
					//OmnScreen << "wait1 invalid idx1 " << eventid << endl;
					continue;
				}

				aos_assert_r(eventid<newElements.size(), false);
				int size = newElements[eventid]->getSize(bound);

				//OmnScreen << "======  GetSize2 idx: " << eventid << ", size: " << size << endl;//  << " begin : " << (newElements[eventid]->getDataSource())->getBegin() << " end : " << (newElements[eventid]->getDataSource())->getBegin() << endl;
				//aos_assert_r(size!=(int)DataSource::eUnReady, false);

				if (size == (int)DataSource::eUnReady)
				{
					//OmnScreen << "wait1 invalid idx2 " << eventid << "  bound: " << bound << endl;
					continue;
				}

				if (size == (int)DataSource::eNull)
				{
					//this data source doesn't have matched data.
					unKnowIdx.set(eventid, 0);
					continue;
				}

				total += size;
				if (size == (int)DataSource::eNaN)
				{
					// This means the largest entry in memory for the file is smaller
					// than 'bound'. 
					total = mMaxSize + 1;
				}

				if (size > max)
				{
					max = size;
					maxSeed = newElements[eventid];
				}
				if (size < min)
				{
					min = size;
					minSeed = newElements[eventid];
				}

				if (total > mMaxSize)
				{
					//total data size is too big
					success = false;		
					max = 0;
					min =mMaxSize;
					if (seed == maxSeed)
					{
						if (rate <= 2)
						{
							//OmnAlarm << "" << enderr;
							rate = rate/2;
						}
						else
						{
							rate = rate - 2;
						}
					}
					seed = maxSeed;
					bound = calculateBoundary(newElements, seed, rate);
					//OmnScreen << " idx: " << eventid << "  recalculate Boundary : " << bound << endl;
					oldElements.clear();
					oldElements. resize(newElements.size());
					break;
				}
				OmnSPtr<Element> element = OmnNew Element(*newElements[eventid]);
				oldElements[eventid] = element;
				unKnowIdx.set(eventid, 0);
				sanitycheck(element->getDataSource().getPtr(), 
						mCmpRaw, element->first(), element->size());
			}
		}
		while(!success);
		if (total < mMinSize && rate<mMaxRate && trymin < 10)
		{
			//total data size is too small 
			trymin++;
			success = false;		
			max = 0;
			min =mMaxSize;
			if (seed == minSeed)
				rate = rate + 20;
			seed = minSeed;
			bound = calculateBoundary(newElements, seed, rate);
//OmnScreen << " recalculate Boundary : " << bound << endl;
			oldElements.clear();
			oldElements. resize(newElements.size());
		}
	}
	while(!success);

	return true;
}

char*
AosMultiFileSorter::calculateBoundary(
		const OmnSPtr<Element> &element, 
		double rate)
{
	aos_assert_r(element, 0);
	int cellSize = (int)mCellSize*rate;
	cellSize = cellSize/mCmpRaw->size*mCmpRaw->size;
	aos_assert_r(cellSize>mCmpRaw->size, 0);
	char* bound = element->getData(cellSize);
checkEntry(bound);
	return bound;
}


char*
AosMultiFileSorter::calculateBoundary(
		vector<OmnSPtr<Element> > &newElements,
		const OmnSPtr<Element> &element, 
		double rate)
{
	int cellSize = (int)mCellSize*rate;
	cellSize = cellSize/mCmpRaw->size*mCmpRaw->size;
	aos_assert_r(cellSize>mCmpRaw->size, 0);
	char *bound = element->getData(cellSize);

	while (bound == 0)
	{
		mSem->wait();

		mLock->lock();
		size_t eventid = mEvents.front();
		mEvents.pop();
		mLock->unlock();

//OmnScreen << "wait2 idx " << eventid << endl;
		if (element != newElements[eventid])
		{
//OmnScreen << "wait2 invalid idx1 " << eventid << endl;
			continue;
		}
		//mLock->unlock();

		bound = calculateBoundary(newElements[eventid], rate);
/*		if (!bound)
		{
OmnScreen << "wait2 invalid idx2 " << eventid << endl;
		}
		else
		{
OmnScreen << " calculateBoundary elements   bound : " << bound << "  eventid : " << eventid << endl;
		}
*/
	}

	return bound;
}


char*
AosMultiFileSorter::calculateBoundary(vector<OmnSPtr<Element> > &newElements, double rate)
{
	char* bound = NULL;
	for (size_t i=0; i<newElements.size(); i++)
	{
		if(!newElements[i]) continue;
		bound = calculateBoundary(newElements[i], rate);
		if (bound != 0)
		{
			break;
		}
		// 'bound == 0' if and only if the file is reading data. 
		// When this happens, it needs to move on to get the 
		// next data source as the new seed.
		// When it comes to this point, it means the some of the files
		// is reading.
	}
	
	while (bound == 0)
	{
		mSem->wait();

		mLock->lock();
		size_t eventid = mEvents.front();
		mEvents.pop();
		mLock->unlock();
//OmnScreen << "wait idx3 " << eventid << endl;

		aos_assert_r(eventid<newElements.size(), NULL);
		
		if (newElements[eventid])
		{
			bound = calculateBoundary(newElements[eventid], rate);
		}
/*
		if (!bound)
		{
OmnScreen << "wait3 invalid idx " << eventid << endl;
		}
		else
		{
OmnScreen << " calculateBoundary elements   bound : " << bound << "  eventid : " << eventid << endl;
		}
*/
	}

	return bound;
}

bool
AosMultiFileSorter::mergeSort(vector<OmnSPtr<Element> > &elements, const AosRundataPtr &rdata)
{
	int total = 0;
	vector<OmnSPtr<Element> > els;
	int total_size = 0;
	for (size_t i=0; i<elements.size(); i++)
	{
		if (elements[i] && elements[i]->size() > 0)
		{
			total++;
			int elsize = elements[i]->size();	
			aos_assert_r(elsize > 0, false);
			elements[i]->setTotalSort(elsize);
			total_size += elsize;
			els.push_back(elements[i]);
		}
	}
	
	if (total == 0) return true;
	mLock->lock();
	mTotalMemory += total_size;
	mTotalSort += total_size;
	//OmnScreen << " mergeSort mTotalSort : " << mTotalSort << " mTotalReadSize : " << mTotalReadSize << " mTotalFinished : " << mTotalFinished << endl;
	mLock->unlock();
	int level = log2(total)+1;
	vector<OmnSPtr<SortController> >* sortCtrls = OmnNew vector<OmnSPtr<SortController> >;
	AosMultiFileSorterPtr thisptr(this, false);
	int sum = 0;
	for (int i=0; i<level; i++)
	{
		total = (int)(ceil((float)total/2));
		sum += total;
		OmnSPtr<SortController> sc = OmnNew SortController(total, i, mCmpRaw, mOpr, thisptr);
		sortCtrls->push_back(sc);
	}

//OmnScreen << " level : " << level << " ctrl size : " << (*sortCtrls).size() << endl;
	aos_assert_r((*sortCtrls)[0], false);
	aos_assert_r((*sortCtrls)[level-1], false);
	(*sortCtrls)[level-1]->setSeqno(mSeqno++);
//OmnScreen << "@@@@ " << this << " mergesort mSeqno:" << mSeqno << endl;

	vector<OmnSPtr<Element> >::iterator itr = els.begin();		
	for (; itr != els.end();)
	{
		OmnSPtr<Element> one = *itr;	
		++itr;
		OmnSPtr<Element> two = NULL;
		if (itr != els.end())
		{
			two = *itr;
			++itr;
			OmnSPtr<AosMergeSorter<OmnSPtr<Element> > > runner = 
				OmnNew AosMergeSorter<OmnSPtr<Element> >(one, two, mOpr, mCmpRaw , 
						(*sortCtrls)[0], 0, sortCtrls , rdata);
			AosMergeSorterMgr<OmnSPtr<AosMergeSorter<OmnSPtr<Element> > > >::getSelf()->
				addHighPriorityReq(runner, 1, rdata);
		}
		else
		{
			AosBuffPtr buff = OmnNew AosBuff(one->size() AosMemoryCheckerArgs);
			one->copy(buff->data(), one->first(), one->size()); 
			buff->setDataLen(one->size());	
			(*sortCtrls)[0]->mergeSortFinish(buff, 0, sortCtrls, rdata);
			one->moveToNext();
			break;
		}
	}
	return true;
}


int
AosMultiFileSorter::getElements(
		vector<OmnSPtr<Element> > &oldElements,
		vector<OmnSPtr<Element> > &newElements)

{
	OmnSPtr<Element> element;
	int totalsources = 0;

	for (size_t i=0; i<mDataSources.size(); i++)
	{
		if (!mDataSources[i]) continue;
		if (oldElements[i])
		{
			if(oldElements[i]->size() > 0)
			{
				newElements[i] = oldElements[i]->copyForNext();
			}
			else
			{
				newElements[i] = oldElements[i];
			}
		}
		if (!newElements[i])
		{
			newElements[i] = OmnNew Element(mDataSources[i]);
		}
		if( newElements[i]->empty())
		{
//OmnScreen << "########  Finished idx " << i << ", " << newElements.size() << endl;
			//mLock->lock();
			//mTotalReadSize += mDataSources[i]->getTotalRead();
//OmnScreen << "read Size mTotalReadSize : " << mTotalReadSize << " mTotalSort : "<< mTotalSort << " mTotalFinished : " << mTotalFinished << endl;
			//mLock->unlock();
			mDataSources[i] = 0;
			oldElements[i] = 0;
			newElements[i] = 0;
			aos_assert_r(mDataSources.size()==oldElements.size() &&
					mDataSources.size()==newElements.size(), 0);
			continue;
		}
	
		totalsources++;
	}
	return totalsources;
}
		 
void
AosMultiFileSorter::sortFinished(const int seqno, const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	// This function adds the block into the queue. If the queue is full, 
	aos_assert_l(seqno >= mCrtSeqno, mLock);
//OmnScreen << "@@@@ " << this << " sortfinish seqno:" << seqno << endl;
	SortedBuff sb(buff, seqno);
	mDataLock->lock();
	mRequests.push(sb);
	mDataCondVar->signal();
	mDataLock->unlock();
}

AosBuffPtr
AosMultiFileSorter::nextBuff()
{
	mDataLock->lock();
	while (mRequests.size() <= 0)
	{
		if (mDiskError)
		{
			mDataLock->unlock();
			return 0;
		}
		if (mIsFinished)
		{
			mDataLock->unlock();
			return 0;
		}
		else
		{
			mDataCondVar->wait(mDataLock);
		}
	}


	while(1)
	{
		if (mDiskError)
		{
			mDataLock->unlock();
			return 0;
		}
		SortedBuff sb = mRequests.top();
		if (mCrtSeqno == sb.mSeqno)
		{
			mCrtSeqno++;
			mRequests.pop();
			mDataLock->unlock();

			mLock->lock();
			int64_t buffsize = sb.mBuff->dataLen();
			mTotalFinished += buffsize;
			aos_assert_rl(mTotalMemory >= buffsize, mLock, 0);
			mTotalMemory -= buffsize;
			mMemCondVar->signal();
			//OmnScreen << "@@@@ " << this << "  nextbuff seqno:" << sb.mSeqno << endl;
			//OmnScreen << "###### Send to proc size : " << buffsize << " mTotalMemory : " << mTotalMemory << " mTotalFinished : " << mTotalFinished  << " mTotalMergeSort : " << mTotalMergeSort << " mTotalSort : " << mTotalSort << " mTotalReadSize : " << mTotalReadSize << endl;
			mLock->unlock();
			bool rslt = sanitycheck(mCmpRaw, sb.mBuff->data(), sb.mBuff->dataLen());
			aos_assert_r(rslt, 0);
			mFinishSem->post();
			return sb.mBuff;
		}
		else
		{
			mDataCondVar->wait(mDataLock);
		}
	}
	mDataLock->unlock();
	OmnAlarm << "Nerver Come Here" << enderr;
	return 0;
}

void
AosMultiFileSorter::raiseEvent(u64 eventid, const int readsize)
{
	mLock->lock();
	mTotalReadSize += readsize;
	mEvents.push(eventid);
	mLock->unlock();	
	mSem->post();
//OmnScreen << "@@@ post idx " << eventid << endl; 
}

bool
AosMultiFileSorter::reduceCacheSize(const int64_t size)
{
	mLock->lock();
	aos_assert_rl(mTotalMemory >= size, mLock, false);
	mTotalMemory -= size;
	mTotalMergeSort += size;
	mMergedTotal += size;
	mMemCondVar->signal();
	mLock->unlock();
//extern void updateAppendTotal(const int64_t &size);
//updateAppendTotal(-size);
	return true;
}


void
AosMultiFileSorter::clear()
{
	mDataSources.clear();
	mScanner = 0;
	//mSorter = 0;
	//OmnScreen << "clear multi file sorter" << endl;
}


void
AosMultiFileSorter::addRequest(
		const u64 reqid,
		const int64_t readsize)
{
	aos_assert(mDataSources.size() > reqid);
	mScanner->addRequest(reqid, readsize);
//OmnScreen << "@@@ " << this <<" reqid : " << reqid << " addRequest " << endl;
}


void
AosMultiFileSorter::callBack(
		const u64 &reqId, 
		const int64_t &expected_size, 
		const bool &finished)
{
	if (mScanner->getDiskError())
	{
		mDiskError = true;
		mDataLock->lock();
		mDataCondVar->signal();
		mDataLock->unlock();
		OmnScreen << " MultiFileSorter diskerror 1111111 " << endl;
		return;
	}
	aos_assert(mDataSources.size() > reqId);
	AosMultiFileSorterPtr thisptr(this, false);
	AosDataSortReqPtr req = OmnNew AosMultiFileCBReq(thisptr, reqId, expected_size, finished);
	AosDataSortMgr::getSelf()->addCallBackReq(req);
//OmnScreen << "@@@ " << this << " reqid : " << reqId << " callBack " << endl;
}


bool
AosMultiFileSorter::procReq(
		const u64 reqId, 
		const int64_t expected_size, 
		const bool finished)
{
	aos_assert_r(mDataSources.size() > reqId, false);
	aos_assert_r(mDataSources[reqId], false);

	AosBuffPtr buff = mScanner->getNextBlock(reqId, expected_size);
	aos_assert_r(buff, false);
	
	bool rslt = mDataSources[reqId]->callBack(buff, finished);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosMultiFileSorter::SortController::reduceCacheSize(const int64_t size, const bool merged)
{
	mCaller->reduceCacheSize(size);
	return true;
}


void
AosMultiFileSorter::SortController::mergeSortFinish(const void* sorter, const int level)
{
	if (level != 0) return;
	AosMergeSorter<OmnSPtr<Element> > *runner = (AosMergeSorter<OmnSPtr<Element> >  *)sorter;
	aos_assert(runner);
	if (runner->getHls()) runner->getHls()->moveToNext();
	if (runner->getRls()) runner->getRls()->moveToNext();
}


bool
AosMultiFileSorter::SortController::mergeSortFinish(
		const AosBuffPtr &buff, 
		const int level, 
		void *data, 
		const AosRundataPtr &rdata)
{
	mCaller->sanitycheck(mCmpRaw, buff->data(), buff->dataLen());
	AosSortCallerPtr thisptr(this, false);
	aos_assert_r(level == mLevel, false);
	aos_assert_r(data, false);
	vector<OmnSPtr<SortController> >* sortCtrls = (vector<OmnSPtr<SortController> >*)data;


//OmnScreen << "2222  level : " << level << " ctrl size : " << (*sortCtrls).size() << endl;
	if (mLevel == (int)sortCtrls->size()-1)
	{
		aos_assert_r(mTotal == 1, false);
		//it's the root 
		OmnDelete sortCtrls;
		mCaller->sortFinished(mSeqno, buff, rdata);
		return true;
	}

	aos_assert_r((*sortCtrls)[level+1], false);

	mLock->lock();
	if (mOne)
	{
		AosBuffArrayPtr array1 = OmnNew AosBuffArray(buff, mCmpRaw, false);
		AosBuffArrayPtr array2 = OmnNew AosBuffArray(mOne, mCmpRaw, false);
		array1->setSorted(true);
		array2->setSorted(true);
		OmnSPtr<AosMergeSorter<AosBuffArrayPtr> > runner = 
			OmnNew AosMergeSorter<AosBuffArrayPtr>(array1, array2, 
					mOpr, mCmpRaw, (*sortCtrls)[level+1], level+1, data, rdata);
	/*	mOne = 0;
		mFinishedTotal++;
		mLock->unlock();
		bool rslt = runner->mergeBlocks();
		aos_assert_r(rslt, false);
		return rslt;	
		*/
		AosMergeSorterMgr<OmnSPtr<AosMergeSorter<AosBuffArrayPtr> > >::getSelf()->
			addReq(runner, rdata);
		mOne = 0;
	}
	else
	{
		if (mFinishedTotal==mTotal-1)
		{
			mFinishedTotal++;
			mLock->unlock();
			(*sortCtrls)[level+1]->mergeSortFinish(buff, level+1, data, rdata);
			return true;
		}
		else
		{
			mOne = buff;
		}

	}
	mFinishedTotal++;

	mLock->unlock();

	return true;
}


AosMultiFileSorter::DataSource::DataSource(
		const AosNetFileObjPtr &file, 
		const int buffsize,
		AosCompareFun * cmp,
		const AosMultiFileSorterPtr &caller,
		const u64 eventid)
:
mFile(file),
mLock(OmnNew OmnMutex()),
mBuff(OmnNew AosBuff(buffsize  AosMemoryCheckerArgs)),
mCaller(caller),
mEventId(eventid),
mCmpRaw(cmp),
mEmpty(false),
mReadingFlag(-1),
mTotalRead(0),
mTotalSort(0),
mBuffLen(mBuff->buffLen()),
mBegin(mBuff->data()),
mEnd(mBuff->data())
{
	mLock->lock();
	mPoss.push_back(mBegin);
	mLock->unlock();
	//loadData(mTotalRead);
}


AosMultiFileSorter::DataSource::~DataSource()
{
}


char*
AosMultiFileSorter::DataSource::getData(char *pos, int cellsize)
{
	aos_assert_r(pos>=mBuff->data() && pos < mBuff->data()+mBuffLen, NULL);
	//if (!mLock->trylock()) return 0;
	//mLock->lock();
	if (cellsize > mBuffLen - 3*mCmpRaw->size)
	{
		cellsize = mBuffLen - 3*mCmpRaw->size;
	}
	
	bool empty = mEmpty;
	int size = distance(pos, mEnd);

	if (size == 0) return 0;

	//felicia, 2012/11/19
	if (empty && size == mCmpRaw->size)
	{
//OmnScreen << " last entry : " << pos << "  idx: " << mEventId << endl;
	//	mLock->unlock();
		return pos;	
	}
	
	if (size < cellsize)
	{
		if (!empty && size < mBuffLen/4)
		{
//OmnScreen << "GetData1" << mEventId << endl;
			return 0;
		}
		if (size == 0 && empty)
		{
			return 0;
		}
	}
	
	if (cellsize > size - 2*mCmpRaw->size)
	{
		if (empty)
		{
			cellsize = size - mCmpRaw->size;
		}
		else
		{
			cellsize = size - 2*mCmpRaw->size;
		}
	}

	if (size<=0)
	{
		aos_assert_r(cellsize == 0, NULL);
		aos_assert_r(empty, NULL);
		//cellsize = 1;
		cellsize = mCmpRaw->size;
	}	
	
	char *data = move(pos, cellsize);
	aos_assert_r(data>=mBuff->data() && data<mBuff->data()+mBuffLen, NULL);
//OmnScreen << "idx: " << mEventId << " pos: " << pos << "  cellsize: " << cellsize << " size: " << size << " mEmpty: " << mEmpty << endl;
	return data;
}

int
AosMultiFileSorter::DataSource::getSize(char* pos, char* key)
{
	// This function starts from 'pos', finds the first entry whose
	// value is no less then 'key'. 
	// 1. The entry is found and this function returns the size 
	//    between 'pos' and the first entry whose value is no less 
	//    than 'key'.
	// 2. The last entry (in memory) is still no less than 'key'. 
	//    It needs to check whether the buff is full. If it is full, 
	//    need to return eNaN. If it is not full, it needs to trigger
	//    reading and returns eUnReady.
	// 3. If the first entry is bigger than 'key', returns eNull.
	aos_assert_r(pos>=mBuff->data() && pos < mBuff->data()+mBuffLen, false);

	int half;
	bool empty = mEmpty;
	char *begin = mBegin;
	char *end = mEnd;
	char *last = move(end, -mCmpRaw->size);
	int size = distance(pos, end);
	aos_assert_r(size >= 0, 0);
	
	if (size == 0 && empty)
	{
//OmnScreen << " getSize empty " << endl;
		return (int)eNull;	
	}

	if (begin == end)
	{
//OmnScreen << " getSize unready " << endl;
		return eUnReady;
	}

	char *mid = 0;
	char* low = pos;
	int len = distance(pos, last);

	// This loop uses the binary search to find the first entry whose
	// value is bigger than 'key'.
	while(len)
	{
		half = (len/mCmpRaw->size)/2;
		mid = low;
		mid = move(mid, half*mCmpRaw->size);
		if (mCmpRaw->cmp(mid, key) <= 0)
		{
			low = mid;
			low = move(low, mCmpRaw->size);
			len -= (half+1)*(mCmpRaw->size);
		}
		else
		{
			len = half*mCmpRaw->size;
		}
	}
	aos_assert_r(low>=mBuff->data() && low < mBuff->data()+mBuffLen, false);

	if (low == last)
	{
		if (mCmpRaw->cmp(low, key) <= 0)
		{
			if (!empty && size < mBuffLen/4)
			{
				// The cache is not full. Return eUnReady. The caller should 
				// trigger reading.
//OmnScreen << "find2 " << mEventId << ", " << size<< ":" << mBuffLen << endl;
				return (int)eUnReady;
			}
			// The cache is full. It returns eNaN. the caller should use this
			// file as the seed. 
			if (empty)
			{
//OmnScreen << " find mEmpty : " << mEventId << " pos: " << pos << " low: " << low << endl;
				return distance(pos, low) + mCmpRaw->size;
			}
			else
			{
//OmnScreen << " find eNaN : " << mEventId << " pos: " << pos << " low: " << low << endl;
				return (int)eNaN;
			}
		}
	}

	aos_assert_r(mCmpRaw->cmp(low, key) > 0, false); 
	aos_assert_r(low == pos || mCmpRaw->cmp(move(low,-mCmpRaw->size), key) <= 0, false); 
	
	int dist = distance(pos, low);
	aos_assert_r(dist < mBuffLen, false);
//if (dist == 0)
//{
//	OmnScreen  << "find dist : " << dist << " id : " << mEventId << " size: " << size << " pos: " << pos << " low: " << low << " mEnd: " << mEnd << endl;
//}
	return dist;
}

bool
AosMultiFileSorter::DataSource::loadData(int64_t offset)
{
	if (mReadingFlag >= offset)
	{
		return true;
	}

	mReadingFlag = offset;
	int len = mBuffLen - distance(mBegin, mEnd) - mCmpRaw->size;
	mCaller->addRequest(mEventId, len);
	return true;
}


void
AosMultiFileSorter::DataSource::getMemory(
		const int64_t size,
		vector<AosBuffPtr> &buffs)
{
	//Read file to buff is async call. firt call getMemory , secondory call 
	//dataRead or readError . the two phase is atomic operation. so we should
	//locked on this function, and unlock on the secondory function.
	char *to = mEnd + size;
	int64_t len = mBuff->data()+mBuffLen - to;
	if (len >= 0)
	{
		aos_assert(size>0);
//OmnScreen << "@@@ getMemory1 len : " << len << " size : " << size << endl;
		buffs.push_back(OmnNew AosBuff(mEnd, size, 0 AosMemoryCheckerArgs));
	}
	else
	{
		len = abs(len);
		aos_assert(size-len>0);
		aos_assert(len>0);

		// Chen Ding, 2015/05/25
		buffs.push_back(OmnNew AosBuff(mEnd, size-len, 0 AosMemoryCheckerArgs));
		buffs.push_back(OmnNew AosBuff(mBuff->data(), len, 0 AosMemoryCheckerArgs));
//OmnScreen << "@@@ getMemory2 len : " << len << " size1 : " << size-len << " size2 : " << len << endl;
	}
}

bool
AosMultiFileSorter::DataSource::callBack(const AosBuffPtr &buff, const bool finished)
{
	int64_t buffsize = buff->dataLen();	
	if (buffsize == 0 && finished)
	{
		mEmpty = true;
		mCaller->raiseEvent(mEventId, 0);
		return true;
	}

	vector<AosBuffPtr> buffs;
	getMemory(buffsize, buffs);
	aos_assert_r(buffs.size() > 0, false);

	int64_t pos = 0;
	for(u32 i=0; i<buffs.size(); i++)
	{
		int64_t bufflen = buffs[i]->buffLen();
		if (bufflen > buffsize-pos)
		{
			bufflen = buffsize - pos;
		}
		if (bufflen <= 0)
		{
			break;
		}
		memcpy(buffs[i]->data(), buff->data()+pos, bufflen);
		buffs[i]->setDataLen(bufflen);
		pos += bufflen;
//OmnScreen << "@@@ buffs[" << i << "] bufflen : " << bufflen << endl;
	}
	dataRead(buffs, finished);
	return true;
}


bool
AosMultiFileSorter::DataSource::dataRead(vector<AosBuffPtr> &buffs, const bool finished) 
{
	int totalreadsize = 0;
	bool empty = false;
	for (size_t i=0; i<buffs.size(); i++)
	{
		int readsize = buffs[i]->dataLen();
		aos_assert_r(readsize >= 0, false);
		if (buffs[i]->buffLen() != readsize)
		{
			// The file finishes.
			//mEmpty = true;
			empty = true;
		}
		totalreadsize += readsize;
	}
	
	mEnd = move(mEnd, totalreadsize);
	if (empty || finished) mEmpty = true;
	mTotalRead += totalreadsize;

	AosMultiFileSorter::sanitycheck(this, mCmpRaw, mBegin, distance(mBegin, mEnd));

//OmnScreen << "@@@ read idx " << mEventId << endl;// << ", mBegin:" << mBegin << "  mLast:" << mEnd << "  distance:"  << distance(mBegin, mEnd) << endl;

	mCaller->raiseEvent(mEventId, totalreadsize);

	return true;
}


bool
AosMultiFileSorter::DataSource::readError(
		const u64 &read_id,
		const OmnString &errmsg,
		const AosRundataPtr &rdata)
{
OmnAlarm << "Invalid read!" << enderr;
OmnScreen << "read invalid idx " << mEventId << endl;// << ", " << distance(mBegin, mEnd) << endl;
	mCaller->raiseEvent(mEventId, 0);
	mEmpty = true;
	mLock->unlock();
	return true;
}


int 
AosMultiFileSorter::DataSource::distance(char *from, char *to)
{
	aos_assert_r(from>=mBuff->data() && from < mBuff->data()+mBuffLen, -1);
	aos_assert_r(to>=mBuff->data() && to<= mBuff->data()+mBuffLen, -1);
	int len = abs(to-from);
	if (from > to)
	{
		len = mBuffLen-len;
	}
	aos_assert_r(len<=mBuffLen, -1);
	return len;
}


char * 
AosMultiFileSorter::DataSource::move(char *from, int length)
{
	aos_assert_r(length<=mBuffLen, NULL);
	aos_assert_r(from>=mBuff->data() && from < mBuff->data()+mBuffLen, NULL);
	char *to = from + length;
	if (length < 0)
	{
		// move forward
		int len = mBuff->data() - to;
		if (len > 0)
		{
			aos_assert_r(len%mCmpRaw->size == 0, NULL);
			to = mBuff->data() + mBuffLen - len;
		}
	}
	else
	{
		// move back
		int len = to - (mBuff->data()+mBuffLen);
		if (len >= 0)
		{
			aos_assert_r(len%mCmpRaw->size == 0, NULL);
			to = mBuff->data()+len;
		}
	}
	aos_assert_r(to>=mBuff->data() && to<=mBuff->data()+mBuffLen, NULL);
	return to;
}


void
AosMultiFileSorter::DataSource::copy(char *to, char *from, int length)
{
	aos_assert(length < mBuffLen);
	aos_assert(from>=mBuff->data() && from < mBuff->data()+mBuffLen);
	char *last = from+length;
	int len = mBuff->data()+mBuffLen - last;;
	if (len >= 0)
	{
		aos_assert(from+length <= mBuff->data()+mBuffLen);
		memcpy(to, from, length);
	}
	else
	{
		len = abs(len);
		aos_assert(len <= mBuffLen);
		aos_assert(from+length-len <= mBuff->data()+mBuffLen);
		memcpy(to, from, length-len);
		memcpy(to+length-len, mBuff->data(), len);
	}
}


void
AosMultiFileSorter::DataSource::moveBegin(char *from, int len)
{
	aos_assert(from>=mBuff->data() && from<mBuff->data()+mBuffLen);
	char *to = move(from, len);
	aos_assert(to>=mBuff->data() && to<mBuff->data()+mBuffLen);

	mLock->lock();
	mPoss.push_back(from);
	mPoss.push_back(to);

	PossCmpFun fun = {this};
	std::sort(mPoss.begin(), mPoss.end(), fun);

	size_t i=1;
	for (; i<mPoss.size();)
	{
		if (mPoss[i-1] != mPoss[i])
			break;
		i+=2;
	}
	if (i>2)
	{
		mPoss.erase(mPoss.begin(), mPoss.begin()+i-1);
		mBegin = mPoss[0];
//OmnScreen << "movebegin-1 id: " << mEventId << "  " << len << endl;
	}
	int cashesize = distance(mBegin, mEnd);
	if (cashesize <= mBuffLen/4)
	{
//OmnScreen << "move begin0 " << mEventId << "  cachedsize: " << cashesize << endl;
		if (cashesize && !mEmpty)
			loadData(mTotalRead);
		else
			mEmpty = true;
	}
	else
	{
//OmnScreen << "move begin1 " << mEventId << "," << cashesize << endl;
	}
	mLock->unlock();
	return;
}


int 
AosMultiFileSorter::Element::distance(char *from, char *to)
{
	return mSource->distance(from, to);
}


char*
AosMultiFileSorter::Element::move(char *from, int length)
{
	return mSource->move(from, length);
}


void 
AosMultiFileSorter::Element::copy(char *to, char *from, int length)
{
	mSource->copy(to, from, length);
}


int
AosMultiFileSorter::Element::getSize(char *bound)
{
	int size =  mSource->getSize(mPos, bound);	
	mSize = size;
	return size;
}


char* 
AosMultiFileSorter::Element::getData(int size)
{
	return mSource->getData(mPos, size);	
}


bool
AosMultiFileSorter::nextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	// This function is called when a caller needs the next block. 
	// It checks whether a block is available. If yes, it returns
	// the block. Otherwise, it waits until a block is available.
	return true;
}

