#include "meshviewer.h"
#include <QtCore>
#include <QObject>
#include <QColorDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

MeshViewerWidget::MeshViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent):ObjectViewerWidget(format,prefix,parent), wireframe(false), normals(false), m_obj(0), 
    m_color(255,255,0),
    m_draw_mode(GL_TRIANGLES)


{
    QResource::registerResource("qrc_meshviewer.cc");


}




void MeshViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    if(m_obj){delete m_obj;}
    m_obj = new Meshf();
    MeshIOUtils::OBJParser<NumericalTraitsXf> parser(m_obj);
    parser.read(filename.toStdString());
    MeshUtils::genTertiaryData(*m_obj);
    MeshUtils::normalize(*m_obj);
    std::cout << m_obj->vertices.size() << std::endl;
    std::cout << m_obj->faces.size() << std::endl;
    m_vertexBuffer.reset(new VertexBufferObject(m_obj->vertices.data(), sizeof(Vector3)/sizeof(float)*m_obj->vertices.size()));
    m_normalBuffer.reset(new VertexBufferObject(m_obj->normals.data(), sizeof(Vector3)/sizeof(float)*m_obj->normals.size()));
    updateBuffers();

}
void MeshViewerWidget::updateBuffers()
{
    qWarning() << "Binding Buffers";
    ObjectViewerWidget::updateBuffers();

    if(!m_obj)
        return;
    m_shader->bind(false);
    m_shader->addAttribute("vertex", m_vertexBuffer);

    m_shader->addAttribute("normal", m_normalBuffer);
    switch(m_draw_mode)
    {
    case GL_TRIANGLES:
        m_indexBuffer.reset(new VertexIndexObject((void*)m_obj->faces.data(),
                                                  sizeof(IndexSet<3>)/sizeof(GLuint) * m_obj->faces.size(), GL_STATIC_DRAW));
        break;
    default:
        m_indexBuffer.reset(new VertexIndexObject((void*)m_obj->edges.data(),
                                                  sizeof(IndexSet<2>)/sizeof(GLuint) * m_obj->edges.size(), GL_STATIC_DRAW, GL_LINES));
    }
    glUniform4f(glGetUniformLocation(m_shader->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
    m_shader->release();


    m_ready = true;
}

void MeshViewerWidget::paintGL()
{

    setModelMatrix();
    ObjectViewerWidget::paintGL();
    if(!m_ready)
        return;
    m_shader->bind();














    // Draw stuff
    switch(m_draw_mode)
    {
    case GL_POINTS:
        glDrawArrays(GL_POINTS, 0, m_obj->vertices.size());
        break;
    default:
        m_indexBuffer->render();

    }
    m_shader->release();


}

void MeshViewerWidget::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_color);
    if (color.isValid())
    {
        m_color = color;
        m_shader->bind(false);
    glUniform4f(glGetUniformLocation(m_shader->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
        m_shader->release();
        update();
    }
}


void MeshViewerWidget::setDrawMode(int index)
{
    switch(index)
    {
    case DM_Point:
        m_draw_mode = GL_POINTS;
        break;
    case DM_Line:
        m_draw_mode = GL_LINES;
        break;
    case DM_Triangle:
        m_draw_mode = GL_TRIANGLES;
        break;
    }
    updateBuffers();
}
void MeshViewerWidget::enableBackfaceCulling(bool enabled)
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

void MeshViewerWidget::enableNormals(bool enabled)
{
    normals= enabled;
}

