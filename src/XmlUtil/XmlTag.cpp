///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlUtil/XmlTag.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Debug/Rslt.h"
#include "IILUtil/IILTypes.h"
#include "SEUtil/DocTags.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/SeTypes.h"
#include "Thread/Mutex.h"
#include "XmlUtil/SeXmlParser.h"
// #include "XmlUtil/XmlDoc.h"
#include "XmlUtil/SeXmlUtil.h"
#include "Util/Buff.h"
#include "Util/OmnNew.h"
#include "SEBase/SecUtil.h"
#include "Util/File.h"
#include "Util/StrParser.h"
#include "UtilComm/ConnBuff.h"
#include "WordParser/WordParser.h"
#include "XmlParser/XpUtil.h"
#include "util2/value.h"
#include "util2/valconv.h"

#define AOS_DFT_ATTR_NAMEVALUE_SEP "/"

// Character map for tags
const char sgTagMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 1, 1, 1, 0, 0, // 32 (space), 33('!'), 35('#'), 36('$'), 37('%')
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 40('('), 41(')'), 42('*'), 43('+'), 
								  // 44(','), 45('-'), 46('.'), 47('/'), 0-1
	1, 1, 1, 1, 1, 1, 1, 1, 0, 1, // 2-9, 49(';')
	0, 1, 0, 1, 1, 1, 1, 1, 1, 1, // 61('='), 63('?'), 64('@'), A-E
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F-O
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // P-Y
	1, 1, 0, 1, 1, 1, 1, 1, 1, 1, // Z, 91('['), 93(']'), 94('^'), 95('_'), a-c
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // d-m
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // n-w
	1, 1, 1, 1, 1, 1, 1, 0 		  // x-z, 123('{'), 124('|'), 125('}'), 126('~')
};

// Characters allowed for otype are letters, digits only
const char sgObjidMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 0, 1, 1, 1, 0, 0, // 32 (space), 33('!'), 35('#'), 36('$'), 37('%')
	1, 1, 0, 1, 0, 1, 1, 1, 1, 1, // 40('('), 41(')'), 43('+'), 45('-'), 46('.')
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, // 2-9, 
	0, 0, 0, 0, 1, 1, 1, 1, 1, 1, // 64('@'), A-E
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F-O
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // P-Y
	1, 1, 0, 1, 1, 1, 1, 1, 1, 1, // Z, 91('['), 93(']'), 94('^'), 95('_'), a-c
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // d-m
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // n-w
	1, 1, 1, 1, 1, 1, 1, 0 		  // x-z, 123('{'), 124('|'), 125('}'), 126('~')
};

// Characters allowed for otype are letters, digits only
const char sgOtypeMap[128] = 
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, // 0-1
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0, // 2-9, 
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1, // 64('@'), A-E
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // F-O
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // P-Y
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1, // Z, a-c
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // d-m
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // n-w
	1, 1, 1, 0, 0, 0, 0, 0 		  // x-z
};


int gAosNumXmlTags = 0;
int gAosLastNumXmlTags = 0;

const bool sgSanityCheck = false;

OmnString AosXmlTag::mAname = ""; 
set<OmnString> AosXmlTag::smMetaAttrs;

bool AosXmlTagSanityCheck(AosXmlTag *tag)
{
	if (!sgSanityCheck) return true;

	AosXmlTagPtr root = tag->getRoot();
	if (root) 
	{
		aos_assert_r(root->sanityCheck(), false);
	}
	else 
	{
		aos_assert_r(tag->sanityCheck(), false);
	}	
	return true;
}



AosXmlTag::AosXmlTag(AosMemoryCheckDeclBegin)
:
mRoot(0),
mParentTag(0),
mTagStart(0),
mTagEnd(-1),
mTagnameStart(0),
mTagnameEnd(0),
mNumTexts(0),
mAttrLoopStatus(eInvalidALS),
mWordAttrLoop(0),
mAttrNextTagLoop(0),
mAttrnameTried(false),
mAttrIdx(0),
mTagLoop(0),
mDocNameIdx(0),
mDocNameStart(0),
mDocNameEnd(0),
mDocTitleIdx(0),
mDocTitleStart(0),
mDocTitleEnd(0),
mNameValueSep(AOS_DFT_ATTR_NAMEVALUE_SEP),
mCrtChild(-1),
mCrtAttrIdx(0),
mDebugFlag(eDebugFlag),
mReadOnly(false),
mDeletedFlag(false),
mDocSource(AosDocSource::eUnknown),
mRootFlag(0)
{
	// OmnScreen << "Create Tag: " << this << endl;
	mText.reserve(eMaxTexts);
	gAosNumXmlTags++;
	int diff = gAosNumXmlTags - gAosLastNumXmlTags;
	if (diff < -1000 || diff > 1000)
	{
		OmnScreen << "Number of Xml Tags: " << gAosNumXmlTags << endl;
		gAosLastNumXmlTags = gAosNumXmlTags;
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosXmlTag);
}


AosXmlTag::AosXmlTag(
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &parent,
		const OmnConnBuffPtr &buff,
		const int tagStartIdx,
		const int nameStart, 
		const int nameEnd
		AosMemoryCheckDecl)
:
mRoot(root.getPtr()),
mParentTag(parent.getPtr()),
mBuff(buff),
mTagStart(tagStartIdx),
mTagEnd(-1),
mTagnameStart(nameStart),
mTagnameEnd(nameEnd),
mNumTexts(0),
mWordAttrLoop(0),
mAttrNextTagLoop(0),
mAttrnameTried(false),
mAttrIdx(0),
mTagLoop(0),
mDocNameIdx(0),
mDocNameStart(0),
mDocNameEnd(0),
mDocTitleIdx(0),
mDocTitleStart(0),
mDocTitleEnd(0),
mNameValueSep(AOS_DFT_ATTR_NAMEVALUE_SEP),
mCrtChild(-1),
mCrtAttrIdx(0),
mDebugFlag(eDebugFlag),
mReadOnly(false),
mDeletedFlag(false),
mDocSource(AosDocSource::eUnknown),
mRootFlag(0)
{
	// OmnScreen << "Create Tag: " << this << endl;
	mText.reserve(eMaxTexts);
	if (parent) parent->addChildTag(this);
	gAosNumXmlTags++;
	int diff = gAosNumXmlTags - gAosLastNumXmlTags;
	if (diff < -1000 || diff > 1000)
	{
		//OmnScreen << "Number of Xml Tags: " << gAosNumXmlTags << endl;
		gAosLastNumXmlTags = gAosNumXmlTags;
	}
	AosMemoryCheckerObjCreated(AosClassName::eAosXmlTag);
}


AosXmlTag::~AosXmlTag()
{
	AosMemoryCheckerObjDeleted(AosClassName::eAosXmlTag);
	// OmnScreen << "Delete Tag: " << this << endl;
	aos_assert(mDebugFlag == eDebugFlag);
	aos_assert(!mDeletedFlag);				// Chen Ding, 2013/05/18
	mDeletedFlag = true;					// Chen Ding, 2013/05/18
	mRoot = 0;
	mParentTag = 0;
	//mTagStart = -1;
	//mTagEnd = -1;
	mDebugFlag = eDeletedFlag;
	gAosNumXmlTags--;
	int diff = gAosNumXmlTags - gAosLastNumXmlTags;
	if (diff < -1000 || diff > 1000)
	{
		//OmnScreen << "Number of Xml Tags: " << gAosNumXmlTags << endl;
		gAosLastNumXmlTags = gAosNumXmlTags;
	}
}


bool
AosXmlTag::addChildTag(const AosXmlTagPtr &child)
{
	int nn = mChildTags.append(child);
	aos_assert_r(nn >= 0, false);
	return true;
}


// Chen Ding, 08/14/2011
// Added the 'mark' parameter. 
bool
AosXmlTag::nextWord(
		const AosWordParserPtr &wordparser,
		char *wordbuff, 
		const int bufflen, 
		int &wordlen, 
		AosEntryMark::E &mark)
{
	// This function retrieves the next word:
	// 1. The tag name is a word
	// 2. Each attribute name is a word
	// 3. Each word in the attribute values is a word
	// 4. The words from each subtag
	// 5. The words from the texts
	// This function ignore some word that is exsit in disabled attribute and text.
	aos_assert_r(bufflen > 0, false);
	char *buffData = mBuff->getData();
	if (mNextWordStatus == eAosWordLoopTagname)
	{
		// Check attributes
		int idx = 0;
		while (idx < mAttrs.entries() && 
			   (isAttrIgnored(mAttrs, idx) || mAttrs[idx].valueLen <= 0))
		{
			idx++;
		}

		if (idx < mAttrs.entries())
		{
			// Retrieve the words for 'idx'-th attribute
			int start = mAttrs[idx].valueStart;
			int len = mAttrs[idx].valueLen;
			mNextWordStatus = eAosWordLoopAttr;
			mNextWordIdx = idx;
			wordparser->setSrc(buffData, start, len);
		}
		else
		{
			// No attributes. Check texts
			idx = 0;
			// while (idx < mNumTexts && isTextIgnored(mText, idx)) idx++;
			// if (idx < mNumTexts)
			// {
			// 	mNextWordStatus = eAosWordLoopText;
			// 	mNextWordIdx = idx;
			// 	if (mText[idx].isCdata)
			// 	{
			// 		wordparser->setSrc(buffData, mText[idx].start + 9, mText[idx].len - 3);
			// 	}
			// 	else
			// 	{
			// 		wordparser->setSrc(buffData, mText[idx].start, mText[idx].len);
			// 	}
			// }
			// else
			// {
			// 	mNextWordStatus = eAosWordLoopTag; 
			// 	mNextWordIdx = 0;
			// }
		
			while (idx < mNumTexts)
			{
				if (isTextIgnored(mText, idx)) 
				{
					idx++;
					continue;
				}

				mNextWordStatus = eAosWordLoopText;
				mNextWordIdx = idx;

				// Young : 2013-05-27
				if (mText[idx].isBdata)
				{
					idx++;
					continue;
				}
				
				if (mText[idx].isCdata)
				{
					if (mText[idx].len > 3)
					{
						wordparser->setSrc(buffData, mText[idx].start + 9, mText[idx].len - 3);
						break;
					}
					else
					{
						idx++;
						continue;
					}
				}
				else
				{
					if (mText[idx].len > 0)
					{
						wordparser->setSrc(buffData, mText[idx].start, mText[idx].len);
						break;
					}
					else
					{
						idx++;
						continue;
					}
				}
			}

			if (idx >= mNumTexts)
			{
				mNextWordStatus = eAosWordLoopTag; 
				mNextWordIdx = 0;
			}
		}
	}

	//get xml anputconttribute value
	if (mNextWordStatus == eAosWordLoopAttr)
	{
		if (wordparser->nextWord(wordbuff, wordlen))
		{
			wordbuff[wordlen] = 0;

			// Chen Ding, 08/14/2011
			mark = mAttrs[mNextWordIdx].mark;
			return true;
		}

		mNextWordIdx++;
		while (mNextWordIdx < mAttrs.entries())
		{
			// if (mAttrs[mNextWordIdx].disabled || mAttrs[mNextWordIdx].mark == eNoIndex)
			if (isAttrIgnored(mAttrs, mNextWordIdx) || mAttrs[mNextWordIdx].valueLen <= 0)
			{
				mNextWordIdx++;
				continue;
			}

			int start = mAttrs[mNextWordIdx].valueStart;
			int len = mAttrs[mNextWordIdx].valueLen;
			wordparser->setSrc(buffData, start, len);
			if (wordparser->nextWord(wordbuff, wordlen))
			{
				wordbuff[wordlen] = 0;
			
				// Chen Ding, 08/14/2011
				mark = mAttrs[mNextWordIdx].mark;
				return true;
			}
			mNextWordIdx++;
		}

		// Chen Ding, 08/10/2011
		int idx = 0;
		while (idx < mNumTexts)
		{
			if (isTextIgnored(mText, idx)) 
			{
				idx++;
				continue;
			}

			mNextWordStatus = eAosWordLoopText;
			mNextWordIdx = idx;

			// create by Young 2013-05-27
			if (mText[idx].isBdata)
			{
				idx++;
				continue;
			}

			if (mText[idx].isCdata)
			{
				if (mText[idx].len > 3)
				{
					wordparser->setSrc(buffData, mText[idx].start + 9, mText[idx].len - 3);
					break;
				}
				idx++;
				continue;
			}
			else
			{
				if (mText[idx].len > 0)
				{
					wordparser->setSrc(buffData, mText[idx].start, mText[idx].len);
					break;
				}
				idx++;
				continue;
			}
		}

		if (idx >= mNumTexts)
		{
			mNextWordStatus = eAosWordLoopTag;
			mNextWordIdx = 0;
		}
	
		// while (idx < mNumTexts && isTextIgnored(mText, idx)) idx++;
		// if (idx < mNumTexts)
		// {
		// 	mNextWordStatus = eAosWordLoopText;
		// 	mNextWordIdx = idx;
		// 	if (mText[idx].isCdata)
		// 	{
		// 		wordparser->setSrc(buffData, mText[idx].start + 9, mText[idx].len - 3);
		// 	}
		// 	else
		// 	{
		// 		wordparser->setSrc(buffData, mText[idx].start, mText[idx].len);
		// 	}
		// }
		// else
		// {
		// 	mNextWordStatus = eAosWordLoopTag;
		// 	mNextWordIdx = 0;
		// }
	}

	if (mNextWordStatus == eAosWordLoopText) 
	{
		// Retrieve the words from all texts
		if (wordparser->nextWord(wordbuff, wordlen))
		{
			wordbuff[wordlen] = 0;
			
			// Chen Ding, 08/14/2011
			mark = mText[mNextWordIdx].mark;
			return true;
		}

		mNextWordIdx++;
		while (mNextWordIdx < mNumTexts)
		{
			// if (mText[mNextWordIdx].disabled || mText[mNextWordIdx].mark == eNoIndex)
			if (isTextIgnored(mText, mNextWordIdx))
			{
				mNextWordIdx++;
				continue;
			}

			// create by Young : 2013-05-27
			if (mText[mNextWordIdx].isBdata)
			{
				mNextWordIdx++;
				continue;
			}

			if (mText[mNextWordIdx].isCdata)
			{
				if (mText[mNextWordIdx].len <= 3)
				{
					mNextWordIdx++;
					continue;
				}

				wordparser->setSrc(buffData, mText[mNextWordIdx].start + 9, 
							mText[mNextWordIdx].len - 3);
			}
			else
			{
				if (mText[mNextWordIdx].len <= 0)
				{
					mNextWordIdx++;
					continue;
				}

				wordparser->setSrc(buffData, mText[mNextWordIdx].start, 
							mText[mNextWordIdx].len);
			}

			if (wordparser->nextWord(wordbuff, wordlen))
			{
				wordbuff[wordlen] = 0;

				// Chen Ding, 08/14/2011
				mark = mText[mNextWordIdx].mark;
				return true;
			}
			mNextWordIdx++;
		}

		mNextWordStatus = eAosWordLoopTag;
		mNextWordIdx = 0;
	}

	// Ken Lee 2013/04/19
	if (isChildNodeIgnored())
	{
		wordlen = 0;
		return false;
	}

	// Try subtags
	while (mNextWordIdx < mChildTags.entries())
	{
		aos_assert_r(mChildTags[mNextWordIdx], false);
		if (mChildTags[mNextWordIdx]->nextWord(wordparser, wordbuff, bufflen, wordlen, mark))
		{
			return true;
		}
		mNextWordIdx++;
	}

	wordlen = 0;
	return false;
}


AosXmlTagPtr
AosXmlTag::getChildByAttr(
		const OmnString &aname, 
		const OmnString &avalue)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getChildByAttr(aname, avalue);
	}

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], 0);
		if (mChildTags[i]->hasAttribute(aname, avalue)) return mChildTags[i];
	}

	return 0;
}


OmnString
AosXmlTag::getChildTextByAttr(
		const OmnString &aname, 
		const OmnString &avalue)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getChildTextByAttr(aname, avalue);
	}

	int len;
	u8 *data = getChildTextByAttr(aname, avalue, len);
	if (!data || len <= 0) return "";
	OmnString str((char *)data, len);
	return str;
}

u32
AosXmlTag::getChildU32ByAttr(
		const OmnString &aname, 
		const OmnString &avalue,
		const u32 dft)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getChildU32ByAttr(aname, avalue,dft);
	}

	int len;
	u8 *data = getChildTextByAttr(aname, avalue, len);
	if (!data || len <= 0) return dft;
	return atol((char*)data);
}

u64
AosXmlTag::getChildU64ByAttr(
		const OmnString &aname, 
		const OmnString &avalue,
		const u64 &dft)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getChildU64ByAttr(aname, avalue,dft);
	}

	int len;
	u8 *data = getChildTextByAttr(aname, avalue, len);
	if (!data || len <= 0) return dft;
	return atoll((char*)data);
}


u8 *
AosXmlTag::getChildTextByAttr(
		const OmnString &aname, 
		const OmnString &avalue,
		int &len)
{
	// It retrieves the text of the first child
	// that has an attribute named 'aname' and the
	// attribute value is avalue. If found, it returns
	// the pointer and 'len'. If not found, it returns 0.
	// If the tag does not text, it returns 0.
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
	 	return mChildTags[0]->getChildTextByAttr(aname, avalue, len);
	}

	len = 0;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->hasAttribute(aname, avalue))
		{
			return mChildTags[i]->getNodeText(len);
		}
	}

	return 0;
}


//Ketty 2011/04/23 for attr access
AosXmlTagPtr
AosXmlTag::xpathGetFirstChild(const OmnString &path)
{
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
	 	return mChildTags[0]->xpathGetFirstChild(path);
	}
	
	OmnStrParser1 parser(path, "/", false, false);
	OmnString childname = parser.nextWord("");
	if (childname == "") return 0;

	AosXmlTagPtr child = getFirstChild(childname);	
	if(!child)	return 0;

	int idx = path.find('/', false);
	if(idx < 0)	return child;

	OmnString subPath = path.substr(idx + 1);
	if (subPath == "")	 return child;

	return child->xpathGetFirstChild(subPath);
}


bool
AosXmlTag::hasChild(const OmnString &name, const bool recursive)
{
	// This function returns true if the tag has a child
	// named 'name'. If 'recursive' is true, it will do the recursive.
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
	 	return mChildTags[0]->hasChild(name, recursive);
	}

	if (name == getTagname()) return true;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->getTagname() == name) return true;
	}

	if (!recursive) return false;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->hasChild(name, true)) return true;
	}

	return false;
}


bool
AosXmlTag::hasAttribute(
		const OmnString &name, 
		const bool recursive)
{
	// This function returns true if the tag has an attribute
	// named 'name'. If 'recursive' is true, it will do the recursive.
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int nstart = mAttrs[i].nameStart;
		int nend = mAttrs[i].nameEnd;
		if (strncmp(name.data(), (const char *)&buffData[nstart], nend-nstart+1) == 0) 
		{
			return true;
		}
	}

	if (!recursive) return false;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->hasAttribute(name, true)) return true;
	}

	return false;
}


bool
AosXmlTag::hasAttribute(
		const OmnString &name, 
		const OmnString &value, 
		const bool recursive)
{
	// This function returns true if the tag has an attribute
	// named 'name' and its value is 'value'. 
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int nstart = mAttrs[i].nameStart;
		int nend = mAttrs[i].nameEnd;
		int namelen = nend - nstart + 1;
		int vstart = mAttrs[i].valueStart;
		int vlen = mAttrs[i].valueLen;
		if (name.length() == namelen &&
			strncmp(name.data(), (const char *)&buffData[nstart], namelen) == 0 &&
			vlen == value.length() && 
			strncmp(value.data(), (const char *)&buffData[vstart], vlen) == 0)
		{
			return true;
		}
	}

	if (!recursive) return false;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->hasAttribute(name, value, true)) return true;
	}

	return false;
}


OmnString
AosXmlTag::getNodeText()
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getNodeText();
	}
	int len;
	char *data = (char *)getNodeText(len);
	return OmnString(data, len);
}


u8 *
AosXmlTag::getNodeText(int &len)
{
	// Note that we allow a tag to have multiple texts. 
	// But this function will return only the first text, if any.
	// if (!mRoot)
	// {
	// 	aos_assert_r(mChildTags.entries() == 1, 0);
	// 	return mChildTags[0]->getNodeText(len);
	// }

	len = 0;
	if (mNumTexts == 0) return 0;	
	int start = mText[0].start;
	len = mText[0].len;
	if (mText[0].isCdata) 
	{
		aos_assert_r(len >= 12, 0);
		start += 9;
		len -= 12;
	}
	char *buffData = mBuff->getData();
	return (u8 *)&buffData[start];
}


OmnString
AosXmlTag::getNodeText(const OmnString &nodename)
{
	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return "";
	else return child->getNodeText();
}


