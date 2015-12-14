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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActUnCreatFile_h
#define Aos_SdocAction_ActUnCreatFile_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"

class AosActUnCreatFile : virtual public AosSdocAction
{
private:
	OmnString  				mPctrs;
	OmnString  				mUnZipFilePath;


public:
	AosActUnCreatFile(const bool flag);
	~AosActUnCreatFile();
	
	virtual bool run(				const AosXmlTagPtr &def,
									const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone(	const AosXmlTagPtr &def,
									const AosRundataPtr &rdata) const;

private:
	OmnString unzip(		OmnString &zipPath);
	bool isXMLFile(			const OmnString fileName);
	bool readXmlFindFile(   const AosRundataPtr &rdata,
						    AosXmlTagPtr &doc,
							const OmnString xml_path,
							AosXmlTagPtr &newDoc);
	bool getZIPFile(        const AosRundataPtr &rdata,
						    const OmnString xml_path,
							OmnString &zip_path);
	bool readXmlFile(	    const AosRundataPtr &rdata,
						    const OmnString &path);
	bool xmlParse(			const AosRundataPtr &rdata,
							AosXmlTagPtr &xml);

};
#endif

