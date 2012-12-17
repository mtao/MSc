#include "viewer/viewerbase/viewer.h"
#include "gridviewer.h"

class QLineEdit;
class QCheckBox;
class GridViewer: public Viewer<GridViewerWidget>
{
    Q_OBJECT
    public:
GridViewer(const QString & prefix = ":/shader", const QString & file_extension="*.ls", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
};