AosXmlTagPtr	
AosXmlTag::getFirstChild(const OmnString &name, const bool recursive)
{
	// This function returns the first child whose tag name
	// is 'name'. If not found, it returns 0.
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getFirstChild(name, recursive);
	}

	queue<AosXmlTagPtr> node; 	
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], 0);
		if (mChildTags[i]->tagnameMatch(name)) 
		{
			mCrtChild = i;
			return mChildTags[i];
		}
		if (recursive) node.push(mChildTags[i]);
	}
	
	if (!recursive) return 0;

	while (!node.empty())
	{
		AosXmlTagPtr childtag = node.front();
		node.pop();
		for (int j=0; j<childtag->mChildTags.entries(); j++)
		{
			aos_assert_r(childtag->mChildTags[j], 0);
			if (childtag->mChildTags[j]->tagnameMatch(name))
			{
				return childtag->mChildTags[j];
			}
			node.push(childtag->mChildTags[j]);
		}
	}
	return 0;
}


AosXmlTagPtr	
AosXmlTag::getNextChild(const OmnString &name)
{
	// This function returns the first child whose tag name
	// is 'name'. If not found, it returns 0.
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getNextChild(name);
	}

	mCrtChild++;
	for (int i=mCrtChild; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], 0);
		if (mChildTags[i]->tagnameMatch(name)) 
		{
			mCrtChild = i;
			return mChildTags[i];
		}
	}

	return 0;
}


bool
AosXmlTag::tagnameMatch(const OmnString &name)
{
	// This function returns true if the tagname matches 'name'.
	int len = mTagnameEnd - mTagnameStart+1;
	char *buffData = mBuff->getData();
	return name.length() == len && 
		   strncmp((const char *)&buffData[mTagnameStart], name.data(),
			mTagnameEnd - mTagnameStart + 1) == 0;
}


AosXmlTag *
AosXmlTag::getParentTag()
{
	return mParentTag;
}


void
AosXmlTag::setParent(const AosXmlTagPtr &parent)
{
	mParentTag = parent.getPtr();
}



bool		
AosXmlTag::addAttr(const u8 *data, 
		const int nameStart,
		const int nameEnd,
		const int valueStart,
		const int valueLen, 
		int &delta) 
{
	// This function is supposedly called by the parser. It
	// adds a new attribute. Before an attribute is added, 
	// it will check the following:
	// 1. Whether the attribute already exist. If yes, the attribute
	//    is not added. Instead, it is appended to the existing attribute.
	// 2. Whether the attribute is empty. Empty attributes are dropped
	//    silently.
	// 3. Whether the attribute contains special characters. If yes,
	//    they are escaped.
	
	delta = 0;
	int nlen = nameEnd - nameStart + 1;

	// 1. Check whether it is empty.
	// Chen Ding, 2011/01/27
	aos_assert_r(valueLen >= 0, false);
	// if (valueLen <= 0) return true;

	// 2. Check whether the attribute already exist
	int llen;
	char *ss = (char *)getAttr((const char *)&data[nameStart], nlen, llen);
	/*
	if (ss)
	{
		// Need to remove the contents: 
		int dd = valueStart - nameStart + valueLen + 1;
		if (data[nameStart + dd] == ' ') dd++;

		OmnString vv(ss, llen);
		OmnString value((char *)&data[valueStart], valueLen);
		vv << "," << value;
		OmnString name((const char *)&data[nameStart], nlen);

		// memmove((char *)&data[nameStart], (char *)&data[nameStart + dd], dd);
		// mBuff->setDataLength(mBuff->getDataLength() - dd);
		if (mRoot)
		{
			aos_assert_r(mRoot->adjustData(nameStart, -dd), false);
		}
		else
		{
			aos_assert_r(adjustData(nameStart, -dd), false);
		}

		setAttr(name, vv.data(), vv.length());
		return true;
	}
	*/
	
	aos_assert_r(nlen > 0, false);
	aos_assert_r(nameEnd >= nameStart, false);

	AttrInfo attr(nameStart, nameEnd, valueStart, valueLen);
	// attr.nameStart = nameStart;
	// attr.nameEnd = nameEnd;
	// attr.valueStart = valueStart;
	// attr.valueLen = valueLen;
	// attr.disabled = false;
	mAttrs.append(attr);
	aos_assert_r(AosXmlTagSanityCheck(this), false);

	if (ss)
	{
		// Duplicated attributes are found. Remove the first one
		OmnString name((const char *)&data[nameStart], nlen);
		int lenBefore = mBuff->getDataLength();
		OmnString value;
		removeAttr1(name, 0, true, true, value);
		if (value != "") value << ",";
		value << getAttrStr(name);
		setAttr(name, value.data(), value.length());
		int lenAfter = mBuff->getDataLength();
		delta = lenAfter - lenBefore;
	}
	return true;
}


bool
AosXmlTag::addBdata(const int start, const int len)
{
	//Linda
	aos_assert_r(mNumTexts < eMaxTexts-1, false);
	mText[mNumTexts].start = start;
	mText[mNumTexts].len = len;
	mText[mNumTexts].isCdata = true;
	mText[mNumTexts].isBdata = true;
	mText[mNumTexts].disabled = false;
	mNumTexts++;
	return true;
}


bool		
AosXmlTag::addCdata(const int start, const int len)
{
	aos_assert_r(mNumTexts < eMaxTexts-1, false);
	mText[mNumTexts].start = start;
	mText[mNumTexts].len = len;
	mText[mNumTexts].isCdata = true;
	mText[mNumTexts].disabled = false;
	mNumTexts++;
	return true;
}


bool		
AosXmlTag::addText(const int start, const int len)
{
	//aos_assert_r(mNumTexts < eMaxTexts-1, false);
	//Medfied By Andy Zhang 
	if (mNumTexts > eMaxTexts -1) return true;

	mText[mNumTexts].start = start;
	mText[mNumTexts].len = len;
	mText[mNumTexts].isCdata = false;
	mText[mNumTexts].disabled = false;
	mNumTexts++;
	return true;
}


AosXmlTagPtr
AosXmlTag::getFirstChild(const bool skiproot)
{
	// Chen Ding, 05/09/2012
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		if (skiproot)
		{
			return mChildTags[0]->getFirstChild();
		}

		return mChildTags[0];
	}

	mCrtChild = -1;
	if (mChildTags.entries() <= 0) return 0;
	mCrtChild = 0;
	return mChildTags[0];
}


AosXmlTagPtr
AosXmlTag::getNextChild()
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getNextChild();
	}

	mCrtChild++;
	aos_assert_r(mCrtChild >= 0, 0);
	if (mCrtChild >= mChildTags.entries()) return 0;
	return mChildTags[mCrtChild];
}


bool
AosXmlTag::verifyTagName(
	const int tagstart, 
	const int tagend)
{
	char *buffData = mBuff->getData();
	bool r1 = (tagend - tagstart == mTagnameEnd - mTagnameStart);
	bool r2 = (strncmp((const char *)&buffData[mTagnameStart], 
				(const char *)&buffData[tagstart], tagend - tagstart + 1) == 0);
	aos_assert_r(r1 && r2, false);
	return true;
}


OmnString
AosXmlTag::nextTagName()
{
	if (mTagLoop == -1)
	{
		mTagLoop = 0;
		if (mTagnameEnd >= mTagnameStart)
		{
			int len = mTagnameEnd - mTagnameStart + 1;
			char *buffData = mBuff->getData();
			OmnString str((char *)&buffData[mTagnameStart], len);
			return str;
		}
	}

	if (mTagLoop >= mChildTags.entries()) return 0;
	return mChildTags[mTagLoop++]->nextTagName();
}


void		
AosXmlTag::resetGetWords() 
{
	mNextWordStatus = eAosWordLoopTagname;
	mNextWordIdx = 0;
	// mWordTextIdx = 0;
	// mWordTagLoop = 0;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert(mChildTags[i]);
		mChildTags[i]->resetGetWords();
	}
}


void
AosXmlTag::resetAttrLoop()
{
	mWordAttrLoop = 0;
	mAttrnameTried = false;
	mAttrIdx = 0;
	mAttrNextTagLoop = 0;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert(mChildTags[i]);
		mChildTags[i]->resetAttrLoop();
	}
}


void
AosXmlTag::resetTagNameLoop()
{
	mTagLoop = -1;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert(mChildTags[i]);
		mChildTags[i]->resetTagNameLoop();
	}
}


//felicia 2011/05/18
bool
AosXmlTag::nodenextAttr(
		const OmnString &path,
		OmnString &name,
		OmnString &value,
		bool &finished)
{
	finished = false;
	char *buffData = mBuff->getData();
	if (mAttrLoopIdx >= mAttrs.entries())
	{
		return false; 
		//mAttrLoopStatus = eMemberTags;
		 //mAttrLoopIdx = 0;
	}
	else
	{
		 while (mAttrLoopIdx < mAttrs.entries())
		 {
			if (mAttrs[mAttrLoopIdx].disabled)
			{
				mAttrLoopIdx++;
				continue;
			}
		 	int start = mAttrs[mAttrLoopIdx].nameStart;
		 	int end = mAttrs[mAttrLoopIdx].nameEnd;
		 	aos_assert_r(end >= start, false);
		 	int len = end - start + 1;
		 	
			//if (path != "")
		 	//{
			 	//name = path;
			 	//name << "/";
			 	//name.append((char *)&buffData[start], len);
		 	//}
		 	//else
		 	//{
		 	name.assign((char *)&buffData[start], len);
		 	//}
		 	start = mAttrs[mAttrLoopIdx].valueStart;
		 	len = mAttrs[mAttrLoopIdx].valueLen;
		 	aos_assert_r(len >= 0, false);
		 	value.assign((char *)&buffData[start], len);
		 	mAttrLoopIdx++;
		 	return true;
		 }
	}
	return false;
}


bool
AosXmlTag::nextAttr(
		const OmnString &path,
		OmnString &name, 
		OmnString &value, 
		bool &finished, 
		const bool withpath,
		AosEntryMark::E &mark,
		bool isIgnore)
{
	// It retrieves the next attribute and its value. 
	// 1. Loop on every of its attributes
	// 2. Loop on each path
	// 3. Loop on each of its member tags
	// This function ignore some attribute that is metadata.
	 
	// Ken Lee 2013/05/15
	if (isChildNodeIgnored())
	{
		finished = true;
		return true;
	}
	
	finished = false;
	char *buffData = mBuff->getData();
	switch (mAttrLoopStatus)
	{
	case eSelfAttrs:
		 if (mAttrLoopIdx >= mAttrs.entries())
		 {
			 mAttrLoopStatus = eMemberTags;
			 mAttrLoopIdx = 0;
		 }
		 else
		 {
			 while (mAttrLoopIdx < mAttrs.entries())
			 {
				if (mAttrs[mAttrLoopIdx].disabled || mAttrs[mAttrLoopIdx].valueLen <= 0)
				{
					mAttrLoopIdx++;
					continue;
				}
			 	int start = mAttrs[mAttrLoopIdx].nameStart;
			 	int end = mAttrs[mAttrLoopIdx].nameEnd;
			 	aos_assert_r(end >= start, false);
			 	int len = end - start + 1;
				
				// This funcion ignored all metadate attribute in the tag
				if (!isIgnore && len > 6 && buffData[start] == 'z' &&
					buffData[start+1] == 'k' &&
					buffData[start+2] == 'y' &&
					buffData[start+3] == '_' && 
				 	attrIgnored(start, len))
				{
					mAttrLoopIdx++;
					continue;
				}

			 	if (withpath && path != "")
			 	{
				 	name = path;
				 	name << "/";
				 	name.append((char *)&buffData[start], len);
			 	}
			 	else
			 	{
			 		name.assign((char *)&buffData[start], len);
			 	}
				
				mark = mAttrs[mAttrLoopIdx].mark;
			 	start = mAttrs[mAttrLoopIdx].valueStart;
			 	len = mAttrs[mAttrLoopIdx].valueLen;
			 	aos_assert_r(len >= 0, false);
			 	value.assign((char *)&buffData[start], len);
			 	mAttrLoopIdx++;
			 	return true;
			 }

			 mAttrLoopStatus = eMemberTags;
			 mAttrLoopIdx = 0;
		 }

		 if (mChildTags.entries() == 0) 
		 {
			 finished = true;
			 return true;
		 }

		 mChildTags[0]->resetAttrNameValueLoop();
		 mAttrLoopIdx = 0;

		 if (withpath)
		 {
		 	mCrtPath = path;
		 	if (path != "") mCrtPath << "/";
		 	int start = mChildTags[0]->mTagnameStart; 
		 	int end = mChildTags[0]->mTagnameEnd; 
		 	aos_assert_r(end >= start, false);
		 	int len = end - start + 1;
		 	mCrtPath.append((char *)&buffData[start], len);
		 }

	case eMemberTags:
		 while (mAttrLoopIdx < mChildTags.entries()) 
		 {
			 aos_assert_r(mChildTags[mAttrLoopIdx], false);
			 aos_assert_r(mChildTags[mAttrLoopIdx]->nextAttr(
					mCrtPath, name, value, finished, withpath, mark), false);
			 if (!finished) return true;
			 mAttrLoopIdx++;
			 if (mAttrLoopIdx >= mChildTags.entries()) 
			 {
				 finished = true;
				 return true;
			 }
			 mChildTags[mAttrLoopIdx]->resetAttrNameValueLoop();

			 if (withpath)
			 {
		 	 	mCrtPath = path;
		 	 	if (path != "") mCrtPath << "/";
		 	 	int start = mChildTags[mAttrLoopIdx]->mTagnameStart; 
		 	 	int end = mChildTags[mAttrLoopIdx]->mTagnameEnd; 
		 	 	aos_assert_r(end >= start, false);
		 	 	int len = end - start + 1;
		 	 	mCrtPath.append((char *)&buffData[start], len);
			 }
		 }

		 finished = true;
		 return true;

	default:
		 OmnAlarm << "Invalid attribute looping status: " 
			 << mAttrLoopStatus << enderr;
		 return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosXmlTag::nextAttrWord(
		const AosWordParserPtr &wordparser,
		char *wordbuff, 
		const int bufflen,
		int &wordlen, 
		bool &isName,
		AosEntryMark::E &mark)
{
	// Retrieve the word from its attributes.
	while (mWordAttrLoop < mAttrs.entries())
	{
		// If the attribute is disabled, go to the next one
		// Chen Ding, 08/14/2011
		// if (mAttrs[mWordAttrLoop].disabled || mAttrs[mWordAttrLoop].mark == eNoIndex)
		if (isAttrIgnored(mAttrs, mWordAttrLoop))
		{
			mAttrnameTried = false;
			mWordAttrLoop++;
			continue;
		}

		// Check whether the attribute name has been retrieved.
		if (!mAttrnameTried)
		{
			// The attribute name has not been retrieved yet.
			mAttrnameTried = true;

			// Chen Ding, 08/14/2011
			mark = mAttrs[mWordAttrLoop].mark;

			mAttrIdx = mAttrs[mWordAttrLoop].valueStart;
			wordlen = mAttrs[mWordAttrLoop].nameEnd - 
				 mAttrs[mWordAttrLoop].nameStart + 1;
			isName = true;
			char *buffData = mBuff->getData();
			aos_assert_r(wordlen < bufflen, false);
			memcpy(wordbuff, &buffData[mAttrs[mWordAttrLoop].nameStart], wordlen);
			wordbuff[wordlen] = 0;
			return true;
		}

		// Retrieve the next word from the attribute value.
		isName = false;
		char *buffData = mBuff->getData();
		u32 thelen = eMaxWordLen;
		AosNextXmlWord(buffData, 0, 0, mAttrIdx, 
			mAttrs[mWordAttrLoop].valueStart + 
			mAttrs[mWordAttrLoop].valueLen, wordbuff, thelen, wordlen);
		if (thelen > 0)
		{
			// Found a word
			mAttrIdx += thelen;

			// Chen Ding, 08/14/2011
			mark = mAttrs[mWordAttrLoop].mark;
			return true;
		}
 
		// No more word from the attribute value. Try the next 
		// attribute.
		mAttrnameTried = false;
		mWordAttrLoop++;
	}

	/* This portion of the code is for the case where adding
	 * new attributes will not modify the original data, which
	 * is much more efficient than modifying the original data.
	 * This new feature is not supported yet, waiting to be 
	 * improved in the future. The code is commented out 
	 * for now. Chen Ding - 2009-10-23
	// Retrieve the word from its added attributes
	while (mAddedAttrLoop < mAddedAttrs.entries())
	{
		// Check whether the attribute name has been retrieved.
		if (!mAttrnameTried)
		{
			// The attribute name has not been retrieved yet.
			mAttrnameTried = true;
			len = mAddedAttrs[mAddedAttrLoop].name.length();
			isName = true;
			mAttrIdx = 0;
			return (char *)mAddedAttrs[mAddedAttrLoop].name.data();
		}

		// Retrieve the next word from the attribute value.
		int start;
		isName = false;
		AosNextWord(mAddedAttrs[mAddedAttrLoop].value.data(), 0, 0, mAttrIdx, 
			mAddedAttrs[mAddedAttrLoop].value.length(), start, len);
		if (len > 0)
		{
			// Found a word
			mAttrIdx = start + len;
			return &mAddedAttrs[mAddedAttrLoop].value.data()[start];
		}
 
		// No more word from the attribute value. Try the next 
		// attribute.
		mAttrnameTried = false;
		mAddedAttrLoop++;
	}
	*/

	// Tried all its attributes. Now we need to try its members. 
	while (mAttrNextTagLoop < mChildTags.entries())
	{
		aos_assert_r(mChildTags[mAttrNextTagLoop], false);
		if (mChildTags[mAttrNextTagLoop]->nextAttrWord(
					wordparser, wordbuff, bufflen, wordlen, isName, mark))
		{
			return true;
		}
		mAttrNextTagLoop++;
	}

	wordbuff[0] = 0;
	wordlen = 0;
	return false;
}


OmnString 
AosXmlTag::getNextAttrStr(const OmnString &name, bool &found)
{
	int len;
	u8 *vv = getNextAttr(name, len);
	if (!vv) 
	{
		found = false;
		return "";
	}
	found = true;
	return OmnString((char *)vv, len);
}

OmnSimpleStr
AosXmlTag::getAttrStrSimp(
		const char *name,
		const OmnSimpleStr &dft)
{
	OmnSimpleStr value = getAttrStrSimp(name, "");	
	if (value.len == 0)
	{
		return dft;
	}
	return value;
}

OmnSimpleStr
AosXmlTag::getAttrStrSimp(
		const char *name,
		const char *dft)
{
	int len = strlen(name);
	int dftlen = strlen(dft);
	if (len <= 0) return OmnSimpleStr(dft, dftlen);

	switch (name[0])
	{
	case '_':
		 // Check whether it is "_#text"
		 if (len == 6 &&
			 name[1] == '#' && 
			 name[2] == 't' && 
			 name[3] == 'e' && 
			 name[4] == 'x' && 
			 name[5] == 't')
		 {
			 char *dd = (char *)getNodeText(len);
			 return OmnSimpleStr(dd, len);
		 }
		 break;
	}

	u8 *vv = getAttr(name, len);
	if (!vv) return OmnSimpleStr(dft, dftlen);
	return OmnSimpleStr((char *)vv, len);		
}


OmnString
AosXmlTag::getAttrStr(
		const OmnString &name, 
		const bool recursive, 
		bool &exist,
		AosXmlTagPtr &node,
		const OmnString &dft)
{
	// It recursively retrieve an attribute named 'name'. 
	OmnString value = getAttrStr1(name, exist, dft);
	node = 0;
	if (exist) 
	{
		if (mRoot) node = this;
		else node = mChildTags[0];
		return value;
	}

	if (!recursive) return dft;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], "");
		value = mChildTags[i]->getAttrStr(name, true, exist, node, dft);
		if (exist) 
		{
			return value;
		}
	}

	exist = false;
	return dft;
}


OmnString
AosXmlTag::getAttrStr1(
		const OmnString &name,
		bool &exist,
		const OmnString &dft)
{
	exist = false;
	const char *data = name.data();
	int len = name.length();
	if (len <= 0) return dft;

	switch (data[0])
	{
	case '_':
		 // Check whether it is "_#text"
		 if (len == 6 &&
			 data[1] == '#' && 
			 data[2] == 't' && 
			 data[3] == 'e' && 
			 data[4] == 'x' && 
			 data[5] == 't')
		 {
			 char *dd = (char *)getNodeText(len);
			 if (dd) exist = true;
			 OmnString ss(dd, len);
			 return ss;
		 }
		 break;
	}

	u8 *vv = getAttr(name, len);
	if (!vv) return dft;
	OmnString str((char *)vv, len);
	exist = true;
	return str;
}


