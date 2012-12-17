#include "window.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QSlider>
#include <QComboBox>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QModelIndexList>
#include <QListView>
#include <QDockWidget>
#include <QVariant>
#include <QTimer>

MACViewerCore::MACViewerCore(std::shared_ptr<MACRenderDataProfile> profile, const QString & prefix , const QString & file_extension, QWidget * parent, Qt::WFlags flags):
    Viewer<MACViewerWidget>(prefix,file_extension,parent,flags), m_profile(profile){
    init(*profile);
}
void MACViewerCore::init(MACRenderDataProfile & profile)
{
    control = 0;
    setMenuBar(new QMenuBar);
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    //Open action
    QAction *openAct = new QAction( tr("&Open"), this );
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction *createNewAct = new QAction( tr("&New"), this );
    createNewAct->setStatusTip(tr("Create New"));
    connect(createNewAct, SIGNAL(triggered()), this, SLOT(createNewSimulation()));

    QAction *createControlsAct = new QAction( tr("&Controls"), this );
    createControlsAct->setStatusTip(tr("Create Controls"));
    connect(createControlsAct, SIGNAL(triggered()), this, SLOT(createControls()));

    //Quit action
    QAction *quitAct = new QAction( tr("&Quit"), this );
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(createNewAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(createControlsAct);
    fileMenu->addAction(quitAct);

    d0box = new QSpinBox;
    d1box = new QSpinBox;
    d2box = new QSpinBox;
    d0box->setRange(0,2000);
    d0box->setValue(20);
    d1box->setRange(0,2000);
    d1box->setValue(20);
    d2box->setRange(0,2000);
    d2box->setValue(20);
    frames = new QSpinBox;
    frames->setRange(0,m_widget->numFrames()-1);
    m_widget->setDim0(d0box->value());
    m_widget->setDim1(d1box->value());
    m_widget->setDim2(d2box->value());
    connect(d0box, SIGNAL(valueChanged(int)), m_widget, SLOT(setDim0(int)));
    connect(d1box, SIGNAL(valueChanged(int)), m_widget, SLOT(setDim1(int)));
    connect(d2box, SIGNAL(valueChanged(int)), m_widget, SLOT(setDim2(int)));
    connect(frames , SIGNAL(valueChanged(int)), this, SLOT(setFrame(int)));
    connect(m_widget, SIGNAL(tailFrame()), this, SLOT(tailFrame()));

    //m_data.push_back(MACFluid2(d0box->value(), d1box->value()));
    m_widget->useProfile(profile);

    connect(this, SIGNAL(activeMaskChange()), m_widget, SLOT(activeMaskChanged()));
    connect(this, SIGNAL(changingProfile()), m_widget, SLOT(setNotReady()));
    connect(this, SIGNAL(profileChanged()), m_widget, SLOT(setReady()));
    doAutoStep = new QCheckBox(tr("AutoStep"));
    printCursorData = new QCheckBox(tr("Print Cursor Data"));
    saveFrames = new QCheckBox(tr("Save Frames"));
    QTimer * timer = new QTimer(this);
    connect( timer, SIGNAL( timeout() ), this,  SLOT( autostep() ) );
    timer->start(16);

}
void MACViewerCore::createControls()
{
    if(control)
    {
        control->show();
        return;
    }
    control = new QWidget(this);
    QVBoxLayout * layout = new QVBoxLayout;
    control->setLayout(layout);
    QDockWidget * dock = new QDockWidget(tr("Controls"), this);
    dock->setWidget(control);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    control->show();





    QGroupBox * gridTypes = new QGroupBox(tr("Grid Types"));

    QVBoxLayout * gridtypelayout = new QVBoxLayout;

    listview = new QListView();
    connect(listview, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectedChanged(const QModelIndex &)));
    listview->setSelectionMode(QListView::MultiSelection);
    QStandardItemModel* model = new QStandardItemModel();
    for(int i=0; i<m_profile->data.size(); ++i)
    {
        QStandardItem * item = new QStandardItem(m_profile->data[i].title);
        item->setData(QVariant(i));

        model->appendRow(item);
    }
    listview->setModel(model);

    gridtypelayout->addWidget(listview);
    gridTypes->setLayout(gridtypelayout);
    control->layout()->addWidget(gridTypes);
    QCheckBox * texturePoint = new QCheckBox(tr("Toggle Recording"));
    QObject::connect(texturePoint, SIGNAL(toggled(bool)), m_widget, SLOT(toggleRecording(bool)));
    control->layout()->addWidget(texturePoint);







    QGroupBox * dimBox = new QGroupBox(tr("Dimension"));
    QHBoxLayout * dimLayout = new QHBoxLayout;


    dimLayout->addWidget(d0box);
    dimLayout->addWidget(d1box);
    dimLayout->addWidget(d2box);

    dimBox->setLayout(dimLayout);
    control->layout()->addWidget(dimBox);



















    QPushButton *colorButton = new QPushButton(tr("Choose model color"));
    connect(colorButton, SIGNAL(clicked()), m_widget, SLOT(setModelColor()));
    control->layout()->addWidget(colorButton);

    shaderName = new QLineEdit(tr("shader prefix"));
    connect(shaderName, SIGNAL(returnPressed()), this, SLOT(newShaderPrefix()));
    control->layout()->addWidget(shaderName);
    QSlider * scaleSlider = new QSlider();
    connect(scaleSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(changeScale(int)));
    control->layout()->addWidget(scaleSlider);


    QSlider * isoSlider = new QSlider();
    isoSlider->setMinimum(-2);
    isoSlider->setMaximum(255);
    isoSlider->setSingleStep(1);
    connect(isoSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(isoChange(int)));
    control->layout()->addWidget(isoSlider);

    QGroupBox * simControlBox = new QGroupBox(tr("Simulation Controls"));

    QVBoxLayout * simControlLayout = new QVBoxLayout;
    simControlBox->setLayout(simControlLayout);
    QPushButton *stepButton = new QPushButton(tr("Step"));
    connect(stepButton, SIGNAL(clicked()), this, SLOT(step()));
    simControlLayout->addWidget(stepButton);


    simControlLayout->addWidget(frames);
    multiStepSpinner = new QSpinBox;
    simControlLayout->addWidget(multiStepSpinner);
    QPushButton *multiStepButtonButton = new QPushButton(tr("Step"));
    connect(multiStepButtonButton, SIGNAL(clicked()), this, SLOT(multiStep()));
    simControlLayout->addWidget(multiStepButtonButton);
    simControlLayout->addWidget(doAutoStep);
    simControlLayout->addWidget(saveFrames);
    simControlLayout->addWidget(printCursorData);
    control->layout()->addWidget(simControlBox);



}

void MACViewerCore::autostep()
{
if(doAutoStep->checkState()) { step();}
}

void MACViewerCore::newShaderPrefix()
{
    m_widget->setShaderProgram(shaderName->text());
}


void MACViewerCore::selectedChanged(const QModelIndex & index)
{
    emit changingProfile();
    QModelIndexList selected = listview->selectionModel()->selectedIndexes();
    int mask=0;
    for(auto it=m_profile->data.begin(); it != m_profile->data.end(); ++it)
    {
        it->active=false;
    }
    for(int i=0; i<selected.size(); ++i)
    {

        int index = (static_cast<QStandardItemModel*>(listview->model())->itemFromIndex(selected[i])->data().toInt());
        m_profile->data[index].active = true;
        mask |= 1 << m_profile->data[index].renderType;

    }
    emit activeMaskChange();
}
void MACViewerCore::multiStep()
{
    for(int i=0; i<multiStepSpinner->value(); ++i)
        step();
}
