#include "lineviewer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

LineViewerWidget::LineViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): MeshViewerWidget(format,prefix,parent) {}


void LineViewerWidget::paintGL()
{
    if(!m_ready)
        return;

    setModelMatrix();
    m_shader->bind();
    RenderWidget::paintGL();




    glm::dmat3 model_invtrans = glm::transpose(glm::inverse(glm::dmat3(glm::make_mat4(m_modelMatrix.data()))));





    m_shader->setUniformValue( "modelMatrix_invTranpose", QMatrix4x4((const qreal *) glm::value_ptr(model_invtrans)));





    // Draw stuff
    glDrawElements( GL_LINES,       // Type of primitive to draw
            sizeof(IndexSet<3>)*m_obj->faces.size(), // The number of indices in our index buffer we wish to draw
            GL_UNSIGNED_INT,    // The element type of the index buffer
            0 );                // Offset from the start of our index buffer of where to begin
    m_shader->release();


}
