#ifndef CUBE_H
#define CUBE_H

#include "viewer/viewerbase/widget.h"

class Cube: public RenderWidget
{
    public:
        Cube(const QGLFormat & format, const QString & prefix, QWidget * parent=0): RenderWidget(format, prefix,parent),
        m_vertexBuffer( QGLBuffer::VertexBuffer ),
        m_normalBuffer( QGLBuffer::VertexBuffer ),
        m_indexBuffer( QGLBuffer::IndexBuffer )
    {}


    protected:
        void initializeGL();
        void paintGL();
    private:
        QGLBuffer m_vertexBuffer;
        QGLBuffer m_normalBuffer;
        QGLBuffer m_indexBuffer;
        int indexSize;

};

#endif // CUBE_H
