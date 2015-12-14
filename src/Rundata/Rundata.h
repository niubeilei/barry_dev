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
// 01/19/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Rundata_Rundata_dd_h
#define Aos_Rundata_Rundata_dd_h


#include "ErrorMgr/ErrmsgId.h"
#include "ErrorMgr/ErrorMgr.h"
#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "LogUtil/Ptrs.h"
#include "Message/Ptrs.h"
#include "Proggie/ReqDistr/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Rundata/ArgNames.h"
#include "Rundata/Ptrs.h"
#include "Rundata/RundataParm.h"
#include "Rundata/ReturnCode.h"
#include "Rundata/RdataErrEntry.h"
#include "Rundata/RundataCallback.h"
#include "SEServer/Ptrs.h"
#include "SEUtil/AsyncRespCaller.h"
#include "SEInterfaces/SessionObj.h"
#include "SEInterfaces/DLLObj.h"
#include "SEBase/SecReq.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Timer/Ptrs.h"
#include "Timer/TimerObj.h"
#include "Util/ValueRslt.h"
#include "Util/TimeTracker.h"
#include "Util/FilePosEncoder.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "XmlInterface/Ptrs.h"
#include "XmlInterface/WebRequest.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/UserAcctObj.h"
#include <vector>
#include <map>
#include <list>
using namespace std;


// Chen Ding, 2014/08/16
#define AosLogUserError(rdata, errmsg_id) \
	(rdata)?\
	(rdata)->pushError((errmsg_id), true, __FILE__, __LINE__):AosRundata::smRdataError

#define AosLogError(rdata, flag, errmsg_id) \
(rdata)?\
	(rdata)->pushError((errmsg_id), flag, __FILE__, __LINE__):AosRundata::smRdataError

#define AosSetEntityError(rdata, errmsg_id, entity_name, entity_info) (rdata)->pushError((errmsg_id), (entity_name), (entity_info), true, __FILE__, __LINE__)
#define AosSetErrorUser(rdata, errmsg_id) (rdata)->pushError((errmsg_id), "", "", true, __FILE__, __LINE__)
#define AosSetErrorUser3(rdata, entity_id, errmsg_id) (rdata)->pushError((errmsg_id), (entity_id), (errmsg_id), true, __FILE__, __LINE__)
//#define AosSetError(rdata, errmsg_id) (rdata)->pushError((errmsg_id), "", "", false, __FILE__, __LINE__)
#define AosSetError(rdata, term_id) AosRundata::setError((rdata), (term_id), __FILE__, __LINE__)
#define AosSetErrorU(rdata, errmsg_id) if (rdata) (rdata)->pushError((errmsg_id), "", "", false, __FILE__, __LINE__)

#ifndef AOS_APPEND_TIME
#define AOS_APPEND_TIME(rdata, name) rdata->appendTime(__FILE__, __LINE__, name)
#endif

#ifndef AOSMONITORLOG_ENTER
#define AOSMONITORLOG_ENTER(rdata) if (rdata) rdata->enterFunction(__FILE__, __LINE__)
#endif

#ifndef AOSLOG_ENTER
#define AOSLOG_ENTER(rdata) if ((rdata) && !rdata->enterFunction(__FILE__, __LINE__)) return
#endif

#ifndef AOSLOG_ENTER_R 
#define AOSLOG_ENTER_R(d, rslt) if ((d) && !(d)->enterFunction(__FILE__, __LINE__)) return (rslt)
#endif

#ifndef AOSMONITORLOG_LINE
#define AOSMONITORLOG_LINE(rdata) if (rdata) (rdata)->getLog() << __LINE__ << ":"
#endif

#ifndef AOSLOG_LINE
#define AOSLOG_LINE(rdata) if (rdata) (rdata)->getLog() << __LINE__ << ":"
#endif

#ifndef AOSLOG_LEAVE
#define AOSLOG_LEAVE(rundata) if (rundata) rundata->leaveFunction(__FILE__, __LINE__)
#endif

