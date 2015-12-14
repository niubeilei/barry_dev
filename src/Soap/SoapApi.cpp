#include "SoapServer/SoapServer.h"
#include "Soap/soapH.h"
#include "Soap/ns.nsmap" 
#include "Util/String.h" 
#include <iostream>
using namespace std;

static const char* resp = "<respone>Invalid appid</response>";

int ns__procMsg(struct soap *soap, xsd__string appid, xsd__string input, xsd__string *output)
{
	// This function get the request from client side.
	// The input should be this format:
	//	<Request>
	//  	<header>
	//  		<version>1.0</version>
	//  		<deviceId>{2013E03B7065,  这个是变量}</deviceId>
	//  		<command>1</command>
	//    </header>
	//    <body>
	//       【具体的参数字符串，这个字符串的内容根据Command不同而不同，所以其解析也依赖于Command】
	//    </body>
	// </Request>
	//
	if (appid == NULL)
	{
		//*output = strdup((char *)resp); 
		*output = (char*)soap_malloc(soap, strlen(resp)+1);
		strcpy(*output, resp);
		return SOAP_OK;
	}
	OmnString id = OmnString(appid);
	OmnString in = OmnString(input);
	OmnString response; 
	AosSoapServer::getSelf()->procMsg(id, in, response);
	// *output = strdup(resp.data());
	*output = (char*)soap_malloc(soap, strlen(response.data())+1);
	strcpy(*output, response.data());
	// cout << "********************************************************" << endl;
	// cout << "soapresp: " << *output << endl;
	// cout << "********************************************************" << endl;
	return SOAP_OK;
}
