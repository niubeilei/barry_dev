 #include "HDFS/hdfs.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <iostream>
#include <signal.h>
 
 int main(int argc, char **argv) {

const char* env = getenv("CLASSPATH");
std::cout << "classpath : " << env << std::endl;

     //hdfsFS fs = hdfsConnectAsUser("192.168.99.206", 9002,"hadoop");//default是连本地文件                                                                                                                
	 signal(SIGPIPE, SIG_IGN);
     hdfsFS fs = hdfsConnect("192.168.99.206", 9002);//default是连本地文件                                                                                                                
     if (!fs) {
         fprintf(stderr, "Oops! Failed to connect to hdfs!\n");
         exit(-1);
     } 
  
//   const char* rfile = "/usr/root/config.json";
     const char* rfile = argv[1];
     tSize fileTotalSize = strtoul(argv[2], NULL, 10);
     tSize bufferSize = strtoul(argv[3], NULL, 10);
	 tOffset offset = strtoul(argv[4], NULL, 10); 
    
     hdfsFile readFile = hdfsOpenFile(fs, rfile, O_RDONLY, bufferSize, 0, 0);
     if (!readFile) {
         fprintf(stderr, "Failed to open %s for writing!\n", rfile);
         exit(-2);
     }
 
     // data to be written to the file
     char* buffer = (char * )malloc(sizeof(char) * bufferSize);
     if(buffer == NULL) {
         return -2;
     }
     
	 int num = hdfsPread(fs, readFile, 0, (void*)buffer, bufferSize);
	printf("num : %d\n" , num);


     // read from the file
     tSize curSize = bufferSize;
     for (; curSize == bufferSize;) {
         curSize = hdfsPread(fs, readFile, offset, (void*)buffer, curSize);
		 offset += curSize;
		 printf("%s", buffer);
     }
	int len = hdfsAvailable(fs, readFile);
	printf("len : %d" , len);

	printf("\n");
 
     free(buffer);
     hdfsCloseFile(fs, readFile);
     hdfsDisconnect(fs);
 
     return 0; 

 }
 
//g++ main.cpp -I$HADOOP_HDFS_HOME/include /home/jackie/HADOOP/hadoop-2.6.0/lib/native/libhdfs.a -lpthread -lhdfs -ljvm  -o above_sample -L/home/jackie/HADOOP/hadoop-2.6.0/lib/native -L/usr/lib/jvm/java-6-openjdk/jre/lib/amd64/server
