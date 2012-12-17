
#include "macviewer.h"
#include <QColorDialog>
#include <QMouseEvent>
#include <iostream>
#include "geometry/surface_reconstruction/marchingsquares.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>


Eigen::Vector2f globalcursor;
void printData(const MACRenderData & data)
{
    std::cout << "MACRenderData: " << data.title.toUtf8().constData()
              << " " << data.index
              << " "  << data.renderType
              << " " << data.size
              << " " << data.active
              << std::endl;
}
void MACViewerWidget::useProfile(MACRenderDataProfile & profile)
{
    m_profile.reset(&profile);
    qWarning() << "Number of buffers: "<< m_profile->numBuffers;
    m_buffers.resize(m_profile->numBuffers);

}
//-----------------------------------------------------
//-----------------------------------------------------
//Precomputation steps
//-----------------------------------------------------
//-----------------------------------------------------
MACViewerWidget::MACViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): GridViewerWidget(format,prefix,parent, GT_END),

    m_color(255,255,0),
    m_active_grid_mask(0),
    m_render_mask(0),
    m_frame(0)

{
    m_texture_point = true;
    m_transientShader = false;
    QResource::registerResource("shaders/macviewer.qrc");
    setMouseTracking(true);
}


void MACViewerWidget::initializeGL()
{
    GridViewerWidget::initializeGL();

    prepareShaderProgram(*m_shaders[GT_PARTICLE],":/mac/particle");
    //prepareShaderProgram(*m_shaders[GT_PARTICLE],":/mac/particle.130");
    //prepareShaderProgram(*m_shaders[GT_VELOCITY   ],":/mac/velocity.v.glsl" ,":/mac/mac.f.glsl",":/mac/velocity.g.glsl");
    //prepareShaderProgram(*m_shaders[GT_PARTICLE ],":/mac/particle.v.glsl" ,":/mac/mac.f.glsl",":/mac/velocity.g.glsl");
    prepareShaderProgram(*m_shaders[GT_VELOCITY   ],":/mac/velocity" );
    m_thickness_program.reset(new ShaderProgram());
    m_normal_program.reset(new ShaderProgram());
    m_smooth_program.reset(new ShaderProgram());
    m_render_program.reset(new ShaderProgram());
    prepareShaderProgram(*m_thickness_program,":/mac/particle.v.glsl", ":/mac/thickness.f.glsl", ":/mac/particle.g.glsl");
    prepareShaderProgram(*m_normal_program,":/mac/particle.v.glsl", ":mac/particle.f.glsl", ":/mac/particle.g.glsl");
    prepareShaderProgram(*m_smooth_program,":/mac/quad.v.glsl", ":/mac/smooth.f.glsl", ":/mac/quad.g.glsl");
    prepareShaderProgram(*m_render_program,":/mac/quad.v.glsl", ":/mac/curvatureflow.f.glsl", ":/mac/quad.g.glsl");

    m_scale = 1.0;
    gridType(GT_VERTEX);
    m_indices.resize(10);
    for(int i=0; i<10; ++i)
        m_indices[i]=i;
    resizeIndices();
    //qWarning() << m_profile->numBuffers;
    for(int i=0; i < m_profile->data.size(); ++i)
    {
        MACRenderData & renderData = m_profile->data[i];
        //qWarning() << renderData.title;
        renderData.setProgram(m_shaders[renderData.renderType]);

        if(renderData.renderType != GT_PARTICLE)
        {
            if(renderData.renderType == GT_VELOCITY)
            {
                renderData.textureLoc = GL_TEXTURE7 +renderData.index;
                glActiveTexture(renderData.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
                int dim0, dim1,dim2;
                myDim(GT_UFLUX,    dim0, dim1,dim2);
                glGenTextures(1, &(renderData.textureData));

                glEnable(GL_TEXTURE_3D);
                glActiveTexture(renderData.textureLoc);
                glBindTexture(GL_TEXTURE_3D, renderData.textureData);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                myDim(GT_VFLUX,    dim0, dim1,dim2);
                glGenTextures(1, &(renderData.textureData2));

                glEnable(GL_TEXTURE_3D);
                renderData.textureLoc2 = GL_TEXTURE7 +renderData.index+1;
                glActiveTexture(renderData.textureLoc2);
                glBindTexture(GL_TEXTURE_3D, renderData.textureData2);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);




                myDim(GT_WFLUX,    dim0, dim1,dim2);
                glGenTextures(1, &(renderData.textureData3));

                glEnable(GL_TEXTURE_3D);
                renderData.textureLoc3 = GL_TEXTURE7 +renderData.index+2;
                glActiveTexture(renderData.textureLoc3);
                glBindTexture(GL_TEXTURE_3D, renderData.textureData3);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
            else
            {
                renderData.textureLoc = GL_TEXTURE7 +renderData.index;
                glActiveTexture(renderData.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
                int dim0, dim1,dim2;
                myDim(renderData.renderType,    dim0, dim1,dim2);
                glGenTextures(1, &(renderData.textureData));

                glEnable(GL_TEXTURE_3D);
                glActiveTexture(renderData.textureLoc);
                glBindTexture(GL_TEXTURE_3D, renderData.textureData);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }


        }

    }

    m_indexBuffer.reset(new VertexBufferObject(m_indices.data(), m_indices.size(), GL_STATIC_DRAW, 1, GL_INT));
    for(int i=0 ; i < GT_PARTICLE; ++i)
    {
        m_shaders[i]->addAttribute("index", m_indexBuffer);

    }
    glEnable(GL_TEXTURE_2D);
    glGenFramebuffers(1,&thick_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,thick_fbo);
    glGenTextures(1,&thick_tex);
    glBindTexture(GL_TEXTURE_2D, thick_tex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thick_tex, 0);


    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width(), height(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

    glGenFramebuffers(1, &depth_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    glGenTextures(1, &depth_tex2);
    glBindTexture(GL_TEXTURE_2D, depth_tex2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width(), height(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);








}

void MACViewerWidget::initializeTextures()
{
    GridViewerWidget::initializeTextures();
    /*
    int dim = 32;
    float mean = dim/2.0f;
    float texture[dim*dim];
    for(int j=0; j<dim; ++j)
        for(int i=0; i<dim; ++i)
        {
            float norm2 = std::pow(i-mean,2.0f) + std::pow(j-mean,2.0f);
            texture[dim*j+i] = std::exp(-norm2);
        }

    glGenTextures(2,&m_spriteTex);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_spriteTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,32,32,0,GL_ALPHA, GL_FLOAT, texture);
    */


}

//-----------------------------------------------------
//-----------------------------------------------------
//Update data
//===========
//These just depend on the active rendering mask
//-----------------------------------------------------
//-----------------------------------------------------


void MACViewerWidget::updateBuffers()
{
    makeCurrent();
    ObjectViewerWidget::updateBuffers();
    for(int i=0; i<GT_END; ++i)
    {
        if(m_active_grid_mask & (1 << i))
        {
            updateBuffers(i);
        }
    }
    if(m_active_grid_mask & (1 << GT_PARTICLE))
    {
        m_normal_program->bind();
        glUniform1f(glGetUniformLocation(m_normal_program->programId, "radius"),
                    particle_radius);
        qWarning() << "Particle Radius: " <<particle_radius;
        glUniform1f(glGetUniformLocation(m_normal_program->programId, "aspectRatio"),
                    m_aspectRatio);
        m_normal_program->release();
        m_thickness_program->bind();
        glUniform1f(glGetUniformLocation(m_thickness_program->programId, "radius"),
                    particle_radius);
        qWarning() << "Particle Radius: " <<particle_radius;
        glUniform1f(glGetUniformLocation(m_thickness_program->programId, "aspectRatio"),
                    m_aspectRatio);
        m_thickness_program->release();
    }
}

void MACViewerWidget::updateBuffers(int type)
{
    ShaderProgram & program = *m_shaders[type];
    program.bind(false);
    if(type != GT_PARTICLE)
    {
        GridViewerWidget::updateBuffers(program);

        float x=1.0/m_dim[0];
        float y=1.0/m_dim[1];
        float z=1.0/m_dim[2];

        int dim0, dim1,dim2;
        myDim(type,dim0,dim1,dim2);
        glUniform3i(glGetUniformLocation(program.programId, "dim"),
                    dim0,dim1,dim2);
        if(m_indexBuffer)
        {
            qWarning() << "Index buffer updating";
            program.addAttribute("index", m_indexBuffer);
        }
        glUniform1f(glGetUniformLocation(program.programId, "scale")
                    , m_scale/10.0f );
    }
    else
    {
        glUniform1f(glGetUniformLocation(program.programId, "radius"),
                    particle_radius);
        qWarning() << "Particle Radius: " <<particle_radius;
        glUniform1f(glGetUniformLocation(program.programId, "aspectRatio"),
                    m_aspectRatio);

    }
    program.release();

}


void MACViewerWidget::activeProgramSelector(std::function<void (ShaderProgram &)> f)
{

    ObjectViewerWidget::activeProgramSelector(f);
    for(int i=0; i<GT_END; ++i)
    {
        if(m_active_grid_mask & (1 << i))
        {
            f(*m_shaders[i]);
        }
    }
    if(m_active_grid_mask & (1 << GT_PARTICLE))
    {
        f(*m_normal_program);
        f(*m_thickness_program);
    }
}


inline void MACViewerWidget::myDim(int type, int & dim0, int & dim1, int & dim2)
{
    switch(type)
    {
    case GT_VERTEX:
        dim0=m_dim[0]+1;
        dim1=m_dim[1]+1;
        dim2=m_dim[2]+1;
        return;
    case GT_UFLUX:
        dim0=m_dim[0]+1;
        dim1=m_dim[1];
        dim2=m_dim[2];
        return;
    case GT_VFLUX:
        dim0=m_dim[0];
        dim1=m_dim[1]+1;
        dim2=m_dim[2];
        return;
    case GT_WFLUX:
        dim0=m_dim[0];
        dim1=m_dim[1];
        dim2=m_dim[2]+1;
        return;
    case GT_VELOCITY:
    case GT_MARCHINGCUBE:
    default:
        dim0=m_dim[0];
        dim1=m_dim[1];
        dim2=m_dim[2];
    }
}
void MACViewerWidget::paintGL()
{
    if(!m_ready)
        return;
    setModelMatrix();
    ObjectViewerWidget::paintGL();

    for(auto it = m_profile->data.begin(); it < m_profile->data.end(); ++it)
    {
        paintGL(*it);
    }
    if(m_record)
    {
        QString buf;
        if(frame < 10)
            buf = "000";
        else if(frame < 100)
            buf = "00";
        else if(frame < 1000)
            buf = "0";

        recordFrame(tr("frame")+buf+QString::number(frame++));

    }

}

void MACViewerWidget::paintGL(MACRenderData & data)
{
    if(!data.active) return;
    int type = data.renderType;
    //qWarning() << "Rendering data: " << data.title << " " << data.bufferIndex<< " "  << type << data.size << " " << data.active;

    if(type == GT_PARTICLE)
    {

        //glEnable(GL_POINT_SPRITE_ARB);
        //glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
        //glPointParameterf(GL_POINT_SIZE_MAX_ARB, m_scale);
        //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);

        auto & vdata = m_profile->data[13];
        auto pptr = vdata.program.lock();
        pptr->bind(false);
        glActiveTexture(vdata.textureLoc);
        glBindTexture(GL_TEXTURE_3D, vdata.textureData);
        glUniform1i(glGetUniformLocation(pptr->programId, "u"),
                    vdata.textureLoc-GL_TEXTURE0);
        glActiveTexture(vdata.textureLoc2);
        glBindTexture(GL_TEXTURE_3D, vdata.textureData2);
        glUniform1i(glGetUniformLocation(pptr->programId, "v"),
                    vdata.textureLoc2-GL_TEXTURE0);
        glActiveTexture(vdata.textureLoc3);
        glBindTexture(GL_TEXTURE_3D, vdata.textureData3);
        glUniform1i(glGetUniformLocation(pptr->programId, "w"),
                    vdata.textureLoc3-GL_TEXTURE0);
        pptr->release();

        glEnable(GL_DEPTH_TEST);
        if(auto programptr = data.program.lock())
        {
            if(auto dataptr = data.data.lock())
            {
                glBindFramebuffer(GL_FRAMEBUFFER,0);
                glBindFramebuffer(GL_FRAMEBUFFER,depth_fbo);
                glClear(GL_DEPTH_BUFFER_BIT);
                programptr->bind();
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
                glEnable(GL_DEPTH_TEST);
                dataptr->bind(programptr->getAttribLocation("vertex"));
                glDrawArrays(GL_POINTS, 0, data.size/3);
                glDisableVertexAttribArray(programptr->getAttribLocation("vertex"));
                glEnable(GL_BLEND);
                glBindFramebuffer(GL_FRAMEBUFFER,0);
            }
            programptr->release();
        }

        glDisable(GL_TEXTURE_2D);
        //glBindTexture(GL_TEXTURE_2D,0); //Bad mojo to unbind the framebuffer using the texture
        //glBindFramebuffer(GL_FRAMEBUFFER, m_thick_fbo);
        if(auto dataptr = data.data.lock())
        {
            //thicko
            glClearColor(0.0,0.0,0.0,1.0);
            glBindFramebuffer(GL_FRAMEBUFFER,thick_fbo);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glBlendEquationSeparate(GL_SRC_ALPHA, GL_FUNC_ADD);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
            m_thickness_program->bind();
            dataptr->bind(m_thickness_program->getAttribLocation("vertex"));
            glDrawArrays(GL_POINTS, 0, data.size/3);
            glDisableVertexAttribArray(m_thickness_program->getAttribLocation("vertex"));
            m_thickness_program->release();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
        }

        glEnable(GL_DEPTH_BUFFER);
        glDepthMask(GL_TRUE);
        int iters=10;
        for(int i=0; i < iters; ++i)
        {
            //in: tex = written to, tex2 = need to write to
            glBindFramebuffer(GL_FRAMEBUFFER, depth_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex2, 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glDisable(GL_BLEND);
            m_smooth_program->bind();

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, depth_tex);
            glUniform1i(glGetUniformLocation(m_smooth_program->programId, "depth"),4);
            glUniform2i(glGetUniformLocation(m_smooth_program->programId, "V"),width(), height());
        glUniform1f(glGetUniformLocation(m_thickness_program->programId, "radius"),
                    particle_radius);
            glDrawArrays(GL_POINTS,0,1);
            m_smooth_program->release();
            depth_tex ^= depth_tex2;
            depth_tex2 ^= depth_tex;
            depth_tex ^= depth_tex2;
            //tex = just written to, tex2 = old written to
        }

        if(iters % 2 == 0)
        {
            depth_tex ^= depth_tex2;
            depth_tex2 ^= depth_tex;
            depth_tex ^= depth_tex2;
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_BUFFER);
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquationSeparate(GL_SRC_ALPHA, GL_FUNC_ADD);
        m_render_program->bind();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, thick_tex);
        glUniform1i(glGetUniformLocation(m_render_program->programId, "thickness"),3);
        glUniformMatrix4fv(
                    glGetUniformLocation(m_render_program->programId,"invPersMatrix")
                    , 1, GL_FALSE, glm::value_ptr(m_invProj));
        m_invView = glm::inverse(m_viewMatrix);
        glUniformMatrix4fv(
                    glGetUniformLocation(m_render_program->programId,"invViewMatrix")
                    , 1, GL_FALSE, glm::value_ptr(m_invView));
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, depth_tex);
        glUniform1i(glGetUniformLocation(m_render_program->programId, "depth"),4);
        glUniform2i(glGetUniformLocation(m_render_program->programId, "V"),width(), height());

        glDrawArrays(GL_POINTS,0,1);
        m_render_program->release();

    }
    else
    {

        //if(data.marchingSquare>=0)
        {
            /*
            int dim0, dim1;
            myDim(type, dim0, dim1);
            m_shaders[GT_MARCHINGCUBE]->bind();
            m_indexBuffer.bind();
            setAttributeObject(m_shaders[GT_MARCHINGCUBE],"index", GL_FLOAT, 0, 1);
            m_shaders[GT_MARCHINGCUBE].setUniformValue( "dim", dim0,dim1);
            m_shaders[GT_MARCHINGCUBE].setUniformValue(m_shaders[GT_MARCHINGCUBE].uniformLocation("dataFieldTex"), GL_TEXTURE2-GL_TEXTURE0+data.marchingSquare);
            glDrawArrays(GL_POINTS, 0,dim0*dim1);
            m_shaders[GT_MARCHINGCUBE].release();
            */
        }
        //else
        {
            data.render();
        }

        // Draw stuff

    }


}
void MACViewerWidget::resizeGL(int w, int h)
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, h );

    // Setup an orthogonal projection matrix
    float viewingAngle = 40.0;
    float nearPlane = 1.0;
    float farPlane = 4.0;
    h = std::max( h, 1 );
    m_aspectRatio = float( w ) / float( h );
    m_projectionMatrix = glm::perspective( viewingAngle, m_aspectRatio, nearPlane, farPlane );
    updatePMvp();
    //GridViewerWidget::resizeGL(w,h);
    m_invProj = glm::inverse(m_projectionMatrix);
    glBindTexture(GL_TEXTURE_2D, thick_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width(), height(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, depth_tex2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width(), height(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    /*
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width(), height(), 0, GL_RGBA, GL_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w,h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    */

}
void MACViewerWidget::setSize(MACRenderData & data)
{
    switch(data.renderType)
    {
    case GT_VERTEX:
        data.size=sizeof(float)*((m_dim[0]+1)*(m_dim[1]+1)*(m_dim[2]+1));
        break;
    case GT_UFLUX:
        data.size=sizeof(float)*((m_dim[0]+1)*(m_dim[1])*(m_dim[2]));
        break;
    case GT_VFLUX:
        data.size=sizeof(float)*((m_dim[0])*(m_dim[1]+1)*(m_dim[2]));
        break;
    case GT_WFLUX:
        data.size=sizeof(float)*((m_dim[0])*(m_dim[1])*(m_dim[2]+1));
        break;
    case GT_CELL:
    case GT_MARCHINGCUBE:
    case GT_VELOCITY:
        data.size=sizeof(float)*((m_dim[0])*(m_dim[1])*(m_dim[2]));
        break;
    }
}

#include <iostream>
void MACViewerWidget::setRenderItems(std::vector<std::tuple<float *, int> > & tuples)
{
    //qWarning() << "Setting data for rendering";
    m_ready = false;
    /*
    if (tuples.size() != m_buffers.size())
    {
        qWarning() << "Buffers and tuples wrong size!!!!!!";
        qWarning() << tuples.size() << " " << m_buffers.size();
        close();
    }
    */
    makeCurrent();
    std::vector<bool> dataSent(m_profile->numBuffers,false);
    for(auto & data: m_profile->data)
    {
        data.size = std::get<1>(tuples[data.index]);
        if(data.renderType == GT_VELOCITY)
        {
            data.size = m_dim[0] * m_dim[1] * m_dim[2];
            continue;

        }
        if(!dataSent[data.index])
        {
            dataSent[data.index] = true;
            data.size = std::get<1>(tuples[data.index]);
            if(data.renderType != GT_PARTICLE)
            {
                {
                    data.textureLoc = GL_TEXTURE7 +data.index;
                    glActiveTexture(data.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
                    int dim0, dim1, dim2;
                    myDim(data.renderType, dim0, dim1, dim2);
                    m_buffers[data.index].reset(new VertexBufferObject(std::get<0>(tuples[data.index]), dim0*dim1*dim2, GL_STATIC_DRAW, 1));
                    data.setData(m_buffers[data.index]);
                    glTexImage3D( GL_TEXTURE_3D, 0, GL_ALPHA32F_ARB, dim0, dim1, dim2, 0, GL_ALPHA, GL_FLOAT, std::get<0>(tuples[data.index]));
                }
            }
            else
            {
                m_buffers[data.index].reset(new VertexBufferObject(std::get<0>(tuples[data.index]), data.size));
                data.setData(m_buffers[data.index]);
            }


        }
    }


}
void MACViewerWidget::setParticleRadius(float pr)

{
    particle_radius = 3*pr;
}


void MACViewerWidget::resizeIndices()
{
    qWarning() << "Resizing indices: " << m_indices.size() << "to " << m_dim[0]+1 << "*" << m_dim[1]+1 << "*" << m_dim[2]+1;
    int prevsize = m_indices.size();
    qWarning() << "prevsize: " << prevsize;
    int newsize = (m_dim[0]+1) * (m_dim[1]+1) * (m_dim[2]+1);
    m_indices.resize(newsize);
    for(int i=prevsize;i < newsize; ++i)
    {
        m_indices[i] = i;
    }
    if(!isVisible()){return;}
    m_indexBuffer.reset(new VertexBufferObject(m_indices.data(), m_indices.size(), GL_STATIC_DRAW, 1, GL_INT));
    for(int i=0 ; i < GT_PARTICLE; ++i)
    {
        m_shaders[i]->addAttribute("index", m_indexBuffer);

    }
}

//-----------------------------------------------------
//-----------------------------------------------------
//UI Interactions
//-----------------------------------------------------
//-----------------------------------------------------

void MACViewerWidget::mouseMoveEvent(QMouseEvent * event)
{
    /*
    Eigen::Map<Eigen::Matrix4f> mvp(glm::value_ptr(m_modelViewProjectionMatrix));
    auto m_screen_cursor =  2*Vector2((double)event->x()/width()-0.5, .5-(double)event->y()/height());
    auto v = (mvp.cast<double>().inverse() * Eigen::Vector4d(m_screen_cursor(0), m_screen_cursor(1), 0.0, 1.0));
    m_cursor = Vector2(Scalar(v(0))+0.5,(Scalar(v(1)))+0.5 );
    emit cursorPos(Eigen::Vector2f(m_cursor));
    globalcursor = m_cursor;
    */
    if(!(event->modifiers() & Qt::ShiftModifier))
    {
        GridViewerWidget::mouseMoveEvent(event);
    }
}
void MACViewerWidget::mousePressEvent(QMouseEvent * event)
{
    if(event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ShiftModifier)
    {
        emit startCursorPolling();
    }
    else
        ObjectViewerWidget::mousePressEvent(event);
}
void MACViewerWidget::mouseReleaseEvent(QMouseEvent * event)
{
    emit stopCursorPolling();
    ObjectViewerWidget::mouseReleaseEvent(event);
}





//-----------------------------------------------------
//-----------------------------------------------------
//Slots
//-----------------------------------------------------
//-----------------------------------------------------
void MACViewerWidget::activeMaskChanged()
{
    m_active_grid_mask = 0;
    for(auto it = m_profile->data.begin(); it < m_profile->data.end(); ++it)
    {
        if(it->active)
        {
            //if(it->marchingSquare>=0)
            //    m_active_grid_mask |= GT_MARCHINGCUBE;
            //else
            m_active_grid_mask |= 1 << it->renderType;

            qWarning() << it->renderType << m_active_grid_mask;
        }

    }
    qWarning () << m_active_grid_mask;
    updateBuffers();
    setReady();
}


void MACViewerWidget::setDim0(int dim0)
{
    m_dim[0]=dim0;
    if(m_indices.size() < (m_dim[0]+1) * (m_dim[1]+1) * (m_dim[2]+1))
    {
        resizeIndices();
    }
}

void MACViewerWidget::changeScale(int scale)
{
    m_scale = scale;
    qWarning() << "Scale changed: " << m_scale;
    updateBuffers();
}

void MACViewerWidget::setDim1(int dim1)
{
    m_dim[1]=dim1;
    if(m_indices.size() < (m_dim[0]+1) * (m_dim[1]+1) * (m_dim[2]+1))
    {
        resizeIndices();
    }
}
void MACViewerWidget::setDim2(int dim2)
{
    m_dim[2]=dim2;
    if(m_indices.size() < (m_dim[0]+1) * (m_dim[1]+1) * (m_dim[2]+1))
    {
        resizeIndices();
    }
}
void MACViewerWidget::dataChanged()
{

    //TODO: reset data and textures
    //Iterate through renderdata fixing their data
    setReady();
}
void MACViewerWidget::setReady() {m_ready = true;}
void MACViewerWidget::setNotReady() {m_ready=false;}
void MACViewerWidget::step() {}
void MACViewerWidget::setFrame(int i) {qWarning() << i;}
void MACViewerWidget::createNew() {}
int MACViewerWidget::numFrames() {return -1;}


void MACRenderData::render(){
    if(auto programptr = program.lock())
    {
        programptr->bind();

        if(renderType == MACViewerWidget::GT_PARTICLE)
        {
            glDisable(GL_BLEND);
            if(auto dataptr = data.lock())
            {
                dataptr->bind(programptr->getAttribLocation("vertex"));
                glDrawArrays(GL_POINTS, 0, size/3);
                glDisableVertexAttribArray(programptr->getAttribLocation("vertex"));
            }

        }
        else if (renderType == MACViewerWidget::GT_VELOCITY)
        {
            glActiveTexture(textureLoc);
            glBindTexture(GL_TEXTURE_3D, textureData);
            glUniform1i(glGetUniformLocation(programptr->programId, "u"),
                        textureLoc-GL_TEXTURE0);
            glActiveTexture(textureLoc2);
            glBindTexture(GL_TEXTURE_3D, textureData2);
            glUniform1i(glGetUniformLocation(programptr->programId, "v"),
                        textureLoc2-GL_TEXTURE0);
            glActiveTexture(textureLoc3);
            glBindTexture(GL_TEXTURE_3D, textureData3);
            glUniform1i(glGetUniformLocation(programptr->programId, "w"),
                        textureLoc3-GL_TEXTURE0);
            glDrawArrays(GL_POINTS, 0, size);
        }
        else
        {
            glActiveTexture(textureLoc);
            glBindTexture(GL_TEXTURE_3D, textureData);
            glUniform1i(glGetUniformLocation(programptr->programId, "dataFieldTex"),
                        textureLoc-GL_TEXTURE0);
            glDrawArrays(GL_POINTS, 0, size);
        }






        programptr->release();
        /*
            if(auto dataptr = data.lock())
            {
                programptr->bind();

                glActiveTexture(textureLoc);
                glBindTexture(GL_TEXTURE_2D, textureData);
                glUniform1i(glGetUniformLocation(programptr->programId, "dataFieldTex"),
                            textureLoc-GL_TEXTURE0);

                dataptr->bind(programptr->getAttribLocation("phi"));





                glDrawArrays(GL_POINTS, 0, size);
                glDisableVertexAttribArray(programptr->getAttribLocation("phi"));
                programptr->release();
            }
            else
            {
                qWarning() << "Data for " << title << "cannot be bound";
                return;
            }
            */
    }
    else
    {
        qWarning() << "Program for " << title << "cannot be bound";
        return;
    }
}