u8 *
AosXmlTag::getAttr(const char *name, const int namelen, int &len)
{
	// This function retrieves the attribute named 'name'.
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
		AosXmlTagPtr childtag = mChildTags[0];
		if (!childtag.getPtr())
		{
			OmnAlarm << "Internal error!" << enderr;
			return 0;
		}
	 	return childtag->getAttr(name, namelen, len);
	}

	len = 0;
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int nlen = end - start + 1;
		if (nlen == namelen && 
			strncmp((char *)&buffData[start], name, namelen) == 0)
		{
			mCrtAttrIdx = i+1;
			len = mAttrs[i].valueLen;
			return (u8 *)&buffData[mAttrs[i].valueStart];
		}
	}
	return 0;
}


u8 *
AosXmlTag::getNextAttr(const OmnString &name, int &len)
{
	char *buffData = mBuff->getData();
	for (int i=mCrtAttrIdx; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		if (namelen == name.length() && 
			strncmp((char *)&buffData[start], name.data(), namelen) == 0)
		{
			mCrtAttrIdx = i+1;
			len = mAttrs[i].valueLen;
			return (u8 *)&buffData[mAttrs[i].valueStart];
		}
	}
	mCrtAttrIdx = mAttrs.entries();
	return 0;
}


u32				
AosXmlTag::getAttrU32(const OmnString &name, const u64 &dft)
{
	int len;
	u8 *value = getAttr(name, len);
	if (!value || len >= 12) //ken lee 2012/03/23 max u32 string length 
	{
		return dft;
	}

	u32 vv;
	aos_assert_r(aos_value_atoul((char *)value, &vv) == 0, dft);
	return vv;
}


u64				
AosXmlTag::getAttrU64(const OmnString &name, const u64 &dft, bool &exist)
{
	// This function retrieves the named attribute as a u64. 
	// Its length shall be exactly 8 bytes. Otherwise, it is
	// an error. It directly converts the data bit-by bit 
	// to a u64. 
	// if (!mRoot)
	// {
	// 	aos_assert_r(mChildTags.entries() == 1, dft);
	// 	return mChildTags[0]->getAttrU64(name, dft);
	// }

	int len;
	u8 *value = getAttr(name, len);
	if (!value || len >= 22) // Ice Yu 24/09/2011 max u64 string length 
	{
		exist = false;
		return dft;
	}

	u64 vv;
	if (aos_value_atoull((char *)value, &vv) != 0)
	{
		OmnAlarm << "Failed to convert: " << value << enderr;
		return dft;
	}

	exist = true;
	return vv;
}


i64				
AosXmlTag::getAttrI64(const OmnString &name, const i64 &dft, bool &exist)
{
	int len;
	u8 *value = getAttr(name, len);
	if (!value || len >= 22) // Ice Yu 24/09/2011 max u64 string length 
	{
		exist = false;
		return dft;
	}

	i64 vv;
	if (aos_value_atoll((char *)value, &vv) != 0)
	{
		OmnAlarm << "Failed to convert: " << value << enderr;
		return dft;
	}

	exist = true;
	return vv;
}


int64_t
AosXmlTag::getAttrInt64(const OmnString &name, const int64_t dft)
{
	int len;
	u8 *value = getAttr(name, len);
	if (!value || len <= 0) return dft;
	return atoll((char *)value);
}


int64_t
AosXmlTag::getAttrInt64(const OmnString &name, const int64_t dft, bool &exist)
{
	int len;
	u8 *value = getAttr(name, len);
	if (!value || len <= 0)
	{
		exist = false;
		return dft;
	}
	exist = true;
	return atoll((char *)value);
}


int
AosXmlTag::getAttrInt(const OmnString &name, const int dft)
{
	int len;
	u8 *value = getAttr(name, len);
	if (!value || len <= 0) return dft;
	return atoi((char *)value);
}


char
AosXmlTag::getAttrChar(const OmnString &name, const char dft)
{
	// This function retrieves the named attribute as char. 
	// Its value length shall be exactly one. 
	// Otherwise, it is an error. 
	// if (!mRoot)
	// {
	// 	aos_assert_r(mChildTags.entries() == 1, dft);
	// 	return mChildTags[0]->getAttrChar(name, dft);
	// }

	int len;
	char *value = (char *)getAttr(name, len);
	if (!value || len != 1) return dft;
	return value[0];
}


bool
AosXmlTag::setAttr(
	const OmnString &name, 
	const char *value,
	const int vlen)
{
	// It adds an attribute to the tag. Note that the tag contents
	// are stored in mBuff. This function will not modify
	// the contents. It will add as the additional attributes. 
	//
	// FUTURE IMPROVEMENT: Currently we will modify the original
	// data, which is very inefficient. We will improve its performance
	// in the future.
	// 
	// XML does not allow attributes with no values. If 'vlen' is 0, 
	// it does nothing.
	
/*	if (mReadOnly)//Linda, 2011/01/16
	{
		//OmnAlarm << "Doc is read only" << enderr;
	}
*/
	// Chen Ding, 07/08/2011
	aos_assert_r(mDebugFlag == eDebugFlag, false);
	if (mRoot) 
	{
		aos_assert_r(mRoot->mDebugFlag == eDebugFlag, false);
	}
	aos_assert_r(AosXmlTagSanityCheck(this), false);
	if (vlen <= 0) return true;

	char *buffData = mBuff->getData();
	if (mTagEnd != -1)
	{
		aos_assert_r(buffData[mTagEnd] == '>', false);
		aos_assert_r(buffData[mTagEnd-1] != ' ', false);
	}

	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, false);
		aos_assert_r(mChildTags[0]->setAttr(name, value, vlen), false);
		buffData = mBuff->getData();

		if (mTagEnd != -1)
		{
			aos_assert_r(buffData[mTagEnd] == '>', false);
			aos_assert_r(buffData[mTagEnd-1] != ' ', false);
		}
		return true;
	}

	// Check whether it matches any of the existing attribute.
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int len = mAttrs[i].nameEnd - mAttrs[i].nameStart + 1;
		if (len == name.length() &&
			strncmp((char *)&buffData[mAttrs[i].nameStart], 
				name.data(), name.length()) == 0)
		{
			// Found it. 
			int valuelen = mAttrs[i].valueLen;
			if (valuelen != vlen)
			{
				if (mRoot)
				{
					aos_assert_r(mRoot->adjustData(mAttrs[i].valueStart, 
						vlen - valuelen), false);
				}
				else
				{
					aos_assert_r(adjustData(mAttrs[i].valueStart, 
						vlen - valuelen), false);
				}
				buffData = mBuff->getData();
				mAttrs[i].valueLen = vlen;
			}
			strncpy((char *)&buffData[mAttrs[i].valueStart], 
				value, vlen);

			if (mTagEnd != -1)
			{
				aos_assert_r(buffData[mTagEnd] == '>', false);
				aos_assert_r(buffData[mTagEnd-1] != ' ', false);
			}
			return true;
		}
	}

	// Not found. Need to add the attribute. It should be in the form:
	// 		name='value'
	// The new attribute is appended to the end of the existing attrs.
	int delta = name.length() + vlen + 4;
	int startPos = mTagnameEnd+1;
	if (mAttrs.entries() > 0) 
	{
		// Note that value end ends at the last letter. There is
		// one more char for the quote. 
		startPos = mAttrs[mAttrs.entries()-1].valueStart + 
			mAttrs[mAttrs.entries()-1].valueLen + 1;
		// startPos = mAttrs[mAttrs.entries()-1].valueEnd+2;
	}

	if (mRoot)
	{
		aos_assert_r(mRoot->adjustData(startPos, delta), false);
	}
	else 
	{
		aos_assert_r(adjustData(startPos, delta), false);
	}

	buffData = mBuff->getData();
	buffData[startPos++] = ' ';		// Add a space

	AttrInfo attr(startPos, startPos + name.length() - 1, startPos + name.length() - 1+3,vlen);
	// attr.nameStart = startPos;
	// attr.nameEnd = startPos + name.length() - 1;
	// attr.valueStart = attr.nameEnd + 3;
	// attr.valueLen = vlen;
	// attr.disabled = false;
	aos_assert_r(attr.nameEnd >= attr.nameStart, false);
	aos_assert_r(attr.valueLen >= 0, false);

	mAttrs.append(attr);
	memcpy(&buffData[attr.nameStart], name.data(), name.length());
	buffData[attr.nameEnd+1] = '=';
	buffData[attr.nameEnd+2] = '"';
	memcpy(&buffData[attr.valueStart], value, vlen);
	buffData[attr.valueStart+attr.valueLen] = '"';
	aos_assert_r(AosXmlTagSanityCheck(this), false);

	if (mTagEnd != -1)
	{
		aos_assert_r(buffData[mTagEnd] == '>', false);
		aos_assert_r(buffData[mTagEnd-1] != ' ', false);
	}
	return true;
}


bool
AosXmlTag::adjustData(const int pos, const int delta)
{
	// The caller wants to change the data at the position 'pos'. 
	// If 'delta' > 0, it means it needs to add data. Otherwise, 
	// it is to reduce the data. If it is to reduce the data, 
	// this function simply moves the memory back. If it is
	// to add data, it checks whether there is enough space
	// to grow the data. If not, it needs to allocate additional
	// memory for it.
	//
	if (delta == 0) return true;
	int datalen = mBuff->getDataLength();
	if (datalen + delta >= mBuff->getBufferLength())
	{
		// Need to expand the memory. 
		aos_assert_r(mBuff->determineMemory(datalen + delta), false);
	}

	// It has enough space. Need to move the contents. If 'delta'
	// is positive, it means starting from 'pos', it needs to make 
	// a room for 'delta' number of characters, which means we need
	// move the memory starting at 'pos' backword 'delta' number of
	// chars. 
	aos_assert_r(datalen >= pos, false);
	char *buffData = mBuff->getData();
	if (delta > 0)
	{
		if (datalen - pos > 0)
			memmove(&buffData[pos+delta], &buffData[pos], datalen - pos);
	}
	else
	{
		// This means we need to shrink the data, starting
		// at position 'pos', 
		int newpos = pos-delta;
		memmove(&buffData[pos], &buffData[newpos], datalen-newpos);
	}
	datalen += delta;
	if (mParentTag && mParentTag != mRoot)
	{
		mParentTag->lengthChanged(delta);
	}
	mBuff->setDataLength(datalen);

	// Adjust the pointers
	aos_assert_r(adjustPointers(pos, delta), false);
	aos_assert_r(AosXmlTagSanityCheck(this), false);
	return true;
}


bool
AosXmlTag::lengthChanged(const int delta)
{
	mTagEnd += delta;
	if (mParentTag && mParentTag != mRoot)
	{
		mParentTag->lengthChanged(delta);
	}
	return true;
}


bool
AosXmlTag::adjustPointers(
		const int pos, 
		const int delta) 
{
	// The data has changed at the position. The change is 'delta', 
	// which means if it is < 0, the data is reduced. Otherwise, 
	// the data is incremented. 
	
	// Adjust the mTagStart and mTagEnd
	if (mTagStart >= pos) mTagStart += delta;
	if (mTagEnd >= pos) mTagEnd += delta;

	if (mTagnameStart > pos)
	{
		mTagnameStart += delta;
		mTagnameEnd += delta;
	}

	// Adjust the texts
	for (int i=0; i<mNumTexts; i++)
	{
		if (mText[i].start > pos)
		{
			mText[i].start += delta;
		}
	}

	// Adjust the attributes
	for (int i=0; i<mAttrs.entries(); i++)
	{
		if (mAttrs[i].nameStart > pos) 
		{
			mAttrs[i].nameStart += delta;
			mAttrs[i].nameEnd += delta;
		}

		if (mAttrs[i].valueStart > pos) 
		{
			mAttrs[i].valueStart += delta;
		}
	}

	// Adjust the subtags
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		mChildTags[i]->adjustPointers(pos, delta);
	}

	if (!mRoot && pos > mTagEnd)
	{
		// This means that new contents are appended to the data. 
		// Need to extend mTagEnd and all the subtags whose 
		// mTagEnd is also < pos
		if (mTagEnd != -1) mTagEnd += delta;
		for (int i=0; i<mChildTags.entries(); i++)
		{
			aos_assert_r(mChildTags[i], false);
			mChildTags[i]->extendData(pos, delta);
		}
	}

	return true;
}


bool
AosXmlTag::extendData(const int pos, const int delta)
{
	// The data buffer has been extended. If this tag's mTagEnd
	// is < pos, it needs to extend its data.
	if (mTagEnd != -1 && mTagEnd < pos) mTagEnd += delta;
	return true;
}


bool
AosXmlTag::changeAttrName(
		const OmnString &aname, 
		const OmnString &newname, 
		bool &found)
{
	found = false;
	int delta = newname.length() - aname.length();
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		if (namelen == aname.length() && 
			strncmp((char *)&buffData[start], aname.data(), namelen) == 0)
		{
			// Found the attribute
			if (mRoot)
			{
				aos_assert_r(mRoot->adjustData(start, delta), false);
			}
			else
			{
				aos_assert_r(adjustData(start, delta), false);
			}
			buffData = mBuff->getData();
			strncpy((char *)&buffData[start], newname.data(), newname.length());
			mAttrs[i].nameEnd = start + newname.length() - 1;
			found = true;
			aos_assert_r(AosXmlTagSanityCheck(this), false);
			return true;
		}
	}
	found = false;
	return true;
}


OmnString
AosXmlTag::removeAttr(const int idx)
{
	if (mReadOnly)
	{
		OmnAlarm << "Doc is read-only" << enderr;
	}

	char *buffData = mBuff->getData();
	aos_assert_r(idx >= 0 && idx < mAttrs.entries(), "");
	int start = mAttrs[idx].nameStart;
	int end = mAttrs[idx].nameEnd;
	int valstart = mAttrs[idx].valueStart;
	int vallen = mAttrs[idx].valueLen;
	int valend = valstart + vallen;
	int delta = valend - start + 1;
	if (start > 0 && buffData[start-1] == ' ')
	{
		start--;
		delta++;
	}
	delta = -delta;

	OmnString name((char *)&buffData[start], end-start+1);
	mAttrs.remove(idx, true);
	if (mRoot)
	{
		aos_assert_r(mRoot->adjustData(start, delta), "");
	}
	else
	{
		aos_assert_r(adjustData(start, delta), "");
	}
	buffData = mBuff->getData();
	aos_assert_r(AosXmlTagSanityCheck(this), "");
	return name;
}


/*
int
AosXmlTag::removeEmptyAttr(const OmnString &aname, const bool recursive)
{
	// This function removes empty attributes. If 'name' is not 
	// empty, it only removes the named attribute. Otherwise, 
	// it removes all empty attributes. 
	int found = 0;
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		if (mAttrs[i].valueLen > 0) continue;

		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		if (aname == "" || 
			(namelen == aname.length() && 
			 strncmp((char *)&buffData[start], aname.data(), namelen) == 0))
		{
			// Found the attribute
			int end = mAttrs[i].valueStart + mAttrs[i].valueLen + 1;
			int delta = -(end - start);
			if (mRoot)
			{
				aos_assert_r(mRoot->adjustData(start, delta), false);
			}
			else
			{
				aos_assert_r(adjustData(start, delta), false);
			}
			buffData = mBuff->getData();
			mAttrs.remove(i, true);
			i--;
			aos_assert_r(AosXmlTagSanityCheck(this), false);
			found++;
		}
	}

	if (!recursive) return found;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		int nn = mChildTags[i]->removeEmptyAttr(aname, recursive);
		aos_assert_r(nn >= 0, -1);
		found += nn;
	}
	return found;
}
*/


int
AosXmlTag::removeSubAttr(
		const OmnString &aname, 
		const int flag, 
		const bool onlyone) 
{
	// Chen Ding, 12/07/2010
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, false);
		return mChildTags[0]->removeSubAttr(aname, flag, onlyone);
	}

	// It removes the attribute 'aname' from the subtags. 
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		mChildTags[i]->removeAttr(aname, flag, onlyone);
	}
	return true;
}


int
AosXmlTag::removeAttr(
		const OmnString &aname, 
		const int flag, 
		const bool onlyone) 
{
	static OmnString lsValue;

	return removeAttr1(aname, flag, onlyone, false, lsValue);
}


int
AosXmlTag::removeAttr1(
		const OmnString &aname, 
		const int flag, 
		const bool onlyone, 
		const bool retrieveValue,
		OmnString &value)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, false);
		return mChildTags[0]->removeAttr1(aname, flag, onlyone, retrieveValue, value);
	}

	int found = 0;
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		if (namelen == aname.length() && 
			strncmp((char *)&buffData[start], aname.data(), namelen) == 0)
		{
			// Found the attribute
			int end = mAttrs[i].valueStart + mAttrs[i].valueLen + 1;
			if (buffData[end] == ' ') end++;
			int delta = -(end - start);

			if (retrieveValue)
			{
				value.assign((char *)&buffData[mAttrs[i].valueStart], 
					mAttrs[i].valueLen);
			}
			if (mRoot)
			{
				aos_assert_r(mRoot->adjustData(start, delta), false);
			}
			else
			{
				aos_assert_r(adjustData(start, delta), false);
			}
			buffData = mBuff->getData();
			mAttrs.remove(i, true);
			aos_assert_r(AosXmlTagSanityCheck(this), false);

			if (onlyone) return 1;

			found++;
		}
	}

	if (flag <= 0) return found;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		int nn = mChildTags[i]->removeAttr1(aname, (flag-1), 
			onlyone, retrieveValue, value);
		aos_assert_r(nn >= 0, -1);

		if (onlyone && nn > 0) return nn;
		found += nn;
	}
	return found;
}


int
AosXmlTag::removeDupeAttr()
{
	// This function removes all the duplicated attributes. 
	// If a duplicate attribute is found, it is simply removed.
	int found = 0;
	int num = mAttrs.entries();
	char *buffData = mBuff->getData();
	for (int i=0; i<num-1; i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		for (int k=i+1; k<num; k++)
		{
			int start1 = mAttrs[k].nameStart;
			int end1 = mAttrs[k].nameEnd;
			int namelen1 = end1 - start1 + 1;

			if (namelen == namelen1 && 
				strncmp((char *)&buffData[start], 
					(char *)&buffData[start1], namelen) == 0)
			{
				// Found the attribute
				int delta = -(mAttrs[k].valueLen+1);
				if (mRoot)
				{
					aos_assert_r(mRoot->adjustData(start1, delta), false);
				}
				else
				{
					aos_assert_r(adjustData(start1, delta), false);
				}
				buffData = mBuff->getData();
				mAttrs.remove(k, true);
				num--;
				k--;
				found++;
			}
		}
	}

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		int nn = mChildTags[i]->removeDupeAttr();
		aos_assert_r(nn >= 0, -1);
		found += nn;
	}

	aos_assert_r(AosXmlTagSanityCheck(this), false);
	return found;
}


bool
AosXmlTag::sanityCheck()
{
	if(!sgSanityCheck)
	{
		return true;
	}
	
	aos_assert_r(mDebugFlag == eDebugFlag, false);

	aos_assert_r(mBuff, false);
	if (mRoot)
	{
		aos_assert_r(mRoot->mDebugFlag == eDebugFlag, false);
		aos_assert_r(mRoot->mChildTags.entries() == 1, false);
		aos_assert_r(mRoot->mBuff == mBuff, false);
	}
	else
	{
		aos_assert_r(mTagStart == 0, false);
		aos_assert_r(mChildTags.entries() == 1, false);
	}

	int datalen = mBuff->getDataLength();
	char *buffData = mBuff->getData();
	aos_assert_r(mTagnameEnd >= mTagnameStart, false);

	if (mTagEnd != -1)
		aos_assert_r(mTagEnd - mTagStart + 1 <= datalen, false);

	for (int i=0; i<mAttrs.entries(); i++)
	{
		aos_assert_r(mAttrs[i].nameEnd >= mAttrs[i].nameStart, false);
		aos_assert_r(mAttrs[i].valueLen > 0, false);

		OmnString aname((char *)&buffData[mAttrs[i].nameStart], 
				mAttrs[i].nameEnd - mAttrs[i].nameStart + 1);
		for (int k=i+1; k<mAttrs.entries(); k++)
		{
			OmnString aname1((char *)&buffData[mAttrs[k].nameStart], 
				mAttrs[k].nameEnd - mAttrs[k].nameStart + 1);
			aos_assert_r(aname != aname1, false);
		}
	}

	for (int i=0; i<mNumTexts; i++)
	{
		aos_assert_r(mText[i].len >= 0, false);
		if (mText[i].isCdata) 
		{
			aos_assert_r(mText[i].len >= 12, false);
			aos_assert_r(strncmp((char *)&buffData[mText[i].start], 
					"<![CDATA[", 9) == 0, false);
			aos_assert_r(strncmp((char *)&buffData[mText[i].start+mText[i].len-3], 
					"]]>", 3) == 0, false);
		}
	}

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		aos_assert_r(mChildTags[i]->sanityCheck(), false);
	}
	return true;
}


