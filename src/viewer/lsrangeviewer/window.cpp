#include "window.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QSlider>
#include <QComboBox>

LevelSetRangeViewer::LevelSetRangeViewer(const QString & prefix , const QString & file_extension, QWidget * parent, Qt::WFlags flags): Viewer<LevelSetRangeViewerWidget>(prefix,file_extension,parent,flags){init();}
void LevelSetRangeViewer::init()
{
    control=0;
    shaderName=0;
    setMenuBar(new QMenuBar);
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    //Open action
    QAction *openAct = new QAction( tr("&Open"), this );
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    QAction *createControlsAct = new QAction( tr("&Controls"), this );
    createControlsAct->setStatusTip(tr("Create Controls"));
    connect(createControlsAct, SIGNAL(triggered()), this, SLOT(createControls()));

    //Quit action
    QAction *quitAct = new QAction( tr("&Quit"), this );
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(openAct);
    fileMenu->addAction(createControlsAct);
    fileMenu->addAction(quitAct);
}
void LevelSetRangeViewer::createControls()
{
    if(control)
        delete control;
    control = new QWidget(this, Qt::Tool);
    QVBoxLayout * layout = new QVBoxLayout;
    control->setLayout(layout);
    control->show();


    QPushButton *colorButton = new QPushButton(tr("Choose model color"));
    connect(colorButton, SIGNAL(clicked()), m_widget, SLOT(setModelColor()));
    control->layout()->addWidget(colorButton);

    QSlider * scaleSlider = new QSlider();
    connect(scaleSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(changeScale(int)));
    control->layout()->addWidget(scaleSlider);


    QCheckBox * culling = new QCheckBox(tr("Backface Culling"));
    QObject::connect(culling, SIGNAL(toggled(bool)), m_widget, SLOT(backfaceCulling(bool)));
    QObject::connect(m_widget, SIGNAL(setBackfaceCulling(bool)), culling, SLOT(setChecked(bool)));
    control->layout()->addWidget(culling);
    culling->setChecked(true);

    QSpinBox * isoMinSlider = new QSpinBox();
    isoMinSlider->setMinimum(0);
    isoMinSlider->setMaximum(255);
    isoMinSlider->setSingleStep(1);
    connect(isoMinSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(isoChangeMin(int)));
    control->layout()->addWidget(isoMinSlider);
    QSpinBox * isoMaxSlider = new QSpinBox();
    isoMaxSlider->setMinimum(0);
    isoMaxSlider->setMaximum(255);
    isoMaxSlider->setSingleStep(1);
    connect(isoMaxSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(isoChangeMax(int)));
    control->layout()->addWidget(isoMaxSlider);
}

void LevelSetRangeViewer::newShaderPrefix()
{
    m_widget->setShaderProgram(shaderName->text());
}