#ifndef AOSMONITORLOG_FINISH
#define AOSMONITORLOG_FINISH(rundata) if (rundata) rundata->getLog() << __LINE__ << ":" << 0 << ".\n"
#endif


class AosRundata :  virtual public OmnRCObject, 
					public AosMemoryCheckerObj
					// public AosRundataCallback
{
	OmnDefineRCObject;

public:
	enum
	{
		eMaxCharPtrs = 10,
		eMaxU64Values = 10,
		eMaxInt64Values = 10,
		eMaxDocsByVar = 5,
		eMaxRundatas = 100,
		eMaxTTL = 64, 
		eMaxMonitorLogLength = 100000,

		eSendRdataInfoLen = 100		// Ketty 2012/05/03
	};

private:
	typedef map<OmnString, AosXmlTagPtr> 			DocMap_t;
	typedef map<OmnString, AosXmlTagPtr>::iterator 	DocMapItr_t;
	typedef map<OmnString, OmnString> 				ArgMap_t;
	typedef map<OmnString, OmnString>::iterator		ArgMapItr_t;
	typedef map<OmnString, OmnString> 				CookieMap_t;

private:
	AosSeReqProcPtr			mReqProc;
	AosWebRequestPtr		mReq;
	OmnString				mErrmsg;
	u32						mSiteid;
	bool					mIsEditing;
	bool					mContinueFlag;
	OmnString				mAppname;
	AosSessionObjPtr		mSession;
	OmnString				mSsid;
	OmnString				mCid;
	AosLocale::E			mLocale;
	AosXmlTagPtr			mReqRoot;
	AosXmlTagPtr			mReceivedDoc;
	AosXmlTagPtr			mWorkingDoc;
	AosXmlTagPtr	 		mOldDoc;
	AosXmlTagPtr	 		mNewDoc;
	DocMap_t				mVarDocs;
	OmnString 				mResults;
	OmnString				mOrigObjid;
	vector<OmnString>		mSendCookies;
	CookieMap_t			 	mReceivedCookies;
	OmnString               mMonitorLog;
	u64                     mStartTime;
	u64						mUserid;
	OmnString				mAccessOperation;
	OmnString				mUserOperation;
	ArgMap_t				mArgs;
	OmnString 				mSdocReturnStaus; 		//smart doc Process Control
	OmnString				mEventStr;				// Chen Ding, 08/13/2011
	u64						mUrldocDocid;			// Chen Ding, 12/26/2011
	AosTimerObjPtr			mTimerCaller;
	AosSecReq				mSecReq;
	vector<char *>			mCharPtrs;			// Chen Ding, 05/30/2012
	vector<int>				mCharPtrLens;		// Chen Ding, 05/30/2012
	vector<u64>				mU64Values;			// Chen Ding, 05/30/2012
	vector<int64_t>			mInt64Values;		// Chen Ding, 05/30/2012
	vector<OmnString>		mOmnStrs;			// Chen Ding, 2013/06/01
	u64						mValueU64;			// Chen Ding, 2013/06/01
	int64_t					mValueInt64;		// Chen Ding, 2013/06/01
	char					mValueChar;			// Chen Ding, 2013/06/01
	OmnString				mValueStr;			// Chen Ding, 2013/06/01
	AosRdataErrEntry		mErrEntry;			// Chen Ding, 06/03/2012
	bool					mOverrideFlag1;		// Chen Ding, 07/15/2012
	AosRundataParmPtr		mParms[AosRundataParm::eMaxType];// Chen Ding, 2013/02/18
	AosJimoPtr				mJimo;				// Chen Ding, 2013/05/29
	AosValueRslt			mValueRslt;			// Chen Ding, 2013/06/10
	AosTimeTracker			mTimeTracker;		// Chen Ding, 2013/10/13
	AosReturnCode::E		mReturnCode;
	OmnString				mDatabase;			// Jackie, 2013/12/26
	OmnString				mApiKey;			// Jackie, 2013/12/26
	OmnString				mJQLData;			// Andy,   2014/03/20		
	OmnString				mJQLMsg;			// Andy,   2014/03/23		
	int						mJQLUpdateCount;	// Young,  2014/04/30

	AosBuffPtr				mColumn;
	AosBuffPtr				mColumnValue;
	bool					mIsInternalCall;
	u64						mSnapshotId;		// ice, 2014/10/31
	

    static list<AosRundataPtr>  smRundatas;
	static OmnString 		smSysSdocEventObjid;
	static OmnString        smModuleId;
	static OmnMutex         smLock;
	static u64				smErrorSeqno;		// Chen Ding, 2014/10/25
	OmnString				mLog;
	AosAsyncRespCallerPtr   mAsyncRespCaller;

	bool					mLogicError;
	u64						mJPID;
	u64						mJSID;
	u32						mRepositoryID;
	u64						mCmpLen;

public:
	static AosRundataPtr	smRundata;
	static AosRdataErrEntry smRdataError;

public:
	AosRundata(AosMemoryCheckDeclBegin);
	AosRundata(const AosSeReqProcPtr &proc AosMemoryCheckDecl);
	AosRundata(const u64 jpid,
				const u64 jsid, 
				const u64 userid AosMemoryCheckDecl);
	AosRundata( const u32 siteid, 
					const OmnString &app_name, 
					const OmnString &ssid, 
					const OmnString &cloudid, 
					const u64 userid AosMemoryCheckDecl);
	~AosRundata();

	OmnString 		getErrmsg(); 
	void			setErrmsg(const OmnString &m) {mErrEntry << m;}
	u32				getSiteid() {return mSiteid;}
	void			appendErrors(const OmnString &m);

	void setResults(const OmnString &str){mResults = str;};
	OmnString& getResults(){return mResults;};
	void appendReslts(const OmnString &contents) {mResults << contents;}

	AosRdataErrEntry & setError() {return mErrEntry;}
	 AosRdataErrEntry & setError(const OmnString &term_id, 
	 					const char *fname, 
	 					const int line,
	 					const bool is_user_error = false);
	bool setErrorEntry(
					const bool is_user_error,
					const OmnString &errmsg, 
					const char *fname, 
					const int line);

	static AosRdataErrEntry & setError(
	 		const AosRundataPtr &rdata, 
	  		const OmnString &errid, 
	  		const char *fname, 
	  		const int line)
	 {
	 	if (!rdata)
	 	{
			return smRundata->pushError(errid, "NoName", "", false, fname, line);
	 	}
		return rdata->pushError(errid, "NoName", "", false, fname, line);
	 }

	 static AosRdataErrEntry & setError(
	 		const AosRundataPtr &rdata, 
	  		const AosErrmsgId::E errorid, 
	  		const char *fname, 
	  		const int line)
	 {
	 	OmnString ss;
	 	ss << errorid;
	 	if (!rdata)
	 	{
	 		return smRundata->setError(ss, fname, line, false);
	 	}
	 	return rdata->setError(ss, fname, line, false);
	 }

	// static AosRdataErrEntry & setUserError(
	// 		const AosRundataPtr &rdata, 
	 // 		const OmnString &errid, 
	 // 		const char *fname, 
	 // 		const int line)
	// {
	// 	if (!rdata)
	// 	{
	// 		return smRundata->setError(errid, fname, line, true);
	// 	}
	// 	return rdata->setError(errid, fname, line, true);
	// }

	OmnString &		operator << (const AosErrmsgId::E errorid);
	OmnString		getAppname() const {return mAppname;}
	OmnString		setAppname(const OmnString &n) 
					{OmnString nn = mAppname; mAppname = n; return nn;}
	AosSessionObjPtr	getSession() const;
	void 			setSsid(OmnString ssid){mSsid = ssid;}
	void			setOk() {mErrEntry.reset(); mErrmsg = "";}
	OmnString		setCid(const OmnString &cid) {OmnString cc = mCid; mCid = cid; return cc;}
	AosXmlTagPtr 	setDocByVarWithRet(
						const OmnString &name, 
						const AosXmlTagPtr &doc, 
						const bool checkDoc);
	AosXmlTagPtr	setSourceDoc(const AosXmlTagPtr &doc, const bool checkdoc) 
					{
						return setDocByVarWithRet(AOSARG_SOURCE_DOC, doc, checkdoc);
					}
	AosXmlTagPtr	setTargetDoc(const AosXmlTagPtr &doc, const bool checkdoc) 
					{
						return setDocByVarWithRet(AOSARG_TARGET_DOC, doc, checkdoc);
					}
	AosXmlTagPtr	setRetrievedDoc(const AosXmlTagPtr &doc, const bool checkdoc) 
					{
						return setDocByVarWithRet(AOSARG_RETRIEVED_DOC, doc, checkdoc);
					}
	void			setOrigObjid(const OmnString &m) {mOrigObjid = m;}
	AosXmlTagPtr	setCreatedDoc(const AosXmlTagPtr &doc, const bool checkdoc) 
					{
						return setDocByVarWithRet(AOSARG_CREATED_DOC, doc, checkdoc);
					}
	AosXmlTagPtr	setReceivedDoc(const AosXmlTagPtr &doc, const bool checkdoc);
	AosXmlTagPtr	getReceivedDoc() const 
					{
						return mReceivedDoc;
					}
	AosXmlTagPtr	getRequestRoot() const {return mReqRoot;}
	AosXmlTagPtr	setRequestRoot(const AosXmlTagPtr &r) 
					{
						aos_assert_r(r, 0); 
						AosXmlTagPtr rr = mReqRoot;
						mReqRoot = r;
						return rr;
					}
	AosXmlTagPtr	getCreatedDoc() 
					{
						DocMapItr_t itr = mVarDocs.find(AOSARG_CREATED_DOC);
						if (itr == mVarDocs.end()) return 0;
						return itr->second;
					}
	AosXmlTagPtr	getRetrievedDoc() 
					{
						DocMapItr_t itr = mVarDocs.find(AOSARG_RETRIEVED_DOC);
						if (itr == mVarDocs.end()) return 0;
						return itr->second;
					}
	AosXmlTagPtr	getSourceDoc() 
					{
						DocMapItr_t itr = mVarDocs.find(AOSARG_SOURCE_DOC);
						if (itr == mVarDocs.end()) return 0;
						return itr->second;
					}
	AosXmlTagPtr	getTargetDoc()
					{
						DocMapItr_t itr = mVarDocs.find(AOSARG_TARGET_DOC);
						if (itr == mVarDocs.end()) return 0;
						return itr->second;
					}
	AosXmlTagPtr	getSdoc() 
					{
						DocMapItr_t itr = mVarDocs.find(AOSARG_SMARTDOC);
						if (itr == mVarDocs.end()) return 0;
						return itr->second;
					}
	OmnString		getOrigObjid() const {return mOrigObjid;}
	AosWebRequestPtr getWebRequest() const;
	u64				getUserid() const;
	OmnString		toString(const int level);
	OmnString		getCid() const {return mCid;}
	
	void 		setRequest(const AosWebRequestPtr &req); 
	int			getTransid() const;
	void		resetError() {mErrEntry.reset();}
	bool 		reset();
	u64			setUserid(const u64 uid){ u64 uu = mUserid; mUserid = uid; return uu;};
	void		setSession(const AosSessionObjPtr &session){ mSession = session; };
	u32	setSiteid(const u32 sid)
	{ 
		u32 ss = mSiteid;
		mSiteid = sid; 
		return ss;
	};
	void		setIsEditing(const bool &isediting){ mIsEditing = isediting; };
	bool		isEditing() const {return mIsEditing;};
	void		setNeedLoginObj(const bool needloginobj);
	void		setReceivedCookies(
				const OmnString &name,
				const OmnString &value)
	{ 
		mReceivedCookies[name] = value; 
	}

	void 	setSendCookie(const OmnString &value)
	{ 
		mSendCookies.push_back(value);
	}
	vector<OmnString> & getSendCookies()
	{
		return mSendCookies;
	}

	AosWebRequestPtr	getReq(){return mReq;};
	void		setUserOperation(const OmnString &opr){ mUserOperation = opr; };
	OmnString&	getUserOperation(){ return mUserOperation; };
	void		setAccessOperation(const OmnString &opr){ mAccessOperation = opr; };
	OmnString&	getAccessOperation(){ return mAccessOperation; };
	u64			getStartTime() {return mStartTime;}

	template <class T>
	bool setArg1(const OmnString &name, const T &t)
	{
		smLock.lock();
		OmnString value;
		value << t;
		mArgs[name] = value;
		smLock.unlock();
		aos_assert_r(name != "", false);
		return true;
	}
	OmnString getUserArg(const OmnString &name);
	const OmnString getArg1(
			const OmnString &name, 
			const bool remove_flag = false, 
			const OmnString &dft = "");
	
	// const OmnString getArg1(const OmnString &name, bool &exist)
	// {
	// 	exist = false;
	// 	aos_assert_r(name != "", "");
	// 	map<OmnString, OmnString>::iterator itr;
	// 	itr = mArgs.find(name);
	// 	if (itr == mArgs.end()) 
	// 	{
	// 		exist = false;
	// 		return "";
	// 	}
	// 	exist = true;
	// 	return itr->second; 
	// }
	
	// Chen Ding, 2011/04/26
	AosXmlTagPtr getDocByVar(const OmnString &varname);
	bool setDocByVar(const OmnString &name, const AosXmlTagPtr &doc, const bool checkdoc);


	OmnString getSdocReturnStaus() const
	{
		//Values range [break, return, continue];
		//[AOSTAG_STATUS_BREAK, AOSTAG_STATUS_RETURN, AOSTAG_STATUS_CONTINUE]
		//break :Failure: AosSMDMgr::procSmartdocs Function exit; 
		//		call AosSMDMgr::procSmartdocs Function continue;
		//return :Exit all failed ;
		//Continue: All Unimpeded;
		return mSdocReturnStaus;
	}
	void setSdocReturnStaus(const OmnString &staus)
	{
		if (staus!= AOSTAG_STATUS_BREAK 
				&& staus != AOSTAG_STATUS_RETURN 
				&& staus != AOSTAG_STATUS_CONTINUE)
		{
			mSdocReturnStaus = AOSTAG_STATUS_CONTINUE;
		}
		else
		{
			mSdocReturnStaus = staus;
		}
	}

	// Chen Ding, 2011/05/08
	// bool isOk() const {return mErrEntry.getErrmsg() == "";} 
	bool isOk() const {return mErrmsg == "" && mErrEntry.isOk();} 

	//felicia 2011/06/08
	static AosRundataPtr getRundata();
	static void returnRundata(const AosRundataPtr &rdata);

	bool copyFrom(AosRundata *rdata);
	bool copyCtlrInfo(AosRundata *rdata);
	void setCookies(const map<OmnString, OmnString> &cookies) {mReceivedCookies = cookies;}
	bool getCookies(map<OmnString, OmnString> &cookies) {cookies = mReceivedCookies; return true;};
	OmnString getCookie(const OmnString &cookie_name);

	//Ken Lee, 2011/06/17
	void setTimerCaller(const AosTimerObjPtr &caller){mTimerCaller = caller;}
	AosTimerObjPtr getTimerCaller(){return mTimerCaller;}
	
	void setSdoc(const AosXmlTagPtr &sdoc);
	OmnString getSsid() const;
	AosXmlTagPtr getWorkingDoc()const{return mWorkingDoc;}
	AosXmlTagPtr setWorkingDoc(const AosXmlTagPtr &doc, const bool checkdoc);
	AosLocale::E getLocale() const {return mLocale;}
	AosRundataPtr clone(AosMemoryCheckDeclBegin);
	void setEvent(const OmnString &s) {mEventStr = s;}
	OmnString getEvent() const {return mEventStr;}
	void setUrldocDocid(const u64 &did) {mUrldocDocid = did;}
	u64 getUrldocDocid() const {return mUrldocDocid;}
	void setReqProc(const AosSeReqProcPtr &req);
	AosSeReqProcPtr getReqProc() const;

	void leaveFunction(const OmnString &fname, const int line)
	{
	}

	bool enterFunction(const OmnString &fname, const int line)
	{
		return true;
	}
	AosXmlTagPtr	getAccessedDoc() const {return mSecReq.getAccessedObj();}
	u64	getAccessedDocUserid() 
	{
		AosRundataPtr thisptr(this, false);
		return mSecReq.getAccessedDocUserid(thisptr);
	}
	AosUserAcctObjPtr	getAccessedAcct() const;
	AosUserAcctObjPtr	getRequesterAcct() const;
	AosXmlTagPtr	getRequesterAcctDoc() const;
	bool 			setVerificationCode(const OmnString &code);
	OmnString		getVerificationCode() const;
	AosSecReq &		getSecReq() {return mSecReq;}
	u64				getAccessedUserid() const;
	u64				getRequesterUserid() const;
	OmnString		getAccessedUsername() const;
	OmnString		getRequesterUsername() const;
	OmnString		getAccessedCid() const;
	OmnString		getRequesterCid() const;
	OmnString		getRequesterDomains() const;
	OmnString		getRequesterUserRoles() const;
	OmnString		getRequesterUserGroups() const;
	OmnString getParmValue(const OmnString &parmname);

	void			setSnapshotId(const u64 snap_id){mSnapshotId = snap_id;}
	u64				getSnapshotId(){return mSnapshotId;}

	void			addToBuff(const AosBuffPtr &buff);	// Ketty 2012/05/04
	void			addToXml(const AosXmlTagPtr &xml);	// Chen Ding, 2012/06/19
	static AosRundataPtr	getRdataByBuff(const AosBuffPtr &buff AosMemoryCheckDecl);
	static AosRundataPtr	getRdataFromXml(const AosXmlTagPtr &xml AosMemoryCheckDecl);

private:
	static AosRundataPtr getRundata(
			const OmnString &siteid, 
			const u64 &userid,
			const OmnString &appname);

public:
	bool setValue(const AosValueRslt &value, const AosXmlTagPtr &sdoc);
	bool setDoc(const AosXmlTagPtr &doc, const AosXmlTagPtr &sdoc);
	bool setLog(const OmnString &doc)
	{
		mLog = doc;
		return true;
	}
	OmnString getLogStr()
	{
		return mLog;
	}

	// Chen Ding, 05/30/2012
	bool setCharPtr(const int idx, const char *str, const int len) 
	{
		if (idx < 0 || (u32)idx >= eMaxCharPtrs) return false;
		if ((u32)idx >= mCharPtrs.size())
		{
			mCharPtrs.resize(idx+1);
			mCharPtrLens.resize(idx+1);
		}
		mCharPtrs[idx] = (char *)str;
		mCharPtrLens[idx] = len;
		return true;
	}
	
	char * getCharPtr(const int idx, int &len) const 
	{
		if (idx < 0 || (u32)idx >= mCharPtrs.size()) return 0;
		len = mCharPtrLens[idx];
		return mCharPtrs[idx];
	}
	
	bool setU64Value(const int idx, const u64 &value)
	{
		if (idx < 0 || (u32)idx >= eMaxU64Values) return false;
		if ((u32)idx >= mU64Values.size()) mU64Values.resize(idx+1);
		mU64Values[idx] = value;
		return true;
	}

	u64 getU64Value(const int idx, const u64 &dft) const
	{
		if (idx < 0 || (u32)idx >= mU64Values.size()) return dft;
		return mU64Values[idx];
	}

	bool setInt64Value(const int idx, const int64_t &value)
	{
		if (idx < 0 || (u32)idx >= eMaxInt64Values) return false;
		if ((u32)idx >= mInt64Values.size()) mInt64Values.resize(idx+1);
		mInt64Values[idx] = value;
	}
	
	u64 getInt64Value(const int idx, const int64_t &dft) const
	{
		if (idx < 0 || (u32)idx >= mInt64Values.size()) return dft;
		return mInt64Values[idx];
	}

	void setContinueFlag(const bool flag) {mContinueFlag = flag;}
	bool continueProc() const {return mContinueFlag;}

	// Chen Ding, 07/15/2012
	void setOverrideFlag(const bool flag) {mOverrideFlag1 = flag;}
	bool needOverride() const {return mOverrideFlag1;}

	// Chen Ding, 2013/02/14
	bool setForSending(const AosXmlTagPtr &xml);
	static AosRundataPtr serializeFromMsg(
			const AosXmlTagPtr &xml AosMemoryCheckDecl);
	AosRundataParmPtr getParm(const AosRundataParm::Type type)
	{
		aos_assert_r(AosRundataParm::isValid(type), 0);
		return mParms[type];
	}	
	 
	bool setParm(const AosRundataParmPtr &parm)
	{
	 	aos_assert_r(parm, false);
	 	AosRundataParm::Type type = parm->getType();
	 	aos_assert_r(AosRundataParm::isValid(type), false);
	 	mParms[type] = parm;
	 	return true;
	}

	bool removeParm(const AosRundataParmPtr &parm)
	{
		aos_assert_r(parm, false);
		AosRundataParm::Type type = parm->getType();
		aos_assert_r(AosRundataParm::isValid(type), false);
		mParms[type] = 0;
		return true;
	}

	bool setReturnCode(const AosReturnCode::E &code)
	{
		aos_assert_r(AosReturnCode::isValid(code), false);
		mReturnCode = code;
		return true;
	}

	AosReturnCode::E getReturnCode()
	{
		return mReturnCode;
	}

	// Chen Ding, 2013/03/05
	bool setForSending(AosBuffMsg &msg);
	static AosRundataPtr serializeFromMsg(AosBuffMsg &msg AosMemoryCheckDecl);

	// Chen Ding, 2013/05/29
	void setJimo(const AosJimoPtr &jimo);
	AosJimoPtr getJimo();
	
	int64_t 	getInt64Value() const {return mValueInt64;}
	u64			getU64Value() const {return mValueU64;}
	char		getCharValue() const {return mValueChar;}
	OmnString	getStrValue() const {return mValueStr;}
	void		setInt64Value(const int64_t vv) {mValueInt64 = vv;}
	void		setU64Value(const u64 vv) {mValueU64 = vv;}
	void		setCharValue(const char vv) {mValueChar = vv;}
	void		setStrValue(const OmnString &vv) {mValueStr = vv;}
	void		setValueRslt(const AosValueRslt &r) {mValueRslt = r;}
	AosValueRslt getValueRslt() const {return mValueRslt;}
	void		setColumn(const AosBuffPtr &column)
	{
		mColumn = column;
	}
	void		setColumnValue(const AosBuffPtr &value)
	{
		mColumnValue = value;
	}
	// Chen Ding, 2013/10/01
	OmnString & getMonitorLog();

	// Chen Ding, 2013/10/13
	void resetTimeTracker()
	{
		// smLock.lock();
		// mTimeTracker.reset();
		// smLock.unlock();
	}

	void appendTime(const char *fname, const int line, const OmnString &name)
	{
		// smLock.lock();
		// mTimeTracker.append(fname, line, name);
		// smLock.unlock();
	}

	void appendTime(const char *fname, 
			const int line, 
			const OmnString &name, 
			const u64 time)
	{
		// smLock.lock();
		// mTimeTracker.append(fname, line, name, time);
		// smLock.unlock();
	}

	OmnString dumpTimesToAscii() const
	{
		// return mTimeTracker.convertToAscii(20, AosTimeTracker::eMicrosec, true);
		return "NoTimeTracker";
	}

	AosTimeTracker getTimeTracker() const
	{
		return mTimeTracker;
	}

	bool appendTimes(const AosTimeTracker &times)
	{
		// return mTimeTracker.append(times);
		return true;
	}

	void pushTimestamp()
	{
		// mTimeTracker.pushTimestamp();
	}

	bool popTimestamp()
	{
		// return mTimeTracker.popTimestamp();
		return true;
	}

	bool setTotalTime(const char *fname, 
					const int line, 
					const OmnString &name)
	{
		// return mTimeTracker.setTotalTime(fname, line, name);
		return true;
	}

	static int getNumInstances();
	AosBuffPtr  getColumn()
	{
		return mColumn;
	}
	AosBuffPtr	getColumnValue()
	{
		return mColumnValue;
	}

	bool setAsyncRespCaller(const AosAsyncRespCallerPtr &resp_caller)
	{
		mAsyncRespCaller = resp_caller;
		return true;
	}
	AosAsyncRespCallerPtr getAsyncRespCaller()
	{
		return mAsyncRespCaller;
	}

	AosRdataErrEntry &pushError(	
					const OmnString &errmsg_id, 
					const OmnString &name, 
					const OmnString &errmsg, 
					const bool is_user_error,
					const char *fname, 
					const int line);

	// Chen Ding, 2014/11/09
	AosRdataErrEntry &pushError(	
					const OmnString &errmsg_id, 
					const bool is_user_error,
					const char *fname, 
					const int line);

	// Chen Ding, 2014/11/09
	AosRdataErrEntry &pushError(	
					const AosErrmsgId::E errmsg_id, 
					const bool is_user_error,
					const char *fname, 
					const int line);

	// Chen Ding, 2013/12/08
	bool isInternalCall() const {return mIsInternalCall;}
	void setInternalCall(const bool b) {mIsInternalCall = b;}

	void setDatabase(OmnString database)
	{
		mDatabase = database;
	}

	// Barry Niu, 2014/11/06
	OmnString getDatabase(){return mDatabase;}


	// Andy, 2014/03/23
	OmnString getJqlData() 
	{
		OmnString msg = mJQLData;
		mJQLData = "";
		return msg;
	}
	OmnString getJqlMsg()
	{
		OmnString msg = mJQLMsg;
		mJQLMsg = "";
		return msg;
	}
	// Young, 2014/04/30
	int getJqlUpdateCount() { return mJQLUpdateCount; }
	void setJqlUpdateCount(const int count) { mJQLUpdateCount = count; }
	void setJqlData(OmnString data) { mJQLData = data; }
	void setJqlMsg(OmnString error) { mJQLMsg = error; }
	void setApiKey(OmnString api_key) { mApiKey = api_key; }
	OmnString getErrorNum() 
	{
		return getArg1(AOSARG_ERROR_NUMBER);
	}

	// Chen Ding, 2014/11/30
	inline void pushU64(const u64 value) {mU64Values.push_back(value);}

	bool serializeToBuffForJimoCall(AosBuff *buff);
	static AosRundataPtr serializeFromBuffForJimoCall(AosBuff *buff);

	// Chen Ding, 2015/05/29
	void setLogicError(const bool b) {mLogicError = b;}
	bool getLogicError() const {return mLogicError;}
	u64  getJPID() const {return mJPID;}
	u64  getJSID() const {return mJSID;}
	void setJPID(const u64 jpid) {mJPID = jpid;}
	void setJSID(const u64 jsid) {mJSID = jsid;}
	u32  getRepositoryID() const {return mRepositoryID;}
	bool setCmpLen(const u64 cmp_len)
	{
		mCmpLen = cmp_len;
		return true;
	}
	u64 getCmpLen(){return mCmpLen;}

};
#endif

