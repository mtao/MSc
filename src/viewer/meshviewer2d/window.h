#include "viewer/viewerbase/viewer.h"
#include "meshviewer.h"

class QLineEdit;
class QCheckBox;
class MeshViewer: public Viewer<MeshViewerWidget>
{
    Q_OBJECT
    public:
MeshViewer(const QString & prefix = ":/shader", const QString & file_extension="*.obj", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
};

