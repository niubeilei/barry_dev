/*generated by gencode.py
 *author: bill xia
 modifiy:Ricky Chen
 */

#include <stdio.h>
#include "systemMisc.h"
#include <iostream.h>

int main(int argc,char ** argv)
{
    char result[10240];
    int ret = 0;
    int len = 10240;
    int total_case = 0;
    int fail_case = 0;
    int index=1;
    int count=0;
    int maxruntime=2;
    FILE *fp=NULL; 
    
 while(index<argc)
	{
		if(strcmp(argv[index],"-t")==0)
		{
			maxruntime=atoi(argv[++index]);
			index++;
			continue;
		}
		if(strcmp(argv[index],"-h")==0)
		{
			cout<<"----------------------helps for usage------------------------------------------"<<endl;
			cout<<"1 usage: ./systemMisctester.exe [-t [count]]"<<endl;
			cout<<""<<endl;
			cout<<"2 exmaple: './systemMisctester.exe -t 10 ' or ' ./systemMisctester.exe '"<<endl;
			cout<<""<<endl;
			cout<<"3 the paramete [-t [count]] is optinal,it's default value is 2 ,it need to set a number of 2's "<<endl;
			cout<<"  times(eg: 2,4,6,...)  "<<endl;
				cout<<""<<endl;
			cout<<"4 the result will be added to the files systemMiscresult1 and systemMiscresult2 in the current directory at the mean time,"<<endl;
			cout<<"  and the contents in systemMiscresult1 and systemMiscresult2 should be the same "<<endl;
			cout<<""<<endl;
			exit(0);
		}
	}



while(count<maxruntime)
	{
		printf("================================ Now run the %d time=========================================\n",count+1);
		printf("==============================================================================================\n");
	
		if(count%2==0)
		{
			fp=fopen("systemresult1","a+");
		}
		else
		{
			fp=fopen("systemresult2","a+");
		} 

cout<<"==============================systemMisc set section ==================================================\n"<<endl;
   ret = aos_system_set_hostname("hac.com");
    if (ret != 0) {
        printf("caseId:1	ret = %d	Error\n", ret);
        fail_case++;
    }
    total_case++;
    cout<<"set hostname   ret= "<<ret<<endl;
		fprintf(fp,"set hostname   ret=%d\n", ret);
    cout<<"-----------------------------------------------------------------------------------------------"<<endl;

    
    len = 10240;
    ret = aos_retrieve_system_info(result, &len);
    if (ret != 0) {
        printf("caseId:7	ret = %d	Error\n", ret);
        fail_case++;
    }
    total_case++;
    cout<<"system info   ret= "<<ret<<"\n"<<result<<endl;
		fprintf(fp,"system info ret=%d\n%s\n", ret,result);
    cout<<"-----------------------------------------------------------------------------------------------"<<endl;
    
    cout<<"the application run total counts:"<<count<<endl;
    printf("total case: %d\nfail  case: %d\n", total_case, fail_case);
    return 0;
 	}
}