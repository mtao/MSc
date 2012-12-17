
#ifndef GRIDVIEWER_H
#define GRIDVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/render.h"
#include "geometry/mesh/processing.h"
#include "geometry/surface_reconstruction/marchingsquares.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>



class GridViewerWidget: public ObjectViewer2dWidget
{

    Q_OBJECT
public:
    enum GRID_TYPE {
        GT_VERTEX
        , GT_UFLUX
        , GT_VFLUX
        , GT_CELL
        , GT_MARCHINGSQUARE
    };
    typedef typename NumericalTraitsXf::Scalar Scalar;
    typedef typename NumericalTraitsXf::Vector2 Vector2;
    GridViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0, int numShaders=GT_MARCHINGSQUARE+1);
    void openFile(const QString & filename);
protected:
    std::shared_ptr<VertexBufferObject> m_indexBuffer;
    std::shared_ptr<VertexBufferObject> m_phiBuffer;
    ShaderProgram * m_active_shader;
    QColor m_color;
    std::unique_ptr<Gridf> m_grid;
    IndexSet<2> m_dim = {{1,1}};

    GRID_TYPE m_grid_type;
    float m_scale;
    GLuint m_edgeTableTex, m_dataFieldTex, m_testTex;
    bool m_texture_point;
    bool m_discard_overflow;
    float m_isolevel;
    std::vector< std::shared_ptr<ShaderProgram> > m_shaders;

    void updateBuffers(ShaderProgram & program);
    virtual void updateBuffers();
    void paintGL();
    void initializeGL();
    void initializeTextures();
    virtual void updateMVP();
    virtual void updatePMvp();
    virtual void updateMMvMvp();
    virtual void updateVMvMvp();
public slots:
    void setModelColor();
    void changeScale(int scale);
    void isoChange(int iso);
    void gridType(int type);
    void viewTexture(bool view);
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
