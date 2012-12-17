#include "viewer/viewerbase/viewer.h"
#include "triangle.h"
#include <QtGui/QApplication>
#include <iostream>


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Viewer<TriangleWidget> w;
    w.show();
    return app.exec();

}
