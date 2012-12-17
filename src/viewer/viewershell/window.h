#include "viewer/viewerbase/viewer.h"
#include "viewershell.h"

class QLineEdit;
class QCheckBox;
class ViewerShell: public Viewer<ViewerShellWidget>
{
    Q_OBJECT
    public:
ViewerShell(const QString & prefix = ":/shader", const QString & file_extension="*.*", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
};

