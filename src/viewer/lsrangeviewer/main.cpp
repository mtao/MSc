#include "window.h"

#include <QtGui>
#include <QGLWidget>

#include <iostream>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    LevelSetRangeViewer viewer(":lsrange/mc");
    //MeshViewer viewer("shader");
    viewer.show();
    if(argc>1)
    {
        std::cout << "Opening file: "<< argv[1] << std::endl;
        viewer.openFile(argv[1]);
    }


    return app.exec();
}
