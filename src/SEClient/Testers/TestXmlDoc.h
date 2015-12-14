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
//
// Modification History:
// 01/12/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_Testers_TestXmlDoc_h
#define Aos_SEClient_Testers_TestXmlDoc_h

#include "SearchEngine/Testers/Ptrs.h"
#include "Tester/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "Util/String.h"


class AosTestXmlDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxMetadata = 100,
		eTextMaxWords = 20,
		eArrayInitSize = 10,
		eArrayIncSize = 10,
		eArrayMaxSize = 1000,
		eAWInitSize = 20,
		eAWIncSize = 10,
		eAWMaxSize = 10000,
		eAW1InitSize = 5000,
		eAW1IncSize = 1000,
		eAW1MaxSize = 20000,
		eMaxSubtags = 100,
		eMaxAttrs = 300,
		eMaxAttrWords = 100
	};

	struct Attr
	{
		OmnString name;
		OmnString value;
	};

	OmnString			mTagname;
	OmnString			mTexts;
	bool				mIsCdata;
	OmnString			mDocid;
	OmnString			mPath;
	AosTestXmlDocPtr	mSubtags[eMaxSubtags];
	int					mNumSubtags;
	Attr				mAttrs[eMaxAttrs];
	int					mMaxQueryFnames;
	int					mNumAttrs;
	OmnString			mSiteid;
	OmnString			mObjid;

	static OmnString	mAttrTname1;
	static OmnString	mAttrwdTname1;
	static OmnString	mDocwdTname1;
	static OmnString	mTagTname1;
	static OmnString	mDocTname1;

public:
	AosTestXmlDoc(
			const OmnString &siteid, 
			const OmnString &docid, 
			const OmnString &objid, 
			const int depth);
	AosTestXmlDoc(const OmnString &path, const int depth);
	~AosTestXmlDoc();

	static bool		 init(const OmnString &attrtname, 
						const OmnString &awdtname, 
						const OmnString &docwdtname, 
						const OmnString &tagtname, 
						const OmnString &doctname);
	static OmnString getAttrname(const OmnString &siteid);
	static OmnString getAttrvalue(const OmnString &siteid, 
						const OmnString &aname, 
						const OmnString &docid);
	static OmnString 	getDocWord();
	static OmnString 	getTexts();
	static OmnString 	getCommonWords();
	static OmnString	getRandomSiteid();
	static void 	 	initPriv();
	static bool			updateCounters();

	OmnString	getObjid() const {return mObjid;}
	OmnString	getSiteid() const {return mSiteid;}
	bool	getAllWords(OmnString &words, int &numWords);
	void 	generateTagName();
	bool 	generateAttrs();
	bool 	generateTexts();
	void 	generateSubtags(const int depth);
	void	composeData(OmnString &data);
	bool 	getFnames(const OmnString &siteid, OmnString &fnames);
	bool 	saveWords();
	void	setDocid(const OmnString &docid);
	void	setSiteid(const OmnString &s);
	OmnString getTagname1() {return mTagname;}
	OmnString getRandomTagname();
	void	setPath(const OmnString &p) {mPath = p;}

private:
	bool	init(const int depth);
	bool	saveDocWord(const OmnString &word);
	bool	addMetadata(const int depth);
	bool	addAttr(const OmnString &, const OmnString &);
};
#endif
