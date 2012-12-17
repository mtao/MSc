
#include "macviewer.h"
#include <QColorDialog>
#include <QMouseEvent>
#include <iostream>
#include "geometry/surface_reconstruction/marchingsquares.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>


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

    //prepareShaderProgram(*m_shaders[GT_PARTICLE],":/mac/particle");
    prepareShaderProgram(*m_shaders[GT_PARTICLE],":/mac/particle.130");
    //prepareShaderProgram(*m_shaders[GT_VELOCITY   ],":/mac/velocity.v.glsl" ,":/mac/mac.f.glsl",":/mac/velocity.g.glsl");
    prepareShaderProgram(*m_shaders[GT_VELOCITY   ],":/mac/velocity" );
    //prepareShaderProgram(*m_shaders[GT_VELOCITY ],":/grid/mac.v.glsl",":/grid/mac.f.glsl",":/grid/ugrid.g.glsl");

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

                qWarning() << renderData.title << " " << renderData.index;
        if(renderData.renderType != GT_PARTICLE)
        {
            renderData.textureLoc = GL_TEXTURE3 +renderData.index;

            if(renderData.renderType == GT_VELOCITY)
            {
            glActiveTexture(renderData.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
            int dim0, dim1;
            myDim(GT_UFLUX,    dim0, dim1);
            glGenTextures(1, &(renderData.textureData));

            glEnable(GL_TEXTURE_2D);
            glActiveTexture(renderData.textureLoc);
            glBindTexture(GL_TEXTURE_2D, renderData.textureData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            myDim(GT_VFLUX,    dim0, dim1);
            glGenTextures(1, &(renderData.textureData2));

            glEnable(GL_TEXTURE_2D);
            renderData.textureLoc2 = GL_TEXTURE3 +renderData.index+1;
            glActiveTexture(renderData.textureLoc2);
            glBindTexture(GL_TEXTURE_2D, renderData.textureData2);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
                else
            {
            glActiveTexture(renderData.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
            int dim0, dim1;
            myDim(renderData.renderType,    dim0, dim1);
            glGenTextures(1, &(renderData.textureData));

            glEnable(GL_TEXTURE_2D);
            glActiveTexture(renderData.textureLoc);
            glBindTexture(GL_TEXTURE_2D, renderData.textureData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            }


        }
    }

    m_indexBuffer.reset(new VertexBufferObject(m_indices.data(), m_indices.size(), GL_STATIC_DRAW, 1, GL_INT));
    for(int i=0 ; i < GT_PARTICLE; ++i)
    {
        m_shaders[i]->addAttribute("index", m_indexBuffer);

    }

}
void MACViewerWidget::initializeTextures()
{
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
    ObjectViewer2dWidget::updateBuffers();
    for(int i=0; i<GT_END; ++i)
    {
        if(m_active_grid_mask & (1 << i))
        {
            updateBuffers(i);
        }
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

        int dim0, dim1;
        myDim(type,dim0,dim1);
        glUniform2i(glGetUniformLocation(program.programId, "dim"),
                    dim0,dim1);
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
        qWarning() << particle_radius;
        glUniform1f(glGetUniformLocation(program.programId, "aspectRatio"),
                    m_aspectRatio);

    }
    program.release();

}
void MACViewerWidget::activeProgramSelector(std::function<void (ShaderProgram &)> f)
{

    ObjectViewer2dWidget::activeProgramSelector(f);
    for(int i=0; i<GT_END; ++i)
    {
        if(m_active_grid_mask & (1 << i))
        {
            f(*m_shaders[i]);
        }
    }
}



inline void MACViewerWidget::myDim(int type, int & dim0, int & dim1)
{
    switch(type)
    {
    case GT_VERTEX:
    case GT_MARCHINGSQUARE:
        dim0=m_dim[0]+1;
        dim1=m_dim[1]+1;
        return;
    case GT_UFLUX:
        dim0=m_dim[0]+1;
        dim1=m_dim[1];
        return;
    case GT_VFLUX:
        dim0=m_dim[0];
        dim1=m_dim[1]+1;
        return;
    default:
        dim0=m_dim[0];
        dim1=m_dim[1];
    }
}
void MACViewerWidget::paintGL()
{
    if(!m_ready)
        return;
    setModelMatrix();
    ObjectViewer2dWidget::paintGL();

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

        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_ONE,GL_ONE);
        //glEnable(GL_POINT_SPRITE_ARB);
        //glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
        //glPointParameterf(GL_POINT_SIZE_MAX_ARB, m_scale);
        //glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);


        data.render();
        //glDisable(GL_POINT_SPRITE_ARB);

    }
    else
    {

        //if(data.marchingSquare>=0)
        {
            /*
            int dim0, dim1;
            myDim(type, dim0, dim1);
            m_shaders[GT_MARCHINGSQUARE]->bind();
            m_indexBuffer.bind();
            setAttributeObject(m_shaders[GT_MARCHINGSQUARE],"index", GL_FLOAT, 0, 1);
            m_shaders[GT_MARCHINGSQUARE].setUniformValue( "dim", dim0,dim1);
            m_shaders[GT_MARCHINGSQUARE].setUniformValue(m_shaders[GT_MARCHINGSQUARE].uniformLocation("dataFieldTex"), GL_TEXTURE2-GL_TEXTURE0+data.marchingSquare);
            glDrawArrays(GL_POINTS, 0,dim0*dim1);
            m_shaders[GT_MARCHINGSQUARE].release();
            */
        }
        //else
        {
            data.render();
        }

        // Draw stuff

    }


}
void MACViewerWidget::setSize(MACRenderData & data)
{
    switch(data.renderType)
    {
    case GT_VERTEX:
        data.size=sizeof(float)*((m_dim[0]+1)*(m_dim[1]+1));
        break;
    case GT_UFLUX:
        data.size=sizeof(float)*((m_dim[0]+1)*(m_dim[1]));
        break;
    case GT_VFLUX:
        data.size=sizeof(float)*((m_dim[0])*(m_dim[1]+1));
        break;
    case GT_CELL:
    case GT_MARCHINGSQUARE:
        case GT_VELOCITY:
        data.size=sizeof(float)*((m_dim[0])*(m_dim[1]));
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
        if(!dataSent[data.index])
        {
            dataSent[data.index] = true;
            data.size = std::get<1>(tuples[data.index]);
            if(data.renderType != GT_PARTICLE)
            {
                if(data.renderType == GT_VELOCITY)
                {
                int dim0, dim1;
                myDim(GT_UFLUX, dim0, dim1);
                data.textureLoc = GL_TEXTURE3 +data.index;
                glActiveTexture(data.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
                glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA32F_ARB, dim0, dim1, 0, GL_ALPHA, GL_FLOAT, std::get<0>(tuples[data.index]));
                myDim(GT_VFLUX, dim0, dim1);
                data.textureLoc2 = GL_TEXTURE3 +data.index+1;
                glActiveTexture(data.textureLoc2);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
                glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA32F_ARB, dim0, dim1, 0, GL_ALPHA, GL_FLOAT, std::get<0>(tuples[data.index+1]));
                }
                else
                {
                data.textureLoc = GL_TEXTURE3 +data.index;
                glActiveTexture(data.textureLoc);//Hackish: the texture defines seem to be sequential and assume first 5 are other people
                int dim0, dim1;
                myDim(data.renderType, dim0, dim1);
                /*
                m_buffers[data.index].reset(new VertexBufferObject(std::get<0>(tuples[data.index]), dim0*dim1, GL_STATIC_DRAW, 1));
                data.setData(m_buffers[data.index]);
                */
                glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA32F_ARB, dim0, dim1, 0, GL_ALPHA, GL_FLOAT, std::get<0>(tuples[data.index]));
                }

            }
            else
            {
                m_buffers[data.index].reset(new VertexBufferObject(std::get<0>(tuples[data.index]), data.size, GL_STATIC_DRAW, 2));
                data.setData(m_buffers[data.index]);
            }


        }
    }


}
void MACViewerWidget::setParticleRadius(float pr)

