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
// 2013/04/27 Created By Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_TaskUtil_DataColCtlr_h
#define AOS_TaskUtil_DataColCtlr_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/LogicTaskObj.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/LogicTask.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"

#include <list>
#include <vector>

using namespace std;


class AosDataColCtlr : virtual public OmnRCObject
{
	OmnDefineRCObject;

	OmnMutexPtr			mLock;
	OmnString			mDataColId;
	AosLogicTaskObjPtr	mLogicTask;
	AosXmlTagPtr		mDataColTag;
	AosXmlTagPtr		mDataProcConf;		// Ketty 2014/08/21
	int					mPhysicalId;
	int					mCubeId;			// Ketty 2014/08/22

	set<u64>			mActiveTaskDocids;
	vector< list<AosXmlTagPtr> >	mFileList;
	vector< list<AosXmlTagPtr> >	mDocFixedFileList;
	vector< list<AosXmlTagPtr> >	mDocCSVFileList;

public:
	AosDataColCtlr(
			const OmnString &datacol_id,
			const AosLogicTaskObjPtr &logic_task);
	~AosDataColCtlr();

	//u64			getActiveTaskDocid() const {return mActiveTaskDocid;}
	//void		setActiveTaskDocid(const u64 &docid){mActiveTaskDocid = docid;}
	int			getActiveTaskDocidNum() const;
	void		insertActiveTaskDocid(const u64 &task_docid);
	void		eraseActiveTaskDocid(const u64 &task_docid);

	OmnString	getDataColId() const {return mDataColId;}
	bool		setDataColTag(
					const AosXmlTagPtr &tag,
					const AosRundataPtr &rdata);
	bool 		setDataColTag(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &data_proc_conf,
					const AosXmlTagPtr &data_col_conf);

	bool		addOutPut(
					const AosXmlTagPtr &xml, 
					const AosRundataPtr &rdata);
	
	//felicia, 2014/04/16
	bool		addOutPutDocFixed(
					const AosXmlTagPtr &xml, 
					const AosRundataPtr &rdata);
	bool		addOutPutDocCSV(
					const AosXmlTagPtr &xml, 
					const AosRundataPtr &rdata);
	bool		mergeAllDocFiles(const AosRundataPtr &rdata);
	bool		cleanOutPutDocFixed(
					const int server_id,
					const u64 &task_docid,
					const AosRundataPtr &rdata);
	bool		cleanOutPutDocCSV(
					const int server_id,
					const u64 &task_docid,
					const AosRundataPtr &rdata);


	bool		mergeAllFiles(const AosRundataPtr &rdata);
	bool		mergeLastFile(const AosXmlTagPtr &info, const AosRundataPtr &rdata);
	bool		cleanOutPut(
					const int level,
					const u64 &task_docid,
					const AosRundataPtr &rdata);

	//void 		calGroupOutputNum();

	bool 		isService(AosRundataPtr rdata);

private:
	bool 	setIILDataColTag(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &tag);

};
#endif

#endif
