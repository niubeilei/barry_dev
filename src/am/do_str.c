#include <stdio.h>
#include <stdlib.h>

int str_segment(char *str, char *item[], char seg1)
{
    int i,j,count;
    i=j=count=0;
    
    while(/*str[j]!='\n' &&*/ str[j]!='\0')
    {    
	if(str[i]==' ' || str[i]=='\t' || str[i]=='\n' || str[i]==seg1 )  
	{
	    i++; j=i;
	}

	else
	{
	    j++;

	    if(str[j]==' ' || str[j]=='\t' || str[j]==seg1 || str[j]=='\n')
	    {
		
			item[count++] = &str[i];
			str[j]='\0';	
			j++; i=j;		
	    }

	     else if(str[j]=='\0' /*|| str[j]=='\n'*/) 
	     {
		 	item[count++] = &str[i];
		 	str[j]='\0';
	     }		
	 }
    }

    return count;
}


//int main()
//{//
//    char str1[]="app wang  8:30-18:50 1,2,5  14,25,23,31  * * *  allow";
//  char str2[]="app1  ytao  10:00-18:00  *  1,4,7   * * *  allow";
//    char str3[]="other  zql   *   *   *   * * *  deny";
//    char str4[]="fuser  zql   *   2,4,6  *   * * *  allow \n";
//    char strip[]="192.168.5.0/255.255.255.0";

 //   int count, k;
   // char *item[32];
//
  //  count=str_segment(str4, item, '/');
    //for(k=0; k<count; k++)
    //{
      // 	printf("%s\n",item[k]);
   // }
	
 //   return 0;

//}


