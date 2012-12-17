#ifndef SIMULATIONSELECTOR_H
#define SIMULATIONSELECTOR_H

#include <QWidget>
#include "window.h"
class QListView;

class SimulationSelector : public QWidget
{
    Q_OBJECT
public:
    explicit SimulationSelector(QWidget *parent = 0);
    void select(int i);
    protected:

    MACRenderDataProfile dataProfile;
    private:
    QListView * listview;
    int m_selected = 0;

signals:
    
public slots:
    void selectedChanged(const QModelIndex &);
    void createSim();

};

#endif // SIMULATIONSELECTOR_H
