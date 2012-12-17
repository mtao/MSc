#ifndef GLSLWIDGET_H
#define GLSLWIDGET_H
#include <GL/glew.h>
#include <GL/glext.h>

#include <QtOpenGL/QGLWidget>

#include <QtGui/QMenuBar>
#include <QtCore/QTime>
#include <QtGui/QVector3D>
#include <QtCore/QPointF>
#include <memory>
#include <functional>
#include "glutil.h"
//#include <QGLFunctions>
#include <glm/glm.hpp>
class QMouseEvent;
class QWheelEvent;


class QTimer;
class RenderWidget : public QGLWidget//, protected QGLFunctions
{
    Q_OBJECT
public:
    RenderWidget( const QGLFormat& format, const QString & shader_prefix="", QWidget* parent = 0 );


    /*TODO*/
    virtual void openFile(const QString & filename);
    virtual QMenuBar * customMenu() {return new QMenuBar;}


protected:
    /*TODO*/
    virtual void initializeGL();
    void resizeGL( int w, int h );
    /*TODO*/
    virtual void paintGL();

    virtual void keyPressEvent( QKeyEvent* e );

private slots:
public:

    bool setShaderProgram(const QString & prefix);
    bool prepareShaderProgram(ShaderProgram & program);
    bool prepareShaderProgram(ShaderProgram & program,const QString & prefix);
    bool  prepareShaderProgram(ShaderProgram & program, const QString& vertexShaderPath,
                               const QString& fragmentShaderPath,
                               const QString& geometryShaderPath = "");
    bool prepareShaderProgram()
    {return prepareShaderProgram(*m_shader);}
    bool prepareShaderProgram(const QString & prefix)
    {return prepareShaderProgram(*m_shader, prefix);}
    bool prepareShaderProgram( const QString& vertexShaderPath,
                               const QString& fragmentShaderPath,
                               const QString& geometryShaderPath = "" )
    {return prepareShaderProgram(*m_shader, vertexShaderPath,
                                 fragmentShaderPath,
                                 geometryShaderPath );}
protected :
    void setAttributeObject(ShaderProgram & program, const char * name, std::shared_ptr<VertexBufferObject> vao);
    GLuint compileShader(GLenum type, const QString & fileName);
    bool prepareBufferObject( GLint buffer,
                              GLenum usagePattern,
                              const void* data,
                              int dataSize );

    virtual void computeMVP();
    virtual void computePMvp();
    virtual void computeMMvMvp();
    virtual void computeVMvMvp();
    virtual void activeProgramSelector(std::function<void(ShaderProgram&)> f){f(*m_shader);}
    void updateMVP();
    void updateMMvMvp();
    void updateVMvMvp();
    void updatePMvp();
    void updateMVP(ShaderProgram & program);
    void updateMMvMvp(ShaderProgram & program);
    void updateVMvMvp(ShaderProgram & program);
    void updatePMvp(ShaderProgram & program);
    virtual void updateActiveShaders();
    virtual void recordFrame(const QString & filename);

protected:
    QString m_shader_prefix;
    std::shared_ptr<ShaderProgram> m_shader;
    bool m_transientShader;

    glm::mat4 m_projectionMatrix,
    m_modelMatrix,
    m_viewMatrix,
    m_modelViewMatrix,
    m_modelViewProjectionMatrix;
    glm::mat3 m_normalTransform;

    float m_aspectRatio;
    bool m_record = false;



    QTimer* m_timer = 0;
signals:
    void shaderProgramChanged();
public slots:
    virtual void updateBuffers();
    void startRecording(){m_record=false;}
    void stopRecording(){m_record=true;}
    void toggleRecording(bool toggle){m_record=toggle;}


};

















class ObjectViewerWidget: public RenderWidget

{
    Q_OBJECT
public:
    ObjectViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
    bool ready(){return m_ready;}

protected:
    virtual void initializeGL();
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent * wheelEvent);
    //virtual void resizeGL(int w, int h);
    virtual void setModelMatrix();
    void activeProgramSelector(std::function<void (ShaderProgram &)> f);
public:
public slots:
    virtual void paintGL();
protected:
    bool m_ready = false;

    QTime m_time;
    int m_lastTime = 0;
    int m_mouseEventTime;
    float m_distance = 1.4f;
    QPointF lastPos;
    QVector3D m_rotation;
    QVector3D m_angularMomentum;
    QVector3D m_accumulatedMomentum;
    std::shared_ptr<ShaderProgram> m_boundingBoxProgram;
    std::shared_ptr<VertexBufferObject> vbo_bb_vertices;
    std::shared_ptr<VertexIndexObject> vio_bb_indices;
};


class ObjectViewer2dWidget: public RenderWidget

{
    Q_OBJECT
public:
    ObjectViewer2dWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
    bool ready(){return m_ready;}

protected:
    virtual void initializeGL();
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent * wheelEvent);
    virtual void resizeGL(int w, int h);
    virtual void setModelMatrix();
    void activeProgramSelector(std::function<void (ShaderProgram &)> f);

public:
public slots:
    virtual void paintGL();
protected:
    bool m_ready=false;

    QTime m_time;
    int m_lastTime=0;
    int m_mouseEventTime;
    float m_distance=1.4f;
    QPointF lastPos;
    QPointF pos;
    std::shared_ptr<ShaderProgram> m_boundingBoxProgram;
    std::shared_ptr<VertexBufferObject> vbo_bb_vertices;
    std::shared_ptr<VertexIndexObject> vio_bb_indices;

};





#endif // GLWIDGET_H
