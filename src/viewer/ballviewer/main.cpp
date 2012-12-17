#include "ballviewer.h"

#include <QtGui>
#include <QGLWidget>

#include <iostream>
class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
    {
        setWindowTitle(tr("3D Model Viewer"));
    }

protected:
    void resizeEvent(QResizeEvent *event) {
        if (scene())
            scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        QGraphicsView::resizeEvent(event);
    }
};

int main(int argc, char **argv)
{
    QString fn(argv[1]);
    QApplication app(argc, argv);

    GraphicsView view;
    view.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view.setScene(new Viewer<BallViewerTraits>(fn));
    view.show();

    view.resize(1024, 768);

    return app.exec();
}