bool
AosXmlTag::attrIgnored(const int start, const int len)
{
	// Currently, the following attributes are ignored when 
	// constructing IILs:
	// 	AOSTAG_CREATOR
	// 	AOSTAG_SITEID
	// 	AOSTAG_OBJID
	// 	AOSTAG_DOCID
	// 	AOSTAG_OTYPE
	// 	AOSTAG_SUBTYPE
	// 	AOSTAG_PARENTC
	// 	AOSTAG_MODUSER
	// 	AOSTAG_CTIME
	// 	AOSTAG_CT_EPOCH
	// 	AOSTAG_MTIME
	// 	AOSTAG_MT_EPOCH
	// 	AOSTAG_COUNTERCM	comment counter
	// 	AOSTAG_COUNTERDW	down counter
	// 	AOSTAG_COUNTERLK	link counter
	// 	AOSTAG_COUNTERRC	recommendataion counter
	// 	AOSTAG_COUNTERWR	modify counter
	// 	AOSTAG_COUNTERUP	up counter
	// 	AOSTAG_COUNTERRD	read counter
	// All reserved attribute names start with "zky_". Currently, 
	// this function is not as efficient as we want it. 
	// Future Enhancement is expected. 
	//
	// The caller has ensured the first four characters are "zky_". 
	

	char *buffData = mBuff->getData();
	//process "__n"
	if (isAttrIgnored(mAttrs, mAttrLoopIdx))
	{
		return true;
	}
	return isMetaAttr(OmnString((char*)&buffData[start], len));
	switch (buffData[start+4])
	{
	case 'c':
		 if (len == 9 &&
		     buffData[start+5] == 'n' &&
		     buffData[start+6] == 't')
		 {
			 // Try those counters
			 if ((buffData[start+7] == 'c' && buffData[start+8] == 'm') ||
			     (buffData[start+7] == 'd' && buffData[start+8] == 'w') ||
			     (buffData[start+7] == 'l' && buffData[start+8] == 'k') ||
			     (buffData[start+7] == 'r' && buffData[start+8] == 'c') ||
			     (buffData[start+7] == 'w' && buffData[start+8] == 'r') ||
			     (buffData[start+7] == 'u' && buffData[start+8] == 'p') ||
			     (buffData[start+7] == 'r' && buffData[start+8] == 'd'))
			 {
				 // These are the counters
				 // 	AOSTAG_COUNTERCM	comment counter
				 // 	AOSTAG_COUNTERDW	down counter
				 // 	AOSTAG_COUNTERLK	link counter
				 // 	AOSTAG_COUNTERRC	recommendataion counter
				 // 	AOSTAG_COUNTERWR	modify counter
				 // 	AOSTAG_COUNTERUP	up counter
				 // 	AOSTAG_COUNTERRD	read counter
				 return true;
			 }
		 }

		 if (len == 9 &&
			 buffData[start+5] == 't' && 
			 buffData[start+6] == 'i' && 
			 buffData[start+7] == 'm' &&
			 buffData[start+8] == 'e') return true;		// AOSTAG_CTIME

		 if (len == 10 &&
			 buffData[start+5] == 't' && 
			 buffData[start+6] == 'm' && 
			 buffData[start+7] == 'e' &&
			 buffData[start+8] == 'p' &&
			 buffData[start+9] == 'o') return true;		// AOSTAG_CT_EPOCH

		 if (len == 9 &&
			 buffData[start+5] == 'r' && 
			 buffData[start+6] == 't' && 
			 buffData[start+7] == 'o' && 
			 buffData[start+8] == 'r') return true;		// AOSTAG_CREATOR
		 return false;

	case 'd':
		 if (len == 9 &&
			 buffData[start+5] == 'o' && 
			 buffData[start+6] == 'c' && 
			 buffData[start+7] == 'i' && 
			 buffData[start+8] == 'd') return true;		// AOSTAG_DOCID
		 return false;

	case 'm':
		 if (len == 8 && 
			 buffData[start+5] == 'u' && 
			 buffData[start+6] == 's' && 
			 buffData[start+7] == 'r') return true;		// AOSTAG_MODUSER

		 if (len == 9 && 
			 buffData[start+5] == 't' && 
			 buffData[start+6] == 'i' && 
			 buffData[start+7] == 'm' && 
			 buffData[start+8] == 'e') return true;		// AOSTAG_MTIME

		 if (len == 10 && 
			 buffData[start+5] == 't' && 
			 buffData[start+6] == 'm' && 
			 buffData[start+7] == 'e' && 
			 buffData[start+8] == 'p' &&
			 buffData[start+9] == 'o') return true;		// AOSTAG_MT_EPOCH Zky2406 Phnix  1/15/2011
		 return false;

	case 'o':
		 if (len == 9 && 
			 buffData[start+5] == 'b' && 
			 buffData[start+6] == 'j' && 
			 buffData[start+7] == 'i' && 
			 buffData[start+8] == 'd') return true;		// AOSTAG_OBJID

		 if (len == 9 && 
			 buffData[start+5] == 't' && 
			 buffData[start+6] == 'y' && 
			 buffData[start+7] == 'p' && 
			 buffData[start+8] == 'e') return true;		// AOSTAG_OTYPE
		 return false;

	case 'p':
		 if (len == 9 && 
			 buffData[start+5] == 'c' && 
			 buffData[start+6] == 't' && 
			 buffData[start+7] == 'r' && 
			 buffData[start+8] == 's') return true;		// AOSTAG_PARENTC
		 return false;

	case 's':
		 if (len == 9 && 
			 buffData[start+5] == 't' && 
			 buffData[start+6] == 'y' && 
			 buffData[start+7] == 'p' && 
			 buffData[start+8] == 'e') return true;		// AOSTAG_SUBTYPE

		 if (len == 10 && 
			 buffData[start+5] == 'i' && 
			 buffData[start+6] == 't' && 
			 buffData[start+7] == 'e' && 
			 buffData[start+8] == 'i' && 
			 buffData[start+9] == 'd') return true;		// AOSTAG_SITEID
		 return false;

	default: return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosXmlTag::isDocSame(const AosXmlTagPtr &rhs)
{
	// Check whether the attributes in this object are defined in 'rhs'
	// and their values are the same. Note that this function does not
	// verify that this object contains all the attributes in 'rhs'
	bool exist;
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int nlen = mAttrs[i].nameEnd - mAttrs[i].nameStart + 1;
		OmnString name((char *)&buffData[mAttrs[i].nameStart], nlen);
		OmnString value((char *)&buffData[mAttrs[i].valueStart], mAttrs[i].valueLen);

		if (rhs->getAttrStr1(name, exist) != value) 
		{
			OmnScreen << "name:" << name << ":" 
				<< rhs->getAttrStr1(name, exist) << ":" << value << endl;
			return false;
		}
	}

	/*
	AosXmlTagPtr subtags[eMaxSubtags];
	int numSubtags = 0;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		// We need to prepare the case that the subtags are in 
		// different orders. 
		while (1)
		{
			AosXmlTagPtr tag1 = mChildTags[i];
			OmnString tname = tag1->getTagname();
			AosXmlTagPtr tag2 = rhs->getFirstChild(tname);
			if (!tag2) 
			{
				OmnMark;
				return false;
			}
			if (!tag1->isDocSame(tag2)) 
			{
				OmnMark;
				return false;
			}
		}
	}

	int len1;
	char *txt1 = (char *)getNodeText(len1);
	int len2;
	char *txt2 = (char *)rhs->getNodeText(len2);
	if (len1 != len2) 
	{
		OmnMark;
		return false;
	}
	if (strncmp(txt1, txt2, len1) != 0) 
	{
		OmnMark;
		return false;
	}
	*/
	return true;
}


bool			
AosXmlTag::getAttr(const int idx, OmnString &name, OmnString &value)
{
	// It retrieves the 'idx'-th attribute. If not found, it returns
	// false. 
	// if (!mRoot)
	// {
	// 	aos_assert_r(mChildTags.entries() == 1, false);
	// 	return mChildTags[0]->getAttr(idx, name, value);
	// }

	aos_assert_r(idx >= 0 && idx < mAttrs.entries(), false);

	char *buffData = mBuff->getData();
	int start = mAttrs[idx].nameStart;
	int end = mAttrs[idx].nameEnd;
	name.assign((char *)&buffData[start], end-start+1);

	start = mAttrs[idx].valueStart;
	value.assign((char *)&buffData[start], mAttrs[idx].valueLen);
	return true;
}


bool 		
AosXmlTag::setText(const OmnString &text, const bool isCdata)
{
	// Ken Lee, 2014/04/16
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->setText(text, isCdata);
	}

	// It removes all the texts and set 'text' as the new text.
	int start = -1;
	int len = text.length();
	bool contains_bracket = false;
	if (isCdata) 
	{
		// Check whether the data already contains <![CDATA[
		const char *data = text.data();
		contains_bracket = true;
		if (len < 12 || strncmp(&data[0], "<![CDATA[", 9) != 0)
		{
			len += 12;
			contains_bracket = false;
		}
	}

	if (mNumTexts > 0)
	{
		aos_assert_r(removeTexts(), false);
	}
	
	// It does not have texts. It can be in the following forms:
	// <tagname ...>xxx</tagname>
	// <tagname .../>
	int idx = mTagnameEnd+1;
	int lastslash = -1;
	char *buffData = mBuff->getData();
	while (idx<mTagEnd && buffData[idx] != '>') 
	{
		if (buffData[idx] == '/') lastslash = idx;
		idx++;
	}

	aos_assert_r(buffData[idx] == '>', false);
	aos_assert_r(idx>0, false);

	// Found the ">". Need to 
	bool hasClosingTag = true;
	if (lastslash != -1)
	{
		// Found a slash. Need to check whether there are anything
		// between '/' and '>'
		bool found = false;
		for (int i=lastslash+1; i<idx; i++) 
		{
			if (buffData[i] != ' ' && buffData[i] != '\t')
			{
				found = true;
				break;
			}
		}

		if (!found) hasClosingTag = false;
	}

	start = idx+1;
	OmnString tagname = getTagname();
	int adjustlen = len;
	if (!hasClosingTag)
	{
		buffData[lastslash] = '>';
		// buffData[lastslash+1] = ' ';
		 int namelen = mTagnameEnd - mTagnameStart + 1;
		// tagname.assign((char *)&buffData[mTagnameStart], namelen);
		adjustlen += namelen + 2;
		start--;
	}

	if (mRoot)
	{
		aos_assert_r(mRoot->adjustData(start, adjustlen), false);
	}
	else
	{
		aos_assert_r(adjustData(start, adjustlen), false);
	}
	buffData = mBuff->getData();

	aos_assert_r(start != -1, false);
	aos_assert_r(mNumTexts < eMaxTexts-1, false);
	mText[mNumTexts].start = start;
	mText[mNumTexts].len = len;
	mText[mNumTexts].isCdata = isCdata;
	mText[mNumTexts].disabled = false;
	mNumTexts++;

	if (isCdata && !contains_bracket)
	{
		strncpy((char *)&buffData[start], "<![CDATA[", 9); start += 9;
		strncpy((char *)&buffData[start], text.data(), text.length());
		start += text.length();
		strncpy((char *)&buffData[start], "]]>", 3);
		start += 3;
	}
	else
	{
		strncpy((char *)&buffData[start], text.data(), text.length());
		start += text.length();
	}

	//james check has subnode.
	if (tagname != "" && (mChildTags.entries() == 0))
	{
		strncpy((char *)&buffData[start], "</", 2); start += 2;
		strncpy((char *)&buffData[start], tagname.data(), tagname.length());
		start += tagname.length();
		strncpy((char *)&buffData[start], ">", 1);
	}

	aos_assert_r(AosXmlTagSanityCheck(this), false);
	return true;
}


bool
AosXmlTag::removeTexts()
{
	// It removes all the texts. 
	char *buffData = mBuff->getData();
	while (mNumTexts > 0)
	{
		int start = mText[0].start;
		int len = mText[0].len;
		int end = start + len - 1;
		/*
		if (mText[0].isCdata)
		{
			// It is cdata. Need to remove <![CDATA[ and ]]>. 
			while (buffData[start] != '<' && start>0) start--;
			aos_assert_r(start > 0 && buffData[start] == '<', false);

			while (buffData[end] != '>' && end < mTagEnd) end++;
//			aos_assert_r(end < mTagEnd && buffData[end] == '>', false);
			len = end - start + 1;
		}
		*/
		if (mText[0].isCdata)
		{
			aos_assert_r(buffData[start] == '<', false);
			aos_assert_r(buffData[end] == '>', false);
		}
		for (int i=0; i<mNumTexts-1; i++) mText[i] = mText[i+1];
		mNumTexts--;

		if (mRoot) 
		{
			aos_assert_r(mRoot->adjustData(start, -len), false);
		}
		else 
		{
			aos_assert_r(adjustData(start, -len), false);
		}
		buffData = mBuff->getData();
	}
	mNumTexts = 0;
	return true;
}


bool
AosXmlTag::removeNode()
{
	// This function is used by torturer.
	int num = mChildTags.entries();
	if (num <= 0) return false;

	for (int i=0; i<num; i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->removeNode()) return true;
	}

	int vv = rand() % num;
	removeNode(vv);
	return true;
}


bool		
AosXmlTag::removeNode(const int idx)
{
	// It removes the 'idx'-th node. 
	aos_assert_r(idx >= 0 && idx < mChildTags.entries(), false);
	int start = mChildTags[idx]->mTagStart;
	int end = mChildTags[idx]->mTagEnd;
	int len = end - start + 1;
	aos_assert_r(len >= 0, false);
	if (mRoot) 
	{
		aos_assert_r(mRoot->adjustData(start, -len), false);
	}
	else 
	{
		aos_assert_r(adjustData(start, -len), false);
	}
	mChildTags.remove(idx, true);
	return true;
}


int
AosXmlTag::removeNode(
		const OmnString &name, 
		const bool recursive,
		const bool removeall)
{
	// It removes the node whose name is 'name'. If found, the node
	// is removed and the number of nodes being removed is returned. 
	// If 'recursive' is true, it will search the child nodes.
	// If 'removeall' is true, it will remove all the matching nodes.
	// Otherwise, it will remove the first matching node.
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->removeNode(name, recursive, removeall);
	}

	int numRemoved = 0;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->tagnameMatch(name)) 
		{
			// Found the node. 
			int start = mChildTags[i]->mTagStart;
			int end = mChildTags[i]->mTagEnd;
			int len = end - start + 1;
			aos_assert_r(len >= 0, false);
			aos_assert_r(mRoot->adjustData(start, -len), false);
			mChildTags.remove(i, true);
			numRemoved++;

			if (!removeall) return 1;
		}
	}

	if (!recursive) return numRemoved;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		numRemoved += mChildTags[i]->removeNode(name, recursive, removeall);
	}

	return numRemoved;
}


OmnString
AosXmlTag::xpathQuery(
		const OmnString &path, 
		bool &exist,
		const OmnString &dft)
{
	exist = false;
	OmnStrParser1 parser(path, "/", false, false);
	OmnString childname = parser.nextWord("");

	if (childname == "") return dft;
	OmnString attrname = parser.nextWord("");
	if (attrname == "") 
	{
		if (childname.data()[0] == '@')
		{
			return getAttrStr1(&childname.data()[1], exist, dft);
		}
		return getAttrStr1(childname, exist, dft);
	}

	AosXmlTagPtr child = getFirstChild(childname);
	if (!child) return dft;
	return child->xpathQuery(attrname, parser, exist, dft);
}


OmnString
AosXmlTag::xpathQuery(
		const OmnString &cname, 
		OmnStrParser1 &parser, 
		bool &exist,
		const OmnString &dft)
{
	exist = false;
	aos_assert_r(cname != "", dft);
	OmnString aname = parser.nextWord("");
	char *buffData = mBuff->getData();
	if (aname == "")
	{
		if (cname == "_#text" || cname == "_$text")
		{
			// Need to retrieve its text
			int len;
			char *data = (char *)getNodeText(len);
			if (len == 0) return "";
			OmnString ss(data, len);
			exist = true;
			return ss;
		}

		if (cname == "_#node")
		{
			// It is to retrieve the node as text
			OmnString ss((char *)&buffData[mTagStart], mTagEnd - mTagStart + 1);
			exist = true;
			return ss;
		}
		
		// It is to retrieve an attribute
		const char *data = cname.data();
		if (data[0] == '@')
		{
			return getAttrStr1(&data[1], exist, dft);
		}

		return getAttrStr1(cname, exist, dft);
	}

	AosXmlTagPtr child = getFirstChild(cname);
	if (!child) return dft;
	return child->xpathQuery(aname, parser, exist, dft);
}



bool
AosXmlTag::xpathSetAttr(const OmnString &path, const OmnString &value, const bool create)
{
	OmnStrParser1 parser(path, "/", false, false);
	OmnString nodename = parser.nextWord();
	OmnString aname = parser.nextWord();
	if (aname == "")
	{
		if (nodename == "_#text")
		{
			return setText(value, true);
		}
		
		const char *data = nodename.data();
		if (data[0] == '@')
		{
			return setAttr(&data[1], value);
		}

		return setAttr(nodename, value);
	}

	AosXmlTagPtr child;
	if (isRootTag())
	{
		child = getFirstChild()->getFirstChild(nodename);
	}
	else
	{
		child = getFirstChild(nodename);
	}


	if (child)
	{
		return child->xpathSetAttr(aname, parser, value, create);
	}
	else
	{
		if (!create) return false;

		// Create new node.
		OmnString nodestr = "<";
		nodestr << nodename << " />";
		AosXmlParser xmlparser;
		AosXmlTagPtr node = xmlparser.parse(nodestr, "" AosMemoryCheckerArgs);
		aos_assert_r(node, false);
		addNode(node);
		child = getFirstChild(nodename);
		aos_assert_r(child, false);
		return child->xpathSetAttr(aname, parser, value, create);
	}
	
	return false;
}

bool
AosXmlTag::xpathSetAttr(const OmnString &path, const OmnString &value)
{
	OmnStrParser1 parser(path, "/", false, false);
	OmnString nodename = parser.nextWord();
	OmnString aname = parser.nextWord();
	if (aname == "")
	{
		if (nodename == "_#text")
		{
			return setText(value, true);
		}
		
		const char *data = nodename.data();
		if (data[0] == '@')
		{
			return setAttr(&data[1], value);
		}

		return setAttr(nodename, value);
	}

	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return false;

	return child->xpathSetAttr(aname, parser, value, false);
}

OmnString
AosXmlTag::xpathGetAttr(const OmnString &path, const OmnString &dft)
{
	OmnStrParser1 parser(path, "/", false, false);
	OmnString nodename = parser.nextWord();
	OmnString aname = parser.nextWord();
	OmnString value;
	if (aname == "")
	{
		if (nodename == "_#text")
		{
			value = getNodeText(nodename);
			if (value != "")
			{
				return value;
			}
		}
		
		const char *data = nodename.data();
		if (data[0] == '@')
		{
			value = getAttrStr(&data[1], "");	
			if (value != "")
			{
				return value;
			}
		}
		
		return dft;
	}

	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return dft;

	return child->xpathGetAttr(aname, dft);
}


bool
AosXmlTag::xpathSetAttr(
		const OmnString &nodename, 
		OmnStrParser1 &parser, 
		const OmnString &value,
		const bool create)
{
	OmnString aname = parser.nextWord();
	// Felicia
	//if (aname == "_#text")
	if (aname == "")
	{
		if (nodename == "_#text")
		{
			return setText(value, true);
		}
		
		const char *data = nodename.data();
		if (data[0] == '@')
		{
			return setAttr(&data[1], value);
		}

		return setAttr(nodename, value);
	}

	AosXmlTagPtr child = getFirstChild(nodename);
	if (child)
	{
		return child->xpathSetAttr(aname, parser, value, create);
	}
	else
	{
		if (!create) return false;

		// Create new node.
		OmnString nodestr = "<";
		nodestr << nodename << " />";
		AosXmlParser xmlparser;
		AosXmlTagPtr node = xmlparser.parse(nodestr, "" AosMemoryCheckerArgs);
		aos_assert_r(node, false);
		addNode(node);
		child = getFirstChild(nodename);
		aos_assert_r(child, false);
		return child->xpathSetAttr(aname, parser, value, create);
	}
	return false;
}


bool
AosXmlTag::setTagEnd(const int idx) 
{
	mTagEnd = idx;
	aos_assert_r(AosXmlTagSanityCheck(this), false);
	return true;
}


