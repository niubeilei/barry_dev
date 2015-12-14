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
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActCreateFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataCalcDyn/DataCalcNames.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "Util/StrParser.h"
#include "Util/ValueRslt.h"
#include "Util/DirDesc.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "time.h"

using namespace std;

static OmnString smTargetDir = "/home/young/codes";
const bool IS_PUBLIC = true;
const int64_t MAX_SIZE = 1 * 1000 * 1000 * 1000; 

AosActCreateFiles::AosActCreateFiles(const bool flag)
:
AosSdocAction(AOSACTTYPE_CREATEFILES, AosActionType::eCreateFiles, flag)
{
}


AosActCreateFiles::~AosActCreateFiles()
{
}

bool
AosActCreateFiles::readFiles(
		const OmnString &path,
		const int rec_level,
		const int crt_level,
		const AosXmlTagPtr &doc,
		const AosRundataPtr &rdata)
{
	DIR * pDir = opendir(path.data());
	aos_assert_r(pDir, false);

	bool rslt = true;
	int ret = 0;
	int new_level = crt_level + 1;
	struct stat st;
	struct dirent *ent;
	OmnString childPath;

	while ((ent = readdir(pDir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		{
			continue;
		}

		childPath = path;
		childPath << "/" << ent->d_name;

		ret = stat(childPath.data(), &st);
		if (ret < 0) 
		{
			continue;
		}

		if (S_ISREG(st.st_mode))
		{
			if (st.st_size > 0)
			{
				OmnFile file(childPath, OmnFile::eReadOnly AosMemoryCheckerArgs);

				int64_t buff_size = file.getLength();
				if (buff_size > MAX_SIZE)
				{
				   AosSetErrorU(rdata, "file_too_big") << buff_size
					   << ". Filename: " << childPath << enderr;
				   return false;
				}

				AosBuffPtr buff = OmnNew AosBuff(buff_size+10 AosMemoryCheckerArgs);
				char *data = buff->data();
				int64_t data_len = buff_size;
				int64_t read_len = file.readToBuff(0, data_len, data);
				aos_assert_rr(read_len == buff_size, rdata, false);
				buff->setDataLen(data_len);
				
				OmnString source_fname;
				int serverid = AosGetSelfServerId();
				doc->setAttr(AOSTAG_SERVERID, serverid);
				doc->setAttr("file_size", buff_size);
				doc->setAttr("source_fname", childPath);
				doc->setAttr(AOSTAG_OTYPE,AOSOTYPE_BINARYDOC); 
				doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
				doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
				rdata->setWorkingDoc(doc, true);
				
				AosXmlTagPtr newdoc = AosCreateBinaryDoc(-1, doc->toString(), IS_PUBLIC, buff, rdata);
				aos_assert_rr(newdoc, rdata, false);

				file.deleteFile();

			}
			continue;
		}

		if (S_ISDIR(st.st_mode))
		{
			if(rec_level == -1 || rec_level >= new_level)
			{
				rslt = readFiles(childPath, rec_level, new_level, doc, rdata);
				aos_assert_r(rslt, false);
			}
			continue;
		}
	}

	closedir(pDir);
	return true;
}


bool
AosActCreateFiles::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This function creates one doc for each file. 'def' is in the form:
	// <config ...>
	//      <doctemplate ...>
	//          ...
	//      </doctemplate>
	// </config>

	AosXmlTagPtr doc_template = def->getFirstChild("doctemplate");
	OmnString dir_name = doc_template->getAttrStr("dirname", "");
	aos_assert_rr(dir_name != "", rdata, false);
	AosXmlTagPtr doc = doc_template->clone(AosMemoryCheckerArgsBegin);
	readFiles(dir_name, -1, -1,doc, rdata);
	rdata->setOk();
	return true;
}


/*
bool 
AosActCreateFiles::run(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This function creates one doc for each file. 'def' is in the form:
	// 	<config ...>
	// 		<doctemplate ...>
	// 			...
	// 		</doctemplate>
	// 	</config>
	int server_id = def->getAttrInt(AOSTAG_SERVERID, -1);
	aos_assert_rr(server_id >= 0, rdata, false);

	OmnString dirname = def->getAttrStr("dirname", "");
	aos_assert_rr(dirname != "", rdata, false);

	AosXmlTagPtr doc_template = def->getFirstChild("doctemplate");
	aos_assert_rr(doc_template, rdata, false);
	doc_template = doc_template->clone(AosMemoryCheckerArgsBegin);

	OmnDirDescPtr dir;
	try
	{
		dir = OmnNew OmnDirDesc(dirname);
	}

	catch (...)
	{
		OmnAlarm << "Directory not exist: " << dirname << enderr;
		return false;
	}

	aos_assert_rr(dir, rdata, false);
	OmnString fname;
	bool rslt = dir->getFirstFile(fname);
	while (rslt)
	{
		aos_assert_rr(fname != "", rdata, false);
		AosXmlTagPtr doc = doc_template->clone(AosMemoryCheckerArgsBegin);
		OmnFile file(fname, OmnFile::eReadOnly AosMemoryCheckerArgs);

		u64 buff_size = file.getLength();
		if (buff_size > MAX_SIZE)
		{
			AosSetErrorU(rdata, "file_too_big") << buff_size
				<< ". Filename: " << fname << enderr;
			return false;
		}

		AosBuffPtr buff = OmnNew AosBuff(buff_size+10 AosMemoryCheckerArgs);
		char *data = buff->data();
		int64_t data_len = buff_size;
		int64_t read_len = file.readToBuff(0, data_len, data);
		aos_assert_rr(read_len == buff_size, rdata, false);
		buff->setDataLen(data_len);
		
		int serverid = AosGetSelfServerId();
		doc->setAttr(AOSTAG_SERVERID, serverid);
		doc->setAttr("source_fname", fname);
		doc->setAttr(AOSTAG_OTYPE,AOSOTYPE_BINARYDOC); 
		doc->setAttr(AOSTAG_PUBLIC_DOC, "true");
		doc->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		rdata->setWorkingDoc(doc, true);

		AosXmlTagPtr newdoc = AosCreateBinaryDoc(-1, doc->toString(), IS_PUBLIC, buff, rdata);
		aos_assert_rr(newdoc, rdata, false);
		
		file.deleteFile();
		rslt = dir->getNextFile(fname);
	}

	return true;
}
*/

AosActionObjPtr
AosActCreateFiles::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActCreateFiles(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