{
    particle_radius = pr;
}


void MACViewerWidget::resizeIndices()
{
    qWarning() << "Resizing indices: " << m_indices.size() << "to " << m_dim[0]+1 << "*" << m_dim[1]+1;
    int prevsize = m_indices.size();
    qWarning() << "prevsize: " << prevsize;
    int newsize = (m_dim[0]+1) * (m_dim[1]+1);
    m_indices.resize(newsize);
    for(int i=prevsize;i < newsize; ++i)
    {
        m_indices[i] = i;
    }
    if(!isVisible()){return;}
    m_indexBuffer.reset(new VertexBufferObject(m_indices.data(), m_indices.size(), GL_STATIC_DRAW, 1, GL_INT));
    for(int i=0 ; i < GT_END; ++i)
    {
        if(i != GT_PARTICLE)
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
    Eigen::Map<Eigen::Matrix4f> mvp(glm::value_ptr(m_modelViewProjectionMatrix));
    auto m_screen_cursor =  2*Vector2((double)event->x()/width()-0.5, .5-(double)event->y()/height());
    auto v = (mvp.cast<double>().inverse() * Eigen::Vector4d(m_screen_cursor(0), m_screen_cursor(1), 0.0, 1.0));
    m_cursor = Vector2(Scalar(v(0))+0.5,(Scalar(v(1)))+0.5 );
    emit cursorPos(Eigen::Vector2f(m_cursor));
    globalcursor = m_cursor;
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
        ObjectViewer2dWidget::mousePressEvent(event);
}
void MACViewerWidget::mouseReleaseEvent(QMouseEvent * event)
{
    emit stopCursorPolling();
    ObjectViewer2dWidget::mouseReleaseEvent(event);
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
            //    m_active_grid_mask |= GT_MARCHINGSQUARE;
            //else
            m_active_grid_mask |= 1 << it->renderType;
        }

    }
    updateBuffers();
    setReady();
}


void MACViewerWidget::setDim0(int dim0)
{
    m_dim[0]=dim0;
    if(m_indices.size() < (m_dim[0]+1) * (m_dim[1]+1))
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
    if(m_indices.size() < (m_dim[0]+1) * (m_dim[1]+1))
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
            if(auto dataptr = data.lock())
            {
                dataptr->bind(programptr->getAttribLocation("vertex"));
        glDrawArrays(GL_POINTS, 0, size/2);
            glDisableVertexAttribArray(programptr->getAttribLocation("vertex"));
            }

        }
        else if (renderType == MACViewerWidget::GT_VELOCITY)
        {
            glActiveTexture(textureLoc);
            glBindTexture(GL_TEXTURE_2D, textureData);
            glUniform1i(glGetUniformLocation(programptr->programId, "u"),
                        textureLoc-GL_TEXTURE0);
            glActiveTexture(textureLoc2);
            glBindTexture(GL_TEXTURE_2D, textureData2);
            glUniform1i(glGetUniformLocation(programptr->programId, "v"),
                        textureLoc2-GL_TEXTURE0);
        glDrawArrays(GL_POINTS, 0, size);
        }
        else
        {

            glActiveTexture(textureLoc);
            glBindTexture(GL_TEXTURE_2D, textureData);
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
