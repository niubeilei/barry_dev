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
#ifndef Aos_Sorter_MultiFileSorter_h
#define Aos_Sorter_MultiFileSorter_h


#include "DataSort/Ptrs.h"
#include "DataSort/DataSort.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/ActionCaller.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/NetFileObj.h"
#include "Sorter/Ptrs.h"
#include "Sorter/SortCaller.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "DataTypes/DataColOpr.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <queue>

//extern int log2(int64_t);



class AosMultiFileSorter : public AosActionCaller
{
	OmnDefineRCObject;	
	
	struct SortedBuff
	{
		AosBuffPtr 				mBuff;
		int     				mSeqno;

		public:
		SortedBuff(
				const AosBuffPtr &buff, 
				const int seqno) 
		:
		mBuff(buff),
		mSeqno(seqno)
		{
		}

		bool operator < (const SortedBuff &rhs) const
		{
			return mSeqno > rhs.mSeqno;
		}
		~SortedBuff(){}
	};
/*
	struct ReqComp
	{
		bool operator () (const SortedBuff &lhs, const SortedBuff &rhs) const
		{
			return lhs->mSeqno <= rhs->mSeqno;
		}
	};
*/

public:
	class DataSource : public AosFileReadListener
	{
		OmnDefineRCObject;	
	public:
		enum
		{
			eNaN = 0x40000000,
			eUnReady = 0xffffffff,
			// eEmpty = 0xfffffffd,
			eNull = 0
		};

	private:
		struct PossCmpFun
		{ 
			DataSource* source;	
			bool operator() (char* lhs, char* rhs) 
			{
				return source->distance(source->mBegin,lhs)<source->distance(source->mBegin,rhs);
			}
		};

		AosNetFileObjPtr 		mFile;
		OmnMutexPtr				mLock;
		AosBuffPtr				mBuff;
		AosMultiFileSorterPtr	mCaller;
//		int 					mEventId;
		u64 					mEventId;
		//AosCompareFunPtr        mCmp;
		AosCompareFun *        	mCmpRaw;
		vector<char*>			mPoss;
		bool					mEmpty;
		int64_t					mReadingFlag;
		int64_t					mTotalRead;
		int64_t					mTotalSort;
		int 					mBuffLen;
		char*					mBegin;
		char* 					mEnd;
	public:
		DataSource(
				const AosNetFileObjPtr &file, 
				const int buffsize, 
				AosCompareFun * cmp,
				const AosMultiFileSorterPtr &caller,
				const u64 eventid);
		~DataSource();
		//bool empty() const {return mEmpty && mBegin == mEnd;}
		bool isEmpty() const {return mEmpty;}
		int getSize(char *crt, char* key);
		char* getData(char *crt, int idx);
		bool callBack(const AosBuffPtr &buff, const bool finished);
		bool dataRead(vector<AosBuffPtr> &buffs, const bool finished); 
		bool readError(const u64 &read_id,
				const OmnString &errmsg,
				const AosRundataPtr &rdata);
		void getMemory(const int64_t size, vector<AosBuffPtr> &buffs);
		char* getBegin() {return mBegin;}
		char* getEnd() {return mEnd;}
		void moveBegin(char *from, int len);
		int distance(char *from, char *to);
		char* move(char *from, int length);
		void copy(char* to, char *from, int length);
		//int find(char* pos, char* key);
		bool loadData(int64_t offset);
		int getEventId(){return mEventId;}
		int64_t getTotalRead(){return mTotalRead;}
		int64_t getTotalSort(){return mTotalSort;}
		void setTotalSort(const int64_t size){mTotalSort += size;}
	};

	class Element
	{
		OmnDefineRCObject;	
		OmnSPtr<DataSource>	mSource;
		char*			mPos;
		int				mSize;

	public:
		Element(const OmnSPtr<DataSource> &source):
		mSource(source),
		mPos(source->getBegin()),
		mSize(0)
		{
		}

		Element(const Element &element):
		mSource(element.mSource),
		mPos(element.mPos),
		mSize(element.mSize)
		{
checkEntry(mPos);
		}

		virtual ~Element()
		{
		}

		OmnSPtr<Element> copyForNext()
		{
			OmnSPtr<Element> element = OmnNew Element(mSource);
checkEntry(element->mPos);
			element->mPos = mSource->move(mPos, mSize);
checkEntry(element->mPos);
			element->mSize = 0;
			return element;
		}

		bool isSorted(){return true;}
		bool  sort(const AosDataColOpr::E &opr){return true;}
		char* first() { return mPos; }
		char *last() { return move(mPos, mSize); }
		int size() { return mSize; }
		OmnSPtr<DataSource> getDataSource() { return mSource;}
		bool empty() { return mSource->isEmpty() && mPos == mSource->getEnd();}
		void moveToNext() { mSource->moveBegin(mPos, mSize);}

		int distance(char *from, char *to);
		char* move(char *from, int length);
		void copy(char *to, char *from, int length);
		int getSize(char *bound);
		char* getData(int size);
		int getEventId(){return mSource->getEventId();}
		void setTotalSort(const int64_t size){mSource->setTotalSort(size);}
		int64_t getTotalSort(){return mSource->getTotalSort();}
		int64_t getTotalRead(){return mSource->getTotalRead();}
		int64_t getRemain()
		{
			char* end = mSource->getEnd();
			return distance(mPos, end)-mSize;
		}
		AosBuffPtr getBuff(){return 0;}
	};

