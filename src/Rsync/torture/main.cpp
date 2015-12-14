#include "torture.h"

using namespace std;
int main()
{
	AOSTorture *tor;
	tor = new AOSTorture();
	tor->generateSQLCase();
	return 0;
}
