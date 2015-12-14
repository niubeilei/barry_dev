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
// 2014/08/06	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_NetFile_NetFileClt_h
#define AOS_NetFile_NetFileClt_h

#include "Rundata/Rundata.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "TransBasic/Trans.h"


OmnDefineSingletonClass(AosNetFileCltSingleton,
                        AosNetFileClt,
                        AosNetFileCltSelf,
                        OmnSingletonObjId::eNetFileClt,
                        "NetFileClt");


class AosNetFileClt : public AosNetFileCltObj
{
	OmnDefineRCObject;

public:
	AosNetFileClt();
	~AosNetFileClt();

    // Singleton class interface
    static AosNetFileClt*	getSelf();
    virtual bool			start();
    virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &config);
	
private:
	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans,
				bool &svr_death);

	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans);

	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans,
				AosBuffPtr &resp);

	bool	addReq(
				AosRundata *rdata,
				const AosTransPtr &trans,
				AosBuffPtr &resp,
				bool &svr_death);

public:
	virtual bool	dirIsGood(
						const OmnString &path,
						const int physicalid,
						const bool checkfile,
						AosRundata *rdata);

	virtual bool	fileIsGood(
						const OmnString &filename,
						const int physicalid,
						AosRundata *rdata);

	virtual bool	getFileLength(
						const u64 &fileId,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	getFileLength(
						const OmnString &filename,
						const int physicalid,
						int64_t &fileLen,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	readFileToBuff(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	readFileToBuff(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						AosBuffPtr &buff,
						AosDiskStat &disk_stat,
						AosRundata *rdata);

	virtual bool	writeBuffToFile(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	appendBuffToFile(
						const OmnString &filename,
						const int physicalid,
						const AosBuffPtr &buff,
						AosRundata *rdata);

	virtual bool	getDirList(
						const OmnString &path,
						const int physicalid,
						vector<AosDirInfo> &dir_list,
						AosRundata *rdata);

	virtual bool	getFileList(
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

	virtual bool	getFileInfo(
						const OmnString &filename,
						const int physicalid,
						AosFileInfo &file_info,
						AosRundata *rdata);

	virtual bool	asyncReadFile(
						const u64 &fileId,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);

	virtual bool	asyncReadFile(
						const OmnString &filename,
						const int physicalid,
						const int64_t &seekPos,
						const u32 bytes_to_read,
						const u64 &reqid,
						const AosAioCallerPtr &caller,
						AosRundata *rdata);

	virtual bool	deleteFile(
						const u64 &fileId,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);

	virtual bool	deleteFile(
						const OmnString &filename,
						const int physicalid,
						bool &svr_death,
						AosRundata *rdata);

	virtual bool 	getFileListByAssignExt(
						const OmnString &ext,
						const OmnString &path,
						const int physicalid,
						const int rec_level,
						vector<AosFileInfo> &file_list,
						AosRundata *rdata);

	virtual OmnLocalFilePtr createRaidFile(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag);

	virtual OmnFilePtr createTempFile(
						AosRundata *rdata,
						u64 &fileId,
						const OmnString &fname_prefix,
						const u64 &requested_space,
						const bool reserve_flag);

	virtual OmnLocalFilePtr openLocalFile(
						const u64 &fileId,
						AosRundata *rdata);

};

#endif