bool
AosXmlTag::consistentCheck()
{
	// Once an object was created, the following attributes
	// shall not be modified:
	// 		siteid
	// 		docid
	// When a user retrieves an object, it may modify these attributes. 
	// In order to detect whether one of these attributes was modified,
	// a signature was added to every object when such an object was
	// created. When an object is to be modified, it should call this
	// function. The function checks whether the object signature is
	// correct.
	return true;
}


bool			
AosXmlTag::replaceAttrValue(
		const OmnString &name, 
		const bool recursive, 
		const OmnString &oldvalue, 
		const OmnString &newvalue)
{
	// This function replaces the value of the attribute named
	// 'name' and its value is 'oldvalue' with the new value 
	// 'newvalue'. If 'recursive' is true, it will do it 
	// recursively.
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
	 	return mChildTags[0]->replaceAttrValue(name, recursive, oldvalue, newvalue);
	}

	int len = 0;
	int oldlen = oldvalue.length();
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		if (name == "" || 
			(namelen == name.length() && 
			 strncmp((char *)&buffData[start], name.data(), namelen) == 0))
		{
			mCrtAttrIdx = i+1;
			len = mAttrs[i].valueLen;
			if (len == oldlen && strncmp(
					(char *)&buffData[mAttrs[i].valueStart], 
					oldvalue.data(), len) == 0)

			{
				OmnString nn;
				if (name == "") 
				{
					nn.assign((char *)&buffData[start], namelen);
					setAttr(nn, newvalue);
					buffData = mBuff->getData();
				}
				else
				{
					setAttr(name, newvalue);
					buffData = mBuff->getData();
					break;
				}
			}
		}
	}

	if (!recursive) return true;

	// Try subtags
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		mChildTags[i]->replaceAttrValue(name, true, oldvalue, newvalue);
	}

	return true;
}


bool			
AosXmlTag::replaceAttrValueSubstr(
		const OmnString &name, 
		const bool recursive, 
		const OmnString &substrfrom, 
		const OmnString &substrto)
{
	// This function replaces the value of the attribute named
	// 'name' and its value is 'substrfrom' with the new value 
	// 'substrto'. If 'recursive' is true, it will do it 
	// recursively.
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
	 	return mChildTags[0]->replaceAttrValue(name, recursive, substrfrom, substrto);
	}

	int len = 0;
	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int namelen = end - start + 1;
		if (name == "" || 
			(namelen == name.length() && 
			 strncmp((char *)&buffData[start], name.data(), namelen) == 0))
		{
			mCrtAttrIdx = i+1;
			len = mAttrs[i].valueLen;
			OmnString vv((char *)&buffData[mAttrs[i].valueStart], len);
			if (vv.replace(substrfrom, substrto, true) > 0)
			{
				if (name == "") 
				{
					OmnString nn;
					nn.assign((char *)&buffData[start], namelen);
					setAttr(nn, vv);
					buffData = mBuff->getData();
				}
				else
				{
					setAttr(name, vv);
					buffData = mBuff->getData();
					break;
				}
			}
		}
	}

	if (!recursive) return true;

	// Try subtags
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		mChildTags[i]->replaceAttrValue(name, true, substrfrom, substrto);
	}

	return true;
}


bool			
AosXmlTag::replaceAttrName(
		const OmnString &existName, 
		const OmnString &newName,
		const bool recursive)
{
	// This function replaces attribute name. 
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
	 	return mChildTags[0]->replaceAttrName(existName, newName, recursive);
	}

	bool exist;
	OmnString value = getAttrStr1(existName, exist, "");
	if (exist)
	{
		removeAttr(existName, 1, false);
		if (value != "")
		{
			setAttr(newName, value);
		}
	}

	if (!recursive) return true;

	// Try subtags
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		mChildTags[i]->replaceAttrName(existName, newName, true);
	}

	return true;
}


bool
AosXmlTag::unescapeCdataContents()
{
	// This function checks the text node to see whether there are 
	// escaped sequences. If yes, it unescape the sequence, and if
	// the text is not already CDATA, it makes it CDATA.
	//
	// The reason why we need this function is a bug in OpenLaszlo.
	// When we create a CDATA content that contains special characters
	// such as &, <, >, etc., OpenLaszlo escape them, and turn the
	// CDATA back to a normal text node. This function undoes what
	// OpenLaszlo did (incorrectly). 
	//
	// Below are the escape sequences:
	// &#10;           	the newline				no
	// &#13;           	carriage return			no
	// &#x0A;          	newline					no
	// &#x0a;          	newline					no
	// &#x0D;          	carriage return			no
	// &#x0d;          	carriage return			no
	// &quot;			"						yes
	// &apos;			'						yes
	// &lt;				<						yes
	// &gt;				>						yes
	// &amp;			&						yes
	//
	// For '&amp;', after unescape it, it will continue unescape. 
	// As an example, for the string: 
	// 			&amp;quot
	// it should be unescaped first to:
	// 			&quot;
	// and then to '"'.
	//
	// For each text, 'start' is the start of the contents. If it is
	// a CDATA, it points to the beginning of "<![CDATA[". 'len' 
	// is the length of the contents. If it is CDATA, 'len' includes
	// "<![CDATA[" and "]]>". 
	//	
	// In addition to unescape in the text node, it also checks the
	// special chars in attribute:
	//	1. quotes: the contents cannot contain the same quotes (unless 
	//			it is escaped by '\'.
	//	2. '<' and '>' shall never appear in attributes. Replace it with
	//	   '&lt;' and '&gt;'.
	//	3. '&' must be followed by one of the following:
	//			&quot;
	//			&apos;
	//			&lt;
	//			&gt;
	//			&amp;
	//			&#dd		where 'dd' are digits
	//			&#xXX		where 'XX' are hex characters
	// 
	if (!mBuff) return true;

	char * data = mBuff->getData();
	for (int i=0; i<mNumTexts; i++)
	{
		int start = mText[i].start;
		int len = mText[i].len;
		bool isCdata = mText[i].isCdata;

		// Find the first '&'
		int idx = start;
		int remain = len;
		int charLen = 1; 
		while (1)
		{
			charLen = 1;
			while (remain && data[idx] != '&')
			{
				remain--;
				idx++;
			}

			if (data[idx] != '&' || remain < 4) 
			{
				// No more '&'
				break;
			}

			// Found a '&'. 
			bool found = true;
			int delta = 0;
			if (remain >= 6 &&
				data[idx+1] == 'q' &&
				data[idx+2] == 'u' &&
				data[idx+3] == 'o' &&
				data[idx+4] == 't' &&
				data[idx+5] == ';')
			{
				// It is '&quot'
				data[idx] = '"';
				delta = -5;
			}
			else if (remain >= 6 &&
				 	data[idx+1] == 'a' &&
				 	data[idx+2] == 'p' &&
				 	data[idx+3] == 'o' &&
				 	data[idx+4] == 's' &&
				 	data[idx+5] == ';')
			{
				// It is '&apos'
				data[idx] = '\'';
				delta = -5;
			}
			else if (data[idx+1] == 'l' &&
				 	data[idx+2] == 't' &&
				 	data[idx+3] == ';')
			{
				// It is '&lt;'
				data[idx] = '<';
				delta = -3;
			}
			else if (data[idx+1] == 'g' &&
				 	data[idx+2] == 't' &&
				 	data[idx+3] == ';')
			{
				// It is '&gt;'
				data[idx] = '>';
				delta = -3;
			}
			//Andy Zhang 2013-08-16 start
			else if (data[idx+1] == 'l' &&
					data[idx+2] == 'd' &&
					data[idx+3] == 'q' &&
					data[idx+4] == 'u' &&
					data[idx+5] == 'o')
			{
				// Tt is '&ldquo;'
				// e2-80-9c is " " " Utf-8 Coding;
				charLen = 3;
				data[idx] = 0xe2; 	
				data[idx+1] = 0x80; 	
				data[idx+2] = 0x9c; 	
				delta = -4;
			}
			else if (data[idx+1] == 'm' &&
					data[idx+2] == 'd' &&
					data[idx+3] == 'a' &&
					data[idx+4] == 's' &&
					data[idx+5] == 'h')
			{
				// It is "&mdash"
				// 0xe2, 0x80, 0x94 " - " Utf-8 Coding;
				charLen = 3;
				data[idx] = 0xe2;  
				data[idx+1] = 0x80;
				data[idx+2] = 0x94;
				delta = -4;
			}
			else if (data[idx+1] == 'r' &&
					data[idx+2] == 'd' &&
					data[idx+3] == 'q' &&
					data[idx+4] == 'u' &&
					data[idx+5] == 'o')
			{
				// Tt is '&amquo;'
				// e2-80-9d is " " " Utf-8 Coding;
				charLen = 3;
				data[idx] = 0xe2; 	
				data[idx+1] = 0x80; 	
				data[idx+2] = 0x9d; 	
				delta = -4;
			}
			else if (data[idx+1] == 'm' &&
					data[idx+2] == 'i' &&
					data[idx+3] == 'd' &&
					data[idx+4] == 'd' &&
					data[idx+5] == 'o' &&
					data[idx+6] == 't' &&
					data[idx+7] == ';')
			{
				// It is "&middot" 
				// c2-b7 is "-" Utf-8 coding in chinese
				charLen = 2;
				data[idx] = 0xc2;
				data[idx+1] = 0xb7;
				delta = -6;
			}
			else if (data[idx+1] == 'l' &&
					data[idx+2] == 'a' &&
					data[idx+3] == 'm' &&
					data[idx+4] == 'b' &&
					data[idx+5] == 'd' &&
					data[idx+6] == 'a')
			{
				// Tt is '&lambda;'
				// c3-97 is "λ " Utf-8 Coding;
				charLen = 2;
				data[idx] = 0xce;  
				data[idx+1] = 0xbb;
				delta = -6;
			}
			else if (data[idx+1] == 't' &&
					data[idx+2] == 'i' &&
					data[idx+3] == 'm' &&
					data[idx+4] == 'e' &&
					data[idx+5] == 's')
			{
				// Tt is '&times;'
				// c3-97 is " × " Utf-8 Coding;
				charLen = 2;
				data[idx] = 0xc3;  
				data[idx+1] = 0x97;
				delta = -5;
			}
			else if (remain >= 5 &&
					data[idx+1] == 'a' &&
					data[idx+2] == 'm' &&
					data[idx+3] == 'p' &&
					data[idx+4] == ';')
			{
				// It is '&amp;'
				data[idx] = '&';
				delta = -4;
			}
			else
			{
				found = false;
				idx++;
				remain--;
			}

			if (found)
			{
				// If it is not already a CDATA, need to convert it into a CDATA.
				if (!isCdata)
				{
					// Need to make room for '<![CDATA['
					if (mRoot)
					{
						aos_assert_r(mRoot->adjustData(start,  9), false);
					}
					else
					{
						aos_assert_r(adjustData(start, 9), false); 
					}
					data = mBuff->getData();
					data[start] = '<';
					data[start+1] = '!';
					data[start+2] = '[';
					data[start+3] = 'C';
					data[start+4] = 'D';
					data[start+5] = 'A';
					data[start+6] = 'T';
					data[start+7] = 'A';
					data[start+8] = '[';

					start = mText[i].start;
					idx += 9;
					len += 9;

					if (mRoot)
					{
						aos_assert_r(mRoot->adjustData(start+len, 3), false);
					}
					else
					{
						aos_assert_r(adjustData(start+len, 3), false); 
					}

					data = mBuff->getData();
					data[start+len] = ']';
					data[start+len+1] = ']';
					data[start+len+2] = '>';
					mText[i].isCdata = true;
					mText[i].len += 12;
					len += 3;
					remain += 3;
					isCdata = true;
				}

				// Need to move the contents
				if (mRoot)
				{
					aos_assert_r(mRoot->adjustData(idx+charLen, delta), false);
				}
				else
				{
					aos_assert_r(adjustData(idx+charLen, delta), false); 
				}

				mText[i].len += delta;
				remain += delta;
			}
		}
	}

	aos_assert_r(checkAttrValues(), false);

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		mChildTags[i]->unescapeCdataContents();
	}

	return true;
}


bool
AosXmlTag::checkAttrValues()
{
	// It checks the special chars in attribute:
	//  1. Attributes must be quoted by double quotes
	//	2. quotes: double quotes inside an attribute must be escaped by 
	//	   '&quot;'. If it is escaped by '\', change it to '&quot;'.
	//	3. '<' and '>' shall never appear in attributes. Replace it with
	//	   '&lt;' and '&gt;'.
	//	4. '&' must be followed by one of the following:
	//			&quot;		untouched
	//			&apos;		replace with the single quote (')
	//			&lt;		untouched
	//			&gt;		untouched
	//			&amp;		untouched
	//			&#dd		untouched, where 'dd' are digits
	//			&#xXX		untouched, where 'XX' are hex characters
	//	    Otherwise, it will be escaped by '&amp;'.
	// 
	if (!mBuff) return true;
	char *data = mBuff->getData();

	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].valueStart;
		int valuelen = mAttrs[i].valueLen;

		data[start-1] = '"';
		data[start+valuelen] = '"';
		int idx = start;
		int remain = valuelen;
		int delta;

		// Looking for a special character
		while (remain)
		{
			bool isEscaped = false;
			switch (data[idx])
			{
			case '&':
				 isEscaped = true;
				 break;

			case '"':
				 // Make sure double quotes are escaped by '&quot;'
				 if (idx > start && data[idx-1] == '\\')
				 {
					// It is escaped by '\'. This is ok.
					idx--;
					delta = 4;
				 }
				 else
				 {
					delta = 5;
				 }

				 // Replace it with '&quot;'
				 if (mRoot)
				 {
					aos_assert_r(mRoot->adjustData(idx+1, delta), false);
				 }
				 else
				 {
					aos_assert_r(adjustData(idx+1, delta), false); 
				 }
				 data = mBuff->getData();
				 data[idx++] = '&';
				 data[idx++] = 'q';
				 data[idx++] = 'u';
				 data[idx++] = 'o';
				 data[idx++] = 't';
				 data[idx++] = ';';
				 mAttrs[i].valueLen += delta;
				 remain--;
				 break;

			case '<':
				 // This is not allowed. Escape it with '&lt;'.
				 if (mRoot)
				 {
					aos_assert_r(mRoot->adjustData(idx,  3), false);
				 }
				 else
				 {
					aos_assert_r(adjustData(idx, 3), false); 
				 }
				 data = mBuff->getData();
				 data[idx++] = '&';
				 data[idx++] = 'l';
				 data[idx++] = 't';
				 data[idx++] = ';';
				 mAttrs[i].valueLen += 3;
				 remain--;
				 break;

			case '>':
				 // This is not allowed. Escape it with '&lt;'.
				 if (mRoot)
				 {
					aos_assert_r(mRoot->adjustData(idx,  3), false);
				 }
				 else
				 {
					aos_assert_r(adjustData(idx, 3), false); 
				 }
				 data = mBuff->getData();
				 data[idx++] = '&';
				 data[idx++] = 'g';
				 data[idx++] = 't';
				 data[idx++] = ';';
				 mAttrs[i].valueLen += 3;
				 remain--;
				 break;

			case 10:
				 // Chen Ding, 12/05/2010
				 // The newline character '\n'. Need to be replaced with '&#10;'
				 if (mRoot)
				 {
					aos_assert_r(mRoot->adjustData(idx, 4), false);
				 }
				 else
				 {
					aos_assert_r(adjustData(idx, 4), false); 
				 }
				 data = mBuff->getData();
				 data[idx++] = '&';
				 data[idx++] = '#';
				 data[idx++] = '1';
				 data[idx++] = '0';
				 data[idx++] = ';';
				 mAttrs[i].valueLen += 4;
				 remain--;
				 break;

			case 13:
				 // Chen Ding, 12/05/2010
				 // The carragie return '\r'. Need to be replaced with '&#13;'
				 if (mRoot)
				 {
					aos_assert_r(mRoot->adjustData(idx, 4), false);
				 }
				 else
				 {
					aos_assert_r(adjustData(idx, 4), false); 
				 }
				 data = mBuff->getData();
				 data[idx++] = '&';
				 data[idx++] = '#';
				 data[idx++] = '1';
				 data[idx++] = '3';
				 data[idx++] = ';';
				 mAttrs[i].valueLen += 4;
				 remain--;
				 break;

			default:
				 remain--;
				 idx++;
			}

			if (isEscaped)
			{
				// Found a '&'. 
				bool processed = true;
				if (remain >= 6 &&
					data[idx+1] == 'q' &&
					data[idx+2] == 'u' &&
					data[idx+3] == 'o' &&
					data[idx+4] == 't' &&
					data[idx+5] == ';')
				{
					// It is '&quot;'. Do nothing
					idx += 6;
					remain -= 6;
				}
				else if (remain >= 6 &&
			 		data[idx+1] == 'a' &&
			 		data[idx+2] == 'p' &&
			 		data[idx+3] == 'o' &&
			 		data[idx+4] == 's' &&
			 		data[idx+5] == ';')
				{
					// It is '&apos;'. Do nothing.
					data[idx] = '\'';
					if (mRoot)
					{
						aos_assert_r(mRoot->adjustData(idx,  -5), false);
					}
					else
					{
						aos_assert_r(adjustData(idx, -5), false); 
					}
					idx++;
					remain -= 6;
				 	mAttrs[i].valueLen -= 5;
				}
				else if (data[idx+1] == 'l' &&
			 		 	data[idx+2] == 't' &&
			 		 	data[idx+3] == ';')
				{
					// It is '&lt;'. Do nothing
					idx += 4;
					remain -= 4;
				}
				else if (data[idx+1] == 'g' &&
			 		 	data[idx+2] == 't' &&
			 		 	data[idx+3] == ';')
				{
					// It is '&gt;'. Do nothing
					idx += 4;
					remain -= 4;
				}
				else if (remain >= 5 &&
			 		 	data[idx+1] == 'a' &&
			 		 	data[idx+2] == 'm' &&
			 		 	data[idx+3] == 'p' &&
			 		 	data[idx+4] == ';')
				{
					// It is '&amp;'. Do nothing
					idx += 5;
					remain -= 5;
				}
				else if (remain >= 4 && data[idx+1] == '#')
				{
					// Check whether it is '&#dd'
					if (data[idx+2] >= '0' && data[idx+2] <= '9')
					{
						// It is '&#ddd;'.
						int esclen = 0;
						const int max_escapelen = 5;
						int mm = idx+3;
						while (esclen < max_escapelen && data[mm] != ';')
						{
							if (data[mm] < '0' || data[mm] > '9')
							{
								// Not allowed. Escape '&'. 
								processed = false;
								break;
							}

							mm++;
							esclen++;
						}

						if (data[mm] != ';') 
						{
							processed = false;
						}
						else
						{
							remain -= mm + 1 - idx;
							idx = mm + 1;
						}
					}
					else if (data[idx+2] == 'x')
					{
						int esclen = 0;
						const int max_escapelen = 5;
						int mm = idx+3;
						while (esclen < max_escapelen && data[mm] != ';')
						{
							char cc = data[mm];
							if (!((cc >= '0' && cc <= '9') ||
								  (cc >= 'A' && cc <= 'F') ||
								  (cc >= 'a' && cc <= 'f')))
							{
								// Not allowed. Escape '&'. 
								processed = false;
								break;
							}

							mm++;
							esclen++;
						}

						if (data[mm] != ';') 
						{
							processed = false;
						}
						else
						{
							remain -= mm + 1 - idx;
							idx = mm + 1;
						}
					}
					else
					{
						processed = false;
					}
				}
				else
				{
					processed = false;
				}

				if (!processed)
				{
					// It is not allow. Replace '&' with '&amp;'
					if (mRoot)
					{
						aos_assert_r(mRoot->adjustData(idx, 4), false);
					}
					else
					{
						aos_assert_r(adjustData(idx, 4), false); 
					}
					data = mBuff->getData();
					idx++;
					data[idx++] = 'a';
					data[idx++] = 'm';
					data[idx++] = 'p';
					data[idx++] = ';';
				 	mAttrs[i].valueLen += 4;
					remain--;
				}
			}	// end of 'while (remain)'
		}
	}	// end of 'for'
	return true;
}


bool		
AosXmlTag::disableAttr(
		const OmnString &aname, 
		const int level)
{
	// This function disables the named attribute. When an attribute
	// is disabled, looping over all attributes will not count 
	// it. If it sees the attribute, it disables the attribute and returns
	// immediately. Otherwise, it goes a level down, if 'level'
	// is not 0. 
	//
	// If the attribute is found, it returns true. 
	// Otherwise, it returns false.
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->disableAttr(aname, level);
	}

	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int nstart = mAttrs[i].nameStart;
		// int nend = mAttrs[i].nameEnd;
		if (strncmp(aname.data(), (const char *)&buffData[nstart], 
					strlen(aname.data())) == 0)
				//nend-nstart+1) == 0)
		{
			mAttrs[i].disabled = true;
			return true;
		}
	}

	if (level <= 1) return false;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->disableAttr(aname, level-1)) 
		{
			return true;
		}
	}
	return false;
}


