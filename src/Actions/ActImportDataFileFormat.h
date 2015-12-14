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
//
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActImputDataFileFormat_h
#define Aos_SdocAction_ActImputDataFileFormat_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosActImportDataFileFormat : virtual public AosSdocAction
{

public:
	AosActImportDataFileFormat(const bool flag);
	~AosActImportDataFileFormat();

	virtual bool run(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata);

	virtual AosActionObjPtr clone(
			const AosXmlTagPtr &def, 
			const AosRundataPtr &rdata) const;

	bool getFileName(
			vector<OmnString> &file_names,
			const OmnString &path,
			const int rec_level, 
			const int crt_level, 
			const AosRundataPtr &rdata);

	bool readFileOfFileFrmat(
			vector<OmnString> &file_names,
			const bool is_access_record,
			const AosRundataPtr &rdata);

	bool readFileOfXmlFrmat(
			vector<OmnString> &file_names,
			const bool isneed_seqno,
			const bool is_access_record,
			const AosRundataPtr &rdata);

	bool readFileOfRecordFrmat(
			vector<OmnString> &file_names,
			const bool is_need_attr,
			const bool is_access_record,
			const OmnString &ctnr,
			const OmnString &sep,
			const OmnString &logpath,
			const AosRundataPtr &rdata);

	bool writeAccessDoc(
			const AosXmlTagPtr &doc,
			const AosRundataPtr &rdata);
};

#endif

