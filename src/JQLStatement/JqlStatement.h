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
// Modification History:
// 2013/09/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JQLSTATEMENT_H
#define AOS_JQLStatement_JQLSTATEMENT_H

#include "AosConf/AosConfig.h"
#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Opr.h"
#include "SEUtil/JqlTypes.h"
#include "SEUtil/JqlUtil.h"
#include "SEInterfaces/JimoProgObj.h"
#include "JQLStatement/Ptrs.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLExpr/Ptrs.h"
#include "MySqlAdapter/MySqlRecord.h"

#include <stdarg.h>
#include <cstdlib>
#include <vector>
#include <set>

#include <boost/shared_ptr.hpp>
using AosConf::AosConfBase;
using boost::shared_ptr;

typedef vector<AosExprNameValuePtr> AosExprNameValues;

class AosJqlStatement : public OmnRCObject
{
	OmnDefineRCObject;

public:
	struct QueryCondtion
	{
		OmnString       mIILName;
		AosOpr          mOpr;
		OmnString       mKey;

		QueryCondtion(const OmnString &iilname,
				const AosOpr opr,
				const OmnString key)
		: mIILName(iilname), mOpr(opr), mKey(key) {}
		~QueryCondtion() {}
	};


public:
	static map<OmnString, OmnString>			mHoldConfs;
	JQLTypes::OpType 							mOp;
	OmnString									mContentFormat;
	AosExprNameValues*							mConfParms;

	//arvin 2015,05,18
	OmnString 									mOrigStmt;
	OmnString 									mErrmsg;
	bool										mIsService;

	// 2015/7/28
	OmnString									mCurDatabase;

	// 2015/9/14
	// jimodb-671
	AosXmlTagPtr								mInputRecord;

public:
	AosJqlStatement();
	AosJqlStatement(const OmnString &contentType);
	~AosJqlStatement();

	virtual AosJqlStatement *clone(){return 0;}
	virtual void dump(){return; }
	//virtual bool run(const AosRundataPtr &rdata){return false;}
	virtual OmnString getErrmsg()const {return mErrmsg; };
	virtual void setOp(JQLTypes::OpType op) { mOp = op; }
	// added by Young: 2014/04/09
	bool setContentFormat(const OmnString &type) {mContentFormat = type; return true;}
	bool setPrintData(const AosRundataPtr &rdata, const OmnString &data);
	bool removeQueryRsltAttr(AosXmlTagPtr &data);
	// arvin 2015/04/21 
	virtual void setOrigStatement(const OmnString &stmt);
	virtual void setIsService(bool &isService);
	virtual bool isService() const ;
	
	virtual OmnString getOrigStatement()const ;
	
	// Chen Ding, 2015/05/31
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual bool run(AosRundata *rdata, 
					AosJimoProgObj *prog, 
					OmnString &statements_str,
					bool isparse);

	virtual bool getStatements(vector<AosJqlStatementPtr> &statements);
	//utility methods
	static bool listDocs(
					const AosRundataPtr &rdata,
					const JQLTypes::DocType type,
					vector<AosXmlTagPtr> &values);

	static bool createTable(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &xml);

	static bool createTable(
					const AosRundataPtr &rdata,
					const OmnString &table_def);

	static bool dropTable(
					const AosRundataPtr &rdata,
					const OmnString &table_name);

	static u64 getTableDocid(
					const AosRundataPtr &rdata,
					const OmnString &tablename);

	static AosXmlTagPtr getTableDoc(
					const AosRundataPtr &rdata,
					const OmnString &tablename);

	static vector<AosJqlColumnPtr> *getTableColumns(
					const AosRundataPtr &rdata,
					const OmnString &tablename,
					u32 &recordLen);

	static AosJqlColumnPtr getTableField(
					const AosRundataPtr &rdata,
					const OmnString &tablename, 
					const OmnString &fieldname);

	//
	//jimodb-671 Phil
	//
	static AosJqlColumnPtr getDatasetField(
					const AosRundataPtr &rdata,
					const AosDatasetObjPtr &dataset, 
					const OmnString &fieldname);
	static bool pickTableByField(
					const AosRundataPtr &rdata,
					const vector<OmnString> &tablenames,
					const OmnString &fname,
					OmnString &tablename);

