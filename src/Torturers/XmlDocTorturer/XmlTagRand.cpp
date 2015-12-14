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
#include "Torturers/XmlDocTorturer/XmlTagRand.h"

#include "Random/RandomUtil.h"
#include "SengTorUtil/StUtil.h"
#include "SengTorUtil/StDoc.h"
#include "SengTorUtil/TesterXmlDoc.h"
#include "SengTorUtil/StOprTranslator.h"
#include "Tester/TestPkg.h"
#include "Util/UtUtil.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosXmlTagRand::AosXmlTagRand():
mModifyCrtTag(50),
mAddCrtTag(50),
mDeleteCrtTag(50),
mRemoveCrtTag(50),
mNumLevel(0),
mDoc(0)
{
}


AosXmlTagRand::~AosXmlTagRand()
{
}


OmnString	
AosXmlTagRand::pickKeyword()
{
	return "";
}


bool		
AosXmlTagRand::modifyAttr(
		const AosXmlTagPtr &doc, 
		OmnString &name, 
		OmnString &oldvalue, 
		OmnString &newvalue)
{
	// 1. Determine whether to modify an attribute at this level or one of its
	//    subtags. 
	// 2. If it is at this level, randomly pick an attribute. Then pick a new
	//    vaule, modify it. 
	// If it cannot modify any attribute, 'name' is set to "".
	//int num_attrs = doc->getNumAttrs();
	mCrtOpr = "modifyAttr";
	if (!doc->mRoot)
	{
		setTagLevel(0,"");
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		return modifyAttr(doc->mChildTags[0], name, oldvalue, newvalue);
	}

	// 1. Determine whether to modify this tag or one of its subtags. 
	if (doc->mChildTags.entries() == 0 || (rand()%100) < mModifyCrtTag)
	{
		if (doc->mAttrs.entries() == 0)
		{
			name = "";
			return true;
		}

		int idx = rand() % doc->mAttrs.entries();
		char *buffData = doc-> mBuff->getData();
		int nstart = doc->mAttrs[idx].nameStart;
		int nend = doc->mAttrs[idx].nameEnd;
		name.assign((const char *)&buffData[nstart], nend-nstart+1);
		if (name == "zky_mtmepo")
			OmnMark;
		oldvalue = doc->getAttrStr(name);
		newvalue = AosGeneratePasswd();

		bool rslt = canAttrModified(name);
		if (!rslt || name == AOSTEST_DOCID || name == AOSTAG_DOCID || name == AOSTAG_OBJID)
		{
			name = "";
			return true;
		}

		doc->setAttr(name, newvalue);
		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		name = "";
		return true;
	}
	
	int idx = rand() % doc->mChildTags.entries();

	OmnString tagname = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tagname);
	return modifyAttr(doc->mChildTags[idx], name, oldvalue, newvalue);

}

bool		
AosXmlTagRand::addAttr(
			const AosXmlTagPtr &doc,
			OmnString &name, 
			OmnString &newvalue)
{
	// 1. Determine whether to add an attribute at this level or one of its
	//    subtags. 
	// 2. If it is at this level, randomly pick an attribute. Then pick a new
	//    vaule, add it. 
	// If it cannot add any attribute, 'name' is set to "".

	// 1. Determine whether to add this tag or one of its subtags. 
	mCrtOpr = "addAttr";
	if (!doc->mRoot)
	{
		setTagLevel(0,"");
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		return addAttr(doc->mChildTags[0], name, newvalue);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
		name = "attrname";
		name <<rand()%100;
		//if (doc->getAttrStr(name)!="") 
		//{
		//	name = "";
		//	return true;
		//}
		newvalue = AosGeneratePasswd();
		doc->setAttr(name, newvalue);
		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		name = "";
		return true;
	}

	int idx = rand() % doc->mChildTags.entries();
	OmnString tagname = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tagname);
	return addAttr(doc->mChildTags[idx], name, newvalue);
}

bool		
AosXmlTagRand::deleteAttr(
			const AosXmlTagPtr &doc,
			OmnString &name)
{
	// 1. Determine whether to delete an attribute at this level or one of its
	//    subtags. 
	// 2. If it is at this level, randomly pick an attribute. Then pick a new
	//    vaule, delete it. 
	// If it cannot delete any attribute, 'name' is set to "".

	// 1. Determine whether to delete this tag or one of its subtags. 
	
	mCrtOpr = "deleteAttr";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0,"");
		return deleteAttr(doc->mChildTags[0], name);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mDeleteCrtTag)
	{
		if (doc->mAttrs.entries() == 0)
		{
			name = "";
			return true;
		}

		int idx = rand() % doc->mAttrs.entries();
		char *buffData = doc->mBuff->getData();
		int nstart = doc->mAttrs[idx].nameStart;
		int nend = doc->mAttrs[idx].nameEnd;
		name.assign((const char *)&buffData[nstart], nend-nstart+1);
		bool rslt = canAttrModified(name);
		if (!rslt || name == AOSTEST_DOCID || name == AOSTAG_DOCID || name == AOSTAG_OBJID)
		{
			name = "";
			return true;
		}
		
		if (doc->getAttrStr(name) != "")
			doc->removeAttr(name);
		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		name = "";
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tagname = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tagname);
	return deleteAttr(doc->mChildTags[idx], name);

}

