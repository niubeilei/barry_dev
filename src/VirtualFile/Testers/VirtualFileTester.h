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
// 06/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Actions_Testers_VirtualFileTester_h
#define Aos_Actions_Testers_VirtualFileTester_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Util/HashUtil.h"
#include "Util/String.h"
#include "Util/StrU64Array.h"
#include "VirtualFile/VirtualFile.h"
#include "VirtualFile/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <map>

using namespace std;
 
class AosVirtualFileTester : virtual public OmnTestPkg
{
private:
	OmnFilePtr								mNormalFile;
	OmnFilePtr								mRcdFile;
	OmnFilePtr								mSplitFile;
	AosRundataPtr							mRundata;
	map<OmnString, AosXmlTagPtr>			mObjidMap;
public:
	AosVirtualFileTester();
	~AosVirtualFileTester();
	virtual bool start();

private:
	bool				basicTest();
	bool				createFiles();
	bool				createVFile(const AosRundataPtr &rdata);
	int					getSegmentSize();
	OmnString			getSegmentType();
	AosXmlTagPtr 		initConfig(
						const OmnString &objid,
						const OmnString &seg_type,
						const AosRundataPtr &rdata);
	bool				importFile(
						const OmnString &seg_type,
						const AosVirtualFilePtr &file,
						const AosRundataPtr &rdata);
	bool				appendFile(
						const OmnString &seg_type,
						const AosVirtualFilePtr &file,
						const AosRundataPtr &rdata);
	bool				appendFileByRcd(
						const OmnFilePtr &file,
						const AosVirtualFilePtr &vf,
						const AosRundataPtr &rdata);
	bool				appendFileBySize(
						const OmnFilePtr &file,
						const AosVirtualFilePtr &vf,
						const AosRundataPtr &rdata);
	bool				appendFileBySpliter(
						const OmnFilePtr &file,
						const AosVirtualFilePtr &vf,
						const AosRundataPtr &rdata);
	AosBuffPtr			getSplitBuff(
						u64 &startpos,
						u64 &rcdpos,
						const AosBuffPtr &buff);
	bool				appendAndImportFile(
						const OmnString &seg_type,
						const AosVirtualFilePtr &file,
						const AosRundataPtr &rdata);
	bool				importAndAppendFile(
						const OmnString &seg_type,
						const AosVirtualFilePtr &file,
						const AosRundataPtr &rdata);
	bool				retrVFileAndAppend(const AosRundataPtr &rdata);

	bool				retrVFileAndImport(const AosRundataPtr &rdata);

	bool				exportFile(const AosRundataPtr &rdata);

	bool				retrieveBlock(const AosRundataPtr &rdata);

	bool				checkBlock(const AosRundataPtr &rdata);

};
#endif