	static bool isFieldInTable(
					const AosRundataPtr &rdata,
					const char *table_name,
					const char *field_name);

	static bool getTableNames(
					const AosRundataPtr &rdata,
					vector<string> &values);

	static bool createJQLTable(
					const AosRundataPtr &rdata,
					const OmnString &table_def);

	static bool dropJQLTable(
					const AosRundataPtr &rdata,
					const OmnString &table_name);

	static bool fieldExist(
					const AosRundataPtr &rdata,
					const AosJqlColumnPtr &column,
					const OmnString &name,
					const OmnString &value);

	static AosXmlTagPtr getTableXml(
					const AosRundataPtr &rdata,
					const OmnString &tablename);

	static OmnString printTableInXml(
					const vector<OmnString> &fields,
					map<OmnString, OmnString> &aliasFields,
					const AosXmlTagPtr &data);

	static AosXmlTagPtr modifyQueryRslt(
					const vector<OmnString> &fields,
					map<OmnString, OmnString> &aliasFields,
					const AosXmlTagPtr &data);

	static OmnString printDividingLine(
					const vector<OmnString> &fields,
					map<int, int> &field_value_len);

	static OmnString printField(
					const OmnString &value,
					const u32 len);

	static OmnString getSingleIILName(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &doc,
					const OmnString &fildname);

	static OmnString getIILName(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &doc,
					const OmnString &fildname);


	static OmnString getIILName(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &doc,
					set<OmnString> &fields);

	static bool createDoc(
					const AosRundataPtr &rdata,
					const JQLTypes::DocType type,
					const OmnString &name,
					const AosXmlTagPtr &doc);

	static bool createDocByObjid(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &doc,
					const OmnString &objid);

	static AosXmlTagPtr getDoc(
					const AosRundataPtr &rdata,
					const JQLTypes::DocType type,
					const OmnString &name);

	static bool getDocs(
					const AosRundataPtr &rdata,
					const JQLTypes::DocType type,
					vector<AosXmlTagPtr> &values);

	static AosXmlTagPtr	getDocByKey(
					const AosRundataPtr &rdata,
					const OmnString &container,
					const OmnString &key_name,
					const OmnString &key_value);

	static AosXmlTagPtr getDocByObjid(
					const AosRundataPtr &rdata,
					const OmnString &objid);

	static AosXmlTagPtr getDocByDocid(
					const AosRundataPtr &rdata,
					const u64 &docid);

	static bool deleteDocByObjid(
					const AosRundataPtr &rdata,
					const OmnString &objid);

	static bool deleteDocByDocid(
					const AosRundataPtr &rdata,
					const u64 &docid);

	static bool modifyDoc(
					const AosRundataPtr &rdata,
					const AosXmlTagPtr &doc);

	static bool deleteDoc(
					const AosRundataPtr &rdata,
					const JQLTypes::DocType type,
					const OmnString &name);

	// Move to AosObjid in SEUtil, Ken Lee, 2015/02/12
	// Modify by Young, cause child class maybe need getObjid
	static OmnString getObjid(
					const AosRundataPtr &rdata,
					const JQLTypes::DocType type,
					const OmnString &name);

	static OmnString getNameByExprObj(AosExprObjPtr &expr);

	// Ken Lee, 2015/08/03 move to SEUtil/JqlUtil.h
	//static bool escape(OmnString &data);
	//static bool unescape(OmnString &data);

	static OmnString getTableFullName(const AosRundataPtr &rdata, const OmnString &name);

	static OmnString getCurDatabase(const AosRundataPtr &rdata);