bool		
AosXmlTag::disableText(const OmnString &child_tagname)
{
	// This function disables the text. When a text
	// is disabled, looping over all words will not count 
	// it. 
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->disableText(child_tagname);
	}

	int num = mChildTags.entries();
	for (int i=0; i<num; i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->getTagname() == child_tagname)
		{
			mChildTags[i]->disableText();
			return true;
		}
	}

	return false;
}


bool
AosXmlTag::disableText()
{
	aos_assert_r(mRoot, false);
	for (int i=0; i<mNumTexts; i++)
	{
		mText[i].disabled = true;
	}
	return true;
}


AosXmlTagPtr		
AosXmlTag::addNode(AosXmlTagPtr &node)
{
	aos_assert_r(sanityCheck(), NULL);
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->addNode(node);
	}

	// Add the node tag first
    if (node->isRootTag()) node = node->getFirstChild();
	OmnString tagname = node->getTagname();
	aos_assert_r(tagname != "", NULL);
	AosXmlTagPtr new_node = addNode1(tagname);
	aos_assert_r(new_node, 0);

	OmnString text = node->getNodeText();
	if (text != "")
	{
		new_node->setText(text, true);
	}

	OmnString name, value;
	for (int i=0; i<node->mAttrs.entries(); i++)
	{
		aos_assert_r(node->getAttr(i, name, value), 0);
		new_node->setAttr(name, value);
	}

	for (int i=0; i<node->mChildTags.entries(); i++)
	{
		AosXmlTagPtr tag = node->mChildTags[i];
		aos_assert_r(new_node->addNode(tag), NULL);
	}
	aos_assert_r(sanityCheck(), NULL);
	return new_node;
}


AosXmlTagPtr
AosXmlTag::addNode1(const OmnString &node_name,  const OmnString &node_body) 
{
	// It adds a new empty node as a subnode of the current node. 
	// The new node name is 'node_name'. 
	// Note that the current node can be one of the following
	// forms:
	// 	<tag ...>xxx</tag>
	// 	<tag .../>
	// If it is the first form, need to find the end tag "</tag>"
	// and insert contents in front of "</tag>". If it is the
	// second form, need to change it to "<tag ...></tag>".
	aos_assert_r(sanityCheck(), 0);
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->addNode1(node_name, node_body);
	}
	int tagend = mTagEnd;
	char *buffData = mBuff->getData();
	aos_assert_r(buffData[tagend] == '>', 0);

	// Check whether the previous char starts with '/' or not
	int idx = tagend-1;
	bool finished = false;
	bool isSimpleTag = false;
	int startpos = -1;
	int right_bracket_pos = tagend;
	int delta1 = 0;
	while (idx >= mTagStart && !finished)
	{
		switch (buffData[idx])
		{
		case '/':
			 // It is the form: 
			 // 	<tag .../>
			 // Change it to <tag ...>
			 buffData[idx] = '>';
			 right_bracket_pos = idx;
			 buffData[tagend] = ' ';
			 finished = true;
			 startpos = tagend+1;
			 isSimpleTag = true;
			 delta1 = tagend - right_bracket_pos;
			 break;

		case ' ':
		case '\t':
			 idx--;
			 break;

		default:
			 // It is the form: 
			 // 	<tag ...>xxx</tag>
			 finished = true;
			 idx--;
			 while (idx >= mTagStart && buffData[idx] != '<') idx--;
			 aos_assert_r(idx >= mTagStart, 0);
			 startpos = idx;
			 right_bracket_pos = startpos-1;
			 break;
		}
	}
	aos_assert_r(startpos > 0, 0);

	// Chen Ding, 11/30/2010
	OmnString contents;
	if (node_body != "")
	{
	// IMPORTANT: In this case, it will add a non-empty node.
	// In the current implementation, it is able to add a
	// non-empty node, but it did not parse the new node
	// (i.e., parsing the attributes and its body). This
	// means that if we want to get attribute from this
	// class for the newly created node, it will fail.
		contents = node_body;
	}
	else
	{
		contents << "<" << node_name << "/>";
	}

	//OmnString contents = "<";
	//contents << node_name << "/>";
	int nodelen = contents.length();
	if (isSimpleTag) contents << "</" << getTagname() << ">";

	int delta = contents.length()-delta1;
	aos_assert_r(mRoot, 0);
	bool rslt = mRoot->adjustData(startpos, delta);
	aos_assert_r(rslt, 0);
	
	buffData = mBuff->getData();
	startpos = right_bracket_pos+1;
	memcpy(&buffData[startpos], contents.data(), delta+delta1);

	// Now the contents have been inserted into the buffer. Need
	// to create a node and insert it.
	int nameend = startpos + node_name.length();
	if (tooManyChildren())
	{
		OmnAlarm << "Too many children. Maximum allowed: " << eArrayMaxSize << enderr;
		return 0;
	}

	AosXmlTagPtr node = OmnNew AosXmlTag(mRoot, this, mBuff, startpos, 
			startpos+1, nameend AosMemoryCheckerArgs);

	tagend = right_bracket_pos + nodelen;	
	node->setTagEnd(tagend);
	mTagEnd = right_bracket_pos + delta + delta1;
	if (!isSimpleTag)
	{
		mTagEnd += getTagname().length() + 3;
	}
	aos_assert_r(AosXmlTagSanityCheck(this), 0);
	return node;
}


bool
AosXmlTag::removeMetadata()
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->removeMetadata();
	}

	// Below are metadata: 
	// 	AOSTAG_DOCID
	// 	AOSTAG_OBJID
	// 	AOSTAG_CTIME
	// 	AOSTAG_CT_EPOCH
	// 	AOSTAG_MTIME
	// 	AOSTAG_MT_EPOCH
	//	AOSTAG_VERSION
	removeAttr(AOSTAG_SITEID, 1, true);
	removeAttr(AOSTAG_OTYPE, 1, true);
	removeAttr(AOSTAG_DOCID, 1, true);
	removeAttr(AOSTAG_OBJID, 1, true);
	removeAttr(AOSTAG_CTIME, 1, true);
	removeAttr(AOSTAG_CT_EPOCH, 1, true);
	removeAttr(AOSTAG_MTIME, 1, true);
	removeAttr(AOSTAG_MT_EPOCH, 1, true);
	removeAttr(AOSTAG_VERSION, 1, true);
	removeAttr(AOSTAG_HPCONTAINER, 1, true);
	removeAttr(AOSTAG_CREATOR, 1, true);
	removeAttr(AOSTAG_FULLDOC_SIGNATURE, 1, true);
	return true;
}


AosXmlTagPtr
AosXmlTag::clone(AosMemoryCheckDeclBegin)
{
	AosXmlParser parser;
	//ice, 2013/05/31
	//return parser.parse(toString(), "" AosMemoryCheckerFileLine);
	return parser.parse(mBuff->getData()+ mTagStart, 
			mTagEnd-mTagStart+1, "" AosMemoryCheckerFileLine);
}


bool
AosXmlTag::setNodeText(
		const OmnString &path, 
		const OmnString &value, 
		const bool iscdata)
{
	// It sets the node text. The node is identified by 'path', which is 
	// in the form:
	// 		"nodename/nodename/.../nodename"
	// If the subnodes are not there, it will add it.
	
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->setNodeText(path, value, iscdata);
	}
	
	if (path == "") return setText(value, iscdata);

	OmnStrParser1 parser(path, "/");
	OmnString prevName = parser.nextWord();
	aos_assert_r(prevName != "", false);
	OmnString nextName;
	AosXmlTagPtr crtNode(this, false);
	AosXmlTagPtr nextNode;
	while ((nextName = parser.nextWord()) != "")
	{
		nextNode = crtNode->getFirstChild(prevName);
		if (!nextNode)
		{
			// It does not exist. Need to create
			nextNode = crtNode->addNode1(prevName);
			aos_assert_r(nextNode, false);
		}
		crtNode = nextNode;
		prevName = nextName;
	}

	// The above created all the nodes but the last one, which is stored
	// in 'prevName'. Create it if it does not exist
	nextNode = crtNode->getFirstChild(prevName);
	if (!nextNode)
	{
		nextNode = crtNode->addNode1(prevName);
		aos_assert_r(nextNode, false);
	}

	bool rslt = nextNode->setText(value, iscdata);
	aos_assert_r(rslt, false);
	return true;
}


int
AosXmlTag::getTotalNumAttrs()
{
	//OmnScreen<< "++++++++++++++++++++++++++++++++++++" <<endl;
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getTotalNumAttrs();
	}

	int total = 0;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		total += mChildTags[i]->getTotalNumAttrs();
	}

	total += mAttrs.entries();
	return total;
}


bool
AosXmlTag::setDftTags()
{
	// This function sets the system default tags. The default tags
	// are set based on the parent containers. Parent containers
	// can be public or private. Some parent containers are 
	// defined by the system:
	// 	AOSOBJID_DATA		AOSSYSTAG_DATA
	// 	AOSOBJID_ALBUM		AOSSYSTAG_ALBUM
	// 	...
	OmnNotImplementedYet;
	return false;
}


// Chen Ding, 12/05/2010
bool
AosXmlTag::normalizeSystemAttrs()
{
	// This function normalize system defined attributes:
	// 	AOSTAG_OTYPE
	// 	AOSTAG_OBJID
	// 	AOSTAG_STYPE
	// 	AOSTAG_TAG
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->normalizeSystemAttrs();
	}

	for (int i=0; i<mAttrs.entries(); i++)
	{
		char *buffData = mBuff->getData();
		int idx = mAttrs[i].nameStart;
		int nend = mAttrs[i].nameEnd;
		int nlen = nend - idx + 1;

		if (nlen < 5) continue;
		if (buffData[idx] != 'z' ||
			buffData[idx+1] != 'k' ||
			buffData[idx+2] != 'y' ||
			buffData[idx+3] != '_') continue;

		idx += 4;
		//while (idx <= nlen)
		while (idx <= nend)
		{
			switch (buffData[idx])
			{
			case 'o':
				 if (idx + 4 == nend &&
					 buffData[idx+1] == 'b' &&
					 buffData[idx+2] == 'j' &&
					 buffData[idx+3] == 'i' &&
					 buffData[idx+4] == 'd')
				 {
					 // It is AOSTAG_OBJID
					 normalizeAttr(i, eAosMaxObjidLen, sgObjidMap);
					 break;
				 }

				 if (idx + 4 == nend &&
					 buffData[idx+1] == 't' &&
					 buffData[idx+2] == 'y' &&
					 buffData[idx+3] == 'p' &&
					 buffData[idx+4] == 'e')
				 {
					 // It is AOSTAG_OTYPE
					 normalizeAttr(i, eAosMaxOtypeLen, sgOtypeMap);
					 break;
				 }
				 break;

			case 's':
				 if (idx + 4 == nend &&
					 buffData[idx+1] == 't' &&
					 buffData[idx+2] == 'y' &&
					 buffData[idx+3] == 'p' &&
					 buffData[idx+4] == 'e')
				 {
					 // It is AOSTAG_OTYPE
					 normalizeAttr(i, eAosMaxStypeLen, sgOtypeMap);
					 break;
				 }
				 break;

			case 't':
				 if (idx + 2 == nend &&
					 buffData[idx+1] == 'a' &&
					 buffData[idx+2] == 'g')
				 {
					 // It is AOSTAG_TAG
					 normalizeAttr(i, eAosMaxSysAttrLen, sgTagMap);
					 break;
				 }
				 break;

			default:
				 break;
			}
			break;//ice 2010-12-06 runconvert docid 5038 过不去
		}
	}

	return true;
}
		

bool
AosXmlTag::normalizeAttr(
		const int attridx,
		const int max_len, 
		const char *charmap)
{
	char *buffData = mBuff->getData();
	int vstart = mAttrs[attridx].valueStart;
	int vend = vstart + mAttrs[attridx].valueLen - 1;

	bool changed = false;
	char quote = buffData[vstart-1];
	if (quote != '"' && quote == '\'')
	{
		OmnAlarm << "Objid incorrect at position: " << vstart << ". Data: "
			<< buffData << enderr;
		return false;
	}

	int idx = vstart;
	int nn = idx;
	while (idx <= vend)
	{
		u8 c = (u8)buffData[idx];
		if (c > 0 && c < 127)
		{
			if (charmap[c]) 
			{
				// The character is allowed
				buffData[nn++] = buffData[idx];
			}
			else
			{
				// The character is not allowed
				changed = true;
			}
		}
		else
		{
			// It is not an ASCII character. Allowed
			buffData[nn++] = buffData[idx];
		}
		idx++;
	}

	if (changed)
	{
		// Need to change all the trailing contents to spaces
		for (int kk=vend; kk>=nn; kk--)
		{
			buffData[kk] = ' ';
		}
	}

	// Make sure objid is not too long
	int value_len = vend - vstart + 1;
	// if (nn >= eAosMaxSysAttrLen) vend = vstart + nn - 1;
	if (value_len > max_len)
	{
		for (int i=vstart + max_len; i<=vend; i++)
		{
			buffData[i] = ' ';
		}

		if (buffData[vend+1] != quote)
		{
			OmnAlarm << "Objid quote incorrect. Expecting: " << quote 
				<< " but actual is: " << buffData[vend+1] 
				<< ". At position: " << vend+1
				<< ". Data: " << buffData << enderr;
			return false;
		}

		buffData[vstart + max_len] = quote;
		buffData[vend+1] = ' ';
		value_len = max_len;
		mAttrs[attridx].valueLen = max_len;
		vend = vstart + max_len - 1;
		changed = true;
	}

	// Make sure objid is not ended with spaces
	nn = vend;
	while (nn >= vstart)
	{
		unsigned char c = (unsigned char)buffData[nn];
		if (c > 32 && c != (unsigned char)'\t') break;
		
		buffData[nn+1] = ' ';
		buffData[nn] = quote;
		vend = nn - 1;
		changed = true;
		nn--;
	}
	value_len = vend - vstart + 1;

	if (changed)
	{
		// buff[value_len] = 0;
		// int nstart = mAttrs[attridx].nameStart;
		// int nlen = mAttrs[attridx].nameEnd - nstart + 1;
		// OmnString name(&buffData[nstart], nlen);
		// setAttr(name, buff);
		mAttrs[attridx].valueLen = value_len;
	}

	return true;
}


//james 2011/01/25
/*
bool
AosXmlTag::escapeContents()
{
	static const OmnString escpCodes[] = {"&amp;#x0a;", "&amp;#x0d;", "&amp;#39;", "&amp;#92;", "&amp;"};

	if (!mBuff) return true;
	char * data = mBuff->getData();
	//escape the xml Text;
	for (int i=0; i<mNumTexts; i++)
	{
		if (mText[i].isCdata)
			continue;
		int idx = mText[i].start;
		int count = mText[i].len;

		for (int j=0; j<count; j++)		
		{
			aos_assert_r(idx < mTagEnd, false);
			char ch = data[idx];
			switch(ch)
			{
			case '\n':
				//if \n , escape to &#xa;
				escapeText(data, escpCodes[0], idx, i);
				break;

			case '\r':
				//if \r , escape to &#xd;
				escapeText(data, escpCodes[1], idx, i);
				break;
				
			case '\'':
				//if ' , escape to &#39;
				escapeText(data, escpCodes[2], idx, i);
				break;

			case '\\':
				//if \ , escape to &#92;
				escapeText(data, escpCodes[3], idx, i);
				break;

			case '&':
				 if (escapeSymbol(data, idx, j))
				 	escapeText(data, escpCodes[4], idx, i);
				 break;
			
			default:
				idx++;
				break;
			}
		}
	}

	//escape the xml attribute.
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int idx = mAttrs[i].valueStart;
		int count = mAttrs[i].valueLen;

		for (int j=0; j<count; j++)		
		{
			aos_assert_r(idx <= mTagEnd, false);
			char ch = data[idx];
			switch(ch)
			{
			case '\n':
				//if \n , escape to &#xa;
				escapeAttr(data, escpCodes[0], idx, i);
				break;

			case '\r':
				//if \r , escape to &#xd;
				escapeAttr(data, escpCodes[1], idx, i);
				break;
				
			case '\'':
				//if ' , escape to &#39;
				escapeAttr(data, escpCodes[2], idx, i);
				break;
			
			case '\\':
				//if \ , escape to &#92;
				escapeAttr(data, escpCodes[3], idx, i);
				break;
				
			case '&':
				 if (escapeSymbol(data, idx, j))
				 	escapeAttr(data, escpCodes[4], idx, i);
				 break;
			
			default:
				idx++;
				break;
			}
		}

	}
	for (int i=0; i<mChildTags.entries(); i++)
	{
		mChildTags[i]->escapeContents();
	}
	return true;
}
*/


inline bool
AosXmlTag::escapeText(char*& data, const OmnString &escpCode, int &idx, int i)
{
	//replace the current character to the escape code, and change the idx, 
	//point it to the next character.
	int adjustlen = escpCode.length()-1;
	aos_assert_r(adjustData(idx, adjustlen), false);
	data = mBuff->getData();
	memcpy(&data[idx], escpCode.data(), escpCode.length());
	mText[i].len += escpCode.length()-1;
	idx += escpCode.length();
	return true;
}


inline bool
AosXmlTag::escapeAttr(char*& data, const OmnString &escpCode, int &idx, int i)
{
	int adjustlen = escpCode.length()-1;
	aos_assert_r(adjustData(idx, adjustlen), false);
	data = mBuff->getData();
	memcpy(&data[idx], escpCode.data(), escpCode.length());
	mAttrs[i].valueLen += escpCode.length() - 1;
	idx += escpCode.length();
	return true;
}


bool
AosXmlTag::escapeSymbol(char* data, int &idx, int &count)
{
	// 'idx' points to the first character right after '&'.
	OmnString symbol[4] = {"&amp;", "&lt;", "&gt;", "&quot;"};
	for (int i=0; i<4; i++)
	{
		if (memcmp(&data[idx], symbol[i].data(), symbol[i].length()) == 0) 
		{
			idx += symbol[i].length()-1;
			count += symbol[i].length()-1;
			return false;
		}
	}
	return true;
}


bool	
AosXmlTag::setNodeName(const OmnString &newname)
{
	// It changes the current tag's tag name to 'newname'. 
	// Tag name starts at 'mTagnameStart', ends at 'mTagnameEnd'. 
	// If it is the form: 
	// 	<tagname .../>
	// then we will change the tagname only. Otherwise, it should
	// be in the form:
	// 	<tagname ...>...</tagname> 
	// we need to change both the opening and closing tagnames. 
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->setNodeName(newname);
	}
	aos_assert_r(newname != "", false);
	char *buffData = mBuff->getData();
	aos_assert_r(buffData, false);
	int datalen = mBuff->getDataLength();
	aos_assert_r(mTagnameStart > 0 && mTagnameStart <= mTagnameEnd, false);
	aos_assert_r(mTagnameEnd > 0 && mTagnameEnd < datalen, false);

	OmnString tagname(&buffData[mTagnameStart], mTagnameEnd - mTagnameStart + 1);
	int delta = newname.length() - tagname.length();

	// Determine the tag form. 'mTagEnd' should point at ">". 
	aos_assert_r(mTagEnd > 0 && mTagEnd < datalen, false);
	aos_assert_r(buffData[mTagEnd] == '>', false);

	// Scan back for '/'. 
	int idx = mTagEnd;
	bool charfound = false;
	while (idx-- > mTagnameEnd)
	{
		if (buffData[idx] == '/') break;
		if (buffData[idx] != ' ') charfound = true;
//		buffData[idx] = ' ';
	}

	aos_assert_r(buffData[idx] == '/', false);

	// It is in the form:
	// 	<tagname ...>...</ tagname >
	aos_assert_r(mRoot->adjustData(mTagnameStart, delta), false);
	buffData = mBuff->getData();
	strncpy(&buffData[mTagnameStart], newname.data(), newname.length());
	mTagnameEnd += delta;

	if (charfound)
	{
		idx += delta+1;
		aos_assert_r(mRoot->adjustData(idx, delta), false);
		buffData= mBuff->getData();
		strncpy(&buffData[idx], newname.data(), newname.length());	
	}
	aos_assert_r(AosXmlTagSanityCheck(this), false);
	return true;
}