bool		
AosXmlTagRand::removeTag(const AosXmlTagPtr &doc, OmnString &tagname)
{
	mCrtOpr = "removeTag";
	if (!doc->mRoot)
	{
		setTagLevel(0, "");
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		return removeTag(doc->mChildTags[0], tagname);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mRemoveCrtTag)
	{
		if (doc->mAttrs.entries() == 0)
		{
			tagname = "";
			return true;
		}
		int num = doc->getNumSubtags();
		if (num ==0) 
		{
			tagname = "";
			return true;
		}

		int idx =  rand() % num;
		if (idx >= 0 && idx < doc->mChildTags.entries())
		{
			tagname = doc->mChildTags[idx]->getTagname();
		}

		doc->removeNode(idx);
		if (doc->getFirstChild(tagname))tagname = "";
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return removeTag(doc->mChildTags[idx], tagname);
}

bool		
AosXmlTagRand::addTag(AosXmlTagPtr &doc, OmnString &tagname)
{
	mCrtOpr = "addTag";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0, "");
		return addTag(doc->mChildTags[0], tagname);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
		//int idx = rand() % doc->mAttrs.entries();
		tagname = "tagname";
		tagname <<rand()%100;
		//if (doc->getAttrStr(name)!="") return true;
		//newvalue = AosGeneratePasswd();
		//doc->setAttr(name, newvalue);
		aos_assert_r(doc->addNode1(tagname), false);
		OmnString n, na;
		int ra = rand()%5;
		AosXmlTagPtr dd = doc->getFirstChild(tagname);
		for (int j = 0; j< ra; j++)
		{
			addAttr(dd, n, na);
		}
		mCrtOpr = "addTag";
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return addTag(doc->mChildTags[idx], tagname);

	/*
	int idx = rand() % doc->mChildTags.entries();
	int vv = rand() % 100;
	int level;
	if (vv < 50) level = 0;
	else if (vv < 90) level = 0;
	else level = 2;

	AosTestXmlDoc xml("", level);
	AosXmlTagPtr strxml = xml.createNewDoc();
	aos_assert_r (strxml, false);
	OmnString str;
	str << (char *) doc->getData();
	aos_assert_r(doc->addNode1(str), false);
	*/
}

bool		
AosXmlTagRand::addTextWords(const AosXmlTagPtr &doc, OmnString &texts)
{
	mCrtOpr = "addTextWords";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0, "");
		return addTextWords(doc->mChildTags[0], texts);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
		if (doc->mNumTexts == 0)
		{
			 texts ="";
			return true;
		}
		int len;
		char *data = (char *) doc->getNodeText(len);	
		OmnString text(data, len);
		AosTestXmlDoc xml("", 2);
		OmnString word = xml.getDocWord();
		if (text.length() ==0) text  = word;
		else text.replace(rand() % text.length(), 0, word);
		doc->setText(text, true);
		//doc->setText(text, (rand()%100) > 50);
		texts << text;
		return true;
	}
	
	if (doc->mChildTags.entries() == 0)
	{
		texts = "";
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return addTextWords(doc->mChildTags[idx], texts);
}

bool
AosXmlTagRand::removeTextWords(const AosXmlTagPtr &doc, OmnString &texts)
{
	mCrtOpr = "removeTextWords";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0, "");
		return removeTextWords(doc->mChildTags[0], texts);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
		if (doc->mNumTexts == 0)
		{
			texts ="";
			return true;
		}

OmnString data1 = doc->toString();
		int len;
		char *data = (char *) doc->getNodeText(len);
		OmnString text(data, len);
		if (text.length() == 0) 
		{
			texts = "";
			return true;
		}

		AosTestXmlDoc xml("", 2);
		OmnString word = xml.getDocWord();
		int vv = rand();
		int removelen = vv %text.length();
		int nn = text.length() - removelen;
		int idx = 0;
		if (nn > 0) idx = vv % nn;

		if (len>=12 && strncmp(data, "<![CDATA[", 9) == 0)
		{
			if (idx < 9 || idx >= len-3) 
			{
				texts = "";
				return true;
			}
		}
		text.replace(idx, removelen, "");
		doc->setText(text, true);
OmnString data2 = doc->toString();
AosXmlParser parser;
AosXmlTagPtr doc1 = parser.parse(data2,"" AosMemoryCheckerArgs);
		//doc->setText(text, (rand()%100) > 60);
		texts << text;
		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		texts = "";
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return removeTextWords(doc->mChildTags[idx], texts);
}

