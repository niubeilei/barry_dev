////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// A document is stored in two forms: header and whole body. This class
// handles the header. There are two groups of fields: integral and 
// string fields. Header storage is fixed size (determined by 
// 'eDocHeaderSize'). This size is guaranteed to fit all integral 
// fields. If the total header size is not bigger than this value, 
// the entire header is stored in the header block. Otherwise, only
// the integral fields are stored in the header block. 
//
// When retrieving a header, it checks the 'mStorageFlag' field. If it
// is 'c', it means the string fields are stored in the Doc itself. 
// It needs to retrieve the doc and then set the header contents. 
//
//
// docid : the higthest 8 bytes used to identify the type of doc:
//    	0 : normal doc;
//    	1 : access record.
//    	2 : system record.
//    	3 : counter;
//    	...
//
// header:
//  0-3			// version (4bytes)
//	4-7  		// site (4bytes)
//	8-15		// docid (8bytes)
//	16-19       // doclocation seqno (4bytes)
//	20-27       // doclocation offset (8bytes)
//	27-33		// docsize (8bytes)
//	34-37	  	// system info location seqno (4bytes)
//	38-45	    // system info location offset (8bytes)
//	46-53		// system info location (8bytes)
// 	    
// body:
// 	normldoc :  docbody 
// 	systemdoc: 
// 		0-3:				countersize (4bytes)
// 		4-countersize:		counterbody (8bytes*n) 
//      next-next+4:    	admin doc size (4byte)
//      next-admindocsize   admin doc (nbyte)
//      next-next+4:    	access doc size (4byte)
//      next-accessdocsize  access doc (nbyte)
//   	
//
//
// Modification History:
// 12/07/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SearchEngineAdmin/XmlDoc2.h"

#include "alarm_c/alarm.h"
#include "Porting/GetTime.h"
#include "Porting/TimeOfDay.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/DocFileMgr.h"
#include "SEUtil/SeConfig.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/SeUtil.h"
#include "IdGen/U64IdGen.h"
#include "IdGen/IdGen.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SeTypes.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlInterface/XmlRc.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "UtilComm/ConnBuff.h"

const u64 sgDocLocationMask = (((u64)0x00ffffff) << 32) + 0xffffffff;
//AosDocFileMgrPtr AosXmlDoc2::smFileMgr = OmnNew AosDocFileMgr();
OmnMutexPtr 	 AosXmlDoc2::smLock = OmnNew OmnMutex();
bool
AosXmlDoc2::staticInit(const AosXmlTagPtr &config)
{
	// aos_assert_r(config, false);
    // bool exist;
    // u64 maxdocfiles = config->getAttrU64(AOSCONFIG_XMLDOC_MAX_DOCFILES, 0, exist);
    // OmnString dirname = config->getAttrStr(AOSCONFIG_DIRNAME);
    // OmnString fname = config->getAttrStr(AOSCONFIG_DOC_FILENAME);

    // return smFileMgr->init(dirname, fname, maxdocfiles);
	return true;
}


bool
AosXmlDoc2::stop(const AosDocFileMgrPtr &filemgr)
{
    // smFileMgr->stop();
	filemgr->stop();
    return true;
}


