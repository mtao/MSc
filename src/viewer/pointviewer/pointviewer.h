
#ifndef LEVELSETVIEWER_H
#define LEVELSETVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/render.h"
#include "geometry/mesh/processing.h"
#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>
#include "viewer/meshviewer/meshviewer.h"



class PointViewerWidget: public ObjectViewerWidget
{

    Q_OBJECT
    public:
        typedef typename NumericalTraitsXf::Scalar Scalar;
        typedef typename NumericalTraitsXf::Vector3 Vector3;
        PointViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
    protected:
        QGLBuffer m_vertexBuffer;
        QColor m_color;
        std::vector<Vector3> m_points;
        void sendBuffers();
        void paintGL();
        public slots:
        void setModelColor();


};


#endif
