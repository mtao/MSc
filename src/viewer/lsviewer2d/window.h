#include "viewer/viewerbase/viewer.h"
#include "lsviewer.h"

class QLineEdit;
class QCheckBox;
class LevelSetViewer: public Viewer<LevelSetViewerWidget>
{
    Q_OBJECT
    public:
LevelSetViewer(const QString & prefix = ":lsviewer2d/shader", const QString & file_extension="*.obj", QWidget * parent=0, Qt::WFlags flags=0);
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

