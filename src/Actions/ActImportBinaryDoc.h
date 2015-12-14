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
// 2013/05/07 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_ActCreateFile_h
#define Aos_Actions_ActCreateFile_h

#include "Actions/TaskAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "zconf.h"
#include "zlib.h"


class AosActImportBinaryDoc : virtual public AosTaskAction
{
private:
	
	bool					mIsGzipFile;
	AosXmlTagPtr			mTemplateDoc;
	AosDataScannerObjPtr	mScanner;

public:
	AosActImportBinaryDoc(const bool flag);
	~AosActImportBinaryDoc();


	AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
	
	virtual bool config(const u64 task_docid,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata){ return true;};

	virtual bool initAction(
					const AosTaskObjPtr &task,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	virtual bool run(const AosRundataPtr &rdata);

	virtual bool finishedAction(const AosRundataPtr &rdata);

	bool	addAttrForUpload(
					const OmnString &fname,
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata);
private:
	bool importFileToBinaryDoc(
			const AosRundataPtr &rdata);

	bool importGzipFileToBinaryDoc(
			const AosRundataPtr &rdata);

	int gzdecompress(
			Byte *zdata, 
			uLong nzdata,    
			Byte *data, 
			uLong *ndata);

};

#endif

