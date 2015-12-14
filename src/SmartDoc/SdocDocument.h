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
// 03/19/2013	Created by Ice
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartDoc_SdocDocument_h
#define Aos_SmartDoc_SdocDocument_h

#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThrdShellProc.h" 
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"


class AosSdocDocument : public AosSmartDoc
{
	enum
	{
		eDftThrdShells = 5,
		eMaxBytes = 1000
	};

	struct Entry
	{
		AosSdocDocumentPtr 	mDocument;
		AosXmlTagPtr 		mSdoc;
		AosXmlTagPtr 		mFileDoc;
		AosRundataPtr 		mRundata;

		Entry(
			  const AosSdocDocumentPtr &document,
			  const AosXmlTagPtr &sdoc,
			  const AosXmlTagPtr &filedoc,
			  const AosRundataPtr &rdata)
		:
		mDocument(document),
		mSdoc(sdoc->clone(AosMemoryCheckerArgsBegin)),
		mFileDoc(filedoc->clone(AosMemoryCheckerArgsBegin)),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
	};

	static int            smThrds;
	static OmnMutexPtr    smLock;
	static queue<Entry>   smTasks;
	static int 			  smActiveTasks;

public:
	AosSdocDocument(const bool flag);
	~AosSdocDocument();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocDocument(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

	bool triggerNextTask();

public:
	class CreateFileDoc: public OmnThrdShellProc
	{
		OmnDefineRCObject;

		public:
		AosSdocDocumentPtr  mDocument;
		AosXmlTagPtr 		mSdoc;
		AosXmlTagPtr		mFileDoc;
		int					mFileIdx;
		AosRundataPtr       mRundata;

		public:
		CreateFileDoc(
				const AosSdocDocumentPtr &document,
				const AosXmlTagPtr &sdoc,
				const AosXmlTagPtr &file_doc,
				const int fileidx,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("CreateFileDoc"),
		mDocument(document),
		mSdoc(sdoc->clone(AosMemoryCheckerArgsBegin)),
		mFileDoc(file_doc->clone(AosMemoryCheckerArgsBegin)),
		mFileIdx(fileidx),
		mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		
		static bool getFileInfo(OmnString &doc_str, OmnString &next_str, const OmnFilePtr &file);
		virtual bool run();
		virtual bool procFinished();
	};

};
#endif
