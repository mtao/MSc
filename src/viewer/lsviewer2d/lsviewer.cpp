#include "lsviewer.h"
#include <QResource>

LevelSetViewerWidget::LevelSetViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): MeshViewerWidget(format,prefix,parent) 
{
    QResource::registerResource("lsviewer.qrc");
}
void LevelSetViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    if(m_obj){delete m_obj;}
    std::ifstream in(filename.toStdString().c_str());
    if(m_grid){delete m_grid;}
    m_grid = new Gridf();
    m_grid->read(in);
    std::cout << m_grid->NI() << " " << m_grid->NJ() << " " << m_grid->NK() << std::endl;
    MarchingSquaresGridf phi(*m_grid); 

    m_obj = new Mesh2f();
    phi.populateMesh(*m_obj,0);
    MeshUtils::normalize(*m_obj);
    std::cout << m_obj->vertices.size() << std::endl;
    std::cout << m_obj->faces.size() << std::endl;
    sendBuffers();

}
