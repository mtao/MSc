#include "viewer/viewerbase/viewer.h"
#include "bloatviewer.h"

class QLineEdit;
class QCheckBox;
class BloatViewer: public Viewer<BloatViewerWidget>
{
    Q_OBJECT
    public:
BloatViewer(const QString & prefix = ":/shader", const QString & file_extension="*.obj", QWidget * parent=0, Qt::WFlags flags=0);
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

