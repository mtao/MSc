#include "gridviewer.h"
#include <QColorDialog>
#include <iostream>
#include "geometry/surface_reconstruction/marchingsquares.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

GridViewerWidget::GridViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent, int numShaders): ObjectViewer2dWidget(format,prefix,parent),

    m_color(255,255,0),
    m_scale(1.0),
    m_texture_point(0),
    m_discard_overflow(true),
    m_isolevel(0),
    m_shaders(numShaders)

{
    m_transientShader = false;
    QResource::registerResource("shaders/gridviewer.qrc");
    m_ready = false;
}


void GridViewerWidget::initializeGL()
{
    ObjectViewer2dWidget::initializeGL();
    for(auto &shader: m_shaders)
    shader.reset(new ShaderProgram());
    prepareShaderProgram(*m_shaders[GT_VERTEX     ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/point.g.glsl");
    prepareShaderProgram(*m_shaders[GT_UFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/ugrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_VFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/vgrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_CELL       ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/cell.g.glsl");
    qWarning() << "Cell program id: " << m_shaders[GT_CELL]->programId;
    prepareShaderProgram(*m_shaders[GT_MARCHINGSQUARE       ],":/grid/ms");

    gridType(GT_VERTEX);
    initializeTextures();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_ready = false;



}



void GridViewerWidget::initializeTextures()
{
    //Datafield//
    //Store the volume data to polygonise
    glGenTextures(1, &(m_dataFieldTex));

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_dataFieldTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);




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

    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 5, 16, 0, GL_ALPHA_INTEGER_EXT, GL_INT, MarchingSquaresGridf::m_edgeTable);


    updateBuffers();




}













void GridViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    m_grid.reset(new Gridf());
    std::ifstream in(filename.toStdString().c_str());
    m_grid->read(in);
    std::vector<GLint> m_indices(m_grid->data().size());
    m_dim = {{m_grid->NI(), m_grid->NJ()}};
    qWarning() << m_grid->NI() << " " << m_grid->NJ() ;
    MarchingSquaresGridf phi(*m_grid); 
    int ind=0;
    for(unsigned int j=0; j<m_grid->NJ(); ++j)
        for(unsigned int i=0; i<m_grid->NI(); ++i)
        {
            //          m_points[ind]=Vector3(i/float(m_dim[0]),j/float(m_dim[1]),k/float(m_dim[2]));
                m_indices[ind]=ind;

            ++ind;

        }


    //MeshUtils::normalize(m_points);
    glActiveTexture(GL_TEXTURE0);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA32F_ARB, m_dim[0], m_dim[1], 0, GL_ALPHA, GL_FLOAT, m_grid->raw_data());          
    m_indexBuffer.reset(new VertexBufferObject(m_indices.data(), m_indices.size(), GL_STATIC_DRAW, 1, GL_INT));
    m_phiBuffer.reset(new VertexBufferObject(m_grid->raw_data(), m_grid->size(), GL_STATIC_DRAW, 1));
    updateBuffers();
    m_ready = true;

}
void GridViewerWidget::updateBuffers()
{
    ObjectViewer2dWidget::updateBuffers();
    updateBuffers(*m_active_shader);

}
void GridViewerWidget::updateMVP()
{
    ObjectViewer2dWidget::updateMVP();
    RenderWidget::updateMVP(*m_active_shader);

}
void GridViewerWidget::updateMMvMvp()
{
    ObjectViewer2dWidget::updateMMvMvp();
    RenderWidget::updateMMvMvp(*m_active_shader);

}
void GridViewerWidget::updateVMvMvp()
{
    ObjectViewer2dWidget::updateVMvMvp();
    RenderWidget::updateVMvMvp(*m_active_shader);

}
void GridViewerWidget::updatePMvp()
{
    ObjectViewer2dWidget::updatePMvp();
    RenderWidget::updatePMvp(*m_active_shader);

}
void GridViewerWidget::updateBuffers(ShaderProgram & program)
{
    makeCurrent();
    program.bind(false);
    if(m_phiBuffer)
    {
        program.addAttribute("phi", m_phiBuffer);
    }
    if(m_indexBuffer)
        program.addAttribute("index", m_indexBuffer);
    glUniform4f(glGetUniformLocation(program.programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
    glUniform2i(glGetUniformLocation(program.programId, "dim"),
                m_dim[0],m_dim[1] );
    glUniform1f(glGetUniformLocation(program.programId, "scale"),
                m_scale );

    glUniform1i(glGetUniformLocation(program.programId, "dataFieldTex"),0);
    glUniform1i(glGetUniformLocation(program.programId, "edgeTableTex"),1);
    glUniform1i(glGetUniformLocation(program.programId, "triTableTex"),2);
    glUniform1f(glGetUniformLocation(program.programId, "isolevel"),
                m_isolevel );
    float x=1.0/m_dim[0];
    float y=1.0/m_dim[1];
    glUniform2f(glGetUniformLocation(program.programId, "vertDecals[0]")
                            ,0.0       ,0.0         );
    glUniform2f(glGetUniformLocation(program.programId, "vertDecals[1]")
                            ,  x       ,0.0         );
    glUniform2f(glGetUniformLocation(program.programId, "vertDecals[2]")
                            ,  x       ,  y         );
    glUniform2f(glGetUniformLocation(program.programId, "vertDecals[3]")
                            ,0.0       ,  y         );
    glUniform1i(glGetUniformLocation(program.programId, "texture_points"),m_texture_point?1:0);
    program.release();

}
void GridViewerWidget::paintGL()
{


    if(!m_ready)
        return;
    if(!m_grid)
        return;

    setModelMatrix();





    ObjectViewer2dWidget::paintGL();
    m_active_shader->bind();


    glDrawArrays(GL_POINTS, 0, m_grid->size());

    // Draw stuff

    m_active_shader->release();


}
void GridViewerWidget::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_color);
    if (color.isValid()) 
    {
        makeCurrent();
        m_color = color;
        m_active_shader->bind(false);
    glUniform4f(glGetUniformLocation(m_active_shader->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
        m_active_shader->release();
        update();
    }
}
void GridViewerWidget::changeScale(int scale)

{
    m_scale = scale/10.0f;
    makeCurrent();
        m_active_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_active_shader->programId, "scale"),
                m_scale );
    m_active_shader->release();
}
void GridViewerWidget::isoChange(int isolevel)

{
    m_isolevel = isolevel;
    makeCurrent();
    m_active_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_active_shader->programId, "isolevel"),
                m_isolevel );
    m_active_shader->release();
}
void GridViewerWidget::viewTexture(bool view)

{
    m_texture_point = view;
    makeCurrent();
    m_active_shader->bind(false);
    glUniform1i(glGetUniformLocation(m_active_shader->programId, "texture_points"),m_texture_point?1:0);
    m_active_shader->release();
}


void GridViewerWidget::gridType(int type)

{
    m_active_shader = m_shaders[type].get();
    updateBuffers();
    m_transientShader = false;
}
