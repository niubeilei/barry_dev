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
// 2013/04/26	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SdocAction_ActImportDoc2_h
#define Aos_SdocAction_ActImportDoc2_h

#include "Actions/TaskAction.h"
#include "Actions/Ptrs.h"
#include "DocUtil/DocProcUtil.h"
#include "WordParser/WordNorm.h"     
#include "Rundata/Ptrs.h"

class AosActImportDoc2 : virtual public AosTaskAction
{
	enum
	{
		eMaxStrLen = 50,
		eMaxThrdIds = 10
	};

private:
	OmnMutexPtr					 				mLock;
	OmnString									mDocumentCtnr;
	AosDataAssemblerObjPtr 						mDocVarAssembler;
	AosDataAssemblerObjPtr 						mHitIILAsm;
	map<OmnString, AosDataAssemblerObjPtr>		mIILAsms;
	vector<OmnString> 							mAttrPaths;
	vector<OmnString> 							mAttrNames;
	map<OmnString, AosDataAssemblerObjPtr>		mOprIILAsms;
	map<OmnString, vector<OmnString> >			mOprValues;
	vector<OmnString> 							mOprAttrPaths;
	vector<OmnString> 							mOprAttrNames;
	vector<OmnString> 							mOprs;
	vector<OmnString> 							mSplitAttrPaths;
	vector<OmnString> 							mSplitAttrNames;
	map<OmnString, OmnString>					mSplitAttrWords;
	map<OmnString, vector<OmnString> >			mSplitValues;
	AosDataScannerObjPtr 						mScanner;
	int											mRcdLen;
	int64_t										mFileIdx;
	int64_t										mRunNum;
	int											mThreadId;

	AosWordParserPtr                            mWordParser[eMaxThrdIds];
	AosWordNormPtr                              mWordNorm[eMaxThrdIds];

	static AosDocProcUtil::AosSeWordHash            mWords[eMaxThrdIds];
public:
	AosActImportDoc2(const bool flag);
	~AosActImportDoc2();


/*	bool run(const AosTaskObjPtr &task,
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata);
*/
	AosActionObjPtr clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const;
	
	virtual bool config(const AosTaskObjPtr &task,
			const AosXmlTagPtr &conf,
			const AosRundataPtr &rdata){ return true;};
	
	virtual bool 			initAction(
								const AosTaskObjPtr &task, 
								const AosXmlTagPtr &sdoc,
								const AosRundataPtr &rdata);
	virtual bool finishedAction(const AosRundataPtr &rdata);
	
	virtual bool runAction(const AosRundataPtr &rdata);
	
	virtual void onThreadInit(const AosRundataPtr &, void **data);
	virtual void onThreadExit(const AosRundataPtr &, void **data);
	
private:
/*	OmnString composeEntry(
		const u64 &docid, 
		const OmnString &filestr,
		const int partnum);
*/
	OmnString convertWordPdf(
		const OmnString &dir,
		const OmnString &source_fname,
		const OmnString &document_type,
		const int64_t fileidx);

	bool proc(
		const AosXmlTagPtr &predoc,
		const vector<OmnString> &has_attrs,
		const vector<OmnString> &has_values,
		map<OmnString, vector<OmnString> > &split_values,
		const OmnString &fname,
		const AosRundataPtr &rdata);

	u64 	getNextDocid(const AosRundataPtr &rdata);
	
	void	iilAppendEntry(
				const u64 docid, 
				const vector<OmnString> &has_attrs,
				const vector<OmnString> &has_values,
				const AosRundataPtr &rdata);
	
	void	OprIILAppendEntry(
				const u64 docid,
				const AosXmlTagPtr &predoc,
				map<OmnString, vector<OmnString> > &split_values,
				const AosRundataPtr &rdata);
	
	OmnString	composeEntry(
					const u64 &docid,
					const OmnString &filestr, 
					const int partnum,
					const AosXmlTagPtr &predoc);

	bool		getIILAsms(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	
	AosXmlTagPtr	getPreDoc(
						const AosXmlTagPtr &doc,
						const OmnString &sfname,
						vector<OmnString> &has_attrs,
						vector<OmnString> &has_values,
						map<OmnString, vector<OmnString> > &split_values,
						const AosRundataPtr &rdata);
	
	bool		asmSendFinish(const AosRundataPtr &rdata);
	bool 		actionFinished(const AosRundataPtr &rdata);
	
	bool		addAttrWordIIL(
					const OmnString &attrname,
					const vector<OmnString> &values,
					const u64 &docid,
					const u64 &title_docid,
					const AosRundataPtr &rdata);

	bool 		asmAppend(
					const OmnString &filestr,
					const int &partnum,
					const AosXmlTagPtr &predoc,
					const vector<OmnString> &has_attrs,
					const vector<OmnString> &has_values,
					map<OmnString, vector<OmnString> > &split_values,
					const AosRundataPtr &rdata);
	
	void		splitBytes(
					const OmnString &word,
					const u64 &docid,
					const u64 &title_docid,
					const AosRundataPtr &rdata);

	bool		createDataScanner(
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);

	bool		query(
					const AosXmlTagPtr &query_xml,
					vector<u64> &docids,
					const AosRundataPtr &rdata);
};
#endif