//Ketty, 04/18/2011 start
bool
AosXmlTag::isDocSame1(
			AosXmlTagPtr &rhs,
			OmnString &path,
			map<OmnString, pair<OmnString, OmnString> > &attrs)
{
    if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->isDocSame1(rhs, path, attrs);
	}

	if (!rhs->mRoot)
	{
		rhs = rhs->getFirstChild();
	}

	//1.tagname
	/*OmnString oldtagname = getTagname();
	OmnString newtagname = rhs->getTagname();
	if (strcmp(oldtagname, newtagname) != 0)
	{
		return false;
	}*/
	
	//2.attr
	for (int i=0; i<mAttrs.entries(); i++)
	{
		// attr value
		char *buffData = mBuff->getData();
		int nlen = mAttrs[i].nameEnd - mAttrs[i].nameStart + 1;
		OmnString name((char *)&buffData[mAttrs[i].nameStart], nlen);
		OmnString value((char *)&buffData[mAttrs[i].valueStart], mAttrs[i].valueLen);
		int start = mAttrs[i].nameStart;
		if (nlen > 6 && buffData[start] == 'z' &&
				buffData[start+1] == 'k' &&
				buffData[start+2] == 'y' &&
				attrIgnoredSame(start, nlen))
		{
			continue;
		}

		OmnString pathName;	
		if(path != "") pathName << path;
		pathName << "@" << name;
		attrs[pathName]	= make_pair(value,"");	
	}
	
	for (int i=0; i<rhs->mAttrs.entries(); i++)
	{
		// attr value
		char *buffData = rhs->mBuff->getData();
		int nlen = rhs->mAttrs[i].nameEnd - rhs->mAttrs[i].nameStart + 1;
		OmnString name((char *)&buffData[rhs->mAttrs[i].nameStart], nlen);
		OmnString value((char *)&buffData[rhs->mAttrs[i].valueStart], rhs->mAttrs[i].valueLen);
		int start = rhs->mAttrs[i].nameStart;
		if (nlen > 6 && buffData[start] == 'z' &&
				buffData[start+1] == 'k' &&
				buffData[start+2] == 'y' &&
				rhs->attrIgnoredSame(start, nlen))
		{
			continue;
		}
	
		OmnString pathName;	
		if(path != "") pathName << path;
		pathName << "@" << name;
		map<OmnString, pair<OmnString,OmnString> >::iterator it = attrs.find(pathName);
		if(it != attrs.end())
		{
			OmnString oldValue = it->second.first;
			if(oldValue == value)
			{
				attrs.erase(it);
			}else
			{
				it->second = make_pair(oldValue, value);	
			}
		}else
		{
			OmnString pathName;	
			if(path != "") pathName << path;
			pathName << "@" << name;
			attrs[pathName]	= make_pair("",value);		
		}
	}

	//3.text
	int len1, len2;
	char *oldtext = (char *)getNodeText(len1);
	char *newtext = (char *)rhs->getNodeText(len2);
	if (len1 != len2 || strncmp(oldtext, newtext, len1) != 0)
	{
		OmnString pathName;
		if(path != "") pathName << path;
		pathName << "_#text";
		attrs[pathName] = make_pair(OmnString(oldtext, len1), OmnString(newtext, len2));
	}
	
	//4.subtags
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		AosXmlTagPtr lTag = mChildTags[i];
		OmnString tname = lTag->getTagname();
			
		AosXmlTagPtr rTag = rhs->getFirstChild(tname);
		if (!rTag)
		{
			OmnString pathName;
			if(path != "") pathName << path;
			pathName << tname;
			attrs[pathName] = make_pair(lTag->toString(),"");	
			continue;
		}

		OmnString subPath;
		subPath << path << tname << "/";
		lTag->isDocSame1(rTag, subPath, attrs);		
		
	}
	for (int i=0; i<rhs->mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		AosXmlTagPtr rTag = rhs->mChildTags[i];
		OmnString tname = rTag->getTagname();
			
		AosXmlTagPtr lTag = getFirstChild(tname);
		if (!lTag)
		{
			OmnString pathName;
			if(path != "") pathName << path;
			pathName << tname;
			attrs[pathName] = make_pair("", rTag->toString());	
			continue;
		}
	}

	if(attrs.empty()) return true;

	return false;
}


bool
AosXmlTag::attrIgnoredSame(const int start, const int len)
{
	//AOSTAG_CTIME
	//AOSTAG_CT_EPOCH
	//AOSTAG_CREATOR
	//AOSTAG_MODUSER
	//AOSTAG_MTIME
	//AOSTAG_MT_EPOCH
	//AOSTAG_SUBTYPE
	//AOSTAG_SITEID
	//AOSTAG_VERSION
	char *buffData = mBuff->getData();
	switch (buffData[start+4])
	{
	case 'c':
		  if (len == 9 &&
			  buffData[start+5] == 't' && 
			  buffData[start+6] == 'i' && 
			  buffData[start+7] == 'm' &&
			  buffData[start+8] == 'e') return true;	// AOSTAG_CTIME

		  if (len == 10 &&
			  buffData[start+5] == 't' &&
			  buffData[start+6] == 'm' &&
			  buffData[start+7] == 'e' &&
			  buffData[start+8] == 'p' &&
			  buffData[start+9] == 'o') return true;     // AOSTAG_CT_EPOCH

		 if (len == 9 &&
		     buffData[start+5] == 'r' &&
		     buffData[start+6] == 't' &&
		     buffData[start+7] == 'o' &&
		     buffData[start+8] == 'r') return true;     // AOSTAG_CREATOR
		return false;

	case 'm':
		 if (len == 8 &&
		     buffData[start+5] == 'u' &&
		     buffData[start+6] == 's' &&
		     buffData[start+7] == 'r') return true;     // AOSTAG_MODUSER
		
		 if (len == 9 &&
		     buffData[start+5] == 't' &&
		     buffData[start+6] == 'i' &&
		     buffData[start+7] == 'm' &&
		     buffData[start+8] == 'e') return true;     // AOSTAG_MTIME
		 
		 if (len == 10 &&
		     buffData[start+5] == 't' &&
		     buffData[start+6] == 'm' &&
		     buffData[start+7] == 'e' &&
		     buffData[start+8] == 'p' &&
		     buffData[start+9] == 'o') return true;     // AOSTAG_MT_EPOCH 
		return false;

	case 's':
		 if (len == 9 &&
		     buffData[start+5] == 't' &&
		     buffData[start+6] == 'y' &&
		     buffData[start+7] == 'p' &&
		     buffData[start+8] == 'e') return true;     // AOSTAG_SUBTYPE
			
		 if (len == 10 &&
		     buffData[start+5] == 'i' &&
		     buffData[start+6] == 't' &&
		     buffData[start+7] == 'e' &&
		     buffData[start+8] == 'i' &&
		     buffData[start+9] == 'd') return true;     // AOSTAG_SITEID
			
		 return false;
	case 'v':
		 if (len == 7 &&
			 buffData[start+5] == 'e' &&
			 buffData[start+6] == 'r') return true; //AOSTAG_VERSION
		 return false;

	default: return false;
	}
}


//Tank 2/22/2011
OmnString
AosXmlTag::sortByAttr(
		const OmnString &aname, 
		const int &startIdx, 
		const int &psize, 
		const bool increase)
{
	// <Records total = "xxx">
	// 		<Record aname = "xxx"/>
	// 		<Record aname = "xxx"/>
	// 		<Record aname = "xxx"/>
	// 		....
	// </Records>
	// 	
	int t = 0;
	OmnString rs;
	OmnString rs1;
	AosXmlTag::mAname = aname;
	
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->sortByAttr(aname, startIdx, psize, increase);
	}

	list<AosXmlTagPtr> sortlist;
	for (int i = 0; i < mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], "");
		sortlist.push_back(mChildTags[i]);
	}
	
	if(increase)
		sortlist.sort(AosXmlTag::cmp);
	else
		sortlist.sort(AosXmlTag::cmp1);
	
	list<AosXmlTagPtr>::iterator vo = sortlist.begin();
	for(; vo!=sortlist.end(); vo++)
	{
		if(t>=startIdx && t<startIdx+psize)	
			rs << (*vo)->toString();
		t++;
	}
	
	sortlist.clear();
	return rs; 
}


//Zky2789, Linda, 02/14/2011 start
bool 
AosXmlTag::xpathRemoveText(const OmnString &path)
{
	OmnStrParser1 parser(path, "/", false, false);
	OmnString nodename = parser.nextWord();

	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return false;

	OmnString aname = parser.nextWord();
	if (aname =="_#text")
	{
		return child->removeTexts(); 
	}
	return child->xpathRemoveText(aname, parser);
}

bool
AosXmlTag::xpathRemoveText( 
			const OmnString &nodename, 	
			OmnStrParser1 &parser)
{
	AosXmlTagPtr child = getFirstChild(nodename); 
	if (!child) return false;

	OmnString aname = parser.nextWord();
	if (aname == "_#text")
	{
		return child->removeTexts();
	}

	return child->xpathRemoveAttr(aname, parser);
}

bool
AosXmlTag::xpathRemoveAttr(const OmnString &path)
{
	OmnStrParser1 parser(path, "/", false, false);
	OmnString nodename = parser.nextWord();
	OmnString aname = parser.nextWord();
	if (aname == "")
	{
		if (nodename == "_#text")
		{
			return removeTexts();
		}
		const char *data = nodename.data();
		if (data[0] == '@')
		{
			return removeAttr(&data[1]);
		}

		return removeAttr(nodename);
	}

	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return false;

	return child->xpathRemoveAttr(aname, parser);
}

bool
AosXmlTag::xpathRemoveAttr(
		const OmnString &nodename, 
		OmnStrParser1 &parser)
{
	OmnString aname = parser.nextWord();
	if (aname == "")
	{
		if (nodename == "_#text")
		{
			return removeTexts();
		}

		return removeAttr(nodename);
	}

	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return false;

	return child->xpathRemoveAttr(aname, parser);
}


// Chen Ding, 2011/03/29
bool
AosXmlTag::attrExist(const OmnString &name, const int level)
{
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
		AosXmlTagPtr childtag = mChildTags[0];
		if (!childtag.getPtr())
		{
			OmnAlarm << "Internal error!" << enderr;
			return 0;
		}
	 	return childtag->attrExist(name, level);
	}

	char *buffData = mBuff->getData();
	const int namelen = name.length();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int nlen = end - start + 1;
		if (nlen == namelen && 
			strncmp((char *)&buffData[start], name.data(), namelen) == 0)
		{
			return true;
		}
	}

	// Did not find the attribute. Check whether we need to check 
	// the subnodes
	if (level <= 0) return false;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->attrExist(name, level-1)) return true;
	}

	return false;
}


bool
AosXmlTag::retrieveAttrStr(const OmnString &name, const int level, OmnString &value)
{
	if (!mRoot)
	{
	 	aos_assert_r(mChildTags.entries() == 1, 0);
		AosXmlTagPtr childtag = mChildTags[0];
		if (!childtag.getPtr())
		{
			OmnAlarm << "Internal error!" << enderr;
			return false;
		}
	 	return childtag->retrieveAttrStr(name, level, value);
	}

	char *buffData = mBuff->getData();
	const int namelen = name.length();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		int start = mAttrs[i].nameStart;
		int end = mAttrs[i].nameEnd;
		int nlen = end - start + 1;
		if (nlen == namelen && 
			strncmp((char *)&buffData[start], name.data(), namelen) == 0)
		{
			int len = mAttrs[i].valueLen;
			value = OmnString(&buffData[mAttrs[i].valueStart], len);
			return true;
		}
	}

	// Did not find the attribute. Check whether we need to check 
	// the subnodes
	if (level <= 0) return false;

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->retrieveAttrStr(name, level-1, value)) return true;
	}

	return false;
}

int
AosXmlTag::removeNode1(
		const OmnString &name,
		const OmnString &aname,
		const OmnString &avalue)
{
	// It removes the node whose name is 'name'. If found, the node
	// is removed and true is returned. 
	// remove node Conditions:getAttrStr(aname) == avalue
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->removeNode1(name, aname, avalue);
	}

	int numRemoved = 0;
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->tagnameMatch(name) && mChildTags[i]->getAttrStr(aname) == avalue)
		{
			// Found the node. 
			int start = mChildTags[i]->mTagStart;
			int end = mChildTags[i]->mTagEnd;
			int len = end - start + 1;
			aos_assert_r(len >= 0, false);
			aos_assert_r(mRoot->adjustData(start, -len), false);
			mChildTags.remove(i, true);
			numRemoved++;
		}
	}
	return numRemoved;
}


AosXmlTagPtr	
AosXmlTag::xpathGetChild(const OmnString &xpath)
{
	// This function returns the first child by the xpath 'xpath'
	// Path is separated by '/'. If not found, it returns 0.
	aos_assert_r(xpath != "", 0);
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->xpathGetChild(xpath);
	}

	OmnString paths[eMaxXpathLen];
	AosStrSplit split;
	int num = split.splitStr(xpath.data(), "/", paths, eMaxXpathLen);
	aos_assert_r(num > 0, 0);

	return xpathGetChild(paths, num, 0);
}


AosXmlTagPtr
AosXmlTag::xpathGetChild(
		const OmnString *paths, 
		const int len, 
		const int idx)
{
	aos_assert_r(paths, 0);
	aos_assert_r(idx < len, 0);

	OmnString name = paths[idx];
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], 0);
		if (mChildTags[i]->tagnameMatch(name)) 
		{
			if (idx == len-1)
			{
				return mChildTags[i];
			}
			else
			{
				return mChildTags[i]->xpathGetChild(paths, len, idx+1);
			}
		}
	}

	return 0;
}


bool
AosXmlTag::xpathRemoveNode(const OmnString &xpath)
{

	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->xpathRemoveNode(xpath);
	}

	OmnString paths[eMaxXpathLen];
	AosStrSplit split;
	int num = split.splitStr(xpath.data(), "/", paths, eMaxXpathLen);
	aos_assert_r(num > 0, 0);

	return xpathRemoveNode(paths, num, 0);

}


bool
AosXmlTag::xpathRemoveNode(
		const OmnString *paths, 
		const int len, 
		const int idx)
{
	aos_assert_r(paths, 0);
	aos_assert_r(idx < len, 0);

	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->xpathRemoveNode(paths, len, idx);
	}

	OmnString name = paths[idx];
	if (idx == len-1)
	{
		return removeNode(name, false, false);
	}

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->tagnameMatch(name)) 
		{
			return mChildTags[i]->xpathRemoveNode(paths, len, idx+1);
		}
	}

	return false;
}


bool
AosXmlTag::xpathAddNode(const OmnString &xpath, AosXmlTagPtr &newnode)
{
	aos_assert_r(newnode, false);
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->xpathAddNode(xpath, newnode);
	}

	OmnString paths[eMaxXpathLen];
	AosStrSplit split;
	int num = split.splitStr(xpath.data(), "/", paths, eMaxXpathLen);
	aos_assert_r(num > 0, 0);

	return xpathAddNode(paths, num, 0, newnode);

}


bool
AosXmlTag::xpathAddNode(
		const OmnString *paths, 
		const int len, 
		const int idx,
		AosXmlTagPtr &newnode)
{
	aos_assert_r(paths, 0);
	aos_assert_r(idx < len, 0);

	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->xpathAddNode(paths, len, idx, newnode);
	}

	OmnString name = paths[idx];
	if (idx == len-1)
	{
		return addNode(newnode);
	}

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i]->tagnameMatch(name)) 
		{
			return mChildTags[i]->xpathAddNode(paths, len, idx+1, newnode);
		}
	}
	return false;
}


OmnString
AosXmlTag::getRandomXpath()
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getRandomXpath();
	}

	OmnString xpath;
	int i = rand()%10;
	if (i < 4 && mAttrs.entries())
	{
		//attr  @aname
		AttrInfo attr = mAttrs[rand()%mAttrs.entries()];
		OmnString aname(&(mBuff->getData()[attr.nameStart]),attr.nameEnd-attr.nameStart+1);
		xpath << "@" <<aname;
	}
	else if (i < 8)
	{
		//tag  node/_#node    node/@aname
		if(!mChildTags.entries()){
			if(mAttrs.entries())
			{
				AttrInfo attr = mAttrs[rand()%mAttrs.entries()];
				OmnString aname(&(mBuff->getData()[attr.nameStart]),attr.nameEnd-attr.nameStart+1);
				xpath << "@" << aname;
			}
			return xpath;
		}
		AosXmlTagPtr tag  = mChildTags[rand()%mChildTags.entries()];
		OmnString t= tag->getRandomXpath();
		if (rand()%4 <4 && t != "")
		{
				xpath << tag->getTagname() << "/" << t;//tag->getRandomXpath(); 
		}
		else
		{
			xpath << tag->getTagname();// << "/_#node"; 
		}
	}
	else
	{
		//test  node/_#text
		if (getNodeText() != "")
		{
			xpath << "_#text";
		}
	}
	return xpath;
}


AosXmlTagPtr
AosXmlTag::getChild(const int idx) const
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getChild(idx);
	}

	aos_assert_r(idx >= 0 && idx < mChildTags.entries(), 0);
	return mChildTags[idx];
}


// Ketty 2011/08/15
bool 
AosXmlTag::markAttrs(const AosXmlTagPtr &parent_doc)
{
	// A container may control how its members are indexed. 
	//
	// IMPORTANT:Mate data does not be affected by the attribute.
	//
	// If AOSTAG_NODFT_INDEX(zky_nodft_index)
	// is set to true, the doc will not be indexed except the following:
	//  AOSTAG_GLOBALIDX_ATTRS(zky_globalidx_attrs):
	//  It lists the attributes whose words are globally indexed.
	//
	//  AOSTAG_CTNRIDX_ATTRS(zky_ctnridx_attrs):
	//  It lists the attributes whose words are container indexed.
	//
	//  AOSTAG_BOTHIDX_ATTRS(zky_bothidx_attrs):
	//  It lists the attributes whose words are indexed both globally and container.
	//
	// If AOSTAG_NODFT_INDEX is not set to true, all words are indexed globally
	// except:
	// 	AOSTAG_NOINDEX_ATTRS(zky_noindex_attrs):
	// 	It lists the attributes whose contents are not indexed.
	//
	// 	AOSTAG_CTNRIDX_ATTRS:
	// 	It lists the attributes whose words are container indexed.
	//
	// All the above attributes formats are:
	// 			www,www,...
	// where 'www' is either an attribute name or '#_text'. 
	//
	// As an example, 
	// 		<doc firstname="xxx"
	// 			lastname="xxx"
	// 			title="xxx"
	// 			AOSTAG_CTNRIDX_ATTRS="title,addr/#_text">
	// 			<addr street_num="xxx" city="xxx" state="xxx">xxx</addr>
	// 			<email>xxx</email>
	// 		</doc>
	// In this example, the doc is indexed globally. In addition, the contents 
	// of 'title' and addr/#_text are indexed by the container. 
	//
	// 		<doc firstname="xxx"
	// 			lastname="xxx"
	// 			title="xxx"
	// 			AOSTAG_NODFT_INDEX="true"
	// 			AOSTAG_GLOBALIDX_ATTRS="title"
	// 			AOSTAG_CTNRIDX_ATTRS="firstname,lastname">
	// 			<addr street_num="xxx" city="xxx" state="xxx">xxx</addr>
	// 			<email>xxx</email>
	// 		</doc>
	// In this example, the entire doc is not indexed except the contents of 
	// 'firstname' and 'lastname' are container indexed and 'title' globally indexed. 
	//
	// Below is the algorithm:
	// If AOSTAG_NODFT_INDEX is "true":
	// 	1. Add an entry [word, eGlobalIndex] to the map for each word in the attributes
	// 	   listed by AOSTAG_GLOBALIDX_ATTRS;
	// 	2. Add an entry [word, eContainerIndex] to the map for each word in the
	// 	   attributes listed by AOSTAG_CTNRIDX_ATTRS; 
	// 	3. Add an entry [word, eBothIndex] to the map for each word in the
	// 	   attributes listed by AOSTAG_BOTHIDX_ATTRS; 
	// 	4. Set the default mark to eNoIndex.
	//
	// If AOSTAG_NODFT_INDEX is not "true":
	// 	1. Add an entry [word, eContainerIndex] to the map for each word in the
	// 	   attributes listed by AOSTAG_CTNRIDX_ATTRS; 
	// 	2. Add an entry [word, eNoIndex] to the map for each word in the
	// 	   attributes listed by AOSTAG_NOINDEX_ATTRS; 
	// 	3. Set the default mark to eGlobalIndex.
	//
	// 	Loop over all the attributes/texts of the doc. For each attribute/text,
	// 	check whether it is in the map. If yes, it uses the entry's mark to 
	// 	mark the attribute/text, and then removes the entry from the map. 
	// 	Otherwise, it marks the attribute/text as the default mark.
	//
	// 	Note: a performance consideration: when the map is empty, mark all the 
	// 	remaining to the default marks.
	bool nodef_index = false;
	if (parent_doc) nodef_index = parent_doc->getAttrBool(AOSTAG_NODFT_INDEX, false);
	AosEntryMark::E dft_mark = (nodef_index)?AosEntryMark::eNoIndex:AosEntryMark::eContainerIndex;

	map<OmnString, AosEntryMark::E>	markMap;
	if(nodef_index)
	{
		// default dont't do index, the attr is path attr
		OmnString global_idx_attrs = "";
		if (parent_doc) global_idx_attrs = parent_doc->getAttrStr(AOSTAG_GLOBALIDX_ATTRS, "");
		addToMarkMap(global_idx_attrs, AosEntryMark::eGlobalIndex, markMap);

		OmnString ctnr_idx_attrs = "";
		if (parent_doc) ctnr_idx_attrs = parent_doc->getAttrStr(AOSTAG_CTNRIDX_ATTRS, "");
		addToMarkMap(ctnr_idx_attrs, AosEntryMark::eContainerIndex, markMap);
		
		OmnString both_idx_attrs = "";
		if (parent_doc) both_idx_attrs = parent_doc->getAttrStr(AOSTAG_BOTHIDX_ATTRS, "");
		addToMarkMap(both_idx_attrs, AosEntryMark::eBothIndex, markMap);

		OmnString idx_attrs_word = "";
		if (parent_doc) idx_attrs_word = parent_doc->getAttrStr(AOSTAG_IDX_ATTRS_WORD, "");
		addToMarkMap(idx_attrs_word, AosEntryMark::eAttrWordIndex, markMap);
	}
	else
	{
		OmnString ctnr_idx_attrs = "";
		if (parent_doc) ctnr_idx_attrs = parent_doc->getAttrStr(AOSTAG_CTNRIDX_ATTRS, "");
		addToMarkMap(ctnr_idx_attrs, AosEntryMark::eContainerIndex, markMap);
		
		OmnString both_idx_attrs = "";
		if (parent_doc) both_idx_attrs = parent_doc->getAttrStr(AOSTAG_BOTHIDX_ATTRS, "");
		addToMarkMap(both_idx_attrs, AosEntryMark::eBothIndex, markMap);
		
		OmnString no_idx_attrs = "";
		if (parent_doc) no_idx_attrs = parent_doc->getAttrStr(AOSTAG_NOINDEX_ATTRS, "");
		addToMarkMap(no_idx_attrs, AosEntryMark::eNoIndex, markMap);

		OmnString idx_attrs_word = "";
		if (parent_doc) idx_attrs_word = parent_doc->getAttrStr(AOSTAG_IDX_ATTRS_WORD, "");
		AosEntryMark::E mm = (AosEntryMark::E)(dft_mark | AosEntryMark::eAttrWordIndex);
		// addToMarkMap(idx_attrs_word, EntryMark(dft_mark|eAttrWordIndex), markMap);
		// addToMarkMap(idx_attrs_word, dft_mark|AosEntryMark::eAttrWordIndex, markMap);
		addToMarkMap(idx_attrs_word, mm, markMap);
	}
	
	OmnString path = "";
	return markAttrsSub(path, dft_mark, markMap);	
}


