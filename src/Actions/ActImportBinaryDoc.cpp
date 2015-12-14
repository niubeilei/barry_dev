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
#include "Actions/ActImportBinaryDoc.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/Ptrs.h"
#include "API/AosApi.h"
#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/BuffData.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/DocClientObj.h"

//const u64 BUFFSIZE  = 1000 * 1000 * 10; // 10mb 
const u64 BUFFSIZE  = 1000 * 1000 * 100; // 100mb 
//const u64 BUFFSIZE  = 1000 * 1000 * 1; // 1mb 

AosActImportBinaryDoc::AosActImportBinaryDoc(const bool flag)
:
AosTaskAction(AOSACTTYPE_IMPORTBINARYDOC, AosActionType::eImportBinaryDoc, flag),
mIsGzipFile(false)
{
}


AosActImportBinaryDoc::~AosActImportBinaryDoc()
{
}

AosActionObjPtr
AosActImportBinaryDoc::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);

	try
	{
		return OmnNew AosActImportBinaryDoc(false);
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
AosActImportBinaryDoc::initAction(
					const AosTaskObjPtr &task,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata)
{
	aos_assert_r(task && sdoc, false);

	mTask = task;
	mSdoc = sdoc;

	mIsGzipFile = sdoc->getAttrBool("zky_is_gzip_file", false);

	mTemplateDoc = sdoc->getFirstChild(AOSTAG_BINARYDOC);
	aos_assert_r(mTemplateDoc, false);

	OmnString pctrs = mTemplateDoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(pctrs != "", false);

	AosXmlTagPtr dd = AosGetParentContainer(rdata, pctrs, true);
	aos_assert_r(dd, false);

	AosTaskDataObjPtr taskdata = task->getTaskData();
	aos_assert_r(taskdata, false);

	mScanner = taskdata->createDataScanner(rdata);
	aos_assert_r(mScanner, false);

	return true;
}


bool
AosActImportBinaryDoc::run(const AosRundataPtr &rdata)
{
	if (!mIsGzipFile)
	{
		return importFileToBinaryDoc(rdata);
	}
	return importGzipFileToBinaryDoc(rdata);
}


bool
AosActImportBinaryDoc::importGzipFileToBinaryDoc(
		const AosRundataPtr &rdata)
{
	AosBuffPtr fileContent = OmnNew AosBuff(BUFFSIZE AosMemoryCheckerArgs);
	AosBuffDataPtr metaData = OmnNew AosBuffData();
	AosFileInfo info;
	AosBuffPtr strDst = OmnNew AosBuff(BUFFSIZE AosMemoryCheckerArgs);
	while (fileContent->dataLen() < (int64_t)BUFFSIZE)
	{
		AosBuffDataPtr metaData = OmnNew AosBuffData();
		bool rslt = mScanner->getNextBlock(metaData, rdata);
		aos_assert_r(rslt, false);
		if (!metaData || !metaData->getBuff() ||
			(metaData->getBuff())->dataLen() <=0)
		{
			if (!mTask->taskDataIsFinished())
			{
				mTask->setTaskDataFinished();
			}
			AosActionObjPtr thisptr(this, true);
			mTask->actionFinished(thisptr,rdata);
			break;
		}

		uLong len = BUFFSIZE;
		info = metaData->getFileInfo();
		char * zipbuff =  metaData->getBuff()->data();
		u64 bufLen = metaData->getBuff()->dataLen();

		//gunzip
		int r = gzdecompress((unsigned char*)zipbuff, bufLen,
				(unsigned char *)strDst->data(), &len);

		while (r == Z_BUF_ERROR)
		{
			strDst = OmnNew AosBuff(len*2 AosMemoryCheckerArgs); 
			r = gzdecompress((unsigned char*)zipbuff, bufLen,
						         (unsigned char *)strDst->data(), &len);
		}
//		OmnScreen << "File name : " << metaData->getFileInfo().mFileName << endl;
		if(r != Z_OK)
		{
			continue;
		}
		strDst->setDataLen(len);
		fileContent->setBuff(strDst);
	}

	aos_assert_r(fileContent, false);
	if (fileContent->dataLen() <= 0) return true;

	AosXmlTagPtr doc = mTemplateDoc->clone(AosMemoryCheckerArgsBegin);
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	doc->setAttr(AOSTAG_SOURCE_FNAME, info.mFileName);
	doc->setAttr(AOSTAG_SOURCE_LENGTH, info.mFileLen);
	doc->setAttr(AOSTAG_JOB_DOCID, mTask->getJobDocid());
	doc->setAttr(AOSTAG_BLOCKIDX, metaData->getCrtBlockIdx());

	//felicia, for batch upload file
	//addAttrForUpload(info.mFileName, doc, rdata);

	AosXmlTagPtr binarydoc = AosCreateBinaryDoc(-1, doc->toString(), true, fileContent, rdata);
	aos_assert_r(binarydoc, false);
	return true;
}


bool
AosActImportBinaryDoc::importFileToBinaryDoc(
		const AosRundataPtr &rdata)
{
	AosBuffDataPtr metaData = OmnNew AosBuffData();
	bool rslt = mScanner->getNextBlock(metaData, rdata);
	aos_assert_r(rslt, false);

	if (!metaData || !metaData->getBuff()|| (metaData->getBuff())->dataLen() <= 0)
	{
		if (!mTask->taskDataIsFinished())
		{
			mTask->setTaskDataFinished();
		}
		AosActionObjPtr thisptr(this, true);
		mTask->actionFinished(thisptr, rdata);
		return true;
	}

	AosXmlTagPtr doc = mTemplateDoc->clone(AosMemoryCheckerArgsBegin);
	doc->setAttr(AOSTAG_OTYPE, AOSOTYPE_BINARYDOC);
	AosFileInfo info = metaData->getFileInfo();
	doc->setAttr(AOSTAG_SOURCE_FNAME, info.mFileName);
	doc->setAttr(AOSTAG_SOURCE_LENGTH, info.mFileLen);
	doc->setAttr(AOSTAG_JOB_DOCID, mTask->getJobDocid());
	doc->setAttr(AOSTAG_BLOCKIDX, metaData->getCrtBlockIdx());

	//felicia, for batch upload file
	addAttrForUpload(info.mFileName, doc, rdata);

	AosXmlTagPtr binarydoc = AosCreateBinaryDoc(-1, doc->toString(), true, metaData->getBuff(), rdata);
	aos_assert_r(binarydoc, false);
	return true;
}


bool
AosActImportBinaryDoc::addAttrForUpload(
		const OmnString &file_name, 
		const AosXmlTagPtr &doc, 
		const AosRundataPtr &rdata)
{
	OmnString file_type = doc->getAttrStr("zky_file_type");
	if (file_type == "上传")
	{
		//AosStrSplit split(info.mFileName, "/");            
		//vector<OmnString> strs= split.entriesV();
		//OmnString fname = strs[strs.size()-1];
		int startidx = file_name.find('/', true);
		OmnString fname = file_name.substr(startidx+1);  
		doc->setAttr(AOSTAG_RSC_FNAME, fname);		
		doc->setAttr(AOSTAG_RSCDIR, "/tmp");

		startidx = fname.find('.', true);
		OmnString document_type = fname.substr(startidx+1);
		document_type = document_type.toLower();
		if (document_type == "doc" || document_type == "docx" || document_type == "wps" || document_type == "wpt")
		{
	//		doc->setAttr("zky_type_pic", "image_6/do89504623689759.png");
			doc->setAttr(AOSTAG_ZKY_TYPE,"DOC");
		}
		else if (document_type == "pdf")
		{
	//		doc->setAttr("zky_type_pic", "image_6/ao89504623689765.png");
			doc->setAttr(AOSTAG_ZKY_TYPE, "PDF");
		}
		else if (document_type == "xls" || document_type == "xlsx" || document_type == "et")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "XLS");
		}
		else if (document_type == "ppt" || document_type == "pptx" || document_type == "dps")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "PPT");
		}
		else if (document_type == "txt")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "TXT");
		}
		else if (document_type == "xml" || document_type == "xps")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "XML");
		}
		else if (document_type == "jpg" || document_type == "jpeg" || 
				 document_type == "bmp" || document_type == "gif" ||
				 document_type == "tif" || document_type == "png")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "IMAGE");
		}
		else if (document_type == "aip" || document_type == "rar" || document_type == "zip")
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "RAR");
		}
		else
		{
			doc->setAttr(AOSTAG_ZKY_TYPE, "OTHER");
		}

		OmnString cid = rdata->getCid();
		aos_assert_r(cid != "", false);

		AosXmlTagPtr user_doc = AosDocClientObj::getDocClient()->getDocByCloudid(cid, rdata);
		aos_assert_r(user_doc, false);

		OmnString user_name = user_doc->getAttrStr(AOSTAG_USERNAME, "");
		aos_assert_r(user_name != "", false);

		doc->setAttr(AOSTAG_USERNAME, user_name);
	}

	return true;
}


