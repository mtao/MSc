
#ifndef MeshViewer2DVIEWER_H
#define MeshViewer2DVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io2.h"
#include "geometry/mesh/processing2.h"
#include <QSpinBox>



class MeshViewerWidget: public ObjectViewer2dWidget
{

    Q_OBJECT
    public:
        typedef typename NumericalTraitsXf::Scalar Scalar;
        typedef typename NumericalTraitsXf::Vector2 Vector2;
        typedef typename NumericalTraitsXf::Vector3 Vector3;
        MeshViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
    protected:
        Mesh2f * m_obj;
        QGLBuffer m_vertexBuffer;
        QGLBuffer m_indexBuffer;
        QColor m_color;
        void sendBuffers();
        void paintGL();
        public slots:
        void setModelColor();


};


#endif
