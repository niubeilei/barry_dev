#include<iostream.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int  main(int argc, char **argv)
{
	char FileName[256]="cli.txt";
	char RsFileName[256]="resultofapi.txt";
	int flag=0; 
	int testcasecnt=0; 
	int index=1;
	int count=1;
	int Interval=0;
	int MaxRunTime=1;
	char SectionName[128]="";
	int Read_All_Module(char *FileName);
	int Read_Run_Cli( char *FileName, char *SectionName,char * rsfile,int flag,int &tcnt,int Interval=0);
	char *Trim(char *p);
	while(index<argc)
	{	
		if(strcmp(argv[index],"-h")==0)
		{
			cout<<"============================================ HELPS ===================================================================="<<endl;
			cout<<"Usage   :   ./runcli [-f [filename]] [-m [module]] [-t [maxruncount]] [-i [interval]] [-s [resultfilename]]"<<endl;
			cout<<"            (all the parameter is  optional,the following is the paramrter's explation)"<<endl;
			cout<<""<<endl;
			cout<<"--------------------------------------- Parameter Explations ---------------------------------------- "<<endl;
			cout<<"     -f :   special the cli file. by default ,it will use the cli.txt file which under the current directory"<<endl;
			cout<<"     -t :   control this application total run count. by default, it's value is 1"<<endl;
			cout<<"     -i :   control the interval between run two cli. by default, it's value is 0(no interval)"<<endl;
			cout<<"     -m :   Special which module's clis will run. if unset it ,it will run all module's clis"<<endl;					
			cout<<"     -s :   Special which file will store the run result. if unset it,the return ressult will not store "<<endl;					
			cout<<"Get Help:   ./runcli.exe -h"<<endl;
			cout<<""<<endl;
			cout<<"--------------------------------- Examples -----------------------------------------------------------"<<endl;
			cout<<"./runcli                                    \n\t (all moudule's clis in cli.txt  will run 1 time) "<<endl;
			cout<<"./runcli  -f filename2 -t 10                 \n\t(all moudule's clis in filename2 will run 10 time) "<<endl;
			cout<<"./runcli  -f filename2 -m mac                \n\t(all mac's clis in filename2 will run 1 time) "<<endl;
			cout<<"./runcli  -f filename2 -m dnsmasq -t 10      \n\t(all dnsmasq's clis in filename2 will run 10 time) "<<endl;
			cout<<"./runcli  -f filename2 -m dnsmasq -t 10 -i 1 \n\t(all dnsmasq's clis in filename2 will run 10 time, every 1 second run a cli) "<<endl;
			cout<<"./runcli  -f filename2 -m dnsmasq -t 10 -i 1 -s rsfile111\n\t(all dnsmasq's clis in filename2 will run 10 time, every 1 second run a cli,and the result will be stored into the \n\t file named rsfile111) "<<endl;
			cout<<"======================================================================================================================"<<endl;
			exit(0);
		}
		if(strcmp(argv[index],"-m")==0)
		{
			strcpy(SectionName,argv[++index]);
			index++;
			continue;
		}
		if(strcmp(argv[index],"-f")==0)
		{
			strcpy(FileName,argv[++index]);
			index++;
			continue;
		}
		if(strcmp(argv[index],"-t")==0)
		{
			MaxRunTime=atoi(argv[++index]);
			index++;
			continue;
		}
		if(strcmp(argv[index],"-i")==0)
		{
			Interval=atoi(argv[++index]);
			index++;
			continue;
		}
		if(strcmp(argv[index],"-s")==0)
		{
			strcpy(RsFileName,argv[++index]);
			flag=1;
			index++;
			continue;
		}
		if(strcmp(argv[index],"-ms")==0)
		{
		  Read_All_Module(FileName);
			  exit(0);
		}
	}

	while(count<=MaxRunTime)
	{
		cout<<"====================================== now run the " <<count<<" time======================================"<<endl;
		Read_Run_Cli(FileName, SectionName,RsFileName,flag,testcasecnt,Interval);
		count++;
	}
	printf("###############################################################################\n");
	printf("################## The total cli run counts of this time is: %d ###############\n",testcasecnt);
	printf("###############################################################################\n");
	return 0;
}

char *Trim(char *p)
{
	char *q = p;
	char *r = NULL;
	q=p+strlen(p);
	while ((*q == ' ' || *q == '\t') && *q != '\n')q--;
	q= p;
	while ((*q == ' ' || *q == '\t') && *q != '\n')q++;
	return q;
}

int strapp(char *des,const char *begain,const char *middle,const char *pre_end,const char *end, int n)
{
	char buf[n];
	strcpy(buf,begain);
	strcat(buf,middle);
	strcat(buf,pre_end);
	strcat(buf,end);
	strcpy(des,buf); 
	return 0;
}

int record(char *filename, char*result)
{
	FILE *fp=NULL;
	if(!(fp=fopen(filename,"a")))
	{
		printf("record result failed \n");
		return -1;
	}
	fputs(result,fp);
	fclose(fp);
	return 0;
}

