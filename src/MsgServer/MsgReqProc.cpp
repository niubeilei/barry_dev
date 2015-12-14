////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// HtmlComplier  
//
// Modification History:
// 06/15/2011 Created by JozhiPeng
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/MsgReqProc.h"

#include "MsgServer/UserConnMgr.h"
#include "MsgServer/MsgRequestProc.h"

extern int gAosLogLevel;

AosMsgReqProc::AosMsgReqProc()
:
mLock(OmnNew OmnMutex()),
mIsStopping(false),
mCurrentId(0),
mEndId(0),
mSsid(""),
mOperation("")
{
	mRundata = OmnApp::getRundata();
	aos_assert(mRundata);
	mRundata = mRundata->clone(AosMemoryCheckerArgsBegin);
}


AosMsgReqProc::~AosMsgReqProc()
{
}


bool
AosMsgReqProc::config(const AosXmlTagPtr &config)
{
	return true;
}

bool
AosMsgReqProc::stop()
{
	OmnScreen << "AosMsgReqProc is stopping!" << endl;
	mIsStopping = false;
	return true;
}


bool			
AosMsgReqProc::procRequest(const OmnConnBuffPtr &buff)
{
OmnScreen << "================================jozhi" << endl;
	// It receives a request from a client.
	OmnApp::running();
	OmnTcpClientPtr conn = buff->getConn();
	aos_assert_r(conn, false);
	AosWebRequestPtr req = OmnNew AosWebRequest(conn, buff);
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "To process request: (transid: " << req->getTransId()
			<< "): \r\n" << req->getData() << endl;
	}
	if (mIsStopping)
	{
		return false;
	}
	char *data = req->getData();
	OmnString str;
	str << data;
	return procHttpReq(req, str);
}

bool
AosMsgReqProc::procHttpReq(
		const AosWebRequestPtr &req,
		const OmnString &httpreq)
{
	OmnString method;
	map<OmnString, OmnString> parms;
	bool rslt = parseHttpReq(method, parms, httpreq);
	if (!rslt || method != "GET")
	{
		return false;
	}
	
	// The format is:
	// 	<request operation="xxxx" cid="xxxx"/>
	OmnString reqStr = parms["xml"];
	OmnString callback = parms["callback"];
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(reqStr, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);
	if (!root)
	{
		mRundata->setError() << "Faild to parse the request";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		return false;
	}
	root->setAttr("callback", callback);

	OmnString cid = root->getAttrStr("cid", "");
	aos_assert_rr(cid != "", mRundata, false);
	mSsid = root->getAttrStr("ssid", "");
	if (mSsid == "")
	{
		mRundata->setError() << "Faild to retrieve session id";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		return false;
	}
	bool login_check = true;
	//login_check = AosSengAdmin::getSelf()->checkLogin(100, cid, mSsid);
	if (!login_check)
	{
		OmnString cnts;
		cnts << "<status type=\"exception\"/>";
		OmnTcpClientPtr conn = req->getClient();
		aos_assert_rr(conn, mRundata, false);
		AosMsgReqProc::sendClientResp(conn, callback, cnts);
		return true;
	}

	root->setAttr("zky_ssid", mSsid);

	mRundata->setSsid(mSsid);
	mRundata->setRequest(req);
	mRundata->setRequestRoot(root);

	mOperation = root->getAttrStr("operation", "");
	if (mOperation == "")
	{
		mRundata->setError() << "Missing request operation";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		return false;
	}

	AosMsgRequestProcPtr proc = AosMsgRequestProc::getProc(mOperation);
	aos_assert_r(proc, false);
	if (!proc)
	{
		mRundata->setError() << "Unrecognized operation: " << mOperation;
		OmnAlarm << mRundata->getErrmsg() << enderr;
		return false;
	}
	if (! proc->proc(root, mRundata))
	{
		mRundata->setError() << "Faild to proc the request";
		OmnAlarm << mRundata->getErrmsg() << enderr;
		return false;
	}
	return true;
}

AosNetReqProcPtr
AosMsgReqProc::clone()
{
	return OmnNew AosMsgReqProc();
}

bool
AosMsgReqProc::sendClientResp(
		const OmnTcpClientPtr &conn,
		const OmnString &callback,
		const OmnString &contents)
{
	OmnString resp;
	resp << "HTTP/1.1 200 ok\r\n";
	if (callback != "")
	{
		resp << "Content-Type: text/javascript\r\n";
	}
	else
	{
		resp << "Content-Type: application/x-json\r\n";
	}
	
	OmnString encoderesp;
	encoderesp << "<response>"
			   << contents
			   << "</response>";
	OmnString ctns;
	ctns << callback
		 << "({results:1,rows:[{"
		 << "id:1, data : '" 
		 << encodeUrl(encoderesp.data())
		 << "'}]});";

	resp << "Content-Length: " << ctns.length()
		 << "\r\n\r\n"
		 << ctns
		 << "\n\r";
	if (gAosLogLevel >= 2)
	{
		OmnScreen << "to proccess response: \r\n" << resp << endl;
	}

	aos_assert_r(conn, false);
	if (!conn || !conn->isConnGood())
	{
		return false;
	}
	bool rslt = conn->smartSend(resp.data(), resp.length());
	aos_assert_r(rslt, false);
	if (!rslt)
	{
		conn->closeConn();
		return false;
	}
	return true;
}

