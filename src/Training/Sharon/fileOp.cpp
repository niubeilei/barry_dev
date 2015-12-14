#include<iostream.h> 
#include<fstream.h> 



int main()
{
  	ofstream File("sharon.txt");
 	File<<"Hello,world!This program is used for file operations, such as open, close, read, write and select.";
	File.close();
	ifstream OpenFile("sharon.txt");
	char ch;
	while(!OpenFile.eof())
    {
		OpenFile.get(ch);
		cout<<ch;
	}
	OpenFile.close();
    fstream editFile("sharon.txt");
	editFile.seekg(12,ios::beg);
	static char str[10];
	editFile>>str;
	cout<<str<<endl;
	editFile.seekg(-8,ios::end);
    editFile>>str;
	cout<<str<<endl;
	editFile.close();
	return 0;
}

