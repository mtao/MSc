#include "viewer/viewerbase/viewer.h"
#include "cube.h"
#include <QtGui/QApplication>
#include <iostream>


int main(int argc, char **argv)
{
    QApplication a( argc, argv );

    Viewer<Cube> w;
    w.show();
    //ViewerWindow window(&w);
    //window.show();

    return a.exec();

}
