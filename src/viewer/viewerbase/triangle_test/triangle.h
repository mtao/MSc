#include "viewer/viewerbase/viewer.h"

class TriangleWidget: public RenderWidget
{

    public:
        TriangleWidget(const QGLFormat & format, const QString & prefix, QWidget * parent=0): RenderWidget(format, prefix,parent),
        m_vertexBuffer( QGLBuffer::VertexBuffer ),
        m_indexBuffer( QGLBuffer::IndexBuffer )
    {}
    protected:
        void initializeGL();
        void paintGL();
        void updateBuffers();
        void updateBuffers(QGLShaderProgram & program);
    private:
        QGLBuffer m_vertexBuffer;
        QGLBuffer m_indexBuffer;



};

