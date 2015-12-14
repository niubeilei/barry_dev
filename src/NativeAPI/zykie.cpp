/*
 * zykie.cpp
 *
 *  Created on: Dec 1, 2014
 *      Author: root
 */

#include <vector>


#include "aosUtil/Types.h"
#include "alarm_c/alarm.h"
#include "TinyXml/TinyXml.h"
#include "Util/String.h"
#include "Util/Buff.h"

#include "NativeAPI/SEClient.h"



#include "NativeAPI/zykie.h"

bool OmnAppExecutionIsActive = true;

// global variable
int 			gAosLogLevel = 0;
bool 			gAosShowNetTraffic = false;
u64				gTransID = 0;
static AosSEClient* 	gConnect = NULL;

static OmnString sgKeepString = "";
static OmnString sgPasswd = "12345";
static OmnString sgHostname = "192.168.99.180";
static OmnString sgUsername = "root";
static int sgRemotePort = 1035;
static OmnString sgStmt = "";   //if not empty, run one stat only
static OmnString sgFormat = "xml";
static OmnString sgOutFile = "";  //direct output to a file
static OmnString sgInFile = "";  //get statements from a file


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
	//cout << "Response: \n" << resp << endl;
	TiXmlElement* root=doc->RootElement();
	return root;
}


static bool loginDB()
{
	OmnString requestInfo = "<request>";
	requestInfo << "<reqid>login</reqid>"
			 << "<siteid>100</siteid>"
			 << "<transid>" << getTransID() << "</transid>"
			 << "<user>" << sgUsername << "</user>"
			 << "<passwd>" << sgPasswd << "</passwd>"
			 << "<domain>zky_sysuser</domain>"
			 << "</request>";

	TiXmlElement* respXml = procRequest(100, "", "", requestInfo);
	//aos_assert_r(respXml, false);
	TiXmlElement* contentNode = respXml->FirstChildElement("content");
	//aos_assert_r(contentNode, false);
	TiXmlElement* ssidNode = contentNode->FirstChildElement("session_id");
	//aos_assert_r(ssidNode, false);
	gSessionID=ssidNode->GetText();
	aos_assert_r(gSessionID != "", false);

	TiXmlElement* errorNode = respXml->FirstChildElement("error");
	//aos_assert_r(errorNode, false);
	if (!errorNode)
	{
		cout << "No status node:\n" << respXml << endl;
		return false;
	}

	//OmnString error = errorNode->getNodeText();
	//if (error == "true") return false;

	return true;
}

static bool runJql(ZYKIE* conn,const OmnString &jql)
{
//cout << "\n" << jql << "\n--------------------------------" << endl;
	OmnString jqlStr = "<request>";
	jqlStr << "<reqid>runjql</reqid>"
		   << "<siteid>100</siteid>"
		   << "<transid>" << getTransID() << "</transid>"
		   << "<objdef>"
		   << "</objdef>"
		   << "<ssid>" << gSessionID << "</ssid>"
		   << "<content><![CDATA[" << jql << "]]></content>"
		   << "<contentformat>" << sgFormat << "</contentformat>"
		 << "</request>";

	TiXmlElement* respXml = procRequest(100, "", "", jqlStr);
	//aos_assert_r(respXml, false);

	TiXmlElement* updatecnt=respXml->FirstChildElement("updatecount");
	conn->affected_rows=atoi(updatecnt->GetText());

	// check status
	TiXmlElement* errorNode = respXml->FirstChildElement("error");
	//aos_assert_r(errorNode, false);
	if (!errorNode)
	{
		cout << "No error node:\n" << respXml << endl;
		return false;
	}
	/*
	OmnString error = errorNode->getNodeText();
	if (error == "true")
	{
		TiXmlElement* messageNode = respXml->FirstChildElement("message");
		if (messageNode)
		{
			OmnString message = messageNode->getNodeText();
			cout << "\nWARNING: run JQL error (" << message << ")" << endl;
		}
		return false;
	}
*/
	// print data or message
	TiXmlElement* contentNode = respXml->FirstChildElement("content");
	if (sgFormat == "xml" && contentNode)
	{
		//cout << contentNode->toString() << endl;
		//gcontentNode=contentNode;
		conn->mcontentNode=(TiXmlElement*)contentNode;
		return true;
	}

	if (contentNode)
	{
		OmnString content = contentNode->GetText();
		if (content != "")
		{
			cout << content << endl;
			return true;
		}
	}


	TiXmlElement* messageNode = respXml->FirstChildElement("message");
	if (messageNode)
	{
		OmnString message = messageNode->GetText();
		if (message != "")
		{
			cout << message << "\n" << endl;
			return true;
		}
		else
		{
			TiXmlElement* errors = respXml->FirstChildElement("errors");
			if (errors)
			{
				TiXmlElement* error = errors->FirstChildElement("error");
				while(error)
				{
					OmnString error_str = "";
					TiXmlElement* child = error->FirstChildElement("details");
					//aos_assert_r(child, false);
					error_str << "**********ERROR**********\n"
							  << " ID   : " << error->Attribute("errmsg_id") << "\n"
						      << " File : " << error->Attribute("fname") << ":" << error->Attribute("line") << "\n"
						      << " MSG  : " << child->GetText() << "\n"
							  << "*************************";

					cout << error_str << "\n" << endl;
					error = errors->NextSibling();
				}
			}
			return true;
		}
	}
	//cout << "syntax error\n" << endl;

	return false;
}


static bool logoutDB()
{
	OmnString requestInfo = "<request>";
	requestInfo << "<reqid>logout</reqid>"
			 << "<siteid>100</siteid>"
			 << "<transid>" << getTransID() << "</transid>"
			 << "<ssid>" << gSessionID << "</ssid>"
			 << "</request>";

	TiXmlElement* respXml = procRequest(100, "", "", requestInfo);
	////aos_assert_r(respXml, false);
	//TiXmlElement* messageNode = respXml->FirstChildElement("message");
	////aos_assert_r(messageNode, false);
//cout << "\n" << respXml->toString() << endl;

	return true;
}


ZYKIE * zykie_init(ZYKIE *zykie)
{
	ZYKIE* conn;
	conn=(ZYKIE*)malloc(sizeof(ZYKIE));
	if(conn)
		return conn;
	return NULL;
}

my_bool zykie_connect(ZYKIE *zykie, const char *host,
				      const char *user, const char *passwd)
{
	// login database
	OmnString req, resp, err;
	gConnect = new AosSEClient(sgHostname, sgRemotePort);
	//aos_assert_r(gConnect, 1);
	if(loginDB())
		return true;
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
	TiXmlElement* content=(TiXmlElement*)conn->mcontentNode;
	int rowcnt=res->row_count=atoi(content->Attribute("num"));
	OmnString cols(content->Attribute("fieldnames"));
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
	TiXmlElement* row=content->IterateChildren(NULL);
	for(int i=0;i<res->row_count;i++)
	{
		res->row[i]=(ZYKIE_ROW)malloc(sizeof(ZYKIE_ROW)*fieldcnt);
		TiXmlElement* child=row->IterateChildren(NULL);
		for(int j=0;j<fieldcnt;j++)
		{
			TiXmlElement* coltag=child;
			OmnString colval(coltag->GetText());
			res->row[i][j]=(char*)malloc(sizeof(colval.length()+1));
			memset(res->row[i][j],0,colval.length()+1);
			memcpy(res->row[i][j],colval.getBuffer(),colval.length());
			*(res->row[i][j]+colval.length())='\0';
			child=row->IterateChildren(child);
		}
		row=content->IterateChildren(row);
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

}
