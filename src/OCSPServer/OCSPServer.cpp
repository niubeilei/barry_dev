////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: OCSPServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "OCSPServer/OCSPServer.h"

#include "ASN1/AsnCodec.h"
#include "CliClient/CliClient.h"
#include "CliClient/ModuleOprId.h"
#include "ASN1/AsnDefs.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TcpAppServer/TcpAppServer.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include <netdb.h>

OmnSingletonImpl(AosOCSPServerSingleton,
				 AosOCSPServer,
				 AosOCSPServerSelf,
				 "AosOCSPServer");


AosOCSPServer::CRL_FILE_RECORD		AosOCSPServer::mCrlFileRecords[AosOCSPServer::eAosOCSPSvr_MaxCrlFileNum];
int					AosOCSPServer::mCrlFileNum = 0;
OmnString 			AosOCSPServer::mCRLFilePos;

AosOCSPServer::AosOCSPServer()
:
mStartFlag(true),
mRespFlag(true),
mRespType(eAosOCSPSvr_EasyMode)
{

	mServer = new AosTcpAppServer();
	OmnTcpMsgReaderPtr thisPtr(this, false);
	mServer->setReader(thisPtr);
	
	for(int i=0;i < eAosOCSPSvr_MaxCrlFileNum;i++)
	{
		mCrlFileRecords[i].no = 0;
	}
	mCrlFileNum = 0;
}


AosOCSPServer::~AosOCSPServer()
{
}


OmnString
AosOCSPServer::getTcpMsgReaderName() const
{
	return "OCSPServer";
}

int			
AosOCSPServer::nextMsg(const OmnConnBuffPtr &buff,
					    const OmnTcpClientPtr &conn)
{

	// If we get a whole msg , call procMsg()
	char *startPos = buff->getBuffer();
	unsigned int  dataLen = buff->getDataLength();

	if(dataLen < 1)
	{
		// The data is less than a length indicator 		return 0;
		return 0;
	}

	// get length of length

	unsigned int lenOfLen = (unsigned int)(*startPos);
	if(lenOfLen > eAosOCSPSvr_MaxMsgLength)
	{
		// The length indicator is too large.

		return 0;
	}


	if(dataLen < lenOfLen +1)
	{
		// The data is less than a length indicator 		return 0;

		return 0;
	}

	unsigned int msgLen = 0;
	// get length
	unsigned char tmp = 0;

	for(unsigned int i = 0;i < lenOfLen; i++)
	{
		msgLen = msgLen << 8;
		tmp = *(startPos + 1 + i);
		msgLen += (unsigned int )tmp;
	}


	if(1+lenOfLen + msgLen <= dataLen )	
	{

		procMsg(startPos+1+lenOfLen,msgLen,conn);

		return msgLen;
	}
	return 0;
}

