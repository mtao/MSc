#include "widget.h"

#include <QCoreApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QFileInfo>
#include <QtCore>
#include <QObject>
#include <QColorDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QImage>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>



RenderWidget::RenderWidget( const QGLFormat& format, const QString & shader_prefix, QWidget* parent )
    : QGLWidget( format, parent ),
      m_shader_prefix(shader_prefix), m_timer(new QTimer(this))
{

    resize( 1200, 674 );
    connect( m_timer, SIGNAL( timeout() ), SLOT( update() ) );

    m_timer->start( 16 );
}


void RenderWidget::openFile(const QString & filename)
{
    qWarning() << "Read File: " << filename.toStdString().c_str();
}


void RenderWidget::initializeGL()
{
    glewInit();
    //initializeGLFunctions();
    QGLFormat glFormat = QGLWidget::format();
    if ( !glFormat.sampleBuffers() )
        qWarning() << "Could not enable sample buffers";

    // Set the clear color to black
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    //qWarning() << "Setting shader";
    m_shader.reset(new ShaderProgram());
    prepareShaderProgram(*m_shader);
    //qWarning() << "Done setting shader";





    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );
    glEnable(GL_MULTISAMPLE);
}

void RenderWidget::updateActiveShaders()
{
}

void RenderWidget::paintGL()
{


    // Clear the buffer with the current clearing color
    glClearColor(0.0,0.0,0.0,0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


}
void RenderWidget::updateBuffers()
{
    makeCurrent();
    updateMVP();
}

void RenderWidget::computeMVP()
{
    m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
    m_modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix ;
}

void RenderWidget::updateMVP()

{

    computeMVP();
    activeProgramSelector([&](ShaderProgram & program)
    {
        updateMVP(program);
    });
}
void RenderWidget::updateMVP(ShaderProgram &program)
{

    makeCurrent();
    GLuint programId =program.programId;
    glUseProgram(programId);
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"viewMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"projectionMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewMatrix));
    glUniformMatrix3fv(
                glGetUniformLocation(programId, "normalTransform"),
                       1, GL_FALSE, glm::value_ptr(m_normalTransform));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewProjectionMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewProjectionMatrix));
    glUseProgram(0);
}
void RenderWidget::updateMMvMvp()

{
    computeMMvMvp();
    activeProgramSelector([&](ShaderProgram & program)
    {
        updateVMvMvp(program);
    });
}

void RenderWidget::updateMMvMvp(ShaderProgram &program)
{
    makeCurrent();
    GLuint programId =program.programId;

    glUseProgram(programId);
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelMatrix));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewMatrix));
    glUniformMatrix3fv(
                glGetUniformLocation(programId, "normalTransform"),
                       1, GL_FALSE, glm::value_ptr(m_normalTransform));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewProjectionMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewProjectionMatrix));
    glUseProgram(0);
}
void RenderWidget::updateVMvMvp()

{
    computeVMvMvp();
    activeProgramSelector([&](ShaderProgram & program)
    {
        updateVMvMvp(program);
    });
}

void RenderWidget::updateVMvMvp(ShaderProgram &program)
{
    makeCurrent();

    GLuint programId =program.programId;
    glUseProgram(programId);
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"viewMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_viewMatrix));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewMatrix));
    glUniformMatrix3fv(
                glGetUniformLocation(programId, "normalTransform"),
                       1, GL_FALSE, glm::value_ptr(m_normalTransform));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewProjectionMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewProjectionMatrix));
    glUseProgram(0);
}

void RenderWidget::updatePMvp()
{
    computePMvp();
    activeProgramSelector([&](ShaderProgram & program)
    {
        updatePMvp(program);

    });
}
void RenderWidget::updatePMvp(ShaderProgram &program)
{
    makeCurrent();

    GLuint programId =program.programId;
    glUseProgram(programId);
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"projectionMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_projectionMatrix));
    glUniformMatrix4fv(
                glGetUniformLocation(programId,"modelViewProjectionMatrix")
                , 1, GL_FALSE, glm::value_ptr(m_modelViewProjectionMatrix));
    glUniform1f(glGetUniformLocation(programId,"aspectRatio")
                , m_aspectRatio);
    glUseProgram(0);
}
void RenderWidget::computeMMvMvp()
{
    m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
    m_normalTransform = glm::transpose(glm::inverse(glm::mat3(m_modelViewMatrix)));
    m_modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix ;
}
void RenderWidget::computeVMvMvp()
{
    m_modelViewMatrix = m_viewMatrix * m_modelMatrix;
    m_normalTransform = glm::transpose(glm::inverse(glm::mat3(m_modelViewMatrix)));
    m_modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix ;
}
void RenderWidget::computePMvp()
{
    m_modelViewProjectionMatrix = m_projectionMatrix * m_modelViewMatrix ;
}

void RenderWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
    /*
           case Qt::Key_Escape:
           QCoreApplication::instance()->quit();
           break;

           case Qt::Key_Space:
           break;

*/
    default:
        QGLWidget::keyPressEvent( e );
    }
}

bool RenderWidget::setShaderProgram(const QString & prefix)
{
    m_shader.reset(new ShaderProgram());
    m_shader_prefix = prefix;
    m_transientShader = true;
    if(!prepareShaderProgram(*m_shader, prefix))
        return false;
    emit shaderProgramChanged();
    return true;

}

bool RenderWidget::prepareShaderProgram(ShaderProgram & program)
{
    return prepareShaderProgram(program,m_shader_prefix);
}
bool RenderWidget::prepareShaderProgram(ShaderProgram & program,const QString & prefix)
{
    //qWarning() << prefix;
    return prepareShaderProgram(program,prefix+".v.glsl", prefix+".f.glsl", prefix+".g.glsl");
}

bool RenderWidget::prepareShaderProgram(ShaderProgram & program, const QString& vertexShaderPath,
                                          const QString& fragmentShaderPath,
                                          const QString& geometryShaderPath )
{
    //qWarning() << "Preparing shader program!";
    program.init();
    GLuint programId = program.programId;
    // First we load and compile the vertex shader...
    GLuint vertexId = compileShader(GL_VERTEX_SHADER, vertexShaderPath);
    // ...now the fragment shader...
    GLuint fragmentId = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath );
    

    glUseProgram(programId);
    if(vertexId)
        glAttachShader(programId, vertexId);
    else
    {
        glDeleteProgram(programId);
        return false;
    }
    if(fragmentId)
        glAttachShader(programId, fragmentId);
    else
    {
        glDeleteProgram(programId);
        return false;
    }
    QFileInfo geoFileInfo(geometryShaderPath);
    GLuint geometryId = 0;
    if(geoFileInfo.exists())
    {
        // ...now the geometry shader...
        geometryId = compileShader(GL_GEOMETRY_SHADER, geometryShaderPath );
        if(geometryId)
            glAttachShader(programId, geometryId);//If geometry shader doesn't compile just throw it out (might actually want it someday though
        else
        {
            glDeleteProgram(programId);
            return false;
        }
    }
    // ...and finally we link them to resolve any references.
    glLinkProgram(programId);
    GLint link_ok = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &link_ok);

    if(link_ok == GL_FALSE)
    {
        GLint log_length = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &log_length);
        char * log = new char[log_length];
        glGetProgramInfoLog(programId, log_length, NULL, log);
        qWarning() << log;
        delete[] log;
        glDeleteProgram(programId);
        return 0;
    }

    glUseProgram(0);
    //Reset projection matrix to system
    resize(width(), height());


    return true;
}
GLuint RenderWidget::compileShader(GLenum shaderType, const QString & fileName)
{

    //qWarning() << fileName;
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "File not found: " << fileName;
        return false;
    }
    QString contents = file.readAll();
    GLuint shaderId = glCreateShader(shaderType);
    char * buf = new char[contents.length()+1];
    memcpy(buf,contents.toStdString().c_str(),contents.length());
    buf[contents.length()] = '\0';
    glShaderSource(shaderId,1, const_cast<const char **>(&buf),NULL);
    delete[] buf;
    glCompileShader(shaderId);
    GLint compile_ok;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compile_ok);
    if(compile_ok == GL_FALSE)
    {
        GLint log_length = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &log_length);
        char * log = new char[log_length];
        glGetShaderInfoLog(shaderId, log_length, NULL, log);
        qWarning() << log;
        delete[] log;
    }

    return shaderId;
}
void RenderWidget::setAttributeObject(ShaderProgram & program, const char * name, std::shared_ptr<VertexBufferObject> vao)
{

    program.addAttribute(name, vao);

}

