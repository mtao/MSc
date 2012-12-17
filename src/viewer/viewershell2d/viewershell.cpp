#include "viewershell.h"//TODO
#include <QColorDialog>
#include <iostream>
#include "geometry/marchingcubes/marchingcubes.h"
#include <QResource>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <QMouseEvent>
#include <QDebug>

ViewerShellWidget::ViewerShellWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): ObjectViewer2dWidget(format,prefix,parent),

    m_color(255,255,0)

{
    m_transientShader = false;
    QResource::registerResource("viewershell.qrc");//TODO
    setMouseTracking(true);
}


void ViewerShellWidget::initializeGL()
{
    ObjectViewer2dWidget::initializeGL();
    m_pprogram.reset(new ShaderProgram);
    prepareShaderProgram(*m_pprogram, ":shell/pointer");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    updateBuffers();



}








void ViewerShellWidget::openFile(const QString & filename)
{
    m_ready = false;
    qWarning() << "Loading file: "<< filename;
    updateBuffers();

}
void ViewerShellWidget::updateBuffers()
{
    ObjectViewer2dWidget::updateBuffers();
    m_ready = true;
}
void ViewerShellWidget::updateMVP()
{
    ObjectViewer2dWidget::updateMVP();
    RenderWidget::updateMVP(*m_pprogram);
}
void ViewerShellWidget::updateMMvMvp()
{
    ObjectViewer2dWidget::updateMMvMvp();
    RenderWidget::updateMMvMvp(*m_pprogram);

}
void ViewerShellWidget::updateVMvMvp()
{
    ObjectViewer2dWidget::updateVMvMvp();
    RenderWidget::updateVMvMvp(*m_pprogram);

}
void ViewerShellWidget::updatePMvp()
{
    ObjectViewer2dWidget::updatePMvp();
    RenderWidget::updateVMvMvp(*m_pprogram);

}

void ViewerShellWidget::updateBuffers(ShaderProgram & program)
{
    
    program.bind();
    program.bind(false);
    glUniform4f(glGetUniformLocation(program.programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
    glUniform1f(glGetUniformLocation(program.programId, "scale"),
                m_scale);
    program.release();

    m_ready = true;
}
void renderTree()
{

    glBegin(GL_QUADS);
    glColor4f(139/255.,69/255.,45/255.,1.0f);
    glVertex3d(-.02,0  ,0.0);
    glVertex3d(.02 ,0  ,0.0);
    glVertex3d(.02 ,.5 ,0.0);
    glVertex3d(-.02,.5 ,0.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glColor4f(0,1,0,1.0f);
    glVertex3d(.12 ,.45 ,0.0);
    glVertex3d(0 ,.55 ,0.0);
    glVertex3d(-.12,.45 ,0.0);
    glEnd();
}

void renderForest()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(0.1,0.1,1.0);
    for(int i = 0; i<100; ++i)
    {
        glPushMatrix();
        glTranslatef((i-50)/1.8,0.0,0.0);//Push ourselves to row i
        for(int j = 0; j<100; ++j)
        {
            glPushMatrix();
            glTranslatef(0.0,(j-50)/1.8,0.0);//Push ourselves to row j
            renderTree();//draw tree at position i,j
            glPopMatrix();
        }
        glPopMatrix();
    }
}
void ViewerShellWidget::paintGL()
{
    if(!m_ready)
        return;


    setModelMatrix();
    ObjectViewer2dWidget::paintGL();




    RenderWidget::updateMVP(*m_pprogram);

    m_pprogram->bind();



    glBegin(GL_POINTS);
    glColor4f(1.0,0.0,0.0,1.0);
    glVertex4d(m_cursor(0),m_cursor(1),0,1);
    glColor4f(0.0,1.0,0.0,1.0);
    glVertex4d(m_screen_cursor(0),m_screen_cursor(1),0,1);
    glEnd();


    m_pprogram->release();



}
void ViewerShellWidget::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_color);
    if (color.isValid()) 
    {
        m_color = color;
        /*
        m_pprogram->bind();
    glUniform4f(glGetUniformLocation(m_pprogram->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
        m_pprogram->release();
        */
        update();
    }
}
void ViewerShellWidget::changeScale(int scale)

{
    m_scale = scale/10.0f;
    updateMMvMvp();
}
void ViewerShellWidget::backfaceCulling(bool enabled)
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
void ViewerShellWidget::mouseMoveEvent(QMouseEvent * event)
{
    ObjectViewer2dWidget::mouseMoveEvent(event);
    Eigen::Map<Eigen::Matrix4f> mvp(glm::value_ptr(m_modelViewProjectionMatrix));
    m_screen_cursor =  2*Vector2((double)event->x()/width()-0.5, .5-(double)event->y()/height());
    Eigen::Vector4d v = (mvp.cast<double>().inverse() * Eigen::Vector4d(m_screen_cursor(0), m_screen_cursor(1), 0.0, 1.0));
    m_cursor = Vector2(Scalar(v(0))+0.5,(Scalar(v(1)))+0.5 );
    
    //std::cout << " Screen Position:" << Eigen::RowVector2f(m_screen_cursor) << std::endl;
    //std::cout << " Position:" << Eigen::RowVector2f(m_cursor) << std::endl << std::endl;
}
