#include "viewershell.h"//TODO
#include <QColorDialog>
#include <iostream>
#include "geometry/marchingcubes/marchingcubes.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <QDebug>

ViewerShellWidget::ViewerShellWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): ObjectViewerWidget(format,prefix,parent),

    m_color(255,255,0)

{
    m_transientShader = false;
    QResource::registerResource("viewershell.qrc");//TODO
}


void ViewerShellWidget::initializeGL()
{
    ObjectViewerWidget::initializeGL();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    updateBuffers();



}








void ViewerShellWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    //prepareBufferObject(m_vertexBuffer, QGLBuffer::StaticDraw, m_points.data(), sizeof(Vector3)*m_points.size());
    updateBuffers();

}
void ViewerShellWidget::updateBuffers()
{
    ObjectViewerWidget::updateBuffers();
    //updateBuffers(m_shader);
}
void ViewerShellWidget::updateMVP()
{
    ObjectViewerWidget::updateMVP();
    //RenderWidget::updateMVP(m_shader);
}
void ViewerShellWidget::updateMMvMvp()
{
    ObjectViewerWidget::updateMMvMvp();
    //RenderWidget::updateMMvMvp(m_shader);

}
void ViewerShellWidget::updateVMvMvp()
{
    ObjectViewerWidget::updateVMvMvp();
    //RenderWidget::updateVMvMvp(m_shader);

}
void ViewerShellWidget::updatePMvp()
{
    ObjectViewerWidget::updatePMvp();
    //RenderWidget::updatePMvp(m_shader);

}
void ViewerShellWidget::updateBuffers(ShaderProgram & program)
{
    
    m_ready = true;
}
void ViewerShellWidget::paintGL()
{

    setModelMatrix();





    ObjectViewerWidget::paintGL();

}
void ViewerShellWidget::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_color);
    if (color.isValid()) 
    {
        m_color = color;
        update();
    }
}
void ViewerShellWidget::changeScale(int scale)

{
    m_scale = scale/10.0f;
}
void ViewerShellWidget::backfaceCulling(bool enabled)
{
    if(enabled)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}
