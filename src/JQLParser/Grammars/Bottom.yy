%%
//*****************************************************************************************
/*
int 
main(int argc, char** argv)
{
    cout << "> ";
  
    extern FILE *yyin;
    yy::SQLParse sqlParser;  // create a SQL Parser

    for(int i=1; i<argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
      	    sqlParser.set_debug_level(1);
        }
        if (strcmp(argv[i], "-f") == 0)
        {
      	    yyin = fopen(argv[i+1], "r");
        }
    }
  
  	char *str = new char[100];
  	yy_scan_bytes(str, 10);

    int v = sqlParser.parse();  // and run it

    cout << "Leave main v : " << v << endl;
    return v;
}
*/

// Bison not create this function, so i create it
namespace yy {
    void
    Parser::error(location const &loc, const std::string& s)
    {
      std::cerr << "error at " << loc << ": " << s << std::endl;
    };

}

