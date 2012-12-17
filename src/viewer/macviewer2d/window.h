#include "viewer/viewerbase/viewer.h"
#include "macviewer.h"

#include <QCheckBox>
class QLineEdit;
class QListView;
class MACViewerCore: public Viewer<MACViewerWidget>
{
    Q_OBJECT
public:
    MACViewerCore(std::shared_ptr<MACRenderDataProfile> profile, const QString & prefix = ":mac/shader", const QString & file_extension="*.ls", QWidget * parent=0, Qt::WFlags flags=0);
public slots:
    void createControls();
    void selectedChanged(const QModelIndex &);
    void newShaderPrefix();
    void multiStep();
    virtual void createNewSimulation() = 0;
    virtual void tailFrame()  = 0;
    virtual void setFrame(int) = 0;
    virtual void step() = 0;
    virtual void readCursor(Eigen::Vector2f) = 0;
    virtual void stopCursorPolling() {sendPosToSim = false;}
    virtual void startCursorPolling() {sendPosToSim = true;}
    void autostep();
signals:
    void setMask(int);
    void frameChanged(int);
    void newFrame();
    void activeMaskChange();
    void changingProfile();
    void profileChanged();
protected:
    void init(MACRenderDataProfile &);
    QLineEdit * shaderName;
    QWidget * control;
    char tmpmask;
    QCheckBox * doAutoStep;
    QCheckBox * saveFrames;
    QCheckBox * printCursorData;
    QListView * listview;
    QSpinBox * d0box;
    QSpinBox * d1box;
    QSpinBox * frames;
    QSpinBox * multiStepSpinner;
    std::shared_ptr<MACRenderDataProfile> m_profile;
    bool sendPosToSim = false;

};


template <typename Simulator, class DataStorage>
class MACViewer: public MACViewerCore
{

public:
    MACViewer(std::shared_ptr<MACRenderDataProfile> profile, const QString & prefix = ":mac/shader", const QString & file_extension="*.ls", QWidget * parent=0, Qt::WFlags flags=0):
        MACViewerCore(profile, prefix, file_extension, parent, flags)
    {
    }
    MACViewer(std::shared_ptr<MACRenderDataProfile> profile, QWidget * parent):
        MACViewerCore(profile, ":mac/shader", ".ls", parent, 0)
    {
    }
    void start(){createNewSimulation();}
protected:

    void createNewSimulation()
    {
        qWarning() << "Creating new Simulation object" << d0box->value() << " " << d1box->value();
        m_sim.reset(new Simulator(d0box->value(), d1box->value()));

        m_data.clear();
        m_data.push_back(m_sim->getState());
        setFrame(0);
        connect(m_widget, SIGNAL(cursorPos(Eigen::Vector2f)), this, SLOT(readCursor(Eigen::Vector2f)));
        connect(m_widget, SIGNAL(stopCursorPolling()), this, SLOT(stopCursorPolling()));
        connect(m_widget, SIGNAL(startCursorPolling()), this, SLOT(startCursorPolling()));
        qWarning() << "Done with new simulation object";
    }
    void step()
    {
        if(!m_sim) createNewSimulation();
        //qWarning() << "Stepping";
        m_sim->step(0.02);

        if(saveFrames->checkState())
        {
            m_data.push_back(m_sim->getState());
            qWarning() << "Number of frames: " << m_data.size();
            frames->setMaximum(m_data.size()-1);
            if(frames->value()+1 == frames->maximum())
            {
                tailFrame();
            }
        }
        else
        {
            if(m_data.capacity()!=1)
            {
                qWarning() << m_data.capacity();
                m_data.resize(1);
                m_data.shrink_to_fit();
            }
            if(m_data.capacity()==0)
                m_data.resize(1);
            m_data[0]=m_sim->getState();
            frames->setMaximum(0);
            setFrame(0);
        }
    }

    void tailFrame()
    {
        //qWarning() << "Setting frame to max: " << frames->maximum();
        frames->setValue(frames->maximum());
        setFrame(frames->value());
    }
    void setFrame(int i)
    {
        if(m_data.size() == 0)
            return;
        emit changingProfile();
        //qWarning() << "Switching to frame " << i;
        auto tuples = m_data[i].getDataTuples();
        m_widget->setRenderItems(tuples);
        m_widget->setParticleRadius(m_data[i].particle_radius);
        emit profileChanged();


    }
    void readCursor(Eigen::Vector2f v)
    {
        if(sendPosToSim && m_sim)
        {
            m_sim->cursorPos(v);
        }
        if(m_data.size()>0 && printCursorData->checkState())
        {
            float fi,fj;
            uint i,j;
            auto frame = m_data[frames->value()];
            /*
            frame.v.barycentric(v,i,fi,j,fj);
            std::cout << "["<< v(0) << " " << v(1) << "]:"
                      <<"(barycentric:"  << i << "," << j << "+" << fi << "," << fj <<")"
                     <<"(sphi:"  << frame.sphi.lerp(v) <<")"
                    <<"(lphi:"  << frame.lphi.lerp(v) <<")"
                   <<"(velocity:"  << frame.u.lerp(v) << "," << frame.v.lerp(v) <<")"
                  << std::endl;
                  */
        }
    }
    void startCursorPolling()
    {
        if(m_sim)
        m_sim->enablePoll();
        MACViewerCore::startCursorPolling();
    }
    void stopCursorPolling()
    {
        if(m_sim)
        m_sim->disablePoll();
        MACViewerCore::stopCursorPolling();
    }
private:
    std::unique_ptr<Simulator> m_sim;
    std::vector<DataStorage> m_data;
};

