#include "viewer/viewerbase/viewer.h"
#include "pointviewer.h"

class QLineEdit;
class QCheckBox;
class PointViewer: public Viewer<PointViewerWidget>
{
    Q_OBJECT
    public:
PointViewer(const QString & prefix = ":/shader", const QString & file_extension="*.obj", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
};

