/*
 * zykie.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: root
 */

#include <vector>


#include "aosUtil/Types.h"
#include "alarm_c/alarm.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "TinyXml/TinyXml.h"
#include "Util/String.h"
#include "Util/Buff.h"
#include "SEClient/SEClient.h"



#include "zykie.h"

//bool OmnAppExecutionIsActive = true;

// global variable
//int 			gAosLogLevel = 0;
//extern int	 	gAosShowNetTraffic;
	u64			gTransID = 0;
static AosSEClient* 	gConnect = NULL;

static OmnString sgKeepString = "";
static OmnString sgPasswd = "12345";
static OmnString sgHostname = "192.168.2.235";
static OmnString sgUsername = "root";
static int sgRemotePort = 11503;
static OmnString sgStmt = "";   //if not empty, run one stat only
static OmnString sgFormat = "xml";
static OmnString sgOutFile = "";  //direct output to a file
static OmnString sgInFile = "";  //get statements from a file

OmnString       gCookieStr = "";

OmnString 		gSessionID = "";


// functions
u64 getTransID() { return gTransID++;}

static TiXmlElement* procRequest(const u32 siteid, const OmnString &appname,
		const OmnString &uname, const OmnString &req)
{
	OmnString resp, errmsg;
	AosBuffPtr buff = OmnNew AosBuff(req.length()+12 AosMemoryCheckerArgs);
	aos_assert_r(buff, NULL);
	buff->setU64(0);
	buff->setBuff(req.data(), req.length());
	OmnString sqlquery(buff->data(), buff->dataLen());
	gConnect->procRequest(siteid, appname, uname, sqlquery, resp, errmsg);

	const char* xmlstr=(const char*)resp.getBuffer();
	TiXmlDocument* doc=new TiXmlDocument;
	doc->Parse(xmlstr);
	//TiXmlElement* respXml = doc.GetDocument()->;
	//aos_assert_r(respXml, NULL);
	//cout << "Response: \n" << resp << endlID
	TiXmlElement* root=doc->RootElement();
	return root;
}


static bool loginDB()
{
	
	OmnString req;
	req	<< "<request>"
		<< "<item name='operation'><![CDATA[serverreq]]></item>"
		<< "<item name='zky_siteid'><![CDATA[100]]></item>"
		<< "<item name='transid'><![CDATA["<< getTransID() <<"]]></item>"
		<< "<item name='reqid'><![CDATA[jql_login]]></item>"
		<< "<item name='zkyurldocdid'><![CDATA[5230]]></item>"
		<< "<item name='loginobj'><![CDATA[true]]></item>"
		<< "<request>"
		<< "<username><![CDATA["<< sgUsername <<"]]></username>"
		<< "<pwd><![CDATA["<< sgPasswd <<"]]></pwd>"
		<< "<ctnr><![CDATA[zky_sysuser]]></ctnr>"
		<< "</request>"
		<< "</request>";
	
	OmnString resp;
	OmnString errmsg;
	if (!gConnect)
	{
		return false;
	}
	bool rslt = gConnect->procRequest(100, "", "", req, resp, errmsg);
	if (!rslt || resp == "")
	{
		return false;	
	}
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	if(!root)
	{
		return false;
	}
	AosXmlTagPtr contents = root->getFirstChild("Contents");
	if (!contents)
	{
		    return false;
	}
	AosXmlTagPtr loginobj = contents->getFirstChild("zky_lgnobj");
	if (!loginobj)
	{
		    return false;
	}
	gSessionID = loginobj->getAttrStr("zky_ssid", "");
	if (gSessionID == "")
	{
		    return false;
	}
	gCookieStr = "<zky_cookies>";
	gCookieStr << "<cookie zky_name=\"zky_ssid_5230\"><![CDATA[" << gSessionID << "]]></cookie></zky_cookies>";
	return true;
}