AosEntryMark::E
AosXmlTag::markAttr(const AosXmlTagPtr &index_doc, const OmnString &xpath)
{
	bool nodef_index = false;
	if (index_doc) nodef_index = index_doc->getAttrBool(AOSTAG_NODFT_INDEX, false);
	// 									0b0000 : 0b0010
	int dft_mark = (nodef_index)?AosEntryMark::eNoIndex:AosEntryMark::eContainerIndex;
	// aos_assert_r(index_doc, (AosEntryMark::E)dft_mark);

	map<OmnString, AosEntryMark::E>	markMap;
	if (nodef_index)
	{
		// default dont't do index, the attr is path attr
		OmnString global_idx_attrs = "";
		if (index_doc) global_idx_attrs = index_doc->getAttrStr(AOSTAG_GLOBALIDX_ATTRS, "");
		if(global_idx_attrs.findSubString(xpath, 0) != -1)
		{
			// dft_mark = AosEntryMark::eGlobalIndex;
			dft_mark |= AosEntryMark::eGlobalIndex;
		}

		OmnString ctnr_idx_attrs = "";
		if (index_doc) ctnr_idx_attrs = index_doc->getAttrStr(AOSTAG_CTNRIDX_ATTRS, "");
		if (ctnr_idx_attrs.findSubString(xpath, 0) != -1)
		{
			// dft_mark = AosEntryMark::eContainerIndex;
			dft_mark |= AosEntryMark::eContainerIndex;
		}

		OmnString both_idx_attrs = "";
		if (index_doc) both_idx_attrs = index_doc->getAttrStr(AOSTAG_BOTHIDX_ATTRS, "");
		if (both_idx_attrs.findSubString(xpath, 0) != -1)
		{
			// dft_mark = AosEntryMark::eBothIndex;
			dft_mark |= AosEntryMark::eBothIndex;
		}

		OmnString idx_attrs_word = "";
		if (index_doc) idx_attrs_word = index_doc->getAttrStr(AOSTAG_IDX_ATTRS_WORD, "");
		if (idx_attrs_word.findSubString(xpath, 0) != -1)
			dft_mark |= AosEntryMark::eAttrWordIndex;
	}
	else
	{
		OmnString ctnr_idx_attrs = "";
		if (index_doc) ctnr_idx_attrs = index_doc->getAttrStr(AOSTAG_CTNRIDX_ATTRS, "");
		if(ctnr_idx_attrs.findSubString(xpath, 0) != -1)
		{
			// dft_mark = AosEntryMark::eContainerIndex;
			dft_mark |= AosEntryMark::eContainerIndex;
		}
		
		OmnString both_idx_attrs = "";
		if (index_doc) both_idx_attrs = index_doc->getAttrStr(AOSTAG_BOTHIDX_ATTRS, "");
		if (both_idx_attrs.findSubString(xpath, 0) != -1)
		{
			// dft_mark = AosEntryMark::eBothIndex;
			dft_mark |= AosEntryMark::eBothIndex;
		}

		OmnString no_idx_attrs = "";
		if (index_doc) no_idx_attrs = index_doc->getAttrStr(AOSTAG_NOINDEX_ATTRS, "");
		if (no_idx_attrs.findSubString(xpath, 0) != -1)
			dft_mark = AosEntryMark::eNoIndex;

		OmnString idx_attrs_word = "";
		if (index_doc) idx_attrs_word = index_doc->getAttrStr(AOSTAG_IDX_ATTRS_WORD, "");
		if (idx_attrs_word.findSubString(xpath, 0) != -1)
			dft_mark |= AosEntryMark::eAttrWordIndex;
	}
	return (AosEntryMark::E)dft_mark;
}


// Ketty 2011/08/15
bool 
AosXmlTag::addToMarkMap(
		const OmnString &idx_attrs,
		AosEntryMark::E	mark_type,
		map<OmnString, AosEntryMark::E> &markMap)
{
	if(idx_attrs != "")
	{
		// All the above attributes formats are:
		// 			www,www,...
		OmnStrParser1 parser(idx_attrs, ",", false, false); 
		OmnString attr;
		while( (attr = parser.nextWord()) != "")
		{
			map<OmnString, AosEntryMark::E>::iterator itr = markMap.find(attr);
			if (itr != markMap.end())
			{
				itr->second = (AosEntryMark::E)(itr->second | mark_type);  
			}
			else
			{
				markMap.insert(make_pair(attr, mark_type) );	
			}
		}
	}
	
	return true;
}


// Ketty 2011/08/15
bool
AosXmlTag::markAttrsSub(
		OmnString &path,
		const AosEntryMark::E dft_mark,
		map<OmnString, AosEntryMark::E> &markMap)
{
	// All attributes/texts are named as:
	// 	tagname/tagname/.../attrname, or 
	// 	tagname/tagname/.../#_text
	// 'nodef_index' is true if the default is AosEntryMark::eNoIndex. 
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->markAttrsSub(path, dft_mark, markMap);
	}

	map<OmnString, AosEntryMark::E>::iterator iter;
	// 1. add attr mark
	for(int i=0; i<mAttrs.entries(); i++)
	{
		char *buffData = mBuff->getData();
		int nlen = mAttrs[i].nameEnd - mAttrs[i].nameStart + 1;
		OmnString aname((char *)&buffData[mAttrs[i].nameStart], nlen);

		OmnString attrPathName;                                                                                  
		if(path != "") attrPathName << path;
		attrPathName << aname;
		
		if(!markMap.empty())
		{
			// The map is not empty. Check whether there is the entry:
			// 		[attrPathName, mark]
			iter = markMap.find(attrPathName);
			if(iter!= markMap.end())
			{
				// Found the entry. Mark the attribute.
				mAttrs[i].mark = iter->second;

				//markMap.erase(iter);
			}
			else
			{
				// Not found. Use the default mark.
				mAttrs[i].mark = dft_mark; 
			}
		}
		else
		{
			mAttrs[i].mark = dft_mark; 
		}

		if (isAttrWord(attrPathName))
		{
			mAttrs[i].mark = AosEntryMark::E(dft_mark | AosEntryMark::eAttrWordIndex);
		}
	}

	// 2. add text mark
	AosEntryMark::E textmark = dft_mark;
	if(mNumTexts != 0)
	{
		OmnString textPathName;
		if(path != "") textPathName << path;
		textPathName << "#_text";
		if (!markMap.empty())
		{
			iter = markMap.find(textPathName);
			if(iter!= markMap.end())
			{
				//markMap.erase(iter);
				textmark = iter->second;
			}
		}

		for (int i=0; i<mNumTexts; i++)
		{
			// Linda
			if (mText[i].isBdata)
			{
				mText[i].mark = AosEntryMark::eNoIndex;
			}
			else
			{
				mText[i].mark = textmark;
			}
		}
	}

	// 3. add subTag mark
	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		OmnString tagName = getTagname();
		
		OmnString subPath;
		subPath << path << tagName << "/";

		AosXmlTagPtr subTag = mChildTags[i];
		subTag->markAttrsSub(subPath, dft_mark, markMap);     
	}

	return true;
}

bool
AosXmlTag::setTextBinary(
		const OmnString &nodename,
		const AosBuffPtr &buff)
{
	// It sets the node text. The node is identified by 'nodename', which is
	// in the form:
	//      "nodename"
	// If the subnodes are not there, it will add it.
	//
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->setTextBinary(nodename, buff);
	}

	if (nodename == "") return setTextBinary(buff);

	AosXmlTagPtr nextNode = getFirstChild(nodename);
	if (!nextNode)
	{
		nextNode = addNode1(nodename);
		aos_assert_r(nextNode, false);
	}
	return nextNode->setTextBinary(buff);
}


bool
AosXmlTag::setTextBinary(const AosBuffPtr &buff)
{
	if (isRootTag()) return mChildTags[0]->setTextBinary(buff);
	// The text's foramt must be:
	// <![BDATA[lenght:buffdata]]>
	int start = -1;
	OmnString buff_len; 
	buff_len  << buff->dataLen();
	int len = buff->dataLen() + buff_len.length() +1 +12;
	aos_assert_r(len +1 > buff->dataLen()  , 0);
	if (mNumTexts > 0)
	{
		aos_assert_r(removeTexts(), false);
	}
	
	// It does not have texts. It can be in the following forms:
	// <tagname ...>xxx</tagname>
	// <tagname .../>
	int idx = mTagnameEnd+1;
	int lastslash = -1;
	char *buffData = mBuff->getData();
	while (idx<mTagEnd && buffData[idx] != '>') 
	{
		if (buffData[idx] == '/') lastslash = idx;
		idx++;
	}

	aos_assert_r(buffData[idx] == '>', false);
	aos_assert_r(idx>0, false);

	// Found the ">". Need to 
	bool hasClosingTag = true;
	if (lastslash != -1)
	{
		// Found a slash. Need to check whether there are anything
		// between '/' and '>'
		bool found = false;
		for (int i=lastslash+1; i<idx; i++) 
		{
			if (buffData[i] != ' ' && buffData[i] != '\t')
			{
				found = true;
				break;
			}
		}

		if (!found) hasClosingTag = false;
	}

	start = idx+1;
	OmnString tagname = getTagname();
	int adjustlen = len;
	if (!hasClosingTag)
	{
		buffData[lastslash] = '>';
		// buffData[lastslash+1] = ' ';
		 int namelen = mTagnameEnd - mTagnameStart + 1;
		// tagname.assign((char *)&buffData[mTagnameStart], namelen);
		adjustlen += namelen + 2;
		start--;
	}

	if (mRoot)
	{
		aos_assert_r(mRoot->adjustData(start, adjustlen), false);
	}
	else
	{
		aos_assert_r(adjustData(start, adjustlen), false);
	}
	buffData = mBuff->getData();

	aos_assert_r(start != -1, false);
	aos_assert_r(mNumTexts < eMaxTexts-1, false);
	mText[mNumTexts].start = start;
	mText[mNumTexts].len = len;
	mText[mNumTexts].isCdata = true;
	mText[mNumTexts].isBdata = true;
	mText[mNumTexts].disabled = false;
	mNumTexts++;

	strncpy((char *)&buffData[start], "<![BDATA[", 9); start += 9;
	strncpy((char *)&buffData[start], buff_len.data(), buff_len.length()); start += buff_len.length(); 
	strncpy((char *)&buffData[start], ":", 1); start += 1;
	memcpy((char *)&buffData[start], buff->data(), buff->dataLen()); 
	start += buff->dataLen();
	strncpy((char *)&buffData[start], "]]>", 3);
	start += 3;

	//james check has subnode.
	if (tagname != "" && (mChildTags.entries() == 0))
	{
		strncpy((char *)&buffData[start], "</", 2); start += 2;
		strncpy((char *)&buffData[start], tagname.data(), tagname.length());
		start += tagname.length();
		strncpy((char *)&buffData[start], ">", 1);
	}

	aos_assert_r(AosXmlTagSanityCheck(this), false);
	return true;
}


AosBuffPtr
AosXmlTag::getNodeTextBinaryUnCopy(const OmnString &nodename AosMemoryCheckDecl)
{
	if (nodename == "") return getNodeTextBinaryUnCopy(AosMemoryCheckerFileLineBegin);
	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return 0;
	else return child->getNodeTextBinaryUnCopy(AosMemoryCheckerFileLineBegin);
}


AosBuffPtr
AosXmlTag::getNodeTextBinaryUnCopy(AosMemoryCheckDeclBegin)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getNodeTextBinaryUnCopy(AosMemoryCheckerFileLineBegin);
	}

	if (mNumTexts == 0) return 0;	
	int start = mText[0].start;
	int len = mText[0].len;
	if (!mText[0].isBdata) return 0;
	aos_assert_r(len >= 12, 0);
	start += 9;
	len -= 12;
	int maxLength = 20;
	int pos = start;
	char *buffData = mBuff->getData();
	while(maxLength--)
	{
		if (buffData[pos++] == ':')
			break;
	}
	OmnString strlen(&buffData[start], pos - start -1);
	aos_assert_r(strlen != "", 0);
	int bufflen = atoi(strlen.data());
	aos_assert_r(bufflen >= 0 && bufflen < len, 0);

	// Chen Ding, 2015/05/25
	AosBuffPtr buff  = OmnNew AosBuff(&buffData[pos], bufflen, bufflen, true AosMemoryCheckerFileLine);
	return buff;
}


// Chen Ding, 03/22/2012
int
AosXmlTag::getNodeTextBinaryCopy(char **data AosMemoryCheckDecl)
{
	aos_assert_r(data, 0);
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getNodeTextBinaryCopy(data AosMemoryCheckerFileLine);
	}

	*data = 0;
	if (mNumTexts == 0) return 0;   
	int start = mText[0].start;
	int len = mText[0].len;
	if (!mText[0].isBdata) return 0;
	aos_assert_r(len >= 12, 0);
	start += 9;
	len -= 12;
	int maxLength = 20;
	int pos = start;
	char *buffData = mBuff->getData();
	while(maxLength--)
	{
		if (buffData[pos++] == ':')
			break;
	}
	OmnString strlen(&buffData[start], pos - start -1);
	aos_assert_r(strlen != "", 0);
	int bufflen = atoi(strlen.data());
	aos_assert_r(bufflen >= 0 && bufflen < len, 0);
	*data = OmnNew char[bufflen];
	memcpy(*data, &buffData[pos], bufflen);
	return bufflen;
}


AosBuffPtr
AosXmlTag::getNodeTextBinaryCopy(const OmnString &nodename AosMemoryCheckDecl)
{
	if (nodename  == "") return getNodeTextBinaryCopy(AosMemoryCheckerFileLineBegin);
	AosXmlTagPtr child = getFirstChild(nodename);
	if (!child) return 0;
	else return child->getNodeTextBinaryCopy(AosMemoryCheckerFileLineBegin);
}

AosBuffPtr
AosXmlTag::getNodeTextBinaryCopy(AosMemoryCheckDeclBegin)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getNodeTextBinaryCopy(AosMemoryCheckerFileLineBegin);
	}

	if (mNumTexts == 0) return 0;	
	int start = mText[0].start;
	int len = mText[0].len;
	if (!mText[0].isBdata) return 0;
	aos_assert_r(len >= 12, 0);
	start += 9;
	len -= 12;
	int maxLength = 20;
	int pos = start;
	char *buffData = mBuff->getData();
	while(maxLength--)
	{
		if (buffData[pos++] == ':')
			break;
	}
	OmnString strlen(&buffData[start], pos - start -1);
	aos_assert_r(strlen != "", 0);
	int bufflen = atoi(strlen.data());
	aos_assert_r(bufflen >= 0 && bufflen < len, 0);

	AosBuffPtr buff = OmnNew AosBuff(bufflen AosMemoryCheckerFileLine);
	buff->setDataLen(bufflen);
	memcpy(buff->data(), &buffData[pos], bufflen);
	return buff;
}


bool 
AosXmlTag::removeAllChildren()
{
	// This function removes all the children for this tag. If it is the root 
	// tag, it will move to the child.
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->removeAllChildren();
	}

	while (mChildTags.entries() > 0)
	{
		int start = mChildTags[0]->mTagStart;
		int end = mChildTags[0]->mTagEnd;
		int len = end - start + 1;
		aos_assert_r(len >= 0, false);
		aos_assert_r(mRoot->adjustData(start, -len), false);
		mChildTags.remove(0, true);
	}

	return true;
}

bool
AosXmlTag::removeNode(const AosXmlTagPtr &node)
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->removeNode(node);
	}

	for (int i=0; i<mChildTags.entries(); i++)
	{
		aos_assert_r(mChildTags[i], false);
		if (mChildTags[i] == node)
		{
			int start = mChildTags[i]->mTagStart;
			int end = mChildTags[i]->mTagEnd;
			int len = end - start + 1;
			aos_assert_r(len >= 0, false);
			aos_assert_r(mRoot->adjustData(start, -len), false);
			mChildTags.remove(i, true);
			return true;
		}
	}

	return false;
}


// Chen Ding, 05/03/2012
double
AosXmlTag::getAttrDouble(const OmnString &name, const double &dft)
{
	int len;
	u8 *value = getAttr(name, len);
	if (!value || len <= 0) return dft;
	return atof((char *)value);
}


bool
AosXmlTag::removeNodeTexts()
{
	if (!mRoot)
	{
		aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->removeNodeTexts();
	}
	return removeTexts();
}


// Chen Ding, 12/20/2012
AosXmlTagPtr
AosXmlTag::getNextSibling()
{
	aos_assert_r(mParentTag, 0);

	int entries = mParentTag->mChildTags.entries();
	for (int i=0; i<entries; i++)
	{
		if (mParentTag->mChildTags[i].getPtr() == this)
		{
			if (i >= entries-1) return 0;
			return mParentTag->mChildTags[i+1];
		}
	}

	OmnShouldNeverComeHere;
	return 0;
}


// Chen Ding, 2013/02/15
bool 
AosXmlTag::addBinaryData(
		OmnString &str,
		const OmnString &tagname, 
		const AosBuffPtr &buff)
{
	aos_assert_r(tagname != "", false);
	aos_assert_r(buff, false);
	str << "<" << tagname << "><![BDATA[" << buff->dataLen() << ":";
	str.append(buff->data(), buff->dataLen());
	str << "]]></" << tagname << ">";
	return true;
}

vector<pair<OmnString ,OmnString> >
AosXmlTag::getAttributes() 
{
	vector<pair<OmnString ,OmnString> > v;
	if (!mRoot)
	{
		//aos_assert_r(mChildTags.entries() == 1, 0);
		return mChildTags[0]->getAttributes();
	}

	char *buffData = mBuff->getData();
	for (int i=0; i<mAttrs.entries(); i++)
	{
		OmnString name(buffData+mAttrs[i].nameStart, mAttrs[i].nameEnd-mAttrs[i].nameStart+1);
		OmnString value(buffData+mAttrs[i].valueStart, mAttrs[i].valueLen);
		v.push_back(make_pair(name, value));	
	}
	return v;
}