bool
AosOCSPServer::parseReq(OCSP_MSG &ocspMsg,char *msg,const unsigned int msgLen)
{
	unsigned int cursor = 0;
	int ret = 0;
	
	unsigned int 	id = 0;
	unsigned char 	theClass = 0;
	unsigned char 	isPrimitive = 0;
	unsigned int 	length = 0;
	int 			intValue = 0;
	char 			strValue[eAosOCSPSvr_MaxStrLength];
	unsigned int	strLen = 0;
	unsigned int	numOfBits = 0;
		
	// get msgAll tag
	ret = aosAsn1Codec_decodeTagId(&id,&theClass,&isPrimitive,msg,msgLen,&cursor);
	if(ret < 0 || 
	   !eAosAsn_TagId_Sequence == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   !isPrimitive)
	{
		return false;
	}
	
	// get msgAll length
	ret = aosAsn1Codec_getLength(msg,&cursor,&length);
	if(ret < 0 || length <= 0)
	{
		return false;
	}
	// now we do not check length
	
	// get msgHeader tag
	ret = aosAsn1Codec_decodeTagId(&id,&theClass,&isPrimitive,msg,msgLen,&cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_Sequence == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   !isPrimitive)
	{
		return false;
	}

	// get msgHeader length
	ret = aosAsn1Codec_getLength(msg,&cursor,&length);
	if(ret < 0 || length <= 0)
	{
		return false;
	}
	// now we do not check length
	
	// get pvno tag
	ret = aosAsn1Codec_integer_decode(msg,msgLen,&cursor,&intValue);
	if(ret < 0)
	{
		return false;
	}
	ocspMsg.pvno = intValue;
	
	// get nonce	
	ret = aosAsn1Codec_string_decode(&id,
									 &theClass,
									 &isPrimitive,
									 strValue,
									 eAosOCSPSvr_MaxStrLength, 
									 &strLen,
									 msg,
									 msgLen,
									 &cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_OctetString == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   isPrimitive)
	{
		return false;
	}
	ocspMsg.nonce = OmnString(strValue,strLen);

	// get OCSQS_Name	
	ret = aosAsn1Codec_string_decode(&id,
									 &theClass,
									 &isPrimitive,
									 strValue,
									 eAosOCSPSvr_MaxStrLength, 
									 &strLen,
									 msg,
									 msgLen,
									 &cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_OctetString == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   isPrimitive)
	{
		return false;
	}
	ocspMsg.OCSQS_Name = OmnString(strValue,strLen);

	// get TransID
	ret = aosAsn1Codec_string_decode(&id,
									 &theClass,
									 &isPrimitive,
									 strValue,
									 eAosOCSPSvr_MaxStrLength, 
									 &strLen,
									 msg,
									 msgLen,
									 &cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_OctetString == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   isPrimitive)
	{
		return false;
	}
	ocspMsg.transID = OmnString(strValue,strLen);

	// get time
	ret = aosAsn1Codec_string_decode(&id,
									 &theClass,
									 &isPrimitive,
									 strValue,
									 eAosOCSPSvr_MaxStrLength, 
									 &strLen,
									 msg,
									 msgLen,
									 &cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_Time_Generalized == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   isPrimitive)
	{
		return false;
	}
	ocspMsg.time = OmnString(strValue,strLen);

	// get msgBody tag
	ret = aosAsn1Codec_decodeTagId(&id,&theClass,&isPrimitive,msg,msgLen,&cursor);
	if(ret < 0 || 
	   !2 == id || 
	   !eAsnTypeClass_Context == theClass ||
	   isPrimitive)
	{
		return false;
	}
	// get msgBody length
	ret = aosAsn1Codec_getLength(msg,&cursor,&length);
	if(ret < 0 || length <= 0)
	{
		return false;
	}
	// now we do not check length

	// get jnsReq tag
	ret = aosAsn1Codec_decodeTagId(&id,&theClass,&isPrimitive,msg,msgLen,&cursor);
	if(ret < 0 || 
	   !22 == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   !isPrimitive)
	{
		return false;
	}
	// get jnsReq length
	ret = aosAsn1Codec_getLength(msg,&cursor,&length);
	if(ret < 0 || length <= 0)
	{
		return false;
	}
	// now we do not check length

	// get status
	ret = aosAsn1Codec_integer_decode(msg,msgLen,&cursor,&intValue);
	if(ret < 0)
	{
		return false;
	}
	ocspMsg.status = intValue;
	

	ret = aosAsn1Codec_bitstr_decode(&id,
									 &theClass,
									 &isPrimitive,
									 strValue,
									 eAosOCSPSvr_MaxStrLength, 
									 &strLen,
									 &numOfBits,
									 msg,
									 msgLen,
									 &cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_BitString== id || 
	   !eAsnTypeClass_Universal == theClass ||
	   isPrimitive)
	{
		return false;
	}
	ocspMsg.nonce2 = OmnString(strValue,strLen);
	ocspMsg.nonce2BitNum = numOfBits;
	
	// get SN
	ret = aosAsn1Codec_string_decode(&id,
									 &theClass,
									 &isPrimitive,
									 strValue,
									 eAosOCSPSvr_MaxStrLength, 
									 &strLen,
									 msg,
									 msgLen,
									 &cursor);
	if(ret < 0 || ! 
	   eAosAsn_TagId_OctetString == id || 
	   !eAsnTypeClass_Universal == theClass ||
	   isPrimitive)
	{
		return false;
	}
	ocspMsg.SN = OmnString(strValue,strLen);

	return true;
}

