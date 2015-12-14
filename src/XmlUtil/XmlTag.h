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
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlUtil_XmlTag_h
#define Aos_XmlUtil_XmlTag_h

#include "alarm_c/alarm.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/Objid.h"
#include "SEUtil/DocTags.h"
#include "Util/DynArray.h"
#include "Util/String.h"
#include "Util/StrParser.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "WordParser/Ptrs.h"
#include "util2/value.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/SeXmlTypes.h"

#include <list>
#include <map>
#include <set>
#include <queue>
using namespace std;

class AosXmlTag : virtual public OmnRCObject, public AosMemoryCheckerObj
{
	OmnDefineRCObject;

	friend class AosXmlTagRand;
	friend class AosXmlDiffTester;

public:
	enum
	{
		eAosWordLoopTagname = -3,
		eAosWordLoopAttr = -2,
		eAosWordLoopText = -1,
		eAosWordLoopTag = 0,
		eMaxTexts = 200,
		eMaxWordLen = 256,
		eMaxParentTags = 3,
		eArrayInitSize = 20,
		eArrayIncSize = 20,
		eArrayMaxSize = 10001,
		eArrayInitSize1 = 23,
		eArrayIncSize1 = 23,
		eArrayMaxSize1 = 10001,
		eDebugFlag = 42642642,
		eDeletedFlag = 22346633,
		eMaxXpathLen = 5
	};

	struct TextInfo
	{
		int	start;
		int	len;
		bool isCdata;
		bool disabled;
		bool isBdata;
		AosEntryMark::E mark;

		TextInfo()
		:
		start(0),
		len(0),
		isCdata(true),
		disabled(false),
		isBdata(false),
		mark(AosEntryMark::eGlobalIndex)
		{
		}

		TextInfo(const int s, const int ln, const bool iscdata)
		:
		start(s),
		len(ln),
		isCdata(iscdata),
		disabled(false),
		mark(AosEntryMark::eGlobalIndex)
		{
		}
	};


	struct AttrInfo
	{
		int	nameStart;
		int nameEnd;
		int valueStart;
		int valueLen;
		bool disabled;
		AosEntryMark::E mark;

		AttrInfo()
		:
		nameStart(0),
		nameEnd(0),
		valueStart(0),
		valueLen(0),
		disabled(false),
		mark(AosEntryMark::eGlobalIndex)
		{
		}
		AttrInfo(const int namestart,
				const int nameend,
				const int valuestart,
				const int valuelen)
		:
		nameStart(namestart),
		nameEnd(nameend),
		valueStart(valuestart),
		valueLen(valuelen),
		disabled(false),
		mark(AosEntryMark::eGlobalIndex)
		{
		}
	};

	struct AddedAttr
	{
		OmnString name;
		OmnString value;
	};

	enum AttrLoopStatus
	{
		eInvalidALS,
		eSelfAttrs,
		eAttrPath,
		eMemberTags
	};

	typedef OmnDynArray<AttrInfo, eArrayInitSize1, eArrayIncSize1, eArrayMaxSize1> AttrArray_t;
	typedef OmnDynArray<AosXmlTagPtr, eArrayInitSize, eArrayIncSize, eArrayMaxSize>	TagArray_t;

protected:
	AosXmlTag	*		mRoot;
	AosXmlTag	*		mParentTag;
	OmnConnBuffPtr		mBuff;
	int					mTagStart;
	int					mTagEnd;
	int					mTagnameStart;
	int					mTagnameEnd;
	int					mNumTexts;
	AttrLoopStatus		mAttrLoopStatus;
	int					mAttrLoopIdx;
	OmnString			mCrtPath;
	vector<TextInfo> 	mText;
	AttrArray_t			mAttrs;
	TagArray_t			mChildTags;

	// The loop variables for next word
	int				mNextWordStatus;		// Chen Ding, 2011/02/15
	int				mNextWordIdx;			// Chen Ding, 2011/02/15
	int				mWordAttrLoop;
	int				mAttrNextTagLoop;
	bool			mAttrnameTried;
	int				mAttrIdx;

	int				mTagLoop;
	int				mDocNameIdx;
	int				mDocNameStart;
	int				mDocNameEnd;

