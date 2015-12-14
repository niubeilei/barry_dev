////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/26/2010 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SengTorturer_XmlTagRand_h
#define AOS_SengTorturer_XmlTagRand_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/DynArray.h"
#include "XmlUtil/XmlTag.h"
#include "SengTorturer/Ptrs.h"
#include "Tester/TestMgr.h" 


class AosXmlTagRand : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxSubtags = 100
	};

	int 		mModifyCrtTag;
	int 		mAddCrtTag;
	int 		mDeleteCrtTag;
	int 		mRemoveCrtTag;
	
	int			mNumLevel;

	AosXmlTag *	mDoc;
	OmnString 	mCrtOpr;
public:
	int 		mSubLevel[eMaxSubtags];
	OmnString 	mSubTagName[eMaxSubtags];

public:
	AosXmlTagRand();
	~AosXmlTagRand();

	OmnString	pickKeyword();
	OmnString 	getCrtOpr(){return mCrtOpr;}
	void	setTagLevel(const int &value,const OmnString &tagname)
			{
				mSubLevel[mNumLevel] = value;
				mSubTagName[mNumLevel] = tagname;
				mNumLevel ++;
			}

	void 	initSub()
			{
				mNumLevel = 0;
				for (int i = 0; i<eMaxSubtags; i++)
				{
					mSubLevel[i] = 0;
					mSubTagName[i] = "";
				}
			}
	int		getTotalNumLevel(){return mNumLevel;}
	bool	modifyAttr(
					const AosXmlTagPtr &doc, 
					OmnString &name, 
					OmnString &oldvalue, 
					OmnString &newvalue);

	bool	addAttr(
					const AosXmlTagPtr &doc,
					OmnString &name, 
					OmnString &newvalue);

	bool	deleteAttr(
					const AosXmlTagPtr &doc,
					OmnString &name);

	bool	removeTag(const AosXmlTagPtr &doc,OmnString &tagname);

	bool	addTag(AosXmlTagPtr &doc, OmnString &tagname);
	bool	addTextWords(const AosXmlTagPtr &doc, OmnString &texts);
	bool	removeTextWords(const AosXmlTagPtr &doc, OmnString &texts);
	bool	modifyTexts(const AosXmlTagPtr &doc, OmnString &name, OmnString &oldname);
	bool	addTexts(const AosXmlTagPtr &doc, OmnString &tagname, OmnString &texts);
	bool	removeTexts(const AosXmlTagPtr &doc);
	bool	canAttrModified(const OmnString &aname);
	bool	randRootAttr(const AosXmlTagPtr &doc, OmnString &aname,	OmnString &avalue);
};
#endif

