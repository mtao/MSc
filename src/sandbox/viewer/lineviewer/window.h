#include "viewer/viewerbase/viewer.h"
#include "lineviewer.h"

class QLineEdit;
class QCheckBox;
class LineViewer: public Viewer<LineViewerWidget>
{
    Q_OBJECT
    public:
LineViewer(const QString & prefix = ":/shader", const QString & file_extension="*.obj", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
        QCheckBox * wireframe;
        QCheckBox * normals;
        QCheckBox * culling;
};