void RenderWidget::resizeGL( int w, int h )
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, h );

    // Setup an orthogonal projection matrix
    float viewingAngle = 40.0;
    float nearPlane = 0.0001;
    float farPlane = 100.0;
    h = std::max( h, 1 );
    m_aspectRatio = float( w ) / float( h );
    m_projectionMatrix = glm::perspective( viewingAngle, m_aspectRatio, nearPlane, farPlane );
    updatePMvp();
}

void RenderWidget::recordFrame(const QString &filename)
{
        QImage img = grabFrameBuffer(true);
        img.save(filename+tr(".tiff"),"TIFF");
}







const unsigned int bb_vertexDataSize = 3 * 8;
const float boundingBoxVertices[bb_vertexDataSize] = 
{
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f

};


const unsigned int bb_indexDataSize = 4 * 8;
const unsigned int boundingBoxIndices[bb_indexDataSize] = 
{
    0,1,
    1,2,
    2,3,
    3,0,
    4,5,
    5,6,
    6,7,
    7,4,
    0,4,
    1,5,
    2,6,
    3,7
};





ObjectViewerWidget::ObjectViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): RenderWidget(format,prefix,parent), m_ready(false)
  , m_lastTime(0)
  , m_distance(1.4f)
  , m_angularMomentum(0, 40, 0)
{
    QResource::registerResource("qrc_resource.cc");

}
void ObjectViewerWidget::initializeGL()
{
    RenderWidget::initializeGL();
    glm::vec3 eyePosition(0.0f,0.0f,1.0f);
    glm::vec3 targetPosition(0.0f,0.0f,0.0f);
    glm::vec3 upDirection(0.0f,1.0f,0.0f);
    m_viewMatrix = glm::lookAt(eyePosition,targetPosition,upDirection);
    connect(this,SIGNAL(shaderProgramChanged()), this, SLOT(updateBuffers()));

    m_boundingBoxProgram.reset(new ShaderProgram());
    if(this->format().majorVersion() > 2)
        prepareShaderProgram(*m_boundingBoxProgram, ":bb/bb");
    else
        prepareShaderProgram(*m_boundingBoxProgram, ":bb/bb.130");
    /*
    prepareBufferObject(m_boundingVertexBuffer, QGLBuffer::StaticDraw, boundingBoxVertices, sizeof(float)*bb_vertexDataSize);
    prepareBufferObject(m_boundingIndexBuffer, QGLBuffer::StaticDraw, boundingBoxIndices, sizeof(unsigned int)*bb_indexDataSize);
    */
    vbo_bb_vertices.reset(new VertexBufferObject((void*)boundingBoxVertices, bb_vertexDataSize));
    vio_bb_indices.reset(new VertexIndexObject((void*)boundingBoxIndices, bb_indexDataSize, GL_STATIC_DRAW, GL_LINES));
    m_boundingBoxProgram->addAttribute("vertex", vbo_bb_vertices);



}
void ObjectViewerWidget::paintGL()//render at the end
{
    RenderWidget::paintGL();
    m_boundingBoxProgram->bind();
    vio_bb_indices->render();
    /*
    glDrawElements( GL_LINES,       // Type of primitive to draw
                    bb_indexDataSize, // The number of indices in our index buffer we wish to draw
                    GL_UNSIGNED_INT,    // The element type of the index buffer
                    0 );                // Offset from the start of our index buffer of where to begin
                    */
    m_boundingBoxProgram->release();
}
void ObjectViewerWidget::activeProgramSelector(std::function<void (ShaderProgram &)> f)
{
    if(m_boundingBoxProgram)
    f(*m_boundingBoxProgram);
}



void ObjectViewerWidget::setModelMatrix()
{
    const int delta = m_time.elapsed() - m_lastTime;
    m_rotation += m_angularMomentum * (delta / 1000.0);
    m_lastTime += delta;



    // Setup the modelview matrix
    m_modelMatrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.f,0.f,-m_distance));
    m_modelMatrix = glm::rotate(m_modelMatrix, (float)m_rotation.x(), glm::vec3(1.f,0.f,0.f));
    m_modelMatrix = glm::rotate(m_modelMatrix, (float)m_rotation.y(), glm::vec3(0.f,1.f,0.f));
    m_modelMatrix = glm::rotate(m_modelMatrix, (float)m_rotation.z(), glm::vec3(0.f,0.f,1.f));
    updateMMvMvp();
}
void ObjectViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        const QPointF delta = event->posF() - lastPos;
        lastPos=event->posF();
        const QVector3D angularImpulse = QVector3D(delta.y(), delta.x(), 0) * 0.1;

        m_rotation += angularImpulse;
        m_accumulatedMomentum += angularImpulse;

        event->accept();
        update();
    }
}

