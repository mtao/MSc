#include <iostream>
#include "triangle.h"



void TriangleWidget::initializeGL()
{
    RenderWidget::initializeGL();
    GLfloat points[] = {
        -0.5f,-0.5f,0.0f,1.0f,
        0.5f,-0.5f,0.0f,1.0f,
        0.0f,0.5f,0.0f,1.0f
    };
    GLuint indices[3] = {0, 1, 2};
    m_shader->bind();
    prepareBufferObject( m_vertexBuffer, QGLBuffer::StaticDraw, points, sizeof(points));
    prepareBufferObject( m_indexBuffer, QGLBuffer::StaticDraw, indices, sizeof(indices) );
    m_shader->release();
    



}
void TriangleWidget::updateBuffers()
{

}
void TriangleWidget::updateBuffers(QGLShaderProgram & program)
{

}

void TriangleWidget::paintGL()
{
    RenderWidget::paintGL();
    // Setup the modelview matrix
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate(45,1,1,0);

    QVector3D eyePosition( 0.0, 0.0, 5.0 );
    QVector3D targetPosition( 0.0, 0.0, 0.0 );
    QVector3D upDirection( 0.0, 1.0, 0.0 );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eyePosition, targetPosition, upDirection );
    m_shader->bind();
    m_shader->setUniformValue( "modelMatrix", m_modelMatrix );
    m_shader->setUniformValue( "viewMatrix", m_viewMatrix );
    m_vertexBuffer.bind();
    setAttributeObject( "vertex", GL_FLOAT, 0, 4 );
    m_indexBuffer.bind();
    // Draw stuff
    glDrawElements( GL_TRIANGLES,       // Type of primitive to draw
            3, // The number of indices in our index buffer we wish to draw
            GL_UNSIGNED_INT,    // The element type of the index buffer
            0 );                // Offset from the start of our index buffer of where to begin
    m_shader->release();
        
    

}
