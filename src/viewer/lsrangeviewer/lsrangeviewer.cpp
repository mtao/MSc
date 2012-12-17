#include "lsrangeviewer.h"
#include <QColorDialog>
#include <iostream>
#include "geometry/marchingcubes/marchingcubes.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

LevelSetRangeViewerWidget::LevelSetRangeViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): ObjectViewerWidget(format,prefix,parent),

    m_color(255,255,0),
    m_grid(0),
    m_scale(1.0),
    m_isolevel(0.0)

{
    m_transientShader = false;
    QResource::registerResource("lsrangeviewer.qrc");
}


void LevelSetRangeViewerWidget::initializeGL()
{
    ObjectViewerWidget::initializeGL();

    initializeTextures();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);



}



void LevelSetRangeViewerWidget::initializeTextures()
{
    m_shader->bind();
    //Datafield//
    //Store the volume data to polygonise
    glGenTextures(1, &(m_dataFieldTex));

    glEnable(GL_TEXTURE_3D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, m_dataFieldTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);




    glGenTextures(1, &m_edgeTableTex);
    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, m_edgeTableTex);
    //Integer textures must use nearest filtering mode
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 256, 1, 0, GL_ALPHA_INTEGER_EXT, GL_INT, MarchingCubeGridf::m_edgeTable);
    qWarning() << (MarchingCubeGridf::m_edgeTable)[0];


    glGenTextures(1, &(m_triTableTex));
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, m_triTableTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 16, 256, 0, GL_ALPHA_INTEGER_EXT, GL_INT, MarchingCubeGridf::m_triTable);



    m_shader->release();

}













void LevelSetRangeViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    if(m_grid){delete m_grid;}
    m_grid = new Gridf();
    std::ifstream in(filename.toStdString().c_str());
    m_grid->read(in);
    std::vector<int> m_indices(m_grid->data().size());
    m_dim = {{m_grid->NI(), m_grid->NJ(), m_grid->NK()}};
    qWarning() << m_grid->NI() << " " << m_grid->NJ() << " " << m_grid->NK();
    int ind=0;
    for(unsigned int k=0; k<m_grid->NK(); ++k)
        for(unsigned int j=0; j<m_grid->NJ(); ++j)
            for(unsigned int i=0; i<m_grid->NI(); ++i)
            {
                //          m_points[ind]=Vector3(i/float(m_dim[0]),j/float(m_dim[1]),k/float(m_dim[2]));
                m_indices[ind]=ind;
                ++ind;

            }
    //MeshUtils::normalize(m_points);
    glActiveTexture(GL_TEXTURE0);
    glTexImage3D( GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, m_dim[0], m_dim[1], m_dim[2], 0, GL_ALPHA, GL_FLOAT, m_grid->raw_data());
    m_indexBuffer.reset(new VertexBufferObject(m_indices.data(), m_indices.size(), GL_STATIC_DRAW, 1));
    updateBuffers();

}
void LevelSetRangeViewerWidget::updateBuffers()
{
    qWarning() << "Binding Buffers";
    m_shader->bind(false);
    if(m_indexBuffer)
        m_shader->addAttribute("index", m_indexBuffer);
    glUniform4f(glGetUniformLocation(m_shader->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
    glUniform3i(glGetUniformLocation(m_shader->programId, "dim"),
                m_dim[0],m_dim[1],m_dim[2] );
    glUniform1f(glGetUniformLocation(m_shader->programId, "scale"),
                m_scale );

    glUniform1i(glGetUniformLocation(m_shader->programId, "dataFieldTex"),0);
    glUniform1i(glGetUniformLocation(m_shader->programId, "edgeTableTex"),1);
    glUniform1i(glGetUniformLocation(m_shader->programId, "triTableTex"),2);
    glUniform1f(glGetUniformLocation(m_shader->programId, "isolevel"),
                m_isolevel );
    glUniform1f(glGetUniformLocation(m_shader->programId, "isoAdd"),
                m_isoAdd );
    float x=1.0/m_dim[0];
    float y=1.0/m_dim[1];
    float z=1.0/m_dim[2];
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[0]")
                              ,0.0       ,0.0        ,0.0       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[1]")
                              ,  x       ,0.0        ,0.0       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[2]")
                              ,  x       ,  y        ,0.0       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[3]")
                              ,0.0       ,  y        ,0.0       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[4]")
                              ,0.0       ,0.0        ,  z       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[5]")
                              ,  x       ,0.0        ,  z       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[6]")
                              ,  x       ,  y        ,  z       );
    glUniform3f(glGetUniformLocation(m_shader->programId, "vertDecals[7]")
                              ,0.0       ,  y        ,  z       );
    m_shader->release();
    ObjectViewerWidget::updateBuffers();

    m_ready = true;
}
void LevelSetRangeViewerWidget::paintGL()
{
    if(!m_ready)
        return;

    setModelMatrix();





    ObjectViewerWidget::paintGL();
    m_shader->bind();

    
    glDrawArrays(GL_POINTS, 0, m_grid->size());


    // Draw stuff

    m_shader->release();


}
void LevelSetRangeViewerWidget::setModelColor()
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
void LevelSetRangeViewerWidget::changeScale(int scale)

{
    m_scale = scale/10.0f;
        m_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_shader->programId, "scale"),
                m_scale );
        m_shader->release();
}
void LevelSetRangeViewerWidget::isoChangeMin(int isolevel)

{
    m_isolevelMin = isolevel;
    m_isolevel = m_isolevelMax + m_isolevelMin;
    m_isolevel /=2;
    m_isoAdd = m_isolevelMax - m_isolevelMin;
    m_isoAdd /=2;
    m_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_shader->programId, "isolevel"),
                m_isolevel );
    glUniform1f(glGetUniformLocation(m_shader->programId, "isoAdd"),
                m_isoAdd );
    m_shader->release();
}
void LevelSetRangeViewerWidget::isoChangeMax(int isolevel)

{
    m_isolevelMax = isolevel;
    m_isolevel = m_isolevelMax + m_isolevelMin;
    m_isolevel /=2;
    m_isoAdd = m_isolevelMax - m_isolevelMin;
    m_isoAdd /=2;
    m_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_shader->programId, "isolevel"),
                m_isolevel );
    glUniform1f(glGetUniformLocation(m_shader->programId, "isoAdd"),
                m_isoAdd );
    m_shader->release();
}

void LevelSetRangeViewerWidget::backfaceCulling(bool enabled)
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
