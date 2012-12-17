#ifndef VIEWERBASE_H
#define VIEWERBASE_H
#include <QtGui/QMainWindow>

class ViewerWindow: public QMainWindow
{
    Q_OBJECT;
    public:
        ViewerWindow(const QString & file_extension="*",QWidget * parent = 0, Qt::WFlags flags = 0);
    public slots:
        virtual void openFile();
        virtual void openFile(const QString & filename);
    private:
        void init();
        QString m_file_extension;
};

#endif