void ObjectViewerWidget::mousePressEvent(QMouseEvent *event)
{

    m_mouseEventTime = m_time.elapsed();
    m_angularMomentum = m_accumulatedMomentum = QVector3D();
    event->accept();
    lastPos=event->posF();
}

void ObjectViewerWidget::mouseReleaseEvent(QMouseEvent *event)
{

    const int delta = m_time.elapsed() - m_mouseEventTime;
    m_angularMomentum = m_accumulatedMomentum * (1000.0 / qMax(1, delta));
    event->accept();
    update();
}

void ObjectViewerWidget::wheelEvent(QWheelEvent *event)
{

    m_distance *= qPow(1.2, -event->delta() / 120);
    event->accept();
    update();
}





















const unsigned int bb2_vertexDataSize = 2*4;
const float boundingBox2dVertices[bb2_vertexDataSize] = 
{
    -0.5f, -0.5f,
    -0.5f,  0.5f,
    0.5f,  0.5f,
    0.5f, -0.5f,

};


const unsigned int bb2_indexDataSize = 2*4;
const unsigned int boundingBox2dIndices[bb2_indexDataSize] = 
{
    0,1,
    1,2,
    2,3,
    3,0,
};



ObjectViewer2dWidget::ObjectViewer2dWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): RenderWidget(format,prefix,parent), m_ready(false)
  , m_lastTime(0)
  , m_distance(1.4f)
{
    m_viewMatrix = glm::mat4(1.0);
    QResource::registerResource("qrc_resource.cc");

}
void ObjectViewer2dWidget::initializeGL()
{
    RenderWidget::initializeGL();
    connect(this,SIGNAL(shaderProgramChanged()), this, SLOT(updateBuffers()));

    m_boundingBoxProgram.reset(new ShaderProgram());
    if(this->format().majorVersion() > 2)
        prepareShaderProgram(*m_boundingBoxProgram, ":bb/bb2");
    else
        prepareShaderProgram(*m_boundingBoxProgram, ":bb/bb2.130");
    vbo_bb_vertices.reset(new VertexBufferObject((void*)boundingBox2dVertices, bb2_vertexDataSize, GL_STATIC_DRAW, 2));
    //qWarning() << vbo_bb_vertices->tupleSize;
    vio_bb_indices.reset(new VertexIndexObject((void*)boundingBox2dIndices, bb2_indexDataSize, GL_STATIC_DRAW, GL_LINES));

    m_boundingBoxProgram->addAttribute("vertex", vbo_bb_vertices);




}
void ObjectViewer2dWidget::paintGL()//render at the end
{
    RenderWidget::paintGL();

    m_boundingBoxProgram->bind();
    vio_bb_indices->render();
    m_boundingBoxProgram->release();
}


void ObjectViewer2dWidget::activeProgramSelector(std::function<void (ShaderProgram &)> f)
{
    if(m_boundingBoxProgram)
    f(*m_boundingBoxProgram);
}


void ObjectViewer2dWidget::setModelMatrix()
{



    // Setup the modelview matrix
    m_modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f/m_distance, 1.f/m_distance, 1.0f));
    m_modelMatrix = glm::translate(m_modelMatrix, glm::vec3(pos.x(), pos.y(), 0.0f));
    updateMMvMvp();
}
void ObjectViewer2dWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        const QPointF delta = event->posF() - lastPos;
        lastPos=event->posF();
        pos += QPointF(delta.x(),-delta.y()) * m_distance/height();

        event->accept();
        update();
    }
}

void ObjectViewer2dWidget::mousePressEvent(QMouseEvent *event)
{

    m_mouseEventTime = m_time.elapsed();
    event->accept();
    lastPos=event->posF();
}

void ObjectViewer2dWidget::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
    update();
}

void ObjectViewer2dWidget::wheelEvent(QWheelEvent *event)
{

    m_distance *= qPow(1.2, -event->delta() / 120);
    event->accept();
    update();
}


void ObjectViewer2dWidget::resizeGL(int w, int h)
{
    // Set the viewport to window dimensions
    glViewport( 0, 0, w, h );

    // Setup an orthogonal projection matrix
    m_aspectRatio = float( w ) / float( h );
    m_projectionMatrix = glm::ortho( -0.5*m_aspectRatio,0.5*m_aspectRatio,-0.5,0.5,-1.0,1.0);
    updatePMvp();
}