bool
AosOCSPServer::checkPermission(OCSP_MSG &ocspMsg)
{
	OmnString *msgFound = 0;
	int index;
cout << __FILE__ << __LINE__ << ":SN =" << ocspMsg.SN << endl;
	bool ret = aosCharPtree_get(mPermTree, ocspMsg.SN.data(), &index, (void **)&msgFound);
	if (ret || !msgFound)
	{

		return false;
	}


	// Found it
	if(index + 1 == ocspMsg.SN.length())
	{

		return true;
	}
	else // longer or shorter than key
	{

		return false;
	}

	
	return false;
}

bool
AosOCSPServer::addPermission(OmnString  *sn)
{
	// insert into ptree

cout << "add permission :" << *sn << endl;
 	if(!sn)
	{
		return false;
	}
	
	if (aosCharPtree_insert(mPermTree, sn->data(), 
			sn->length(), (void *)sn, 1))
	{
		OmnAlarm << "Failed to add sn" << enderr;
		return false;
	}

	return true;
}

bool
AosOCSPServer::procMsg(char* msg,const int msgLen,const OmnTcpClientPtr &conn)
{
	// 
	// check the response flag
	//
	if(!mRespFlag)
	{
		return true;
	}
	
	bool permission = false;

	OCSP_MSG ocspMsg;
	if(!parseReq(ocspMsg,msg,msgLen))
	{

		// Error format.
		return false;
	}
	

	//
	// check the permission
	//
	permission = checkPermission(ocspMsg);
	

	OmnString strOcspResp;
	// create a response
	createRespMsg(ocspMsg,permission,strOcspResp);
	// memcpy orig msg

	// 2 send back the response
	if(!sendResp(strOcspResp,conn))
	{
		OmnWarn << "send response failed: " << strOcspResp << enderr;
		return false;
	}
	
	return true;
}

/*bool		
AosOCSPServer::getCmd(const OmnConnBuffPtr &buff , OmnString &cmd)
{
	int msgLen = buff->getDataLength();
	char* msgHeader = buff->getBuffer();
	char *msgTail =  msgHeader + msgLen;
	while (msgLen > 0 && (*msgTail == '\r' || *msgTail == '\n'))
	{
		msgLen --;
		msgTail --;
	}
	cmd = OmnString(msgHeader, msgLen);
	return true;

}
*/
bool		
AosOCSPServer::sendResp(OmnString &rslt,const OmnTcpClientPtr &conn)
{
	if(mServer)
	{
		rslt << "\n\n";
		mServer->writeTo(rslt,conn);
	}
	return true;
}


bool
AosOCSPServer::config(const OmnXmlParserPtr &conf)
{
	mPermTree = aosCharPtree_create();

	OmnXmlItemPtr def = conf->tryItem(OmnXmlItemName::eOCSPServerConfig);
	if (!def)
	{
		return false;
	}
	
	//
	//	Get OCSP data configuration
	//
	OmnXmlItemPtr OCSP_data = def->tryItem(OmnXmlItemName::eOCSPData);
	if (OCSP_data)
	{
		OmnString sn;
		OmnString *theSn;
		OmnXmlItemPtr Sns = OCSP_data->tryItem(OmnXmlItemName::eSNs);
		if(!Sns)
		{

			return false;
		}

		while (Sns->hasMore())
		{

			OmnXmlItemPtr xmlSN = Sns->next();
			sn = xmlSN->getStr();
			if(sn == "")
			{

				continue;
			}
			theSn = new OmnString(sn);


			if(!addPermission(theSn))
			{
				return false;
			}
		}
		
	}

	//
	//	Get CRL configuration
	//
	OmnXmlItemPtr CRL_data = def->tryItem(OmnXmlItemName::eCRLData);
	if (CRL_data)
	{
		int	no;
		OmnString fileName;
		
		while (CRL_data->hasMore())
		{
			OmnXmlItemPtr crlFile = CRL_data->next();
			no = crlFile->getInt(OmnXmlItemName::eNo, -1);
			if(-1 == no)
			{
				continue;
			}
		
			fileName = crlFile->getStr(OmnXmlItemName::eFileName, "");
			if(fileName == "")
			{
				continue;
			}

			if(!addCRLFile(no,fileName))
			{
				return false;
			}
		}
		mCRLFilePos = CRL_data->getStr(OmnXmlItemName::eCRLFilePos,"");
		if(mCRLFilePos == "")
		{
			return false;
		}
	}

	
	OmnXmlItemPtr OCSP_tcp_server = def->tryItem(OmnXmlItemName::eOCSPTcpServer);
	if(OCSP_tcp_server.isNull())
	{
		return false;
	}

	OmnIpAddr localIP = OCSP_tcp_server->getIpAddr(OmnXmlItemName::eLocalAddr,OmnIpAddr::eInvalidIpAddr);
	if(OmnIpAddr::eInvalidIpAddr == localIP)
	{
		// 
		// localIP not set
		//
    	char hostname[256] = "OCSPSERVER_ADDR";
		//gethostname(hostname,256);
		hostent * host = gethostbyname(hostname);
		if(host)
		{
			unsigned long iplong;
			memcpy(&iplong,*(host->h_addr_list),4);
			localIP = iplong;
		}
		else
		{
			// can not get local ip
			cout << "can not get local IP" << endl;
			return false;
		}
	}
	
	if(mServer)
	{
		return mServer->config(OCSP_tcp_server,localIP);
	}

	cout << "\nNo TcpAppServer\n" << endl;
	return false;
}

