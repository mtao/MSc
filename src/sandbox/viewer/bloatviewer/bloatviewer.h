#ifndef BLOATVIEWER_H
#define BLOATVIEWER_H
#include "viewer/meshviewer/meshviewer.h"



class BloatViewerWidget: public MeshViewerWidget
{

    Q_OBJECT
    public:
        BloatViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);

    protected:
        void sendBuffers();

        public slots:
        void bloatChange(int);
    protected:
        float bloat;

};


#endif
