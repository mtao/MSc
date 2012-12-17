#include "meshviewer.h"
#include <QColorDialog>
#include <QResource>

MeshViewerWidget::MeshViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): ObjectViewer2dWidget(format,prefix,parent), m_obj(0),
    m_vertexBuffer(QGLBuffer::VertexBuffer),
    m_indexBuffer(QGLBuffer::IndexBuffer),
    m_color(255,255,0)

{
    QResource::registerResource("meshviewer.qrc");
}
void MeshViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    if(m_obj){delete m_obj;}
    m_obj = new Mesh2f();
    Mesh2IOUtils::OBJParser<NumericalTraitsXf> parser(m_obj);
    parser.read(filename.toStdString());
    MeshUtils::normalize(*m_obj);
    std::cout << m_obj->vertices.size() << std::endl;
    std::cout << m_obj->faces.size() << std::endl;
    sendBuffers();

}
void MeshViewerWidget::sendBuffers()
{
    qWarning() << "Binding Buffers";
    if(!m_obj)
        return;
    m_shader->bind();
    if(prepareBufferObject(m_vertexBuffer, QGLBuffer::StaticDraw, m_obj->vertices.data(), sizeof(Vector2)*m_obj->vertices.size()))
        setAttributeObject("vertex", GL_FLOAT, 0, 2);
    else
        return;
    if(!prepareBufferObject(m_indexBuffer, QGLBuffer::StaticDraw, m_obj->faces.data(), sizeof(IndexSet<2>)*m_obj->faces.size()))
        return;
    m_shader->setUniformValue( "u_color", m_color );
    m_shader->release();
    ObjectViewer2dWidget::sendBuffers();

    m_ready = true;
}
void MeshViewerWidget::paintGL()
{
    if(!m_ready)
        return;

    setModelMatrix();
    ObjectViewer2dWidget::paintGL();
    m_shader->bind();

    m_vertexBuffer.bind();
    setAttributeObject(*m_shader,"vertex",GL_FLOAT,0,2);
//    glDrawArrays(GL_POINTS, 0, m_obj->vertices.size());
    m_indexBuffer.bind();

    glDrawElements( GL_LINES,       // Type of primitive to draw
            sizeof(IndexSet<2>)*m_obj->faces.size(), // The number of indices in our index buffer we wish to draw
            GL_UNSIGNED_INT,    // The element type of the index buffer
            0 );                // Offset from the start of our index buffer of where to begin



    // Draw stuff

    m_shader->release();


}
void MeshViewerWidget::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_color);
    if (color.isValid()) 
    {
        m_color = color;
        m_shader->bind();
        m_shader->setUniformValue( "u_color", m_color );
        m_shader->release();
        update();
    }
}
