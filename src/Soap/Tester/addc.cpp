#include <stdio.h>
#include <stdlib.h>
#include "soapStub.h"
#include "ns.nsmap"
 
int ns_procMsg(const char *server, char *appid, char *input, char **output);
 
//int ns_procMsg(const char *server, char * num1, char * num2, char **sum)
int ns_procMsg(const char *server, char *appid, char *input, char **output)
{
        struct soap SmsWBS_soap;
        int result = 0;
 
        soap_init(&SmsWBS_soap);
    	soap_set_mode(&SmsWBS_soap, SOAP_C_UTFSTRING);
		soap_set_namespaces(&SmsWBS_soap, namespaces);
 
        soap_call_ns__procMsg(&SmsWBS_soap, server, "", appid, input, output);
 
        if(SmsWBS_soap.error)
        {
                printf("soap error:%d, %s, %s ", SmsWBS_soap.error, *soap_faultcode(&SmsWBS_soap), *soap_faultstring(&SmsWBS_soap));
                result = SmsWBS_soap.error;
         }
 
        soap_end(&SmsWBS_soap);
        soap_done(&SmsWBS_soap);
 
        return result;
}
 
int main(int argc, char **argv)
{
        int result = -1;
        const char* server="http://192.168.99.91:10000";
 
        char * appid = "garea";
        char * input = "<Request><header><version>1.0</version><deviceId>2013E03B7065</deviceId><command>2</command></header><body><request><updateTime>0</updateTime><page>0</page></request></body></Request>";
        char * output;
 
        result = ns_procMsg(server, appid, input, &output);
		std::cout << output << std::endl;
		if (result != 0)
        {
        	printf("soap err, errcode = %d /n", result);
        }
        else
        {
            printf("%s : %s : %s /n", appid, input, output);
        }
 
        return 0;
}
//g++ soapC.cpp soapClient.cpp addc.cpp -o addclient -lgsoap++
