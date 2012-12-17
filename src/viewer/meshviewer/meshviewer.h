#ifndef MESHVIEWER_H
#define MESHVIEWER_H
#include "viewer/viewerbase/viewer.h"
#include "geometry/mesh/io.h"
#include "geometry/mesh/processing.h"




class MeshViewerWidget: public ObjectViewerWidget
{

    Q_OBJECT
    public:
        enum DrawModes {DM_Point, DM_Line, DM_Triangle};
        typedef typename Meshf::NumTraits::Vector3 Vector3;
        MeshViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
        ~MeshViewerWidget(){if(m_obj) delete m_obj;}

    protected:
        void paintGL();
    public:
        bool wireframe, normals;
        public slots:
            void setDrawMode(int index);
        void enableNormals(bool);
        void enableBackfaceCulling(bool);
        void setModelColor();
        void updateBuffers();

    protected:
        Meshf * m_obj;
        std::shared_ptr<VertexBufferObject> m_vertexBuffer;
        std::shared_ptr<VertexIndexObject> m_indexBuffer;
        std::shared_ptr<VertexBufferObject> m_normalBuffer;
        QColor m_color;
        GLenum m_draw_mode;



};


#endif
