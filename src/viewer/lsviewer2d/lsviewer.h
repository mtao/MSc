
#ifndef LEVELSETVIEWER_H
#define LEVELSETVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io2.h"
#include "geometry/mesh/processing2.h"
#include "geometry/surface_reconstruction/marchingsquares.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>
#include "viewer/meshviewer2d/meshviewer.h"



class LevelSetViewerWidget: public MeshViewerWidget
{

    Q_OBJECT
    public:
        LevelSetViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
    protected:
        Gridf * m_grid;


};


#endif