bool
AosXmlDoc2::saveToFile(
		const u64 &docid, 
		const AosXmlTagPtr &doc, 
		const bool savenew,
		const AosDocFileMgrPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	aos_assert_r(doc->getAttrStr(AOSTAG_FULLDOC_SIGNATURE) == "", false);
	smLock->lock();
	OmnString siteid = doc->getAttrStr(AOSTAG_SITEID, "");
	if (siteid == "")
	{
		smLock->unlock();
		rdata->setError() <<"Siteid is empty!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	AosDocType type = getDocType(docid);
	u64 did = docid & sgDocLocationMask;

	char bb[eDocHeaderSize];
	int length;
	u32 seqno 	= 0;
	u32 seqSys 	= 0;
	u64 offset 	= 0;
	u64 docsize = 0; 
	u64 offSys 	= 0;
	u64 sizeSys = 0;

	u32 docIdxSeqno = did / eMaxHeaderPerFile;
	u32 docIdxOffset = (did % eMaxHeaderPerFile) * eDocHeaderSize;	

	// Read the header
	bool rslt = docfilemgr->readHeader(docIdxSeqno, docIdxOffset, bb, eDocHeaderSize, length);
	if (length <= 0) 
	{
		length = eVersion11HeaderSize;
	}
	else
	{
		version10to11(bb, length);
	}
	AosBuff buff(bb, length, length, 0 AosMemoryCheckerArgs);
	if (!rslt)
	{
		if (!savenew)
		{
			// It is not save new. This is an error
			smLock->unlock();
			if (type != eAosNormalDoc) return true;
			rdata->setError()<< "Failed reading the header (a serious internal error)!!!!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}

		// It is to save new. No need to read the buff. 
	}
	else
	{
		// Ice, 08/04/2011
		rslt = readFromBuff(buff, did, seqno, 
			offset, docsize, seqSys, offSys, sizeSys, savenew, rdata);
		aos_assert_rl(rslt, smLock, false);
	}

	OmnString newdocstr = doc->toString();
	u64 size = newdocstr.length();
	rslt = false;
	switch(type)
	{
	case eAosNormalDoc:
		 // save body
		 rslt = docfilemgr->saveDoc(seqno, offset, size, newdocstr.data());
		 aos_assert_rl(rslt, smLock, false);

		 // update header
		 buff.reset();
		 buff.setU32(eCurrentVersion);
		 buff.setU32(atoi(siteid));
		 buff.setU64(did);
		 buff.setU32(seqno);
		 buff.setU64(offset);
		 buff.setU64(size);
		 buff.setU32(seqSys);
		 buff.setU64(offSys);
		 buff.setU64(sizeSys);
		 break;

	case eAosAccessDoc:
		 // save body
		 rslt = docfilemgr->saveDoc(seqSys, offSys, size, newdocstr.data());
		 aos_assert_rl(rslt, smLock, false);

		 // update header
		 buff.reset();
		 buff.setU32(eCurrentVersion);
		 buff.setU32(atoi(siteid));
		 buff.setU64(did);
		 buff.setU32(seqno);
		 buff.setU64(offset);
		 buff.setU64(docsize);
		 buff.setU32(seqSys);
		 buff.setU64(offSys);
		 buff.setU64(size);
		 break;

	default: 
		 smLock->unlock();
		 OmnAlarm << "Invalid Doc type" << enderr;
		 return false;
	}
	aos_assert_rl(docfilemgr->saveHeader(docIdxSeqno, docIdxOffset, 
				bb, buff.dataLen(), true), smLock, false);
	smLock->unlock();
	return true;
}


bool
AosXmlDoc2::deleteDoc(
		const u64 &docid,
		const AosDocFileMgrPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	// Currently we simply mark it as being deleted.
	// In the future, we need to reuse the space allocated for
	// the doc, and if possible, reuse the docid.
	
	// Read in the header
	smLock->lock();
	if (docid == 0)
	{
		smLock->unlock();
		rdata->setError() << "To delete a doc but the Docid is null";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Get the doc type
	getDocType(docid);
	u64 did = docid & sgDocLocationMask;

	u32 docIdxSeqno = did / eMaxHeaderPerFile;
	u32 os = (did % eMaxHeaderPerFile) * eDocHeaderSize;

	char bb1[eDocHeaderSize];
	int len;
	if (!docfilemgr->readHeader(docIdxSeqno, os, bb1, eDocHeaderSize, len))
	{
		smLock->unlock();
		rdata->setError() <<"Failed reading the header: " << did ;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Ice, 08/04/2011
	version10to11(bb1, len);
	AosBuff buff1(bb1, len, len, 0 AosMemoryCheckerArgs);
	u32 seqno 	= 0;
	u64 offset 	= 0;
	u64 docsize = 0; 
	u32 seqSys 	= 0;
	u64 offSys 	= 0;
	u64 sizeSys = 0;
	if (!readFromBuff(buff1, did, seqno, offset, docsize, seqSys, offSys, sizeSys, false, rdata))
	{
		smLock->unlock();
		return false;
	}

	char bb[eDocHeaderSize];
	AosBuff buff(bb, eDocHeaderSize, 0, 0 AosMemoryCheckerArgs);
	//buff.setU32(eInvVersion);
	buff.setU32(eDelVersion);
	buff.setU32(AOS_INVSID);
	buff.setU64(AOS_INVDID);
	buff.setU32(eAosInvFseqno);
	buff.setU64(eAosInvFoffset);
	buff.setU64(eAosInvDocSize);
	buff.setU32(0);
	buff.setU64(0);
	buff.setU64(0);	

	// Save the header
	bool rslt = docfilemgr->saveHeader(docIdxSeqno, os, bb, buff.dataLen(), true);
	if (!rslt)
	{
		smLock->unlock();
		rdata->setError() << "Failed saving the header";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool func_rslt = true;
	// Delete the normal doc
	if (offset != 0 && docsize != 0)
	{
		bool rslt = docfilemgr->deleteDoc(seqno, offset);
		if (!rslt)
		{
			rdata->setError() << "Failed deleting the doc: " << did 
				<< ". Location: [" << seqno << ", " << offset << "]!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			func_rslt = false;
		}
	}

	if (offSys != 0 && sizeSys != 0) 
	{
		bool rslt = docfilemgr->deleteDoc(seqSys, offSys);
		if (!rslt)
		{
			rdata->setError() << "Failed delete the access record: " << did 
				<< ". Location: [" << seqSys << ", " << offSys << "]!";
			OmnAlarm << rdata->getErrmsg() << enderr;
			func_rslt = false;
		}
	}

	smLock->unlock();
	return func_rslt;
}


bool
AosXmlDoc2::isDocDeleted(
		const u64 &docid,
		const AosDocFileMgrPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid != AOS_INVDID, true);

	smLock->lock();

	// Convert the docid
	u64 did = docid & sgDocLocationMask;

	u32 docIdxSeqno = did / eMaxHeaderPerFile;
	u32 os = (did % eMaxHeaderPerFile) * eDocHeaderSize;
	char bb[eDocHeaderSize];
	int len;
	if (!docfilemgr->readHeader(docIdxSeqno, os, bb, eDocHeaderSize, len))
	{
		smLock->unlock();
		return true;
	}

	// Ice, 08/04/2011
	version10to11(bb, len);

	AosBuff buff(bb, len, len, 0 AosMemoryCheckerArgs);
	u32 ver = buff.getU32(eInvVersion);
	if (ver == eInvVersion) 
	{
		smLock->unlock();
		return true;
	}

	u32 siteid = buff.getU32(AOS_INVSID);
	if (siteid == AOS_INVSID) 
	{
		smLock->unlock();
		return true;
	}
	buff.getU64(0);
	u32 seqno1 =buff.getU32(eAosInvFseqno);
	if (seqno1 ==eAosInvFseqno) 
	{
		smLock->unlock();
		return true;
	}

	u64 offset1 = buff.getU64(eAosInvFoffset);
	if (offset1 == eAosInvFoffset) 
	{
		smLock->unlock();
		return true;
	}

	u64 docsize1 = buff.getU64(eAosInvDocSize);
	if (docsize1 == eAosInvDocSize) 
	{
		smLock->unlock();
		return true;
	}
	smLock->unlock();	
	return false;
}


AosXmlTagPtr
AosXmlDoc2::readFromFile(
		const u64 &docid, 
		const AosDocFileMgrPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	if (docid == AOS_INVDID)
	{
		rdata->setError() << "To read a doc but the docid is NULL";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}
	
	// Get the doc type
	AosDocType type = getDocType(docid);
	u64 did = docid & sgDocLocationMask;

	smLock->lock();
	u32 docIdxSeqno = did / eMaxHeaderPerFile;
	u32 os = (did % eMaxHeaderPerFile) * eDocHeaderSize;
	char bb[eDocHeaderSize];
	int len;	

	// Read the header
	if (!docfilemgr->readHeader(docIdxSeqno, os, bb, eDocHeaderSize, len))
    {
		smLock->unlock();
		rdata->setError() << "Failed reading the header: " << did;
        return 0;
    }
 
	// Ice, 08/04/2011
	version10to11(bb, len);
	AosBuff buff(bb, len, len, 0 AosMemoryCheckerArgs);
	u32 seqno 	= 0;
	u64 offset 	= 0;
	u64 docsize = 0; 
	u32 seqSys 	= 0;
	u64 offSys 	= 0;
	u64 sizeSys = 0;
	if (!readFromBuff(buff, did, seqno, offset, docsize, seqSys, offSys, sizeSys, false, rdata))
	{
		smLock->unlock();
		return 0;
	}

	bool rslt = false;
	switch(type)
	{
	case eAosNormalDoc:
		 {
			//read header by docid
			//read doc from file
			OmnConnBuffPtr docBuff = OmnNew OmnConnBuff(docsize);
			aos_assert_rl(docBuff->determineMemory(docsize), smLock, NULL);

			rslt = docfilemgr->readDoc(seqno, offset, docBuff->getBuffer(), docsize);
			aos_assert_rl(rslt, smLock,  NULL);

			docBuff->setDataLength(docsize);
		 
			AosXmlParser parser;
			AosXmlTagPtr doc = parser.parse(docBuff, "" AosMemoryCheckerArgs);
			aos_assert_rl(doc, smLock, NULL);
			smLock->unlock();
			return doc;
		 }
		 break;

	case eAosAccessDoc:
		 {
			 if (seqSys == 0 && offSys == 0) 
			 {
				 smLock->unlock();
				 return NULL;
			 }
			 OmnConnBuffPtr docBuff = OmnNew OmnConnBuff(sizeSys);
			 aos_assert_rl(docBuff->determineMemory(sizeSys), smLock, NULL);
			 rslt = docfilemgr->readDoc(seqSys, offSys, docBuff->getBuffer(), sizeSys);
			 aos_assert_rl(rslt, smLock,  NULL);
			 docBuff->setDataLength(sizeSys);
			 AosXmlParser parser;
			 AosXmlTagPtr doc = parser.parse(docBuff, "" AosMemoryCheckerArgs);
			 aos_assert_rl(doc, smLock, NULL);
			 smLock->unlock();
			 return doc;
		 }
		 break;

	default:
		 break;
	}
	smLock->unlock();
	rdata->setError()<< "Unrecognized doc type: " << type;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return NULL;
}


OmnString
AosXmlDoc2::readHeader(
		const u64 &docid,
		const AosDocFileMgrPtr &docfilemgr,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid != AOS_INVDID, "Invalid Docid");
	
	smLock->lock();

	//read header
	u32 docIdxSeqno = docid / eMaxHeaderPerFile;
	u32 os = (docid % eMaxHeaderPerFile) * eDocHeaderSize;
	char bb[eDocHeaderSize];
	int len;	

	if (!docfilemgr->readHeader(docIdxSeqno, os, 
        bb, eDocHeaderSize, len))
    {
		smLock->unlock();
        return "";
    }
 
	//read body information from header	
	AosBuff buff(bb, len, len, 0 AosMemoryCheckerArgs);

	OmnString str;
	str << "Docid: " << docid
		<< "Version : " << buff.getU32(eInvVersion)
		<< ". Siteid: " << buff.getU32(AOS_INVSID)
		<< ". Docid: " << buff.getU64(0)
		<< ". Seqno: " << buff.getU32(eAosInvFseqno)
		<< ". Offset: " << buff.getU64(eAosInvFoffset)
		<< ". Docsize: " << buff.getU64(eAosInvDocSize); 
	smLock->unlock();
    return str;
}


// Ice, 08/04/2011
void
AosXmlDoc2::version10to11(char *bb, int &length)
{
	AosBuff bbb(bb, length, length, 0 AosMemoryCheckerArgs);
	u32 vvv = bbb.getU32(eInvVersion);
	if (vvv == eVersion10)
	{
		length += 20;
	}
}


bool
AosXmlDoc2::readFromBuff(
		AosBuff &buff, 
		const u64 &did,
		u32 &seqno, 
		u64 &offset, 
		u64 &docsize, 
		u32 &seqSys, 
		u64 &offSys, 
		u64 &sizeSys, 
		const bool savenew,
		const AosRundataPtr &rdata)
{
	// 	version		u32
	// 	siteid		u32
	// 	docid		u64
	// 	seqno		u32
	// 	offset		u64
	//	docsize		u64
	//	seqsys		u32
	//	offsys		u64
	//	sizesys		u64
	u32 ver	= buff.getU32(eInvVersion); // version
	if (ver == eDelVersion)
	{
		if (savenew)
		{
			seqno = 0;
			offset = 0;
			docsize = 0;
			seqSys = 0;
			offSys = 0;
			sizeSys = 0;
			return true;
		}
		return false;
	}

	if (ver == eInvVersion) 
	{
		rdata->setError() << "Invalid version: ";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u32 site 	= buff.getU32(AOS_INVSID);  // site
	if (site == AOS_INVSID)
	{
		rdata->setError() = "Invalid siteid";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	u64 ddd		= buff.getU64(0);			// docid
	if (ddd != did)
	{
		OmnAlarm << "Invalid header: docid mismatch. Expecting: "
			<< did << " but got: " << ddd
			<< ". Will reset!" << enderr;
	}

	seqno   = buff.getU32(0);           // doclocation
	offset  = buff.getU64(0);           // doclocation
	docsize = buff.getU64(0);			// docsize
	seqSys	= 0;
	offSys	= 0;
	sizeSys = 0;

// Chen Ding, Temporary
	if (ver == eCurrentVersion)
	{
		seqSys	= buff.getU32(0);	  	    // system info seqno 
		offSys	= buff.getU64(0);	  	    // system info offset 
		sizeSys = buff.getU64(0);	  	    // system info size
	}

	if (offset == 0 && offSys == 0)
	{
		rdata->setError() << "Invalid offset: " << offset;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if (docsize == 0 && sizeSys == 0)
	{
		rdata->setError() << "Invalid docsize!";
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	return true;
}

u64
AosXmlDoc2::getOwnDocid(const u64 &docid)
{
	u64 did = docid & sgDocLocationMask;
	return did;
}                                       
#endif
