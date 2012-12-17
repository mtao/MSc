
#ifndef GRIDVIEWER_H
#define GRIDVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/processing.h"
#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>
#include "viewer/meshviewer/meshviewer.h"



class GridViewerWidget: public ObjectViewerWidget
{

    Q_OBJECT
public:
    enum GRID_TYPE { GT_VERTEX, GT_UFLUX, GT_VFLUX, GT_WFLUX, GT_DUFLUX, GT_DVFLUX, GT_DWFLUX, GT_CELL, GT_MARCHINGCUBE};
    typedef typename NumericalTraitsXf::Scalar Scalar;
    typedef typename NumericalTraitsXf::Vector3 Vector3;
    GridViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0, int numShaders=GT_MARCHINGCUBE+1);
    void openFile(const QString & filename);
    void setShaderProgram(const QString &prefix)
    {
        RenderWidget::setShaderProgram(prefix);
        m_active_shader = m_shader.get();
        updateBuffers();
    }
protected:
    std::shared_ptr<VertexBufferObject> m_indexBuffer;
    ShaderProgram * m_active_shader;
    QColor m_color;
    std::unique_ptr<Gridf>  m_grid;
    IndexSet<3> m_dim = {{1,1,1}};

    GRID_TYPE m_grid_type;
    float m_scale;
    GLuint m_edgeTableTex, m_triTableTex, m_dataFieldTex, m_testTex;
    bool m_texture_point;
    bool m_discard_overflow;
    float m_isolevel;
    std::vector<std::shared_ptr<ShaderProgram> > m_shaders;


    void updateBuffers();
    void updateBuffers(ShaderProgram & program);
    void paintGL();
    virtual void initializeGL();
    virtual void initializeTextures();
public slots:
    void setModelColor();
    void changeScale(int scale);
    void viewTexture(bool enabled);
    void backfaceCulling(bool enabled);
    void isoChange(int iso);
    void gridType(int type);
    void discardOverflow(bool enabled);
    void activeProgramSelector(std::function<void (ShaderProgram &)> f);
signals:
    void setBackfaceCulling(bool);


    /*
         * TODO:
         * data as a texture
         * push edgetable and tritable
         * push vertex decals
         * push isolevel
         * push position
         * */
};


#endif
