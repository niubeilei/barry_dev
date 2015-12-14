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
// 2014/10/22 Created by Bryant Zhou
////////////////////////////////////////////////////////////////////////////

#include "JQLStatement/JqlStmtActor.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "JQLStatement/JQLCommon.h"
#include "JQLStatement/JqlStmtInsertItem.h"
#include "JQLStatement/JqlStmtTable.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "API/AosApi.h"
#include "QueryUtil/QrUtil.h"
#include "QueryClient/QueryClient.h"
#include "XmlUtil/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEBase/SeUtil.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/UrlMgr.h"
#include "SEUtil/Objid.h"
#include "SEUtil/Docid.h"
#include "SEUtil/SysLogName.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/SysInfo.h"
AosJqlStmtActor::AosJqlStmtActor()
{
	mName = "";
	mClassName = "";
	mPath = "";
	mType_language = "";
	mDescription = "";

	mOp = JQLTypes::eOpInvalid;
}

AosJqlStmtActor::~AosJqlStmtActor()
{
}

/***************************  
 *Getter/setter
 ***************************/
 void 
 AosJqlStmtActor::setName(
	         const OmnString &name)
 {
	     mName = name;
 }

 void 
 AosJqlStmtActor::setClassName(
	         const OmnString &className)
 {
	     mClassName = className;
 }

 void 
 AosJqlStmtActor::setPath(
	         const OmnString &path)
 {
	     mPath = path;
 }

 void 
 AosJqlStmtActor::setType_language(
	         const OmnString &type_language)
 {
	     mType_language = type_language;
 }

 void 
 AosJqlStmtActor::setDescription(
	         const OmnString &description)
 {
	     mDescription = description;
 }

void
 AosJqlStmtActor::setKeys(AosExprList* par_key_names)
{
	mPNames = par_key_names;
}

void
 AosJqlStmtActor::setValues(AosExprList* run_vals_list)
{
	mPValues = run_vals_list;
}

bool
AosJqlStmtActor::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mOp == JQLTypes::eCreate) return createActor(rdata);
	if (mOp == JQLTypes::eRun) return runActor(rdata);

	AosSetEntityError(rdata, "JQL_Actor_run_err", "JQL Actor", "")
		<< "opr is undefined!" << enderr;                               
	return false;
}

bool
AosJqlStmtActor::createActor(const AosRundataPtr &rdata)
{
	OmnString msg;
	AosXmlTagPtr doc;
	map<OmnString, OmnString> fieldMap;

	OmnString dbname = getCurDatabase(rdata);
	if (dbname == "") return false;

	//build insert value map:  fieldname->fieldvalue
	fieldMap[JIMOACTORS_NAME] = mName;
	fieldMap[JIMOACTORS_CLASSNAME] = mClassName;
	fieldMap[JIMOACTORS_CLASSPATH] = mPath;
	fieldMap[JIMOACTORS_TYPE_LANGUAGE] = mType_language;
	fieldMap[JIMOACTORS_DESCRIPTION] = mDescription;

	AosXmlTagPtr record = AosXmlParser::parse("<record></record>" AosMemoryCheckerArgs); 
	aos_assert_r(record, NULL);
		
	record->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	record->setAttr(AOSTAG_PUBLIC_DOC, "true");
	record->setAttr(AOSTAG_HPCONTAINER, SYSTABLE_JIMOACTORS); 
	record->setAttr(AOSTAG_OBJID, "");  

	map<OmnString, OmnString>::iterator itr = fieldMap.begin();
	while (itr != fieldMap.end())
	{
		record->setAttr(itr->first, itr->second);
		itr++;
	}

	//string container_objid = record->getAttrStr(AOSTAG_HPCONTAINER);
	string container_objid = record->getAttrStr(AOSTAG_HPCONTAINER);
	OmnString str_doc = record->toString();
	if ( !createDoc1(rdata, str_doc) )
	{
		
		msg << "Actor: " << mName << " created failed.";
		rdata->setJqlMsg(msg);
	}
	else
	{
		msg << "Actor: " << mName << " created successfully.";       
		rdata->setJqlMsg(msg);
	}

	return true;
}