	int				mDocTitleIdx;
	int				mDocTitleStart;
	int				mDocTitleEnd;

	OmnString		mNameValueSep;
	int				mCrtChild;
	int				mCrtAttrIdx;

	int				mDebugFlag;
	int 			mAttrWordLoop;
	int 			mAttrWordIdx;
	bool 			mReadOnly;
	bool			mDeletedFlag;		// Chen Ding, 2011/01/28
	AosDocSource::E	mDocSource;			// Chen Ding, 09/01/2011
	AosXmlTagPtr	mPrev;
	AosXmlTagPtr	mNext;
	static OmnString mAname;
	static set<OmnString> smMetaAttrs;
	u32				mRootFlag;			// Chen Ding, 11/13/2011

public:
	AosXmlTag(AosMemoryCheckDeclBegin);
	AosXmlTag(
		const AosXmlTagPtr &root,
		const AosXmlTagPtr &parent,
		const OmnConnBuffPtr &buff,
		const int tagStartIdx,
		const int nameStart,
		const int nameEnd
		AosMemoryCheckDecl);
	~AosXmlTag();

	void 		setReadOnly(const bool flag){mReadOnly = flag;}
	void		resetGetWords();
	bool	    nextWord(
					const AosWordParserPtr &wordparser,
					char *wordbuff,
					const int bufflen,
					int &wordlen,
					AosEntryMark::E &mark);

	void		resetTagNameLoop();
	OmnString	nextTagName();

	void		resetAttrNameValueLoop()
				{
					mAttrLoopStatus = eSelfAttrs;
					mAttrLoopIdx = 0;
				}
	bool		nodenextAttr(
					const OmnString &path,
					OmnString &name,
					OmnString &value,
					bool &finished
					);
	bool		nextAttr(
					const OmnString &path,
					OmnString &name,
					OmnString &value,
					bool &finished,
					const bool withpath,
					AosEntryMark::E &mark,
					bool isIgnore=false);
	bool		nextAttr(
					const OmnString &path,
					OmnString &name,
					OmnString &value,
					bool &finished,
					const bool withpath,
					bool isIgnore=false)
	{
		AosEntryMark::E mark;
		return nextAttr(path, name, value, finished, withpath, mark, isIgnore);
	}

	AosXmlTag *		getRoot() const {return mRoot;}
	OmnString		getTagname()
					{
						char *data = mBuff->getData();
						return OmnString(&data[mTagnameStart],
							mTagnameEnd - mTagnameStart + 1);
					}
	u8 *			getAttr(const char *name, const int namelen, int &len);
	u8 *			getAttr(const OmnString &name, int &len)
					{
						return getAttr(name.data(), name.length(), len);
					}
	u8 *			getNextAttr(const OmnString &name, int &len);
	OmnString		getAttrStr(const OmnString &name,
						const bool recursive,
						bool &exist,
						AosXmlTagPtr &node,
						const OmnString &dft = "");

	OmnSimpleStr    getAttrStrSimp(const char *name, const char *dft = "");
    OmnSimpleStr    getAttrStrSimp(const char *name, const OmnSimpleStr &dft);

