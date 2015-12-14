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
// 03/03/2012	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_SmartDoc_SdocImportDocument_h
#define Aos_SmartDoc_SdocImportDocument_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include <vector>

class AosSdocImportDocument: public AosSmartDoc
{
	OmnString					mLogSmtDocObjid;
	OmnString					mLogContainerObjid;
	int							mLoopTmp;
	OmnString					mDocumentCtnr;
	OmnString					mZipFormat;
	OmnString 					mRmDataDir;
	OmnString					mFileName;
	OmnString 					mUuicomTitlePctrs;
	OmnString 					mUuicomFilePctrs;
	map<OmnString, OmnString>	mAttrMap;
	map<OmnString, int>			mFileStart;

public:
	AosSdocImportDocument(const bool flag);
	~AosSdocImportDocument();

	virtual AosSmartDocObjPtr clone() {return OmnNew AosSdocImportDocument(false);}
	virtual bool run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata);

private:
	bool			readUnicomFileBuff(
						AosBuffDataPtr &info,
						const AosRundataPtr &rdata);
	
	bool 			readXmlFindFile(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &doc,
						AosBuffPtr &zip_buff,
						vector<AosBuffPtr> &new_buffs,
						vector<AosXmlTagPtr> &new_xmls);

	bool			addAttrForUpload(
						const AosXmlTagPtr &doc, 
						const AosRundataPtr &rdata);

	bool			finishedAction(const AosRundataPtr &rdata);

	OmnString 		unzip(      
				        OmnString &zipPath,
					    const OmnString &fname);
	
	bool 			xmlParse(             
				        const AosRundataPtr &rdata,
					    const AosXmlTagPtr &xml);     

	vector<AosXmlTagPtr> getDomainItem(       
				        const AosXmlTagPtr &doc,
				        int loopNum,    
					    const AosRundataPtr &rdata);
	
	bool			importData(
						const AosBuffPtr &buff,
						const AosXmlTagPtr &binary_doc,
						const AosRundataPtr &rdata);

	OmnString		convertWordPdf(
						const OmnString &dir,
						const OmnString &source_fname,
						const OmnString &document_type);

	OmnString		getPreDocStr(
						const AosXmlTagPtr &doc,
						const OmnString &sfname,
						const AosRundataPtr &rdata);
	
	bool			proc(
						const OmnString &predocstr,
						const OmnString &fname, 
						const AosRundataPtr &rdata);

	OmnString		composeEntry(
						OmnString &filestr, 
						const int partnum,
						const OmnString &predocstr);
};
#endif
#endif