int
AosActImportBinaryDoc::gzdecompress(
        Byte *zdata, 
        uLong nzdata,                 
        Byte *data, 
        uLong *ndata)
{
    int err = 0;
    z_stream d_stream = {0}; /* decompression stream */
    static char dummy_head[2] = 
    {
        0x8 + 0x7 * 0x10,
        (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    d_stream.next_in  = zdata;
    d_stream.avail_in = 0;
    d_stream.next_out = data;
    //if(inflateInit2(&d_stream, 15) != Z_OK) return -1;
    if((err = inflateInit2(&d_stream, 16+MAX_WBITS)) != Z_OK) return err;
    //if(inflateInit2(&d_stream, 47) != Z_OK) return -1;                           
    while (d_stream.total_out < *ndata && d_stream.total_in < nzdata) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        if((err = inflate(&d_stream, Z_NO_FLUSH)) == Z_STREAM_END) break;
        if(err != Z_OK )
        {
            if(err == Z_DATA_ERROR)
            {
                d_stream.next_in = (Bytef*) dummy_head;
                d_stream.avail_in = sizeof(dummy_head);
                if((err = inflate(&d_stream, Z_NO_FLUSH)) != Z_OK) 
                {
                    return err;
                }
            }
            else return -1;
        }
    }
    if(inflateEnd(&d_stream) != Z_OK) return -1;
    *ndata = d_stream.total_out;
    return 0;
}


bool
AosActImportBinaryDoc::finishedAction(const AosRundataPtr &rdata)
{
	return true;
}