	OmnString		getAttrStr(const OmnString &name, const OmnString &dft = "")
					{
						bool exist;
						return getAttrStr1(name, exist, dft);
					}
	bool			replaceAttrName(
						const OmnString &existName,
						const OmnString &newName,
						const bool recursive);
	bool			replaceAttrValue(
						const OmnString &name,
						const bool recursive,
						const OmnString &oldvalue,
						const OmnString &newvalue);
	bool			replaceAttrValueSubstr(
						const OmnString &name,
						const bool recursive,
						const OmnString &oldvalue,
						const OmnString &newvalue);
	bool			getAttrBool(const OmnString &name)
					{
						return (getAttrStr(name) == "true");
					}
	bool 			getAttrBool(const OmnString &name, bool dft)
					{
						if (getAttrStr(name) == "") return dft;
						return (getAttrStr(name) == "true");
					}
	OmnString		getAttrStr1(const OmnString &name, bool &exist, const OmnString &dft = "");
	OmnString		getNextAttrStr(const OmnString &name, bool &found);
	char			getAttrChar(const OmnString &name, const char dft);
	int				getAttrInt(const OmnString &name, const int dft);
	int64_t			getAttrInt64(const OmnString &name, const int64_t dft);
	int64_t         getAttrInt64(const OmnString &name, const int64_t dft, bool &exist);
	u32				getAttrU32(const OmnString &name, const u64 &dft);
	i64				getAttrI64(const OmnString &name, const i64 &dft)
					{
						bool exist;
						return getAttrI64(name, dft, exist);
					}
	i64				getAttrI64(const OmnString &name, const i64 &dft, bool &exist);
	u64				getAttrU64(const OmnString &name, const u64 &dft)
					{
						bool exist;
						return getAttrU64(name, dft, exist);
					}
	u64				getAttrU64(const OmnString &name, const u64 &dft, bool &exist);
	bool			getAttr(const int idx, OmnString &name, OmnString &value);
	OmnString		getAttrWithEscape(const OmnString &name, const char c);
	u8 *			getData()
					{
						char *data = mBuff->getData();
						return (u8 *)&data[mTagStart];
					}
	OmnString		toString() const
					{
						if (mRoot)
						{
							char *data = mBuff->getData();
							return OmnString((char *)&data[mTagStart], getDataLength());
						}
						else
						{
							aos_assert_r(mChildTags.entries() == 1, 0);
							return mChildTags[0]->toString();
						}
					}
	const char *	getData(int &len) const
					{
						const char *data = mBuff->getData();
						len = getDataLength();
						return data;
					}
	AosXmlTagPtr	getFirstChild1()
	{
		return getFirstChild(true);
	}

	AosXmlTagPtr	getFirstChild(const bool skiproot = false);
	AosXmlTagPtr	getFirstChild(const OmnString &name, const bool recursive = false);
	AosXmlTagPtr	getFirstChild(const char *name)
	{
		OmnString newname = name;
		return getFirstChild(newname);
	}

	AosXmlTagPtr	getNextChild();
	AosXmlTagPtr	getNextChild(const OmnString &name);

	AosXmlTagPtr getLastChild () const
	{
		int idx = getNumChilds() - 1;
		return getChild(idx);
	}

	int				getNumAttrs() const {return mAttrs.entries();}
	int				getNumChilds() const {return mChildTags.entries();}
	int				getRootNumSubtags() const
	{
		aos_assert_r(!mRoot, -1);
		return mChildTags.entries();
	}
	int				getNumSubtags() const
	{
		if (!mRoot)
		{
			aos_assert_r(mChildTags.entries() == 1, 0);
			return mChildTags[0]->getNumSubtags();
		}
		return mChildTags.entries();
	}
	AosXmlTag*		getParentTag();
	int				getTagEnd() const {return mTagEnd;}
	int				getSize() const {return mTagEnd - mTagStart + 1;}
	u8 * 			getNodeText(int &len);
	OmnString		getNodeText();
	OmnString		getNodeText(const OmnString &nodename);
	AosXmlTagPtr	getChild(const int idx) const;
	AosXmlTagPtr	getChildByAttr(
						const OmnString &aname,
						const OmnString &avalue);
	OmnString		getChildTextByAttr(
						const OmnString &aname,
						const OmnString &avalue);
	u32				getChildU32ByAttr(
						const OmnString &aname,
						const OmnString &avalue,
						const u32 dft);
	u64				getChildU64ByAttr(
						const OmnString &aname,
						const OmnString &avalue,
						const u64 &dft);
	u8 *			getChildTextByAttr(
						const OmnString &aname,
						const OmnString &avalue,
						int &len);
	int				getDataLength() const
					{
						if (mRoot) return mTagEnd - mTagStart + 1;
						aos_assert_r(mChildTags.entries() == 1, 0);
						return mChildTags[0]->getDataLength();
					}
	int			getDocsize() const {return getDataLength();}