int Read_Run_Cli( char *FileName, char *SectionName,char * rsfile, int flag,int &tcnt,int Interval=0)
{
	FILE *fp=NULL;
	char key[256]="";
	char cmdtmp[256]="";
	char tmp[256]="";
	char tmp1[256]="";

	strcpy(key, "[");
	strcat(key, SectionName);
	strcat(key, "]");	
	if(!(fp=fopen(FileName,"r")))
	{
		cout<<"open file failed, please make sure this cli file is exist and it can be read! "<<endl;
		cout<<"==============================================================================="<<endl;
		exit(0);
	}
	while (!(feof(fp)))
	{
		if ((fgets(cmdtmp, sizeof(cmdtmp), fp)) != NULL)
		{			
			if(strcmp(SectionName,"")!=0) 
			{		
				if (strncmp(Trim(cmdtmp), key, strlen(key)) == 0)  
				{
					cout<<Trim(cmdtmp);
					if(flag)
					{
						record(rsfile,Trim(cmdtmp));
					}
					while (!feof(fp))		    		 		    			
					{
						if (fgets(cmdtmp, sizeof(cmdtmp), fp) != NULL)
						{
							if (*Trim(cmdtmp) == '[')
							{
								break;
							}
							if (*Trim(cmdtmp) == '#')					
							{
								cout<<cmdtmp;
								if(flag)
								{
									if(-1==record(rsfile,cmdtmp))
									{
										cout<<"record comment failed "<<endl;
									}
								}
								continue;
							}		 		    			    	 
							else					
							{
								cout<<Trim(cmdtmp);
								strncpy(tmp,Trim(cmdtmp),strlen(Trim(cmdtmp))-1);
								strcpy(tmp1,tmp);
								if(strlen(tmp1)>1)
								{
									strapp(tmp,tmp," > runapi_tmp_result ","\0","",sizeof(tmp));			
									system(tmp);
									system("more runapi_tmp_result");
									system("echo =================================================================================================\n");
									if(flag)
									{
										strapp(cmdtmp,"THE COMMAND IS: ",tmp1,"","\n",sizeof(cmdtmp));
										record(rsfile,cmdtmp);
										strapp(cmdtmp,"","echo THE RESULT  IS:"," >> ",rsfile, sizeof(cmdtmp));
										system(cmdtmp);
										strapp(cmdtmp,"more runapi_tmp_result >> ","","",rsfile, sizeof(cmdtmp));
										system(cmdtmp);
										strapp(cmdtmp,"echo ","================================================================================================================="," >> ",rsfile,sizeof(cmdtmp));
										system(cmdtmp);
									}
									tcnt++;
									memset(tmp,'\0',sizeof(tmp));
									memset(cmdtmp,'\0',sizeof(cmdtmp));
									sleep(Interval);
									system("rm -f runapi_tmp_result");
								}
							}	    			    			    			    	 						
						}
					}		    		    		    		   		    		   
				}			    	  	    	
			}
			else 
			{
				if (*Trim(cmdtmp) == '[')
				{
					cout<<Trim(cmdtmp);
					if(flag)
					{
						record(rsfile,Trim(cmdtmp));
					}
					continue;    	  
				}	
				if (*Trim(cmdtmp) == '#')
				{
					cout<<cmdtmp;
					if(flag)
					{
						record(rsfile,cmdtmp);
					}
					continue;    	  
				}		   				   		
				else 
				{
					cout<<Trim(cmdtmp);
					strncpy(tmp,Trim(cmdtmp),strlen(Trim(cmdtmp))-1);
					strcpy(tmp1,tmp);
					if(strlen(tmp)>1)
					{
						strapp(tmp,tmp," > runapi_tmp_result ","\0","",sizeof(tmp));			
						system(tmp);
						system("more runapi_tmp_result");
						system("echo =================================================================================================\n");
						if(flag)
						{
							strapp(cmdtmp,"THE COMMAND IS: ",tmp1,"","\n",sizeof(cmdtmp));
							record(rsfile,cmdtmp);
							strapp(cmdtmp,"","echo THE RESULT  IS:"," >> ",rsfile, sizeof(cmdtmp));
							system(cmdtmp);
							strapp(cmdtmp,"more runapi_tmp_result >> ","","",rsfile, sizeof(cmdtmp));
							system(cmdtmp);
							strapp(cmdtmp,"echo ","================================================================================================================="," >> ",rsfile,sizeof(cmdtmp));
							system(cmdtmp);
						}
						tcnt++;
						memset(tmp,'\0',sizeof(tmp));
						memset(cmdtmp,'\0',sizeof(cmdtmp));
						sleep(Interval);
						system("rm -f runapi_tmp_result");
					}
				}		   				   		
			}		   
		}
	}
	fclose(fp);
	return 1;
}

int Read_All_Module(char *FileName )
{
	FILE *fp=NULL;
	char cmdtmp[256]="";
	char *p;
	int i=1;
	if(!(fp=fopen(FileName,"r")))
	{
		printf("open script file failed, please make sure this cli file is exist and it can be read!\n ");
		return -1;
	}
	printf("---------------------------------------------------------------------------------------------------\n");
	printf("The script file include the following modules:\n");
	printf("---------------------------------------------\n");
	while (!(feof(fp)))
	{
		if ((fgets(cmdtmp, sizeof(cmdtmp), fp)) != NULL)
		{			
				if (strncmp(Trim(cmdtmp), "[",1) == 0)  
				{
					p=Trim(cmdtmp);
					p++;
					printf("%d: ",i++);
					while(*p!=']')
					{
						printf("%c",*p++);
					}
					printf("\n");
				}		   				   		
		}
	}
	fclose(fp);
	printf("---------------------------------------------\n");
	return 0;
}



