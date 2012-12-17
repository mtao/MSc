#ifndef BLOATVIEWER_H
#define BLOATVIEWER_H
#include "viewer/meshviewer/meshviewer.h"



class LineViewerWidget: public MeshViewerWidget
{

    Q_OBJECT
    public:
        LineViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
    protected:
        void paintGL();


};


#endif
