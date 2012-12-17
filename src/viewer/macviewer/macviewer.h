
#ifndef MACVIEWER_H
#define MACVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/render.h"
#include "geometry/mesh/processing.h"
#include "geometry/surface_reconstruction/marchingsquares.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>
#include "viewer/gridviewer/gridviewer.h"
#include "simulation/utils/mac.h"


/*
    */
struct MACRenderData
{
    MACRenderData(const QString & t, int i, int rt):
        title(t),
        index(i),
        renderType(rt)
      //marchingSquare(mc)
    {}

    QString title = "Untitled";
    int index = 0;
    int renderType = 0;
    int size = 0;
    bool active = false;
    void setProgram(std::shared_ptr<ShaderProgram> & program_)
    {program=program_;}
    void setData(std::shared_ptr<VertexBufferObject> & vbo)
    {
        data = vbo;
    }
    void render();
    std::weak_ptr<ShaderProgram> program;
    std::weak_ptr<VertexBufferObject> data;
    GLuint textureData;
    GLenum textureLoc;
    GLuint textureData2;
    GLenum textureLoc2;
    GLuint textureData3;
    GLenum textureLoc3;
};
struct MACRenderDataProfile
{
    std::vector<MACRenderData> data;
    int numBuffers;
    void enable(int i)
    {
        data[i].active = true;
    }
    void disable(int i)
    {
        data[i].active = false;
    }

};

class MACViewerWidget: public GridViewerWidget
{

    /*
struct RenderInfo{
    QString name;
    QGLBuffer buffer;
    int type;

};
*/

    Q_OBJECT
public:
    enum {GT_PARTICLE = GT_MARCHINGCUBE+1, GT_VELOCITY, GT_END};
    typedef typename GridViewerWidget::Scalar Scalar;
    typedef typename GridViewerWidget::Vector3 Vector3;
    using GridViewerWidget::GRID_TYPE;
    MACViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
    unsigned int m_active_grid_mask;
    int numFrames();
    void useProfile(MACRenderDataProfile & profile);
    void setSize(MACRenderData & data);
    void setRenderItems(std::vector<std::tuple<float *, int> > &);
    void setParticleRadius(float particle_radius);
protected:
    int frame=0;
    std::shared_ptr<MACRenderDataProfile> m_profile;

    std::vector<std::shared_ptr<VertexBufferObject> > m_buffers;
    unsigned int m_render_mask;
    QColor m_color;
    std::vector<int> m_indices;
    int m_frame;
    GLuint m_spriteTex;
    //Vector2 m_cursor;
    float particle_radius = 0.01;
    GLuint thick_fbo, thick_tex, depth_fbo, depth_tex, depth_tex2;
    std::unique_ptr<ShaderProgram> m_normal_program, m_thickness_program, m_smooth_program, m_render_program;
    glm::mat4 m_invProj, m_invView;


    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    virtual void resizeGL(int w, int h);
    void setMAC(int i);



    void paintGL();
    void paintGL(MACRenderData & data);
    void updateBuffers();
    void updateBuffers(int type);
    void activeProgramSelector(std::function<void (ShaderProgram &)> f);

    void initializeGL();
    void initializeTextures();
    void resizeIndices();
    void myDim(int type, int & dim0, int & dim1, int & dim2);
public slots:
    void dataChanged();
    void activeMaskChanged();
    void createNew();
    void setReady();
    void setNotReady();
    void setDim0(int);
    void setDim1(int);
    void setDim2(int);
    void setFrame(int i);
    void changeScale(int);
    void step();

signals:
    void cursorPos(Eigen::Vector2f);
    void stopCursorPolling();
    void startCursorPolling();
    void newFrameCount(int);
    void tailFrame();

};

#endif
