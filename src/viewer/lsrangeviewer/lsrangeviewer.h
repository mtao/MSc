
#ifndef GRIDVIEWER_H
#define GRIDVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/render.h"
#include "geometry/mesh/processing.h"
#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/grid/grid.h"
#include <QSpinBox>
#include "viewer/meshviewer/meshviewer.h"



class LevelSetRangeViewerWidget: public ObjectViewerWidget
{

    Q_OBJECT
    public:
        enum GRID_TYPE { GT_VERTEX, GT_UFLUX, GT_VFLUX, GT_WFLUX, GT_CELL, GT_MARCHINGCUBE};
        typedef typename NumericalTraitsXf::Scalar Scalar;
        typedef typename NumericalTraitsXf::Vector3 Vector3;
        LevelSetRangeViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
    protected:
        std::shared_ptr<VertexBufferObject> m_indexBuffer;
        QColor m_color;
        std::vector<Vector3 > m_points;
        Gridf * m_grid;
        IndexSet<3> m_dim;

        GRID_TYPE m_grid_type;
        float m_scale;
        GLuint m_edgeTableTex, m_triTableTex, m_dataFieldTex, m_testTex;
        bool m_texture_point;
        bool m_discard_overflow;
        float m_isolevelMin;
        float m_isolevelMax;
        float m_isolevel, m_isoAdd;

        void updateBuffers();
        void paintGL();
        void initializeGL();
        void initializeTextures();
        public slots:
        void setModelColor();
        void changeScale(int scale);
        void backfaceCulling(bool enabled);
        void isoChangeMin(int iso);
        void isoChangeMax(int iso);
        signals:
        void setBackfaceCulling(bool);

        protected:
void updateMVP()
{
    ObjectViewerWidget::updateMVP();
    RenderWidget::updateMVP(*m_shader);
}
void updateMMvMvp()
{
    ObjectViewerWidget::updateMMvMvp();
    RenderWidget::updateMMvMvp(*m_shader);
}
void updateVMvMvp()
{
    ObjectViewerWidget::updateVMvMvp();
    RenderWidget::updateVMvMvp(*m_shader);
}
void updatePMvp()
{
    ObjectViewerWidget::updatePMvp();
    RenderWidget::updatePMvp(*m_shader);
}


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