char
AosMsgReqProc::charToInt(char ch)
{
	if(ch>='0' && ch<='9')return (char)(ch-'0');
	if(ch>='a' && ch<='f')return (char)(ch-'a'+10);
	if(ch>='A' && ch<='F')return (char)(ch-'A'+10);
	return -1;
}

string
AosMsgReqProc::char2hex(char dec)
{
	char dig1 = (dec&0xF0)>>4;
	char dig2 = (dec&0x0F);
	if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
	if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
	if ( 0<= dig2 && dig2<= 9) dig2+=48;
	if (10<= dig2 && dig2<=15) dig2+=97-10;

	string r;
	r.append( &dig1, 1);
	r.append( &dig2, 1);
	return r;
}


string
AosMsgReqProc::encodeUrl(const char *c)  
{ 
	string escaped="";
    int max = strlen(c);
	for(int i=0; i<max; i++)
	{
		if ( (48 <= c[i] && c[i] <= 57) ||//0-9
		(65 <= c[i] && c[i] <= 90) ||//abc...xyz
		(97 <= c[i] && c[i] <= 122) || //ABC...XYZ
		(c[i]=='~' || c[i]=='!' || c[i]=='*' || 
		 c[i]=='(' || c[i]==')')// || 
		 //c[i]=='\''|| c[i]=='\"')
		)
		{
			escaped.append( &c[i], 1);
		}
		else
		{
			escaped.append("%");
			escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
		}
	}
	return escaped;
}  

char*
AosMsgReqProc::decodeUrl(const char* data)
{
	int i=0, j=0, len = strlen(data);
	char * newData = new char[len];
	while(i<len)
	{
		if(data[i] == '%')
		{
			char tempWord[2];
			char chn;
			tempWord[0] = charToInt(data[i+1]);
			tempWord[1] = charToInt(data[i+2]);
			chn = (tempWord[0] << 4) | tempWord[1];
			newData[j] = chn;
			j++;
			i += 3;
		}
		else if(data[i] == '+')
		{
			newData[j] = ' ';
			i++;
			j++;
		}
		else
		{
			newData[j] = data[i];
			i++;
			j++;
		}
	}
	newData[j] = '\0';
	return newData;
}

bool
AosMsgReqProc::parseHttpReq(
		OmnString &method,
		map<OmnString, OmnString> &parms,
		const OmnString &hhead)
{
	if (hhead.findSubString("GET", 0, false) == 0)
	{
		method = "GET";
		int qidx = hhead.indexOf("HTTP/1.1\r\n", 0);
		OmnString querystr = hhead.substr(6, qidx-2);
		aos_assert_r(querystr != "", false);
		if (querystr == "")
		{
			mRundata->setError() << "Not found the query string";
			OmnAlarm << mRundata->getErrmsg() << enderr;
			return false;
		}
		OmnStrParser1 parser(querystr, "&");
		OmnString parm;
		while((parm = parser.nextWord()) != "")
		{
			OmnStrParser1 subParser(parm, "=");
			OmnString name = subParser.nextWord();
			OmnString value = "";
			if (name == "xml")
			{
				if (querystr.indexOf("%", 0) == -1)
				{
					int idx = querystr.indexOf("&", 0);
					value = querystr.substr(4, idx -1);
					parms.insert(make_pair(name, value));
				}
				else
				{
					value = subParser.nextWord();
					parms.insert(make_pair(name, decodeUrl(value.data())));
				}
			}
			else
			{
				value = subParser.nextWord();
				parms.insert(make_pair(name, decodeUrl(value.data())));
			}
		}
	}
	else
	{
		method = "";
		OmnScreen << "Unrecognized request method, Ignored" << endl;
		return false;
	}
	int cookie_idx = hhead.findSubString("Cookie: ", 0);
	int ppidx = hhead.indexOf("\r\n\r\n", 0);
	if (cookie_idx>0 && ppidx>0)
	{
		OmnString cookie = hhead.substr(cookie_idx+8, ppidx-1);
		OmnStrParser1 parser(cookie, "; ");
		OmnString cookie_item;
		while((cookie_item = parser.nextWord()) != "")
		{
			OmnStrParser1 subParser(cookie_item, "=");
			OmnString name = subParser.nextWord();
			OmnString value = subParser.nextWord();
			if (name == "zky_ssid")
			{
				mSsid = value;
				break;
			}
		}
	}
	return true;
}

bool
AosMsgReqProc::conClosed(const OmnTcpClientPtr &conn)
{
	//1. user send request to offline
	return true;
}
