#ifndef VIEWERSHELL_H
#define VIEWERSHELL_H
#include "viewer/viewerbase/widget.h"
#include "geometry/util/types.h"



class ViewerShellWidget: public ObjectViewerWidget
{

    Q_OBJECT
    public:
        typedef typename NumericalTraitsXf::Scalar Scalar;
        typedef typename NumericalTraitsXf::Vector3 Vector3;
        ViewerShellWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
    protected:
        QColor m_color;

        float m_scale;

        void updateBuffers(ShaderProgram & program);
        void updateBuffers();
        void updateMVP();
        virtual void updateMMvMvp();
        virtual void updateVMvMvp();
        virtual void updatePMvp();
        void paintGL();
        virtual void initializeGL();
        public slots:
        void setModelColor();
        void changeScale(int scale);
        void backfaceCulling(bool enabled);
        signals:
        void setBackfaceCulling(bool);


};


#endif
