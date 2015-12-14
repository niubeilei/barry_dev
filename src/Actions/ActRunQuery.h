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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActRunQuery_h
#define Aos_SdocAction_ActRunQuery_h

#include "Actions/SdocAction.h"
#include "Actions/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Rundata/Rundata.h"
#include <vector>


class AosActRunQuery : virtual public AosSdocAction
{
	struct batchquery
	{
		AosXmlTagPtr 	config;
		AosXmlTagPtr 	querydoc;
		OmnString 		result;
		bool			hasdocid;
		bool			firstway;
		AosXmlTagPtr	query_template;
		AosXmlTagPtr	replace_entries;
		AosXmlTagPtr	createdoc_action;
		int				max_of_record;
		bool			hasquerydoc;
		OmnString		whereisvalue;
		map<int, pair<OmnString, OmnString> >	fields;
		OmnString 		filetype;
		OmnString       codingType;
	};

	enum sendmethods
	{
		eSendResult = 1,
		eSendWait = 2 
	};

public:
	AosActRunQuery(const bool flag);
	~AosActRunQuery();

	virtual bool run(const AosXmlTagPtr &def, const AosRundataPtr &rdata);
	virtual AosActionObjPtr clone( const AosXmlTagPtr &def, const AosRundataPtr &rdata) const;
	//batch query start
private:
	bool			parseConfig(const AosXmlTagPtr &config, struct batchquery &bq, const AosRundataPtr &rdata);
	AosXmlTagPtr	procQuery(struct batchquery &bq, OmnFilePtr &file, AosXmlTagPtr &doc, const AosRundataPtr &rdata, bool &tryquery);
	bool	 		saveQueryToFile(
						struct batchquery &bq, 
						AosXmlTagPtr &doc, 
						OmnFilePtr &file, 
						const bool savefile,
						bool firstline,
						const AosRundataPtr &rdata);

	bool			sendMsgToFront(
						const AosXmlTagPtr &doc,
						const int method,
						const AosRundataPtr &rdata);

	bool			createDoc(const struct batchquery &bq, AosXmlTagPtr &doc, const AosRundataPtr &rdata);//get doc from working doc
	bool			openUserFile(const struct batchquery &bq, OmnString &filepath, OmnString &filename, OmnFilePtr &file, const AosRundataPtr &rdata);
	bool			closeUserFile(const struct batchquery &bq, OmnFilePtr &file, const AosRundataPtr &rdata);

	class BatchQuery: public OmnThrdShellProc
	{
		OmnDefineRCObject;

		AosActRunQuery		*mActQuery;
		struct batchquery	mBq;
		OmnFilePtr			mFile;
		AosXmlTagPtr		mDoc;
		AosRundataPtr		mRdata;

	public:
		BatchQuery(
				AosActRunQuery *obj,
				const struct batchquery bq,
				const OmnFilePtr &file,
				const AosXmlTagPtr &doc,
				const AosRundataPtr &rdata)
		:
		OmnThrdShellProc("BatchQuery"),
		mActQuery(obj),
		mBq(bq),
		mFile(file),
		mDoc(doc),
		mRdata(rdata->clone(AosMemoryCheckerArgsBegin))
		{
		}
		virtual	bool	run();
		virtual bool	procFinished();
	};
};
#endif

