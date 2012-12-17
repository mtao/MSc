#include "viewer/viewerbase/viewer.h"
#include "lsrangeviewer.h"

class QLineEdit;
class QCheckBox;
class LevelSetRangeViewer: public Viewer<LevelSetRangeViewerWidget>
{
    Q_OBJECT
    public:
LevelSetRangeViewer(const QString & prefix = ":/shader", const QString & file_extension="*.ls", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
};