	bool		lengthChanged(const int delta);
	void		resetAttrLoop();
	bool		nextAttrWord(
					const AosWordParserPtr &wordparser,
					char *wordbuff,
					const int bufflen,
					int &wordlen,
					bool &isName,
					AosEntryMark::E &mark);
	OmnString	xpathQuery(const OmnString &path)
	{
		bool exist;
		return xpathQuery(path, exist, "");
	}
	OmnString	xpathQuery(const OmnString &path, const OmnString &dft)
	{
		bool exist;
		return xpathQuery(path, exist, dft);
	}
	OmnString	xpathQuery(const OmnString &path, bool &exist, const OmnString &dft);
	void		resetDocNameLoop() {mDocNameIdx = mDocNameStart;}
	void		resetDocTitleLoop() {mDocTitleIdx = mDocTitleStart;}

	void		setParent(const AosXmlTagPtr &parent);

	bool setAttr(const OmnString &name, const char * vv)
	{
		OmnString value;
		value << vv;
		return setAttr(name, value.data(), value.length());
	}
	bool setAttr(const OmnString &name, const bool vv)
	{
		OmnString value;
		value << vv;
		return setAttr(name, value.data(), value.length());
	}
	bool setAttr(const OmnString &name, const int64_t &vv)
	{
		OmnString value;
		value << vv;
		return setAttr(name, value.data(), value.length());
	}
	bool setAttr(const OmnString &name, const u64 &vv)
	{
		OmnString value;
		value << vv;
		return setAttr(name, value.data(), value.length());
	}
	bool setAttr(const OmnString &name, const u32 &vv)
	{
		OmnString value;
		value << vv;
		return setAttr(name, value.data(), value.length());
	}
	bool setAttr(const OmnString &name,  const int vv)
	{
		OmnString value;
		value << vv;
		return setAttr(name, value.data(), value.length());
	}

	bool		setAttr(const OmnString &name, const OmnString &value)
				{
					return setAttr(name, value.data(), value.length());
				}

	template <class T>
	bool setAttrSimp(const char *name, const T &t)
	{
		OmnString value;
		value << t;
		return setAttr(name, value.data(), value.length());
	}

	bool		setAttr(const OmnString &name,
					const char *value,
					const int vlen);
	int			removeSubAttr(
					const OmnString &name,
					const int recusrive,
					const bool onlyone);
	bool		removeAttr(const OmnString &name)
				{
					return (removeAttr(name, 1, true) == 1);
				}
	int			removeAttr(
					const OmnString &name,
					const int recusrive,
					const bool onlyone);
	int			removeAttr1(
					const OmnString &name,
					const int recusrive,
					const bool onlyone,
					const bool retrieveValue,
					OmnString &value);
	OmnString	removeAttr(const int idx);
	int			removeDupeAttr();
	bool		changeAttrName(const OmnString &aname,
					const OmnString &newname,
					bool &found);
	bool		addAttr(const u8 *data,
					const int nameStart,
					const int nameLen,
					const int valueStart,
					const int valueLen,
					int &delta);
	bool		addCdata(const int start, const int len);
	bool		addText(const int start, const int len);
	AosXmlTagPtr	addNode(AosXmlTagPtr &node);
	AosXmlTagPtr addNode1(const OmnString &name, const OmnString &node_body = "");
	int			removeNode(
					const OmnString &name,
					const bool recursive,
					const bool removeall);
	bool		removeNode();
	bool		removeNode(const int idx);
	int			removeNode1(
					const OmnString &name,
					const OmnString &aname,
					const OmnString &avalue);
	bool		removeTexts();
	bool 		hasChild(const OmnString &name, const bool recursive);
	bool 		hasAttribute(
					const OmnString &name,
					const bool recursive);
	bool 		hasAttribute(
					const OmnString &name,
					const OmnString &value,
					const bool recursive = false);
	bool 		setNodeText(const OmnString &text, const bool isCdata)
	{
		return setNodeText("", text, isCdata);
	}
	bool 		setNodeText(const OmnString &path, const OmnString &text, const bool isCdata);
	bool 		setText(const OmnString &text, const bool isCdata);
	bool 		tagnameMatch(const OmnString &name);
	bool		verifyTagName(const int start, const int end);
	bool		addChildTag(const AosXmlTagPtr &child);				// Chen Ding, 2013/08/29
	bool		setTagEnd(const int idx);
	bool 		adjustData(const int pos, const int delta);
	bool 		adjustPointers(const int pos, const int delta);
	bool		sanityCheck();
	bool		isDocSame(const AosXmlTagPtr &rhs);
	bool 		attrIgnored(const int start, const int len);

