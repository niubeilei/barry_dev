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
// A BDFS file is defined by a doc.
//   
//
// Modification History:
// 2013/04/20 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigFile/BigFile.h"

#include "Rundata/Rundata.h"



AosBigFile::AosBigFile(const OmnString &objid)
:
mObjid(objid)
{
}


bool 
AosBigFile::getFileNames(
		const AosRundataPtr &rdata, 
		vector<AosGenericFilePtr> &files,
		bool &complete,
		const int max)
{
	if (!mDoc) getDoc(rdata);
	aos_assert_rr(mDoc, false);

	// File names are organized as:
	// 	<doc ...>!<[BDATA[length:buff]]></doc>
	// It is encoded to an AosBuff:
	// 		type		u8
	// 		int			physical_id
	// 		id			u64,		or
	// 		name		String 		
	//		...
	filenames.clear();
	AosBuffPtr buff = mDoc->getNodeTextBinaryUnCopy();
	if (!buff)
	{
		// No files.
		return true;
	}

	u8 type;
	u64 file_id;
	int phyid;
	OmnString fname;
	AosGenericFilePtr raw_file;
	while ((type = buff->getU8(AosFileType::eInvalid)) != AosFileType::eInvalid)
	{
		phyid = buff->getInt(-1);
		aos_assert_rr(phyid >= 0, rdata, false);
		switch (type)
		{
		case AosFileType::eStorageFile:
			 file_id = buff->getU64(0);
			 raw_file = OmnNew AosBdfsRawFile(rdata, phyid, file_id);
			 break;

		case AosFileType::eSystemFile:
			 fname = buff->getStr("");
			 aos_assert_rr(fname != "", rdata, false);
			 raw_file = OmnNew AosBdfsRawFile(rdata, phyid, fname);
			 break;

		default:
			 OmnAlarm << "Unrecognized file type: " << type << enderr;
			 return false;
		}

		files.push_back(raw_file);
		if (max > 0 && files.size() > (u32)max) 
		{
			complete = false;
			return true;
		}
	}

	complete = true;
	return true;
}