	class SortController : public AosSortCaller
	{
		OmnDefineRCObject
	protected:
		int 			mTotal;
		int 			mFinishedTotal;
		int 			mLevel;
		AosBuffPtr 		mOne;
		AosCompareFun *  mCmpRaw;
		AosDataColOpr::E mOpr;
		OmnMutexPtr		 mLock;
		AosMultiFileSorterPtr mCaller;
		int				mSeqno;

	public:
		SortController(
				const int total, 
				const int level, 
				AosCompareFun * cmp, 
				const AosDataColOpr::E opr, 
				const AosMultiFileSorterPtr &caller):
		mTotal(total),
		mFinishedTotal(0),
		mLevel(level),
		mCmpRaw(cmp),
		mOpr(opr),
		mLock(OmnNew OmnMutex()),
		mCaller(caller),
		mSeqno(0)
		{
		}
		~SortController(){}
		bool reduceCacheSize(const int64_t size, const bool merged);
		bool mergeSortFinish(
				const AosBuffPtr &buff, 
				const int level, 
				void *data,
				const AosRundataPtr &rdata);
		void mergeSortFinish(const void* sorter, const int level);
		void setSeqno(const int seqno){mSeqno = seqno;}
	};


	/*
	class SortControllerLevel0: public SortController
	{
	public:
	void reduceCacheSize(const int reduce_size);
	bool mergeSortFinish(
			const AosBuffPtr &buff, 
			const int level,
			const AosRundataPtr &rdata);
	};
	*/


private:
	enum
	{
		// eMaxFileNum = 2000
		eMaxFileNum = 5000
	};

	vector<OmnSPtr<DataSource> >	mDataSources;
	OmnSemPtr						mSem;
	OmnSemPtr						mFinishSem;
	OmnMutexPtr						mLock;
	OmnMutexPtr						mDataLock;
	queue<u64>						mEvents;

	AosDataColOpr::E				mOpr;
	//AosCompareFunPtr				mCmp;
	AosCompareFun *					mCmpRaw;
	int64_t							mMinSize;
	int64_t							mMaxSize;
	int 							mCellSize;
	int 							mMaxRate;
    int64_t							mTotalMemory;
	int64_t 						mMaxTotalMemory;
	int64_t							mTotalFinished;
	int64_t 						mTotalMergeSort;
	int64_t 						mTotalSort;
	int64_t							mTotalReadSize;
	OmnCondVarPtr     			    mMemCondVar;
	OmnCondVarPtr        		    mCondVar;
	OmnCondVarPtr           		mDataCondVar;
	int 							mSeqno;
	int								mCrtSeqno;
	bool							mIsFinished;
	int64_t 						mMergedTotal;
	AosDataScannerObjPtr   			mScanner;
	priority_queue<SortedBuff>	 	mRequests;
	bool							mDiskError;

public:
	AosMultiFileSorter(const bool regflag);
	AosMultiFileSorter(
			const int64_t minsize,
			const int64_t maxsize,
			int64_t buffsize,
			const AosDataColOpr::E opr,
			AosCompareFun * cmp,
			const vector<AosNetFileObjPtr> &files,
			const AosRundataPtr &rdata);
	~AosMultiFileSorter();

	virtual bool sort(AosRundata * rdata);
	
	static bool sanitycheck(
			DataSource *ds,
			AosCompareFun * cmp,
			char *pos,
			const int length);

 	static bool sanitycheck(
			AosCompareFun * cmp,
			char *pos, 
			const int length);
	static bool	checkEntry(const char* entry);
	bool 	nextBlock(AosBuffPtr &buff, const AosRundataPtr &rdata);
	void 	raiseEvent(u64 eventid, const int readsize);
	bool 	reduceCacheSize(const int64_t size);
	void    clear();
	AosBuffPtr	nextBuff();
	int64_t getMergedTotal(){return mMergedTotal;}
	int64_t getTotalReadSize(){return mTotalReadSize;}
	void	callBack(const u64 &reqId, const int64_t &expected_size, const bool &finished);
	void    addRequest(
				const u64 reqid,
				const int64_t readsize);
	bool	procReq(
				const u64 reqId, 
				const int64_t expected_size, 
				const bool finished);

	bool	deleteScanner()
	{
		aos_assert_r(mScanner, false);
		mScanner->destroyedMember();
		return true;
	}

	bool getDiskError() const { return mDiskError; }

private:
	char* calculateBoundary(const OmnSPtr<Element> &datasource, double rate);
	char* calculateBoundary(
			vector<OmnSPtr<Element> > &newElements,
			const OmnSPtr<Element> &element, 
			double rate);
	char* calculateBoundary(vector<OmnSPtr<Element> > &elements, double seed);
	bool mergeSort(vector<OmnSPtr<Element> > &elements, const AosRundataPtr &rdata);
	void sortFinished(const int seqno, const AosBuffPtr &buff, const AosRundataPtr &rdata);
	void sortFinished(const AosRundataPtr &rdata);
	int getElements( 
			vector<OmnSPtr<Element> > &oldElements, 
			vector<OmnSPtr<Element> > &newElements);
	bool getOneBatch(
			vector<OmnSPtr<Element> > &oldElements, 
			vector<OmnSPtr<Element> > &newElements, 
			const AosRundataPtr &rdata);
	};

#endif

