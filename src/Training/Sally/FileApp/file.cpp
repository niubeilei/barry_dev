#include <stdio.h>


int main()
{
	FILE *fp;
	char c;
	if((fp=fopen("my.txt","rt+"))==NULL)
	{
		printf("cannot open file\n");
	}
	
	// input characters 

	printf("input a string and end with a space:");
    c=getchar();
	while(c!=' ')
	{
		fseek(fp,0,2);	
		fputc(c,fp);
		c=getchar();
	}
	rewind(fp);

	// print out all characters

	c=fgetc(fp);
	while(c!=EOF) 
	{	
		putchar(c);	
		c=fgetc(fp);	
	}
	printf("\n");

	// print out the characters from 20 to 30

	fseek(fp,20,0);
	char buffer[11];
	fread(buffer,1,10,fp);
	buffer[10]='\0';	
	fclose(fp);
	printf("print out the characters from 20 to 30:");
	for(int i=0;i<10;i++) printf("%c",buffer[i]);

	return 0;
}
