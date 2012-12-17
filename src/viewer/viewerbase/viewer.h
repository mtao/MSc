#ifndef VIEWER_H
#define VIEWER_H
#include "window.h"
#include "widget.h"

#include <QFileDialog>
#include <QDebug>


template <typename Widget, int MAJOR_VERSION=3, int MINOR_VERSION=3>
class Viewer: public ViewerWindow
{
    public:
        Viewer(const QString & prefix, const QString & file_extension="*", QWidget * parent=0, Qt::WFlags flags=0): ViewerWindow(file_extension,parent,flags)
        {
            init(prefix);
        }
        Viewer(const QString & file_extension="*", QWidget * parent=0, Qt::WFlags flags=0): ViewerWindow(file_extension,parent,flags)
        {
            init();
        }
        void init(const QString & prefix_=":/shader")
        {
            QString prefix = prefix_;
            QGLFormat glFormat;
            qWarning() << glFormat.openGLVersionFlags();
            qWarning() << (QGLFormat::OpenGL_Version_3_0 <= glFormat.openGLVersionFlags());
            if(QGLFormat::OpenGL_Version_3_3 & glFormat.openGLVersionFlags())
            {
                glFormat.setVersion( MAJOR_VERSION, MINOR_VERSION );
            }
            else
            {
                glFormat.setVersion( 2, 1 );
                prefix = prefix + ".130";
            }
            qWarning() << "GL Version: " << glFormat.majorVersion() << " " << glFormat.minorVersion();
            glFormat.setProfile( QGLFormat::CompatibilityProfile );
            glFormat.setSampleBuffers( true );
            m_widget = new  Widget(glFormat,prefix);
            ViewerWindow::setCentralWidget(m_widget);
        }

        void openFile(const QString &filename)
        {
            m_widget->openFile(filename);
        }

    protected:
        Widget * m_widget;

};
#endif
