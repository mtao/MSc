#include "textureviewer.h"
#include <QColorDialog>
#include <QResource>
#include "texture.h"
#include "geometry/marchingcubes/marchingcubes.h"

TextureViewerWidget::TextureViewerWidget(const QGLFormat & format, const QString & prefix, QWidget * parent): ObjectViewerWidget(format,prefix,parent),

    m_color(255,255,0)

{
    QResource::registerResource("textureviewer.qrc");
    m_ready = true;
    m_points.push_back(Vector3(0,0,0.0));
    m_points.push_back(Vector3(0,1,0.0));
    m_points.push_back(Vector3(1,1,0.0));
    m_points.push_back(Vector3(1,0,0.0));
}
void TextureViewerWidget::initializeGL()
{

    glEnable(GL_TEXTURE_2D);
    ObjectViewerWidget::initializeGL();
    unsigned char table[] = {
        255,  0,  0,
        0  ,255,  0,
        0  ,  0,255,
        255,255,255
    };
    glGenTextures(1, &m_testTex);
    glBindTexture(GL_TEXTURE_2D, m_testTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            2,
            2,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            table);




    glGenTextures(1, &m_testTex2);

    glBindTexture(GL_TEXTURE_2D, m_testTex2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 256, 16, 0, GL_ALPHA_INTEGER_EXT, GL_INT, MarchingCubeGridf::m_triTable);
    /*
    for(int i=0; i < 256; ++i)
    {
        for(int j = 0; j < 16; ++j)
    std::cout << MarchingCubeGridf::m_triTable[i][j] << " " ;
        std::cout << std::endl;
    }
    */
    /*
    int inttable[3][3] = 
    {
        {0, 1, 2},
        {2, 0, 1},
        {1, 2, 0},

    };
    glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA16I_EXT, 3,3, 0, GL_ALPHA_INTEGER_EXT, GL_INT, inttable);
    */


    m_vertexBuffer.reset(new VertexBufferObject(m_points.data(), sizeof(Vector3)/sizeof(float)*m_points.size()));


    m_shader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_testTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_testTex2);
    m_shader->release();
    updateBuffers();

}
void TextureViewerWidget::updateBuffers()
{
    qWarning() << "Binding Buffers";
    m_shader->bind();
    if(m_vertexBuffer)
    {
        qWarning() << "yes";
        m_shader->addAttribute("vertex", m_vertexBuffer);
    }
    else
    {
        qWarning() << "no";
    }
    glUniform4f(glGetUniformLocation(m_shader->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
    glUniform1i(glGetUniformLocation(m_shader->programId, "tex"),0);
    glUniform1i(glGetUniformLocation(m_shader->programId, "tex2"),1);
    m_shader->release();
    ObjectViewerWidget::updateBuffers();

    m_ready = true;
}
void TextureViewerWidget::paintGL()
{
    if(!m_ready)
        return;

    setModelMatrix();
    ObjectViewerWidget::paintGL();
    m_shader->bind();
    //qWarning() << m_shader->attributes.size();

    glDrawArrays(GL_QUADS, 0, m_points.size());



    // Draw stuff

    m_shader->release();


}
void TextureViewerWidget::setModelColor()
{
    const QColor color = QColorDialog::getColor(m_color);
    if (color.isValid()) 
    {
        m_color = color;
        m_shader->bind(false);
    glUniform4f(glGetUniformLocation(m_shader->programId, "u_color"),
                m_color.red()/255.0, m_color.green()/255.0f, m_color.blue()/255.0f, m_color.alpha()/255.0f );
        m_shader->release();
        update();
    }
}
