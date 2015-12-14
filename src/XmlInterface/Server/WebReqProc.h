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
// 03/24/2009: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlInterface_Server_WebReqProc_h
#define Aos_XmlInterface_Server_WebReqProc_h

#include "Database/Ptrs.h"
#include "Proggie/ReqDistr/NetReqProc.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "SearchEngine/Ptrs.h"
#include "Util/DynArray.h"
#include "XmlInterface/XmlRc.h"



class TiXmlNode;
class TiXmlElement;
class OmnString;

class AosWebReqProc : public AosNetReqProc
{
	OmnDefineRCObject;

public:
	enum
	{
		eArrayInitSize = 100,
		eArrayIncSize = 50,
		eArrayMaxSize = 10000,
		eMaxRequesters = 1000,
		eTimeoutSec = 600			// 600 seconds
	};

private:
	OmnString			mUsername;
	bool				mIsAscending;
	AosNetRequestPtr 	mConns[eMaxRequesters]; 
	int					mTime[eMaxRequesters]; 
	OmnString			mReqNames[eMaxRequesters]; 
	int					mNumReqs;
	OmnMutexPtr			mLock;

public:
	enum 
	{
		eDefaultPageSize = 30,
		eMaxFields = 50
	};

	AosWebReqProc();
	~AosWebReqProc();

	virtual bool			procRequest(const OmnConnBuffPtr &req);

private:
	bool 	modifyObj(const AosNetRequestPtr &req, TiXmlNode *root);
	bool 	retrieveObj(const AosNetRequestPtr &req, TiXmlNode *root);
	bool 	retrieveTagData(
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	retrieveList(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	retrieveList(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem, 
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	procServerReq(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	void 	sendResp(const AosNetRequestPtr &req, 
				const AosXmlRc errcode,
				const OmnString &errmsg, 
				const OmnString &contents);
	bool 	getFileNames(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	createContainer(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	checkLogin(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	constructQuery(
				const char *def, 
				OmnString &query, 
				OmnString &errmsg);
	bool 	createObj(
				const AosNetRequestPtr &req, 
				TiXmlNode *root, 
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	retrieveMembers(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem, 
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	retrieveObj(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem, 
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	isStdFname(
				const OmnString &tname, 
				const OmnString &fname);
	bool 	procNonStdResults(
				const OmnString &order_fname,
				const int numFields,
				const OmnDbTablePtr &table, 
				OmnDynArray<OmnDbRecordPtr, eArrayInitSize, 
					eArrayIncSize, eArrayMaxSize> &records,
				OmnDynArray<AosXmlTagPtr, eArrayInitSize, 
					eArrayIncSize, eArrayMaxSize> &xmls);
	bool 	parse_order(
				const OmnString &tname, 
				const OmnString &order_fname, 
				OmnString &stdOrderFnames, 
				OmnString &xmlOrderFnames);
	bool 	sendmail(
				const AosNetRequestPtr &req, 
				TiXmlElement *childelem,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	sendmsg(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	regServer(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	procServerreq(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	getOnlineUsers(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	notifyUsers(const OmnString &username);
	bool 	getDomain(
				const AosNetRequestPtr &req, 
				TiXmlNode *root,
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
	bool 	uploadImgReq(
				const AosNetRequestPtr &req, 
				OmnString &contents,
				AosXmlRc &errcode,
				OmnString &errmsg);
};
#endif