	// For Quick search config
	static bool isNeedSave(const JQLTypes::DocType type);
	static bool setHoldConf(
						const AosRundataPtr &rdata,
						const JQLTypes::DocType type,
						const OmnString &name,
						const OmnString &conf);
	static OmnString getHoldConf(
						const AosRundataPtr &rdata,
						const JQLTypes::DocType type,
						const OmnString &name);
	static bool getHoldConfs(
						const AosRundataPtr &rdata,
						const JQLTypes::DocType type,
						vector<AosXmlTagPtr> &values);
	static bool dropHoldConf(
						const AosRundataPtr &rdata,
						const JQLTypes::DocType type,
						const OmnString &name);
	static bool isConfExist(
						const AosRundataPtr &rdata,
						const JQLTypes::DocType type,
						const OmnString &name);

public:
	bool 		createUser(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &userdoc);

	bool		doInsertInto(
						const AosRundataPtr &rdata,
						const OmnString &dataconf);

	// return the doc we created
	AosXmlTagPtr createDoc1(
						const AosRundataPtr &rdata,
						const OmnString &conf);

	bool		deleteIIL(
						const AosRundataPtr &rdata,
						const OmnString &iilname);

	AosXmlTagPtr executeQuery(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &conf);

	bool 		modifyDocAttr(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &doc,
						const OmnString &name,
						const OmnString &value);

   AosBitmapObjPtr querySafe(
                       const AosRundataPtr &rdata,
                       vector<QueryCondtion> &conds);

   AosBitmapObjPtr querySafe(
                       const AosRundataPtr &rdata,
                       QueryCondtion &conds);

	bool		querySafeOnIIL(
						const AosRundataPtr &rdata,
						const i64 &pageSize,
						const i64 blockSize,
						const OmnString &termStr,
						vector<i64>	&docids,
						vector<string> &values,
						i64 &numDocs);

	bool 		setCurDatabase(
						const AosRundataPtr &rdata,
						const OmnString &dbname);

	bool 		startJob(
						const AosRundataPtr &rdata,
						const OmnString &objid);

	bool 		restartJob(
						const AosRundataPtr &rdata,
						const OmnString &objid);

	bool 		stopJob(
						const AosRundataPtr &rdata,
						const OmnString &objid);

	bool 		stopJob(const OmnString &objid, OmnString &args);

	bool 		runSmartDoc(
						const AosRundataPtr &rdata,
						const AosXmlTagPtr &conf);

	AosXmlTagPtr getConf(
						const AosRundataPtr &rdata,
						boost::shared_ptr<AosConfBase> confObj,
						AosExprNameValues* parms);

	OmnString	getFieldDataType(const AosRundataPtr &rdata, 
						const OmnString &field_name);	

	void setConfParms(AosExprNameValues* parms) { mConfParms = parms; }

	// Add by Young (for shawn)
	bool 		rebuildBitmap(
						const AosRundataPtr &rdata,
						const OmnString &iilname);

	// for ketty
	bool 		refactorInternalStatConf(
						const AosRundataPtr &rdata,
						const OmnString &stat_doc_objid,
						const AosXmlTagPtr &stat_def_doc);

	static OmnString AosJType2DType(const OmnString &type);
	static OmnString AosDType2JType(const OmnString &type);
	
	// 2015/7/28
	virtual bool  checkDatabase(const AosRundataPtr &rdata);
	virtual bool setDatabase(const AosRundataPtr &rdata);

	// jimodb-671
	AosXmlTagPtr getTableDataField(
			const AosRundataPtr &rdata,
			const OmnString &table_name,
			const AosJqlSelectFieldPtr &field);
	
	AosDataFieldType::E  convertToDataFieldType(
			AosRundata* rdata,
			const AosExprObjPtr &expr,
			const AosDataRecordObjPtr &record);

private:
	static AosXmlTagPtr getDocByName(
						const AosRundataPtr &rdata,
						const JQLTypes::DocType type,
						const OmnString &name);
						
	static u32 getNonEnglishCharCount(const OmnString& oString);

public:
	// Chen Ding, 2015/05/31
	virtual bool isJimoLogic() const {return false;}
	virtual bool compileJQL(AosRundata *rdata, AosJimoProgObj *prog) {return true;}
	virtual bool semanticsCheck(AosRundata *rdata, AosJimoProgObj *prog){return true;}

};

#endif
