
#ifndef LEVELSETVIEWER_H
#define LEVELSETVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/render.h"
#include "geometry/mesh/processing.h"
#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>



class TextureViewerWidget: public ObjectViewerWidget
{

    Q_OBJECT
public:
    typedef typename NumericalTraitsXf::Scalar Scalar;
    typedef typename NumericalTraitsXf::Vector3 Vector3;
    TextureViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
    void initializeGL();
protected:
    std::shared_ptr<VertexBufferObject> m_vertexBuffer;
    QColor m_color;
    std::vector<Vector3> m_points;
    void updateBuffers();
    void paintGL();
public slots:
    void setModelColor();
    void updateMVP()
    {
        ObjectViewerWidget::updateMVP();
        RenderWidget::updateMVP(*m_shader);
    }
    void updateMMvMvp()
    {
        ObjectViewerWidget::updateMMvMvp();
        RenderWidget::updateMMvMvp(*m_shader);
    }
    void updateVMvMvp()
    {
        ObjectViewerWidget::updateVMvMvp();
        RenderWidget::updateVMvMvp(*m_shader);
    }
    void updatePMvp()
    {
        ObjectViewerWidget::updatePMvp();
        RenderWidget::updatePMvp(*m_shader);
    }
protected:
    GLuint m_testTex;
    GLuint m_testTex2;


};


#endif