	bool		isDocSame1(
					AosXmlTagPtr &rhs,
					OmnString &path,
					map<OmnString, pair<OmnString, OmnString> > &attrs);
	bool		attrIgnoredSame(const int start, const int len);

	bool        xpathSetAttr(
					const OmnString &path,
					const OmnString &value,
					const bool create);
	bool		xpathSetAttr(const OmnString &path, const OmnString &value);
	bool 		xpathSetAttr(
					const OmnString &nodename,
					OmnStrParser1 &parser,
					const OmnString &value,
					const bool create);
	bool		consistentCheck();
	bool 		unescapeCdataContents();
	bool		checkAttrValues();
	bool		disableAttr(const OmnString &aname, const int level = 1);
	bool		disableText(const OmnString &child_tagname);
	bool        disableText();
	bool		modifyDoc();
	bool		removeMetadata();
	int			getTotalNumAttrs();
	bool 		xpathRemoveText(const OmnString &path);
	bool		xpathRemoveText(
					const OmnString &nodename,
					OmnStrParser1 &parser);
	bool		xpathRemoveAttr(const OmnString &path);
	bool		xpathRemoveAttr(
					const OmnString &nodename,
					OmnStrParser1 &parser);

	//Ketty 2011/04/23 for attr access
	AosXmlTagPtr	xpathGetFirstChild(const OmnString &path);

	OmnString getContainer1() {return getAttrStr(AOSTAG_PARENTC);}
	OmnString getPrimaryContainer()
	{
		OmnString pctnrs = getAttrStr(AOSTAG_PARENTC);
		if (pctnrs.length() <= 0) return "";
		OmnStrParser1 parser(pctnrs, AOS_CTNR_SEP);
		return parser.nextWord();
	}

	void
	setContainer1(const OmnString &parent_ctnr)
	{
		setAttr(AOSTAG_PARENTC, parent_ctnr);
	}

	bool
	isContainer()
	{
		return (getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_CONTAINER);
	}

	AosXmlTagPtr	clone(AosMemoryCheckDeclBegin);
	bool		setDftTags();

	bool
	isSmartDoc()
	{
		return (getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_SMARTDOC);
	}

	// Chen Ding, 12/05/2010
	bool normalize()
	{
		normalizeSystemAttrs();
		return true;
	}
	bool normalizeSystemAttrs();
	inline bool escapeText(char*& data, const OmnString &escpCode, int &idx, int i);
	inline bool escapeAttr(char*& data, const OmnString &escpCode, int &idx, int i);
	bool escapeSymbol(char* data, int &idx, int &count);

protected:
	OmnString 	xpathQuery(const OmnString &cname,
					OmnStrParser1 &parser,
					bool &exist,
					const OmnString &dft);
	bool 		extendData(const int pos, const int delta);

private:
	// Chen Ding, 12/05/2010
	bool normalizeAttr(
			const int attridx,
			const int max_len,
			const char *charmap);

	//Tank 2/23/2011
	static bool cmp(const AosXmlTagPtr& a,const AosXmlTagPtr& b)
				{
					OmnString s_a = a->getAttrStr(mAname);
					OmnString s_b = b->getAttrStr(mAname);
					if(s_a == "") s_a = "0";
					if(s_b == "") s_b = "0";
					u64 vv = atoll(s_a.data());
					OmnString vvs;
					vvs << vv;
					u64 vv1 = atoll(s_b.data());
					OmnString vv1s;
					vv1s << vv1;
					if (s_a == vvs && s_b == vv1s)
					{
						return vv < vv1;
					}

					return s_a  < s_b;
				}

