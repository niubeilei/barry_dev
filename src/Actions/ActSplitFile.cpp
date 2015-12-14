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
// Modification History:
// 06/02/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActSplitFile.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Util/File.h"


AosActSplitFile::AosActSplitFile(const bool flag)
:
AosSdocAction(AOSACTTYPE_SPLITFILE, AosActionType::eSplitFile, flag)
{
}


AosActSplitFile::AosActSplitFile(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_JOINIIL, AosActionType::eSplitFile, false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosActSplitFile::~AosActSplitFile()
{
}


AosActionObjPtr
AosActSplitFile::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActSplitFile(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActSplitFile::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_r(rdata && rdata->getSiteid() > 0, false);
	aos_assert_rr(def, rdata, false);

	return config(mFilename, mRecordSize, 
			mNumRcdsPerTask, mEvenDist, mJobDocid, def, rdata);
}


bool
AosActSplitFile::config(
		OmnString &fname, 
		int &record_size, 
		int &num_rcds_per_task,
		bool &even_dist,
		u64 &job_docid,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, false);
	fname = sdoc->getAttrStr(AOSTAG_FILENAME);
	if (fname == "")
	{
		AosSetErrorU(rdata,"missing_filename:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	record_size = sdoc->getAttrInt(AOSTAG_RECORD_SIZE, -1);
	if (record_size <= 0)
	{
		AosSetErrorU(rdata,"invalid_record_size:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	even_dist = sdoc->getAttrBool(AOSTAG_EVEN_DISTRIBUTION, false);
	num_rcds_per_task = -1;
	if (!even_dist)
	{
		num_rcds_per_task = sdoc->getAttrInt(AOSTAG_NUM_RCDS_PER_TASK, -1);
		if (num_rcds_per_task <= 0)
		{
			AosSetErrorU(rdata,"invalid_num_rcds_per_task:") << sdoc->toString();
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	job_docid = sdoc->getAttrU64(AOSTAG_JOB_DOCID, 0);
	if (job_docid == 0)
	{
		AosSetErrorU(rdata,"invalid_job_docid:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}


bool 
AosActSplitFile::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	if (mIsTemplate) return runTemplate(sdoc, rdata);
	
	if (mFilename == "")
	{
		// It needs to config
		if (!config(mFilename, mRecordSize, mNumRcdsPerTask, 
					mEvenDist, mJobDocid, sdoc, rdata)) return false;
	}

	return runPriv(mFilename, mRecordSize, mNumRcdsPerTask, mEvenDist, mJobDocid, rdata);
}


bool
AosActSplitFile::runTemplate(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	OmnString fname;
	int record_size;
	int num_rcds_per_task;
	bool even_dist;
	u64 job_docid;
	if (!config(fname, record_size, num_rcds_per_task, even_dist, job_docid, sdoc, rdata))
	{
		return false;
	}

	return runPriv(fname, record_size, num_rcds_per_task, even_dist, job_docid, rdata);
}


bool
AosActSplitFile::runPriv(
		const OmnString &fname, 
		const int record_size,
		const int num_rcds_per_task,
		const bool even_dist,
		const u64 &job_docid,
		const AosRundataPtr &rdata)
{
	if (!AosIsFileLocal(fname))
	{
		AosSetErrorU(rdata, "file_not_local:") << fname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int64_t filesize = OmnFile::getFileLengthStatic(fname);
	if (filesize <= 0)
	{
		AosSetErrorU(rdata, "file_is_empty:") << fname;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	int64_t blocksize = 0;
	if (even_dist)
	{
		// It is even distribution
		int64_t num_rcds = filesize / record_size;
		int num_physicals = AosGetNumPhysicals();
		if (num_physicals <= 0)
		{
			AosSetErrorU(rdata, "no_physicals:") << num_physicals;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		u32 nn = num_rcds / num_physicals;
		blocksize = record_size * nn;
	}
	else
	{
		blocksize = record_size * num_rcds_per_task;
	}
	aos_assert_r(blocksize > 0, false);

	// 1. split the file
	u32 block_num = filesize/blocksize;
	if((filesize % blocksize) > 0) block_num++;
	// u32 num_files = block_num;

	// Retrieve the current working directory
	char crt_dir[200];
	getcwd(crt_dir, 200);
	OmnString dirname = fname.getPrefix('/');
	int rslt = chdir(dirname.data());
	if (rslt)
	{
		AosSetErrorU(rdata, "failed_change_dir:") << fname.data();
		OmnAlarm << rdata->getErrmsg() << enderr;
		chdir(crt_dir);
		return false;
	}

	// "split -b 100000000 -a 6 -d raw_data_0 jobdocid_";
	// The files to be generated are in the form:
	// 		jobdocid_000000
	// 		jobdocid_000001
	// 		jobdocid_000002
	OmnString command = "split -b ";
	command << blocksize << " -a 6 -d " << fname 
		 << " " << job_docid << "_";
	system(command.data());

	chdir(crt_dir);
	return true;
}

