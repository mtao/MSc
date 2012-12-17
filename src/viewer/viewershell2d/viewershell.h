#ifndef VIEWERSHELL_H
#define VIEWERSHELL_H
#include "viewer/viewerbase/widget.h"
#include "geometry/util/types.h"

class QMouseEvent;


class ViewerShellWidget: public ObjectViewer2dWidget
{

    Q_OBJECT
    public:
        typedef typename NumericalTraitsXf::Scalar Scalar;
        typedef typename NumericalTraitsXf::Vector2 Vector2;
        ViewerShellWidget(const QGLFormat & format, const QString & prefix, QWidget * parent = 0);
        void openFile(const QString & filename);
    protected:
        std::unique_ptr<ShaderProgram> m_pprogram;
        QColor m_color;
        Vector2 m_cursor;
        Vector2 m_screen_cursor;

        float m_scale;

        void updateBuffers(ShaderProgram & program);
        void updateBuffers();
        void updateMVP();
        virtual void updateMMvMvp();
        virtual void updateVMvMvp();
        virtual void updatePMvp();
        void paintGL();
        virtual void initializeGL();
        void mouseMoveEvent(QMouseEvent * event);
        public slots:
        void setModelColor();
        void changeScale(int scale);
        void backfaceCulling(bool enabled);
        signals:
        void setBackfaceCulling(bool);


};


#endif