	static bool cmp1(const AosXmlTagPtr& a,const AosXmlTagPtr& b)
				{
					OmnString s_a = a->getAttrStr(mAname);
					OmnString s_b = b->getAttrStr(mAname);
					if(s_a == "") s_a = "0";
					if(s_b == "") s_b = "0";
					u64 vv = atoll(s_a.data());
					OmnString vvs;
						vvs << vv;
					u64 vv1 = atoll(s_b.data());
					OmnString vv1s;
						vv1s << vv1;
					if (s_a == vvs && s_b == vv1s)
					{
						return vv > vv1;
					}

					return s_a  > s_b;
				}

	//Ketty 2011/08/15 for mark
	bool	addToMarkMap(const OmnString &idx_attrs,
					AosEntryMark::E mark_type,
					map<OmnString, AosEntryMark::E> &markMap);
	bool	markAttrsSub(OmnString &path,
					const AosEntryMark::E dft_mark,
					map<OmnString ,AosEntryMark::E> &markMap);

public:
	// Chen Ding, 2011/01/28
	bool	isDeleted() const
			{
				// Chen Ding, 2013/05/18
				return mDebugFlag != eDebugFlag;
				// if (!mRoot) return mDeletedFlag;
				// return mRoot->mDeletedFlag;
			}
	bool	setNodeName(const OmnString &newname);
	OmnString sortByAttr(
					const OmnString &aname,
					const int &startIdx,
					const int &psize,
					const bool increase);

	// Chen Ding, 2011/03/29
	bool	attrExist(const OmnString &name, const int level);
	bool	retrieveAttrStr(const OmnString &name, const int level, OmnString &);

	// Chen Ding, 2011/04/25
	AosXmlTagPtr	xpathGetChild(const OmnString &name);
	AosXmlTagPtr	xpathGetChild(const OmnString *paths, const int len, const int idx);
	bool			xpathRemoveNode(const OmnString &name);
	bool			xpathRemoveNode(const OmnString *paths, const int len, const int idx);
	bool			xpathAddNode(const OmnString &xpath, AosXmlTagPtr &newnode);
	bool			xpathAddNode(
						const OmnString *paths,
						const int len,
						const int idx,
						AosXmlTagPtr &newnode);

	//felicia 2011/05/19
	OmnString       xpathGetAttr(const OmnString &path, const OmnString &dft="");
	OmnString 		getRandomXpath();

	// Chen Ding, 07/19/2011
	// bool    signAttr(const OmnString &aname, const OmnString &sign_name);
	// bool verifySignedAttr(const OmnString &aname, const OmnString &sign_name);
	// bool verifySignedAttr(OmnString &fields,const OmnString &aname, const OmnString &sign_name);

	bool			markAttrs(const AosXmlTagPtr &index_doc);
	AosEntryMark::E markAttr(const AosXmlTagPtr &index_doc, const OmnString &xpath);

	inline bool isAttrIgnored(AttrArray_t &attrs, const int idx)
	{
		// Chen Ding, 08/28/2011
		// Attributes are ignored if and only if:
		// 	1. It is disabled, or
		// 	2. It is marked as 'eNoIndex', or
		//  3. Its name ends with "__n"
		if (attrs[idx].disabled || attrs[idx].mark == AosEntryMark::eNoIndex) return true;
		int nameEnd = attrs[idx].nameEnd;
		if (nameEnd - attrs[idx].nameStart < 4) return false;
		char *data = mBuff->getData();
		return (data[nameEnd-2] == eAosAttrTypeChar &&
				data[nameEnd-1] == eAosAttrTypeChar &&
				data[nameEnd] == eAosAttrType_Ignore);
	}

	static bool isAttrIgnored(const AosEntryMark::E mark, const OmnString &aname)
	{
		if (mark  == AosEntryMark::eNoIndex) return true;
		return (aname.data()[aname.length()-1-2] == eAosAttrTypeChar &&
				aname.data()[aname.length()-1-1] == eAosAttrTypeChar &&
				aname.data()[aname.length()-1] == eAosAttrType_Ignore);
	}

	static void addMetaAttr(const OmnString &aname)
	{
		smMetaAttrs.insert(aname);
	}

	static bool isMetaAttr(const OmnString &aname)
	{
		return smMetaAttrs.count(aname);
	}

