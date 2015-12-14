#include <qapplication.h>
#include "MainWindow.h"

int main( int argc, char ** argv )
{
    QApplication a( argc, argv );
    MainWindow w;
    w.initTorTree();
    w.initWorkspace();
    w.showMaximized();
    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    return a.exec();
}
