#include "viewer/base/viewer.h"
#include "geometry/mesh/io.h"
#include <QDoubleSpinBox>

class BallViewerObject: public QObject
{
    Q_OBJECT
    typedef NumericalTraitsXf NumTraits;
    typedef typename NumTraits::Vector3 Vector3;

    public:
        void init(QWidget * m_controls) {
            QDoubleSpinBox * radiusDSB = new QDoubleSpinBox;
            radiusDSB->setRange(0.001,1.0);
            radiusDSB->setSingleStep(0.001);
            radiusDSB->setDecimals(4);
            radiusDSB->setValue(radius);
            radius = radiusDSB->value();

            connect(radiusDSB, SIGNAL(valueChanged(double)), this, SLOT(setRadius(double)));
            m_controls->layout()->addWidget(radiusDSB);


        }
        BallViewerObject(): radius(0.002),  m_ready(false){}
        void render();
        void load(std::ifstream & infile);
        bool ready()
        {
            return m_ready;
        }
        public slots:
            void setRadius(double r){radius = r;}
    private:
        std::vector<Vector3> m_obj;
        double radius;
        bool m_ready;

};


struct BallViewerTraits
{
    typedef BallViewerObject ObjectStorage;
    static const std::string FileExtension;
    static const bool GLSLReady = false;
};

