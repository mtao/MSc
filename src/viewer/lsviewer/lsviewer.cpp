#include "lsviewer.h"

LevelSetViewerWidget::LevelSetViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): MeshViewerWidget(format,prefix,parent), m_grid(0) {}
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
    MarchingCubeGridf phi(*m_grid); 

    m_obj = new Meshf;
    phi.populateMesh(*m_obj,0);
    MeshIOUtils::OBJParser<NumericalTraitsXf> parser(m_obj);
    parser.read(filename.toStdString());
    MeshUtils::genTertiaryData(*m_obj);
    MeshUtils::normalize(*m_obj);
    std::cout << m_obj->vertices.size() << std::endl;
    std::cout << m_obj->faces.size() << std::endl;
    updateBuffers();

}