bool
AosJqlStmtActor::runActor(const AosRundataPtr &rdata)
{
	/*
	<request>
		<actname>name</actname>
		<actparams>
			<methed>runActor</methed>
			<act_database>_sysdb</act_database>
			<act_table>t1</act_table>
		</actparams>
	</request>
		*/
	OmnString xmlMsg = "";
	int len;
	
	//get list values
	OmnString name;
	OmnString value;
	vector <OmnString> names;
	vector <OmnString> values;
	for (u32 i = 0; i < mPNames->size(); i++)
	{
		name = "", value = "";
		if (i < mPValues->size()) {
			value = ((*mPValues)[i])->getValue(rdata.getPtrNoLock());
		}
		name = ((*mPNames)[i])->getValue(rdata.getPtrNoLock());
	names.push_back(name);
	values.push_back(value);
	}
	//build the xml message
		xmlMsg << "<request> <actname>"
		   	   << mName
		   	   << "</actname>"
		   	   << "<actparams>";
	for (u32 i = 0; i < names.size(); i++)
	{
		xmlMsg << "<" << names[i] << ">";
		xmlMsg << values[i] << "</" << names[i] << ">";
	}
		xmlMsg <<"</actparams>"
		  	   << "</request>";

	len = xmlMsg.length();

	int sockfd = 0;
	//SERVER_PORT = 10001;
	struct sockaddr_in    servaddr;                           
	memset(&servaddr, 0x00, sizeof(struct sockaddr_in));

	 /* create socket */    
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("create socket error! errno = %d\n", errno);
		exit(0);
	}

	/* init server address */
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(10001);
	if( inet_pton(AF_INET, "192.168.99.83", &servaddr.sin_addr) == -1){
		printf("inet_pton error for %s\n", "192.168.99.83");
		exit(0);
	}

	/*connect */
	if( connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
		printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
		exit(0);
	}

	/*send xml message to java actor server*/
	const char *buff = xmlMsg.data();
	char   sendbuff[1024] = {0};
	sendbuff[3] = (char)(len &0xff);
	sendbuff[2] = (char)((len>>8) &0xff);
	sendbuff[1] = (char)((len>>16) &0xff);
	sendbuff[0] = (char)((len>>24) &0xff);

	sendbuff[4] = 0x00;
	sendbuff[5] = 0x00;
	sendbuff[6] = 0x00;
	sendbuff[7] = 0x01;
	
	for (int i=0; i<len; i++)
	{
		sendbuff[i+8] = buff[i];
	}

	if( send(sockfd, sendbuff, len+8, 0) == -1)
	{
		printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		exit(0);
	}                                     

	/*get response message from Java actor server*/
	char *recvbuff = new char[8];
	int recvlen = recv(sockfd, recvbuff, 8, 0);

	recvlen = (((int)(recvbuff[0]))&0xff)+
			(((int)(recvbuff[1]))&0xff)+
			(((int)(recvbuff[2]))&0xff)+
			(((int)(recvbuff[3]))&0xff);
	
	delete [] recvbuff;
	recvbuff = new char[recvlen+8];

	recvlen = recv(sockfd, recvbuff, recvlen+8, 0);
	printf("recv msg from client: %s\n", recvbuff);

	close(sockfd);

	//OmnString message(&recvbuff[0], recvlen);
	OmnString response(&recvbuff[0]);
	OmnString message = "";
	message <<  "<![CDATA[" << response << "]]>";
	//AosXmlTagPtr xmlResponse= AosXmlParser::parse(response AosMemoryCheckerArgs);
	//OmnString message = xmlResponse->toString();
	rdata->setJqlMsg(message);
	return true;
}

AosJqlStatement *
AosJqlStmtActor::clone()
{
	return OmnNew AosJqlStmtActor(*this);
}


void 
AosJqlStmtActor::dump()
{
}