bool
AosOCSPServer::addCRLFile(const int no,const OmnString &fileName)
{
	if(eAosOCSPSvr_MaxCrlFileNum -1 <= mCrlFileNum)
	{
		return false;
	}
	mCrlFileRecords[mCrlFileNum].no = no;
	mCrlFileRecords[mCrlFileNum].fileName = fileName;
	mCrlFileNum ++;
	return true;	
}


bool
AosOCSPServer::start()
{
	if(mServer)
	{
		return mServer->start();
	}
	cout << "\nNo TcpAppServer\n" << endl;

	return false;
	
}

bool
AosOCSPServer::stop()
{
	return true;
}



bool			
AosOCSPServer::setCliClient(const AosCliClientPtr cliClient)
{
	mCliClient = cliClient;
	return true;
}


int 
AosOCSPServer::changeCrlList(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	*optlen = 0;
	int no = parms->mIntegers[0];
	if(no <= 0)
	{
		return 1;
	}
	OmnString crlResourceFile;
	for(int i = 0;i < mCrlFileNum;i++)
	{
		if(mCrlFileRecords[i].no == no)
		{
			crlResourceFile = mCrlFileRecords[i].fileName;
			break;
		}
	}
	if(crlResourceFile == "")
	{
		return 1;
	}
	
	if(mCRLFilePos == "")
	{
		return 1;
	}
	
	
	
	OmnString rslt;
	OmnString cmd("rm ");
	cmd << mCRLFilePos;
	int stat = system(cmd);
	if(0 == WIFEXITED(stat))
	{
		rslt = "command abnormal";
		return false;
	}
	if(WIFSTOPPED(stat))
	{
		rslt = "process stopped";
		return false;
	}
	
	cmd = "cp ";
	cmd << crlResourceFile << " " << mCRLFilePos;

	stat = system(cmd);
	if(0 == WIFEXITED(stat))
	{
		rslt = "command abnormal";
		return false;
	}
	if(WIFSTOPPED(stat))
	{
		rslt = "process stopped";
		return false;
	}
	rslt = "command excuted";
	return 0;
}

int 
AosOCSPServer::startModule(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosOCSPServerSelf->setStartFlag(true);
	AosOCSPServerSelf->setRespFlag(true);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "OCSP Module on";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}


int 
AosOCSPServer::stopModule(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosOCSPServerSelf->setStartFlag(false);
	AosOCSPServerSelf->setRespFlag(true);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "OCSP Module off";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}


int 
AosOCSPServer::responseOn(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosOCSPServerSelf->setRespFlag(true);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "OCSP response on";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}

int 
AosOCSPServer::responseOff(char *data, 
			 unsigned int *optlen, 
			 struct aosKernelApiParms *parms, 
			 char *errmsg, 
			 const int errlen)
{
	//
	// real operation
	//
	*optlen = 0;
	AosOCSPServerSelf->setRespFlag(false);

	//
	// set response
	//
	OmnString resp;
	if(*optlen > 50)
	{
		resp = "OCSP response off";
	}
	else
	{
		resp = "lerr";
	}
	memcpy(data,resp.data(),resp.length());
	*optlen = resp.length();
	return 0;
}

