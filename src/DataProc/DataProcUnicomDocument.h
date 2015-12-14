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
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcUnicomDocument_h
#define Aos_DataProc_DataProcUnicomDocument_h

#include "DocUtil/DocProcUtil.h"     
#include "WordParser/WordNorm.h"     
#include "DataProc/DataProc.h"
#include "Util/DataTypes.h"


class AosDataProcUnicomDocument : virtual public AosDataProc
{
	enum
	{
		eMaxStrLen = 50,
		eMaxThrdIds = 15,
		eMaxWordsPerDoc = 1000000
	};

	struct OutputInfo
	{
		OmnString			mAttrName;
		OmnString 			mAttrPath;
		OmnString			mOpr;
		OmnString			mIILType;
		OmnString			mDocidFieldName;
		OmnString			mValueFieldName;
		RecordFieldInfo		mOutput;

		OutputInfo (){}
	};
private:
	OmnMutexPtr                                 mLock;  
	int											mDocNum;
	RecordFieldInfo								mInput;
	RecordFieldInfo								mOutputDoc;
	OutputInfo									mOutputHitInfo;
	OutputInfo									mOutputAttrInfo;
	vector<OutputInfo>							mOutputKeys;
	OmnString 									mUuicomTitlePctrs;
	OmnString 									mUuicomFilePctrs;
	OmnString									mDocumentCtnr;
	map<OmnString, OmnString>					mAttrMap;
	
	int64_t										mFileIdx;
	int											mThreadId;

	AosWordParserPtr                            mWordParser[eMaxThrdIds];
	AosWordNormPtr                              mWordNorm[eMaxThrdIds];

	static AosDocProcUtil::AosSeWordHash            mWords[eMaxThrdIds];
	static AosDocProcUtil::AosSeAttrHash            mAttrs[eMaxThrdIds];

public:
	AosDataProcUnicomDocument(const bool flag);
	AosDataProcUnicomDocument(const AosDataProcUnicomDocument &proc);
	~AosDataProcUnicomDocument();

	virtual AosDataProcStatus::E procData(
				const AosDataRecordObjPtr &record,			
				const u64 &docid,
				const AosDataRecordObjPtr &output,			
				const AosRundataPtr &rdata);

	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	
	virtual bool resolveDataProc(
	 					map<OmnString, AosDataAssemblerObjPtr> &asms,
 						const AosDataRecordObjPtr &record,
 						const AosRundataPtr &rdata);

	// Ketty 2013/12/19
	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);

	virtual void onThreadInit(const AosRundataPtr &, void **data);

	virtual void onThreadExit(const AosRundataPtr &, void **data);
	
private:
	bool		config(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);
	
	u64			getNextDocid(
					OmnString &objid,
					const AosRundataPtr &rdata);
	
	bool		getOutPuts(
					const AosXmlTagPtr &def,
					const AosRundataPtr &rdata);

	bool		addAttrForUpload(
					const AosXmlTagPtr &doc, 
					const AosRundataPtr &rdata);

	bool		importDoc(
					const AosXmlTagPtr &binary_doc,
					const AosBuffPtr &buff,
					AosDataRecordObj **input_records,
					AosDataRecordObj **output_records,
					const AosRundataPtr &rdata);

	OmnString	convertWordPdf(
					const OmnString &dir,
					const OmnString &source_fname,
					const OmnString &document_type,
					const int64_t fileidx);

  AosXmlTagPtr	getPreDoc(
					const AosXmlTagPtr &doc,
					const OmnString &sfname,
					map<OmnString, OmnString> &has_values,
					map<OmnString, vector<OmnString> > &split_values,
					const AosRundataPtr &rdata);

	bool		proc(
					const AosXmlTagPtr &predoc,
					map<OmnString, OmnString> &has_values,
					map<OmnString, vector<OmnString> > &split_values,
					const OmnString &fname, 
					AosDataRecordObj **input_records,
					AosDataRecordObj **output_records,
					const AosRundataPtr &rdata);

	bool 		asmAppend(
					const OmnString &filestr,
					const int &partnum,
					const AosXmlTagPtr &predoc,
					map<OmnString, OmnString> &has_values,
					map<OmnString, vector<OmnString> > &split_values,
					AosDataRecordObj **input_records,
					AosDataRecordObj **output_records,
					const AosRundataPtr &rdata);

	bool		appendHitRecord(
					AosDataRecordObj * hit_record,
					const OmnString &word,
					const u64 docid,
					const u64 title_docid,
					const AosRundataPtr &rdata);

	void		splitBytes(
					const OmnString &word,
					const u64 &docid,
					const u64 &title_docid,
					AosDataRecordObj * hit_record,
					const AosRundataPtr &rdata);

	bool		addAttrWordIIL(
					const OmnString &attrname,
					const vector<OmnString> &values,
					const u64 &docid,
					const u64 &title_docid,
					AosDataRecordObj *hit_record,
					const AosRundataPtr &rdata);

	bool		iilAppendEntry(
					const u64 docid,
					const u64 title_docid,
					map<OmnString, OmnString> &has_values,
					map<OmnString, vector<OmnString> > &split_values,
					AosDataRecordObj * hit_record,
					AosDataRecordObj **input_records,
					AosDataRecordObj **output_records,
					const AosRundataPtr &rdata);

	OmnString	composeEntry(
					const u64 &docid,
					const OmnString &filestr, 
					const int partnum,
					const AosXmlTagPtr &predoc);

	bool		createDocPriv(
					const AosXmlTagPtr &doc,
					AosDataRecordObj **input_records,
					AosDataRecordObj **output_records,
					const AosRundataPtr &rdata);

	bool 		addAttrIIL(
					const OmnString &name, 
					const OmnString &value, 
					const u64 docid, 
					const OmnString &ctnr_objid, 
					const AosEntryMark::E mark,
					AosDataRecordObj *attr_record,
					const AosRundataPtr &rdata);

};

#endif
