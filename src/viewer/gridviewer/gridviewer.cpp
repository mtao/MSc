#include "gridviewer.h"
#include <QColorDialog>
#include <iostream>
#include "geometry/marchingcubes/marchingcubes.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

GridViewerWidget::GridViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent, int numShaders): ObjectViewerWidget(format,prefix,parent),

    m_color(0,0,255),
    m_scale(1.0),
    m_texture_point(0),
    m_discard_overflow(0),
    m_isolevel(0.0),
    m_shaders(numShaders)

{
    m_transientShader = false;
    QResource::registerResource("gridviewer.qrc");
}


void GridViewerWidget::initializeGL()
{
    ObjectViewerWidget::initializeGL();
    for(auto &shader: m_shaders)
        shader.reset(new ShaderProgram());
    prepareShaderProgram(*m_shaders[GT_VERTEX     ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/point.g.glsl");
    prepareShaderProgram(*m_shaders[GT_UFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/ugrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_VFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/vgrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_WFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/wgrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_DUFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/dugrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_DVFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/dvgrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_DWFLUX      ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/dwgrid.g.glsl");
    prepareShaderProgram(*m_shaders[GT_CELL       ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/cell.g.glsl");
    prepareShaderProgram(*m_shaders[GT_MARCHINGCUBE],":/grid/mc");
    gridType(GT_VERTEX);
    gridType(GT_CELL);
    initializeTextures();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    updateBuffers();





}

void GridViewerWidget::initializeTextures()
{
    m_active_shader->bind();
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



    m_active_shader->release();

}













void GridViewerWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    m_grid.reset(new Gridf());
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
void GridViewerWidget::updateBuffers()
{
    ObjectViewerWidget::updateBuffers();
    activeProgramSelector([&](ShaderProgram & program)
    {
        updateBuffers(program);

    });
    m_ready = true;
}
void GridViewerWidget::updateBuffers(ShaderProgram & program)
{
    program.bind(false);
    if(m_indexBuffer)
        program.addAttribute("index", m_indexBuffer);
    glUniform4f(glGetUniformLocation(program.programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
    glUniform3i(glGetUniformLocation(program.programId, "dim"),
                m_dim[0],m_dim[1],m_dim[2] );
    glUniform1f(glGetUniformLocation(program.programId, "scale"),
                m_scale );
    glUniform1i(glGetUniformLocation(program.programId, "discard_overflow"),m_discard_overflow?1:0);
    glUniform1i(glGetUniformLocation(program.programId, "texture_points"),m_texture_point?1:0);

    glUniform1i(glGetUniformLocation(program.programId, "dataFieldTex"),0);
    glUniform1i(glGetUniformLocation(program.programId, "edgeTableTex"),1);
    glUniform1i(glGetUniformLocation(program.programId, "triTableTex"),2);
    glUniform1f(glGetUniformLocation(program.programId, "isolevel"),
                m_isolevel );
    float x=1.0/m_dim[0];
    float y=1.0/m_dim[1];
    float z=1.0/m_dim[2];
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[0]")
                ,0.0       ,0.0        ,0.0       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[1]")
                ,  x       ,0.0        ,0.0       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[2]")
                ,  x       ,  y        ,0.0       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[3]")
                ,0.0       ,  y        ,0.0       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[4]")
                ,0.0       ,0.0        ,  z       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[5]")
                ,  x       ,0.0        ,  z       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[6]")
                ,  x       ,  y        ,  z       );
    glUniform3f(glGetUniformLocation(program.programId, "vertDecals[7]")
                ,0.0       ,  y        ,  z       );
    program.release();

}
void GridViewerWidget::activeProgramSelector(std::function<void (ShaderProgram &)> f)
{
    f(*m_active_shader);
}
void GridViewerWidget::paintGL()
{
    if(!m_ready)
        return;
    if(!m_grid)
        return;

    setModelMatrix();





    ObjectViewerWidget::paintGL();
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
    m_active_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_active_shader->programId, "scale"),
                m_scale );
    m_active_shader->release();
}
void GridViewerWidget::isoChange(int isolevel)

{
    m_isolevel = isolevel;
    m_active_shader->bind(false);
    glUniform1f(glGetUniformLocation(m_active_shader->programId, "isolevel"),
                m_isolevel );
    m_active_shader->release();
}


void GridViewerWidget::gridType(int type)

{
    m_active_shader = m_shaders[type].get();
    qWarning() << "Active shader id: " << m_active_shader->programId;
    updateBuffers();
    switch(type)
    {
    case GT_MARCHINGCUBE:
        glEnable(GL_DEPTH_TEST);
        emit setBackfaceCulling(true);
    }
    m_transientShader = false;
}
void GridViewerWidget::backfaceCulling(bool enabled)
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
void GridViewerWidget::discardOverflow(bool enabled)
{
    m_discard_overflow = enabled;
    m_active_shader->bind(false);
    glUniform1i(glGetUniformLocation(m_active_shader->programId, "discard_overflow"),m_discard_overflow?1:0);
    m_active_shader->release();
}
void GridViewerWidget::viewTexture(bool view)
{
    m_texture_point = view;
    m_active_shader->bind(false);
    glUniform1i(glGetUniformLocation(m_active_shader->programId, "texture_points"),m_texture_point?1:0);
    m_active_shader->release();
}