static bool runJql(ZYKIE* conn,const OmnString &jql)
{
	OmnString req;
	req << "<request>"
		<< "<item name='operation'><![CDATA[serverreq]]></item>"
		<< "<item name='zky_siteid'><![CDATA[100]]></item>"
		<< "<item name='transid'><![CDATA[" << getTransID() << "]]></item>"
		<< "<item name='reqid'><![CDATA[runsql]]></item>"
		<< "<item name='zkyurldocdid'><![CDATA[5230]]></item>"
		<< "<item name='loginobj'><![CDATA[true]]></item>"
		<< "<request>"
		<< "<content><![CDATA[" << jql << "]]></content>"
		<< "<contentformat>" << sgFormat << "</contentformat>"
		<< "</request>"
		<< gCookieStr
		<< "</request>";

	OmnString errmsg;                                                          
	OmnString resp;
	aos_assert_r(gConnect, false);
	aos_assert_r(gConnect->procRequest(100, "", "", req, resp, errmsg), false);
	aos_assert_r(resp != "", false);
	AosXmlParser parser;
	AosXmlTagPtr root = parser.parse(resp, "" AosMemoryCheckerArgs);
	aos_assert_r(root, false);

	AosXmlTagPtr status = root->getFirstChild("status");
	aos_assert_r(status, false);
	bool is_error = status->getAttrBool("error");
	if (is_error)
	{
		AosXmlTagPtr error = status->getFirstChild("error");
		OmnString err_str;
		OmnString error_str = "";
		AosXmlTagPtr child = error->getFirstChild();
		aos_assert_r(child, false);
		error_str << "**********ERROR**********\n"
			<< " ID   : " << error->getAttrStr("errmsg_id") << "\n"
			<< " File : " << error->getAttrStr("fname") << ":" << error->getAttrStr("line") << "\n"
			<< " MSG  : " << child->getNodeText() << "\n"
			<< "*************************";
		cout << error_str << "\n" << endl;
		return true;
	}

	AosXmlTagPtr messageNode = root->getFirstChild("message");
	if (messageNode)
	{
		OmnString message = messageNode->getNodeText();
		if (message != "") 
		{
			cout << message << "\n" << endl;
			return true;
		}
	}

	AosXmlTagPtr contents_tag = root->getFirstChild("content");
	if (contents_tag)
	{
		if (sgFormat == "xml")
		{
			conn->affected_rows = contents_tag->getAttrU64("num", 0); 
			int len;
//			aos_assert_r(!conn->mContentString, false);
			conn->mContentString = new char[contents_tag->toString().length()+1]; 
			strcpy(conn->mContentString,contents_tag->toString().data());
			cout << contents_tag->toString() << endl;
			return true;
		}
		OmnString contents = contents_tag->getNodeText();

		// Add by Young, covert XML CDATA
		contents.replace("0x0333333", "<![CDATA[", true);
		contents.replace("0x0444", "]]>", true);                                                                                  
		cout << "\n" << contents << "\n" << endl;
	}
	return false;	
}

/*
static bool logoutDB()
{
	OmnString requestInfo = "<request>";
	requestInfo << "<reqid>logout</reqid>"
		<< "<siteid>100</siteid>"
		<< "<transid>" << getTransID() << "</transid>"
		<< "<ssid>" << gSessionID << "</ssid>"
		<< "</request>";

	//TiXmlElement* respXml = procRequest(100, "", "", requestInfo);
	////aos_assert_r(respXml, false);
	//TiXmlElement* messageNode = respXml->FirstChildElement("message");
	////aos_assert_r(messageNode, false);
	//cout << "\n" << respXml->toString() << endl;

	return true;
}
*/

ZYKIE * zykie_init(ZYKIE *zykie)
{
	ZYKIE* conn;
	conn=(ZYKIE*)malloc(sizeof(ZYKIE));
	if(conn)
		return conn;
	return NULL;
}

