#include "window.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QSlider>
#include <QComboBox>

GridViewer::GridViewer(const QString & prefix , const QString & file_extension, QWidget * parent, Qt::WFlags flags): Viewer<GridViewerWidget>(prefix,file_extension,parent,flags){init();}
void GridViewer::init()
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
void GridViewer::createControls()
{
    if(control)
        delete control;
    control = new QWidget(this, Qt::Tool);
    QVBoxLayout * layout = new QVBoxLayout;
    control->setLayout(layout);
    control->show();

    QComboBox * gridType = new QComboBox();
    gridType->insertItem(GridViewerWidget::GT_VERTEX      , "Vertex"        );
    gridType->insertItem(GridViewerWidget::GT_UFLUX       , "U Flux"        );
    gridType->insertItem(GridViewerWidget::GT_VFLUX       , "V Flux"        );
    gridType->insertItem(GridViewerWidget::GT_CELL        , "Cell"          );
    gridType->insertItem(GridViewerWidget::GT_MARCHINGSQUARE, "Marching Square");
    gridType->setCurrentIndex(GridViewerWidget::GT_VERTEX);
    QObject::connect(gridType, SIGNAL(currentIndexChanged(int)), m_widget, SLOT(gridType(int)));
    control->layout()->addWidget(gridType);

    QPushButton *colorButton = new QPushButton(tr("Choose model color"));
    connect(colorButton, SIGNAL(clicked()), m_widget, SLOT(setModelColor()));
    control->layout()->addWidget(colorButton);

    shaderName = new QLineEdit(tr("shader prefix"));
    connect(shaderName, SIGNAL(returnPressed()), this, SLOT(newShaderPrefix()));
    control->layout()->addWidget(shaderName);
    QSlider * scaleSlider = new QSlider();
    connect(scaleSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(changeScale(int)));
    control->layout()->addWidget(scaleSlider);

    QCheckBox * texturePoint = new QCheckBox(tr("Use Data Texture"));
    QObject::connect(texturePoint, SIGNAL(toggled(bool)), m_widget, SLOT(viewTexture(bool)));
    control->layout()->addWidget(texturePoint);

    QCheckBox * discardOverflow = new QCheckBox(tr("Discard Overflowing Values"));
    QObject::connect(discardOverflow, SIGNAL(toggled(bool)), m_widget, SLOT(discardOverflow(bool)));
    control->layout()->addWidget(discardOverflow);

    QCheckBox * culling = new QCheckBox(tr("Backface Culling"));
    QObject::connect(culling, SIGNAL(toggled(bool)), m_widget, SLOT(backfaceCulling(bool)));
    QObject::connect(m_widget, SIGNAL(setBackfaceCulling(bool)), culling, SLOT(setChecked(bool)));
    control->layout()->addWidget(culling);

    QSlider * isoSlider = new QSlider();
    isoSlider->setMinimum(-2);
    isoSlider->setMaximum(255);
    isoSlider->setSingleStep(1);
    connect(isoSlider, SIGNAL(valueChanged(int)), m_widget, SLOT(isoChange(int)));
    control->layout()->addWidget(isoSlider);
}

void GridViewer::newShaderPrefix()
{
    m_widget->setShaderProgram(shaderName->text());
}
