#include "pointviewer.h"
#include <QColorDialog>
#include <QResource>

PointViewerWidget::PointViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): ObjectViewer2dWidget(format,prefix,parent),

    m_vertexBuffer(QGLBuffer::VertexBuffer),
    m_color(255,255,0)

{
    QResource::registerResource("pointviewer.qrc");
}
void PointViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    std::ifstream in(filename.toStdString().c_str());
    if(!in.is_open())return;
    //temporary variables
    std::string line;
    std::string s;
    Vector3 x;
    Face f;

    //expect lines to only be up to this long
    line.reserve(64);

    while( in.good() )
    {
        getline(in,line);
        if(line.length()>0)
        {
            boost::trim(line);
            std::stringstream ss(line);
            std::string type;
            ss >> type;

            if(type.compare("v") == 0)
            {
                //vertex
                ss >> x(0) >> x(1) >> x(2);
                if(!ss.eof())
                {
                    Scalar w;
                    ss >> w;
                    x/=w;
                }
                m_points.push_back(Vector2(x(0),x(1)));
            }

        }

    }
    MeshUtils::normalize(m_points);
    sendBuffers();

}
void PointViewerWidget::sendBuffers()
{
    qWarning() << "Binding Buffers";
    m_shader->bind();
    if(prepareBufferObject(m_vertexBuffer, QGLBuffer::StaticDraw, m_points.data(), sizeof(Vector2)*m_points.size()))
        setAttributeObject("vertex", GL_FLOAT, 0, 2);
    else
        return;
    m_shader->setUniformValue( "u_color", m_color );
    m_shader->release();
    ObjectViewer2dWidget::sendBuffers();

    m_ready = true;
}
void PointViewerWidget::paintGL()
{
    if(!m_ready)
        return;

    setModelMatrix();
    ObjectViewer2dWidget::paintGL();
    m_shader->bind();

    m_vertexBuffer.bind();
    setAttributeObject(*m_shader,"vertex",GL_FLOAT,0,3);

    glDrawArrays(GL_POINTS, 0, m_points.size());



    // Draw stuff

    m_shader->release();


}
void PointViewerWidget::setModelColor()
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
