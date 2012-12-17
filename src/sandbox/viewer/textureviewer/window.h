#include "viewer/viewerbase/viewer.h"
#include "textureviewer.h"

class QLineEdit;
class QCheckBox;
class TextureViewer: public Viewer<TextureViewerWidget>
{
    Q_OBJECT
    public:
TextureViewer(const QString & prefix = ":/shader", const QString & file_extension="*.obj", QWidget * parent=0, Qt::WFlags flags=0);
        public slots:
            void createControls();
        void newShaderPrefix();
    private:
        void init();
        QLineEdit * shaderName;
        QWidget * control;
};

