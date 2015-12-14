////////////////////////////////////////////////////////////////////////////
//
// copyright (c) 2005
// packet engineering, inc. all rights reserved.
//
// redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of packet engineering, inc. or its derivatives
//
// description:
//
// modification history:
// 09/11/2015 created by barry niu
////////////////////////////////////////////////////////////////////////////
#include "QueryInto/QueryIntoLocalFile.h"
#include "SEInterfaces/JimoType.h"
#include "SEInterfaces/DataRecordType.h"
#include "SEInterfaces/DataRecordObj.h"

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosQueryIntoLocalFile_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosQueryIntoLocalFile(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosQueryIntoLocalFile::AosQueryIntoLocalFile()
:
AosJimo(AosJimoType::eQueryInto, 0),
mBuff(0),
mFile(0),
mFileNum(0)
{
}


AosQueryIntoLocalFile::AosQueryIntoLocalFile(const int version)
:
AosJimo(AosJimoType::eQueryInto, version),
mBuff(0),
mFile(0),
mFileNum(0)
{
}


AosQueryIntoLocalFile::AosQueryIntoLocalFile(const AosQueryIntoLocalFile &queryInto)
{
	mFileName = queryInto.mFileName;
	mBuff = queryInto.mBuff;
	mFile = queryInto.mFile;
	mFileNum = queryInto.mFileNum;
}


AosQueryIntoLocalFile::~AosQueryIntoLocalFile()
{
}


bool
AosQueryIntoLocalFile::config(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	//into_file
	//<into_file name=\"local_file\">
	//  	<arg name=\"filename\"><![CDATA[chen02]]></arg>
	//</into_file>
	aos_assert_r(def, false);
	AosXmlTagPtr arg = def->getNextChild("arg");
	OmnString name = "", value = "";
	map<OmnString, OmnString> argMap;
	map<OmnString, OmnString>::iterator itr;
	while(arg)
	{
		name = arg->getAttrStr("name");
		value = arg->getNodeText();
		argMap.insert(make_pair(name, value));
		arg = def->getNextChild("arg");
	}
	itr = argMap.find("filename");
	if(itr == argMap.end())
	{
		OmnString msg = "Missing 'filename' in INTO!";
		rdata->setJqlMsg(msg);
		return false;
	}

	mFileName = itr->second;
	//check dir is exist
	bool rslt = checkDirIsExist(rdata);
	if (!rslt) return false;

	mBuff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	return true;
}


bool 
AosQueryIntoLocalFile::checkDirIsExist(
		AosRundata *rdata)
{
	int pos = mFileName.find('/', true);
	if(pos == -1)
		return true;

	OmnString path = mFileName.substr(0, pos);
	DIR *dir;
	bool dir_exist = ((dir = opendir(path.data())) != NULL);
	if (!dir_exist)
	{
		closedir(dir);
		OmnString msg = "Directory \"";
		msg << path << "\" does not exist!";
		rdata->setJqlMsg(msg);
		return false;
	}             
	closedir(dir);
	return true;
}


bool
AosQueryIntoLocalFile::appendEntry(
				AosDataRecordObj *record,
		        AosRundata *rdata)
{
	const char *data = record->getData(rdata);
	i64 rcd_len = record->getRecordLen();
	aos_assert_r(rcd_len > 0, false);

	bool rslt = mBuff->setBuff(data, rcd_len);
	aos_assert_r(rslt, false);

	i64 len = mBuff->dataLen();
	if (len >= eDftBuffMaxSizeToSend)
	{
		rslt = flush(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosQueryIntoLocalFile::flush(
		AosRundata *rdata)

{
	bool rslt;
	aos_assert_r(mBuff, false);
	i64 len = mBuff->dataLen();
	if (len > 0)
	{
		if (!mFile)
		{
			rslt = createFile(rdata);
			aos_assert_r(rslt, false);
		}

		char* data = mBuff->data();
		u64 filesize = mFile->getLength();
		if (filesize < eDftMaxFileSize)             
		{
			mFile->openFile(OmnFile::eWriteCreate);
			aos_assert_r(rslt, false);

			rslt = mFile->append(data, len, true);
			aos_assert_r(rslt, false);

			rslt = mFile->closeFile();
			aos_assert_r(rslt, false);
		}   
		else
		{
			rslt = createFile(rdata);
			aos_assert_r(rslt, false);

			mFile->openFile(OmnFile::eWriteCreate);
			aos_assert_r(rslt, false);

			rslt = mFile->append(data, len, true);
			aos_assert_r(rslt, false);

			rslt = mFile->closeFile();
			aos_assert_r(rslt, false);
		}

		mBuff->reset();
		mBuff->setDataLen(0);
	}

	return true;
}


bool
AosQueryIntoLocalFile::createFile(
		AosRundata *rdata)
{
	OmnString file_name = "";
	if(mFile)
	{
		file_name << mFileName << "_" << mFileNum;
	}
	else
	{
		file_name = mFileName;
	}
	//check file is exist
	//if exist, delete it first
	if(OmnFile::fileExist(file_name))
	{
		::remove(file_name.data());
	}
	mFile = OmnNew OmnFile(file_name, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	aos_assert_r(mFile, false);

//OmnScreen << "queryInto filenam:" << file_name << endl;
	mFileNum++;
	return true;
}


AosJimoPtr
AosQueryIntoLocalFile::cloneJimo() const                                 
{
	return OmnNew AosQueryIntoLocalFile(*this);
}


AosQueryIntoObjPtr
AosQueryIntoLocalFile::clone() const                                 
{
	return OmnNew AosQueryIntoLocalFile(*this);
}