void		
AosOCSPServer::setStartFlag(const bool flag)
{
	mStartFlag = flag;
	if(mServer)
	{
		mServer->setConnFlag(flag);
	}
	return;
}

void		
AosOCSPServer::setRespFlag(const bool flag)
{
	mRespFlag = flag;
	return;
}

bool				
AosOCSPServer::registerCliCallbackFunc()
{
	if(!mCliClient)
	{
		return false;
	}
	mCliClient->addCliFunc(startModule,AosModuleOprId::eOCSPStartModule);
	mCliClient->addCliFunc(stopModule,AosModuleOprId::eOCSPStopModule);
	mCliClient->addCliFunc(responseOn,AosModuleOprId::eOCSPResponseOn);
	mCliClient->addCliFunc(responseOff,AosModuleOprId::eOCSPResponseOff);
	mCliClient->addCliFunc(changeCrlList,AosModuleOprId::eCRLChangeList);
	return true;
}


bool
AosOCSPServer::createRespMsg(const OCSP_MSG ocspMsg,const bool permission,OmnString &strOcspResp)
{

	if(eAosOCSPSvr_EasyMode == mRespType)
	{

		if(permission)
		{

			strOcspResp = "allow";
		}
		else
		{

			strOcspResp = "forbid";
		}
		
		return true;
	}
	
	return false;
/*	strOcspResp.assign(aosOcspReq_encode,'\0');
	char *buffer = strOcspResp.getBuffer();
	unsigned int buflen = strOcspResp.length();
	unsigned int cursor = 0;
	int ret = 0;
	unsigned int ocspRespStart, tbsRespStart;

	// 
	// Encode the identifier (should be 00010000)
	//
	ret = aosAsn1Codec_setid(16, eAsnTypeClass_Universal, 1, buffer, buflen, &cursor);
	aosKernelAssert(ret == 0, ret);

	// 
	// We do not know the length of the structure yet. We will reserve one byte
	// for the length. If the result length is less than 128, everything will
	// be just fine. Otherwise, we will have to make room for the length. 
	// This is something stupid about ASN.1. It did not consider the encoding
	// efficiency.
	//
	ocspRespStart = cursor;
	cursor++;

	// 
	// Encode TBSRequest ::= SEQUENCE {...}
	//
	ret = aosAsn1Codec_setid(16, eAsnTypeClass_Universal, 1, buffer, buflen, &cursor);
	aosKernelAssert(ret == 0, ret);
	tbsRespStart = cursor;
	cursor++;

	// 
	// Since version is default to v1 and v1 is the only allowed version, DER 
	// requires version shall not be encoded into the string. 

	// 
	// Encode requestorName:
	//	requestName [1] EXPLICIT GeneralName OPTIONAL
	// Note that this is an optional component. It should not be encoded unless
	// it is set.
	//
	if (req->mReqName[0] > 0)
	{
		ret = aosOcspReq_encodeReqName(req, buffer, buflen, &cursor);
		aosKernelAssert(ret == 0, ret);
	}

	// 
	// Encode requests:
	//	requestList SEQUENCE Of Request
	// 
	ret = aosOcspReq_encodeRequests(req, buffer, buflen, &cursor);
	aosKernelAssert(ret == 0, ret);

	// 
	// Encode the extensions. 
	// In the current implementation, no extensions are allowed.
	//

	// 
	// Encode the signature:
	//	optionalSignature [0] EXPLICIT Signature OPTIONAL
	// The signature is present if and only if its algorithm ID is valid.
	//
	if (req->mSignatureBitLen > 0)
	{
		ret = aosOcspReq_encodeSignature(req, buffer, buflen, &cursor);
		aosKernelAssert(ret == 0, ret);
	}

	// 
	// Check the length
	//
	       
	ret |= aosAsn1Codec_adjustLength(buffer, buflen, &cursor, tbsRespStart, cursor - tbsRespStart + 1);
	ret |= aosAsn1Codec_adjustLength(buffer, buflen, &cursor, ocspRespStart, cursor - ocspRespStart + 1);

*/	
	
	
	
}

			 