bool
AosXmlTagRand::modifyTexts(const AosXmlTagPtr &doc, OmnString &name, OmnString &oldname)
{
	mCrtOpr = "modifyTexts";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0, "");
		return modifyTexts(doc->mChildTags[0], name, oldname);
	}

	if (doc->mNumTexts == 0)
	{
		name = "";
		return true;
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
		int len;
		char *data = (char *)doc->getNodeText(len);
		OmnString texts(data, len);
		AosTestXmlDoc xml("", 2);
		if (texts.length() == 0)
		{
			name = "";
			return true;	
		}
	
		int vv = rand();
		int removelen = vv % texts.length();
		int nn = texts.length() - removelen;
		int idx = 0;
		if (nn > 0) idx = vv % nn;

		//OmnString newstr = xml.getTexts();
		int len1 = (rand() % 50) + 5;
		char data3[60];
		AosRandomLetterStr(len1, data3);
		OmnString newstr = data3;

		texts.replace(idx, removelen, newstr);	
		oldname = doc->getNodeText();
		doc->setText(texts, (rand()%100) > 60);
		name << texts;
		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		name = "";
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return modifyTexts(doc->mChildTags[idx], name, oldname);
}

bool
AosXmlTagRand::addTexts(const AosXmlTagPtr &doc, OmnString &tagname, OmnString &texts)
{
	mCrtOpr = "addTexts";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0, "");
		return addTexts(doc->mChildTags[0], tagname, texts);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
OmnString data1 = doc->toString();
		AosTestXmlDoc xml("", 2);
		/*
		int len = (rand() % 10) + 5;
		char data3[40];
		AosRandomLetterStr(len, data3);
		tagname =  data3;
		*/
		tagname = xml.getRandomTagname(); 
		OmnString word = xml.getTexts();
		//doc ->setNodeText(tagname, word,(rand()%100) >60);
		doc ->setNodeText(tagname, word, true);
		texts<<word;

OmnString data = doc->toString();
AosXmlParser parser;
AosXmlTagPtr doc1 = parser.parse(data,"" AosMemoryCheckerArgs);

		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		tagname ="";
		texts = "";
		return true;
	}

	int idx = rand() % doc->mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return addTexts(doc->mChildTags[idx], tagname, texts);
}

bool
AosXmlTagRand::removeTexts(const AosXmlTagPtr &doc)
{
	mCrtOpr = "removeTexts";
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		setTagLevel(0, "");
		return removeTexts(doc->mChildTags[0]);
	}

	if (doc->mChildTags.entries() == 0 || (rand()%100) < mAddCrtTag)
	{
		if (doc->mNumTexts == 0)
		{
			return true;
		}

		doc->removeTexts();
		return true;
	}

	if (doc->mChildTags.entries() == 0)
	{
		return true;
	}

	int idx = rand() % doc-> mChildTags.entries();
	OmnString tag = doc->mChildTags[idx]->getTagname();
	setTagLevel(idx, tag);
	return removeTexts(doc->mChildTags[idx]);
}

bool
AosXmlTagRand::canAttrModified(const OmnString &aname)
{
	// Following attributes cannot be modified
	// AOSTAG_VERSION
	// AOSTAG_SITEID
	// AOSTAG_DOCID
	// AOSTAG_OBJID
	// AOSTAG_CLOUDID
	// AOSTAG_CREATOR
	// AOSTAG_FULLDOC_HASH
	// AOSTAG_OTYPE
	// All standard attributes start with 'zky_'.
	//
	// if 'aname' is in the form:
	//      AosIILName::composeAttrIILName(aname) 
	// then we need check the following attributes:
	// AOSTAG_VERSION
	// AOSTAG_SITEID
	// AOSTAG_DOCID
	// AOSTAG_OBJID
	//if (len <=5 ) return true;
	//if (aname[0] == '_' &&
	//	aname[1] == 'z' &&
	//	aname[2] == 't' &&
	//	aname[3] == '1' &&
	//	aname[4] == '9')
	//{
	//	if (len <= 9) return true;
	if (strcmp(aname.data(), AOSTAG_VERSION) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_OTYPE) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_SITEID) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_DOCID) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_OBJID) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_MTIME) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_MT_EPOCH) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_MODUSER) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_USERNAME) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_CLOUDID) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_CREATOR) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_FULLDOC_SIGNATURE) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_PARENTC) == 0) return false;
	if (strcmp(aname.data(), AOSTAG_MEMBEROF) == 0) return false;
	//	return true;
	//}
	return true;
}

bool
AosXmlTagRand::randRootAttr(
		const AosXmlTagPtr &doc,
		OmnString &aname,
		OmnString &avalue)
{
	if (!doc->mRoot)
	{
		aos_assert_r(doc->mChildTags.entries() == 1 ,0);
		return randRootAttr(doc->mChildTags[0], aname, avalue);
	}

	int numAttrs = doc->getNumAttrs();
	int idx = rand() % numAttrs;
	bool finished = false;
	OmnString path;
	doc->resetAttrNameValueLoop();
	int i = 0;
	while(!finished)
	{
		if (!doc->nextAttr(path, aname, avalue, finished, false)||finished) break;
		if (i++ == idx) break;
	}
	return true;
}
