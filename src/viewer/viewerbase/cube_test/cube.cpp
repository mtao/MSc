#include "cube.h"


void Cube::initializeGL()
{
    // Vertices
    //
    // 4 floats per vertex
    // 4 vertices per face
    // 6 faces
    const int vertexDataCount = 6 * 4 * 4;

    const float vertexData[vertexDataCount] = {
        // Left face
        -0.5f, -0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,

        // Top face
        -0.5f, 0.5f, -0.5f, 1.0f,
        -0.5f, 0.5f,  0.5f, 1.0f,
        0.5f, 0.5f,  0.5f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f,

        // Right face
        0.5f,  0.5f, -0.5f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f,

        // Bottom face
        0.5f, -0.5f, -0.5f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,

        // Front face
        0.5f, -0.5f, 0.5f, 1.0f,
        0.5f,  0.5f, 0.5f, 1.0f,
        -0.5f,  0.5f, 0.5f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f,

        // Back face
        0.5f,  0.5f, -0.5f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f, 1.0f
    };


    // Normal vectors
    const int normalDataCount = 6 * 4 * 3;

    const float normalData[normalDataCount] = {
        // Left face
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,

        // Top face
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // Right face
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        // Bottom face
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,

        // Front face
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        // Back face
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f
    };



    // Indices
    //
    // 3 indices per triangle
    // 2 triangles per face
    // 6 faces
    const int indexDataCount = 6 * 3 * 2;
    indexSize = indexDataCount;

    const unsigned int indexData[indexDataCount] = {
        0,  1,  2,  0,  2,  3,  // Left face
        4,  5,  6,  4,  6,  7,  // Top face
        8,  9,  10, 8,  10, 11, // Right face
        12, 14, 15, 12, 13, 14, // Bottom face
        16, 17, 18, 16, 18, 19, // Front face
        20, 22, 23, 20, 21, 22  // Back face
    };



    //bufferName, buffer type, data, size
    //attribute name, glenum type, offsetsize, stride
    RenderWidget::initializeGL();
    // Prepare the vertex, normal and index buffers
    m_shader->bind();
    if ( prepareBufferObject( m_vertexBuffer, QGLBuffer::StaticDraw, vertexData, sizeof(vertexData)) )
    {
        // Enable the "vertex" attribute to bind it to our vertex buffer
        setAttributeObject( "vertex", GL_FLOAT, 0, 4 );
    }
    else
        return;
    if ( prepareBufferObject( m_normalBuffer, QGLBuffer::StaticDraw, normalData, sizeof(normalData) ) )
    {
        // Enable the "normal" attribute to bind it to our texture coords buffer
        setAttributeObject( "normal", GL_FLOAT, 0, 3 );
    }
    else
        return;
    if ( !prepareBufferObject( m_indexBuffer, QGLBuffer::StaticDraw, indexData, sizeof(indexData) ) )
        return;
    m_shader->release();
}

void Cube::paintGL()
{
    RenderWidget::paintGL();
    m_shader->bind();
    // Setup the modelview matrix
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate(45,1,1,0);

    QVector3D eyePosition( 0.0, 0.0, 5.0 );
    QVector3D targetPosition( 0.0, 0.0, 0.0 );
    QVector3D upDirection( 0.0, 1.0, 0.0 );
    m_viewMatrix.setToIdentity();
    m_viewMatrix.lookAt( eyePosition, targetPosition, upDirection );
    m_shader->setUniformValue( "modelMatrix", m_modelMatrix );
    m_shader->setUniformValue( "viewMatrix", m_viewMatrix );
    // Draw stuff
    glDrawElements( GL_TRIANGLES,       // Type of primitive to draw
            indexSize, // The number of indices in our index buffer we wish to draw
            GL_UNSIGNED_INT,    // The element type of the index buffer
            0 );                // Offset from the start of our index buffer of where to begin

    m_shader->release();
}