	inline bool isTextIgnored(vector<TextInfo> &texts, const int idx)
	{
		// Chen Ding, 08/28/2011
		// Texts are ignored if and only if:
		// 	1. It is disabled, or
		// 	2. It is marked as eNoIndex, or
		// 	3. Its tag name ends with "__n"
		if (texts[idx].disabled || texts[idx].mark == AosEntryMark::eNoIndex) return true;
		if (mTagnameEnd - mTagnameStart < 4) return false;
		char *data = mBuff->getData();
		return (data[mTagnameEnd-2] == eAosAttrTypeChar &&
				data[mTagnameEnd-1] == eAosAttrTypeChar &&
				data[mTagnameEnd] == eAosAttrType_Ignore);
	}
	inline bool isChildNodeIgnored()
	{
		// Ken Lee, 2013/04/19
		// Texts are ignored if and only if:
		// 	3. Its tag name ends with "__n"
		if (mTagnameEnd - mTagnameStart < 4) return false;
		char *data = mBuff->getData();
		return (data[mTagnameEnd-2] == eAosAttrTypeChar &&
				data[mTagnameEnd-1] == eAosAttrTypeChar &&
				data[mTagnameEnd] == eAosAttrType_Ignore);
	}
	inline bool isAttrWord(const OmnString &aname)
	{
		return aname == AOSTAG_TAG;
	}

	// Chen Ding, 08/31/2011
	bool isRootTag() const {return (!mRoot && mRootFlag == 0xe639ab4f);}

	// Chen Ding, 09/01/2011
	bool isFromClient() const {return mDocSource == AosDocSource::eFromClient;}
	void setDocSource(const AosDocSource::E s) {mDocSource = s;}

	AosXmlTagPtr getPrev() const {return mPrev;}
	AosXmlTagPtr getNext() const {return mNext;}
	void setPrev(const AosXmlTagPtr &p) {mPrev = p;}
	void setNext(const AosXmlTagPtr &n) {mNext = n;}

	inline void removeFromList()
	{
		// It assumes this tag is in a linked list. It removes itself
		// from the linked list.
		if (mNext) mNext->mPrev = mPrev;
		if (mPrev) mPrev->mNext = mNext;
	}

	inline bool insertToList(const AosXmlTagPtr &node)
	{
		// It inserts itself to the front of 'node'
		aos_assert_r(node, false);
		mPrev = node->mPrev;
		if (mPrev) mPrev->mNext = this;
		mNext = node;
		node->mPrev = this;
		return true;
	}

	inline void resetLinkNode()
	{
		mPrev = 0;
		mNext = 0;
	}
	void setRootFlag()
	{
		mRootFlag = 0xe639ab4f;
	}

	bool	addBdata(const int start, const int len);

	bool	setTextBinary(const AosBuffPtr &buff);
	bool	setTextBinary(const OmnString &nodename, const AosBuffPtr &buff);

	AosBuffPtr	getNodeTextBinaryUnCopy(const OmnString &nodename AosMemoryCheckDecl);
	AosBuffPtr	getNodeTextBinaryUnCopy(AosMemoryCheckDeclBegin);
	int getNodeTextBinaryCopy(char **data AosMemoryCheckDecl);
	AosBuffPtr	getNodeTextBinaryCopy(const OmnString &nodename AosMemoryCheckDecl);
	AosBuffPtr	getNodeTextBinaryCopy(AosMemoryCheckDeclBegin);

	// Chen Ding, 01/17/2012
	bool removeAllChildren();
	bool removeNode(const AosXmlTagPtr &node);

	// Chen Ding, 05/03/2012
	double getAttrDouble(const OmnString &name, const double &dft);
	bool removeNodeTexts();

	// Chen Ding, 12/20/2012
	AosXmlTagPtr getNextSibling();

	// Chen Ding, 2013/02/15
	static bool addBinaryData(
					OmnString &str,
					const OmnString &tagname,
					const AosBuffPtr &buff);

	// Chen Ding, 2013/05/18
	int getDebugFlag() const {return mDebugFlag;}

	// Chen Ding, 2013/08/29
	bool tooManyChildren()
	{
		return (mChildTags.entries() >= eArrayMaxSize);
	}
	vector<pair<OmnString ,OmnString> > getAttributes();
};

#endif
