
#include "window.h"
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>

ViewerWindow::ViewerWindow(const QString & file_extension,QWidget * parent, Qt::WFlags flags): QMainWindow(parent,flags), m_file_extension(file_extension)
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));


    //Open action
    QAction *openAct = new QAction( tr("&Open"), this );
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));


    //Quit action
    QAction *quitAct = new QAction( tr("&Quit"), this );
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    fileMenu->addAction(openAct);
    fileMenu->addAction(quitAct);

}
void ViewerWindow::openFile()
{
    openFile(QFileDialog::getOpenFileName(0, tr("Choose file"), QString(), m_file_extension));
}
void ViewerWindow::openFile(const QString & filename)
{
    qWarning() << "Dummy file open: " << filename.toStdString().c_str();
}
