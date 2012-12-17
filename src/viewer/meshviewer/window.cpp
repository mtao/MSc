#include "window.h"
#include <QPushButton>
#include <QCheckBox>
#include <QLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QLineEdit>
#include <QComboBox>
#include <QDockWidget>

MeshViewer::MeshViewer(const QString & prefix , const QString & file_extension, QWidget * parent, Qt::WFlags flags): Viewer<MeshViewerWidget>(prefix,file_extension,parent,flags){init();}

void MeshViewer::init()
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
void MeshViewer::createControls()
{
    if(control)
        delete control;
    control = new QWidget(this, Qt::Tool);
    QVBoxLayout * layout = new QVBoxLayout;
    QDockWidget * dock = new QDockWidget(tr("Controls"), this);
    dock->setWidget(control);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    control->setLayout(layout);
    control->show();

    drawMode = new QComboBox();
    drawMode->insertItem(MeshViewerWidget::DM_Point       , "GL_POINTS"     );
    drawMode->insertItem(MeshViewerWidget::DM_Line        , "GL_LINES"      );
    drawMode->insertItem(MeshViewerWidget::DM_Triangle    , "GL_TRIANGLES"  );
    drawMode->setCurrentIndex(MeshViewerWidget::DM_Triangle);
    QObject::connect(drawMode, SIGNAL(currentIndexChanged(int)), m_widget, SLOT(setDrawMode(int)));
    control->layout()->addWidget(drawMode);

    normals = new QCheckBox(tr("Display normals vectors"));
    QObject::connect(normals, SIGNAL(toggled(bool)), m_widget, SLOT(enableNormals(bool)));
    control->layout()->addWidget(normals);

    culling = new QCheckBox(tr("Backface Culling"));
    QObject::connect(culling, SIGNAL(toggled(bool)), m_widget, SLOT(enableBackfaceCulling(bool)));
    control->layout()->addWidget(culling);

    QPushButton *colorButton = new QPushButton(tr("Choose model color"));
    connect(colorButton, SIGNAL(clicked()), m_widget, SLOT(setModelColor()));
    control->layout()->addWidget(colorButton);

    shaderName = new QLineEdit(tr("shader prefix"));
    connect(shaderName, SIGNAL(returnPressed()), this, SLOT(newShaderPrefix()));
    control->layout()->addWidget(shaderName);

}

void MeshViewer::newShaderPrefix()
{
    m_widget->setShaderProgram(shaderName->text());
}