my_bool zykie_connect(ZYKIE *zykie, const char *host,int port,
				      const char *user, const char *passwd)
{
	// login database
	sgHostname=host;
	sgUsername=user;
	sgPasswd=passwd;
	sgRemotePort=port;
	OmnString hostname(host);
	OmnString req, resp, err;
	gConnect = new AosSEClient(hostname, port);
	//aos_assert_r(gConnect, 1);
	if(loginDB())
		return true;
	return false;
}

my_bool zykie_query(ZYKIE *zykie,const char* req)
{
	OmnString jql(req);
	if(runJql(zykie,jql))
		return true;
	return false;
}

void zykie_close(ZYKIE *conn)
{
	free(conn);
}

void zykie_free_result(ZYKIE_RES *res)
{
	free(res);
}

ZYKIE_ROW zykie_fetch_row(ZYKIE_RES *res)
{
	if(res->current_row_idx<res->row_count)
	{
		ZYKIE_ROW row=res->row[res->current_row_idx];
		res->current_row_idx++;
		return row;
	}
	return NULL;
}

ZYKIE_RES * zykie_store_result(ZYKIE *conn)
{

	ZYKIE_RES* res;
	res=(ZYKIE_RES*)malloc(sizeof(ZYKIE_RES));
	
	AosXmlParser parser;
	
	OmnString str = OmnString(conn->mContentString, strlen(conn->mContentString));
	AosXmlTagPtr content = parser.parse(str, "" AosMemoryCheckerArgs);
	aos_assert_r(content, false);
//	AosXmlTagPtr content = root->getFirstChild("content");
	int rowcnt;
	res->row_count=content->getAttrU64("num", 0);
	rowcnt = res->row_count;
	OmnString cols(content->getAttrStr("fieldnames", ""));
	vector<OmnString> colvec;
	res->row=(ZYKIE_ROW*)malloc(sizeof(ZYKIE_ROW*)*rowcnt);
	int fieldcnt=1;
	int startidx=0;
	int findidx=-1;
	while((findidx=cols.find(',',false))!=-1)
	{
		fieldcnt++;
		startidx=findidx+1;
		colvec.push_back(cols.substr(0,findidx-1));
		cols=cols.substr(startidx,cols.length()-1);
	}
	res->field_count=fieldcnt;
	res->fields=(ZYKIE_FIELD**)malloc(sizeof(ZYKIE_FIELD*)*fieldcnt);
	for(int i=0;i<fieldcnt;i++)
	{
		ZYKIE_FIELD* field=(ZYKIE_FIELD*)malloc(sizeof(ZYKIE_FIELD));
		field->name=colvec[i].getBuffer();
		res->fields[i]=field;
	}

	AosXmlTagPtr row = content->getFirstChild("record");
	for(u64 i=0;i<res->row_count;i++)
	{
		res->row[i]=(ZYKIE_ROW)malloc(sizeof(ZYKIE_ROW)*fieldcnt);
		AosXmlTagPtr child = row->getFirstChild();
		for(int j=0;j<fieldcnt;j++)
		{
			AosXmlTagPtr coltag=child;
			OmnString colval(coltag->getNodeText());
			res->row[i][j]=(char*)malloc(sizeof(colval.length()+1));
			memset(res->row[i][j],0,colval.length()+1);
			memcpy(res->row[i][j],colval.getBuffer(),colval.length());
			*(res->row[i][j]+colval.length())='\0';
			child=row->getNextChild();
		}
		row = content->getNextChild();
	}
	res->current_row_idx=0;
	res->current_row=res->row[0];
	
	return res;
}

my_ulonglong zykie_num_rows(ZYKIE_RES *res)
{
	return res->row_count;
}

unsigned int zykie_num_fields(ZYKIE_RES *res)
{
	return res->field_count;
}

my_ulonglong zykie_affected_rows(ZYKIE *conn)
{
	return conn->affected_rows;
}

unsigned int zykie_errno(ZYKIE *conn)
{
	return 0;
}

const char * zykie_error(ZYKIE *conn)
{
	return "";
}
