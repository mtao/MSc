#include "bloatviewer.h"
#include <QResource>

BloatViewerWidget::BloatViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): MeshViewerWidget(format,prefix,parent) {qWarning() << QResource::registerResource("qrc_bloatviewer.cc");}
void BloatViewerWidget::sendBuffers()
{
    m_shader->bind();
    m_shader->setUniformValue( "bloat", bloat );
    m_shader->release();
    MeshViewerWidget::sendBuffers();
}


void BloatViewerWidget::bloatChange(int change)
{
    bloat = change/10000.0;
    m_shader->bind();
    m_shader->setUniformValue( "bloat", bloat );
    m_shader->release();
}

