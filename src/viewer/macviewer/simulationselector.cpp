#include "simulation/stablefluids/3d/sim.h"
#include "simulation/stablefluids_free_surface/3d/sim.h"
#include "simulation/flip/3d/sim.h"
#include "simulation/uic/3d/sim.h"
#include "simulation/darcy/3d/sim.h"
/*
#include "simulation/stablefluids_density/3d/sim.h"
#include "simulation/stablefluids_viscosity/3d/sim.h"
#include "simulation/stablefluids_freesurface_viscosity/3d/sim.h"
*/
#include "simulationselector.h"
#include <QtGui/QRadioButton>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QStandardItemModel>

template <typename Viewer, typename B>
void startViewer(MACRenderDataProfile profile, B * parent)
{
    std::shared_ptr<MACRenderDataProfile> p(new MACRenderDataProfile);
    *p = profile;
    Viewer * viewer = new Viewer(p, parent);
    viewer->show();
    viewer->start();

}

//int name_size = 7;
int name_size = 5;
QString names[] =
{
    "Stable Fluids",
    "Stable Fluids Free Surface",
    "FLIP",
    "UIC",
    "Darcy",
    "Stable Fluids Density",
    "Stable Fluids Viscosity"
};
void SimulationSelector::select(int i)
{
    switch(i){
    case 0:
        startViewer< MACViewer<MACStableFluids, MACFluid> > (dataProfile,this);
        break;
    case 1:
        startViewer< MACViewer<MACStableFluidsFreeSurface, MACFluid> > (dataProfile,this);
        break;
    case 2:
        startViewer< MACViewer<MACFLIP, MACFluid> > (dataProfile,this);
        break;
    case 3:
        startViewer< MACViewer<UIC, MACFluid> > (dataProfile,this);
        break;
    case 4:
        startViewer< MACViewer<MACDarcy, MACFluid> > (dataProfile,this);
        break;
        /*
    case 5:
        startViewer< MACViewer<MACStableFluidsDensity2, MACFluid2> > (dataProfile,this);
        break;
    case 6:
        startViewer< MACViewer<MACStableFluidsViscosity2, MACFluid2> > (dataProfile,this);
        break;
        */
    default:
        break;
    }
}

SimulationSelector::SimulationSelector(QWidget *parent) :
    QWidget(parent)
{
    int i = 0;
    MACRenderData data[] =
    {
        MACRenderData("Liquid Phi"       , 0 , MACViewerWidget::GT_CELL        ),//0
        MACRenderData("U Flux"           , 1 , MACViewerWidget::GT_UFLUX       ),//1
        MACRenderData("V Flux"           , 2 , MACViewerWidget::GT_VFLUX       ),//2
        MACRenderData("W Flux"           , 3 , MACViewerWidget::GT_WFLUX       ),//3
        MACRenderData("U Weights"        , 4 , MACViewerWidget::GT_UFLUX       ),//4
        MACRenderData("V Weights"        , 5 , MACViewerWidget::GT_VFLUX       ),//5
        MACRenderData("W Weights"        , 6 , MACViewerWidget::GT_WFLUX       ),//6
        MACRenderData("Pressure"         , 7 , MACViewerWidget::GT_CELL        ),//7
        MACRenderData("RHS"              , 8 , MACViewerWidget::GT_CELL        ),//8
        MACRenderData("Solid Phi"        , 9 , MACViewerWidget::GT_VERTEX      ),//9
        MACRenderData("Solid Boundary"   , 9 , MACViewerWidget::GT_PARTICLE    ),//10
        //MACRenderData("Liquid Surface"   , 0 , MACViewerWidget::GT_VERTEX ),//11
        MACRenderData("Liquid Surface"   , 0 , MACViewerWidget::GT_MARCHINGCUBE        ),//11
        MACRenderData("Particles"        , 11 , MACViewerWidget::GT_PARTICLE    ),//12
        MACRenderData("Velocity"         , 1 , MACViewerWidget::GT_VELOCITY    ),//13
        MACRenderData("Vorticity"        , 10 , MACViewerWidget::GT_VERTEX      ),//14
        MACRenderData("Weight Velcoity "        , 4 , MACViewerWidget::GT_VELOCITY      ),//15


    };
    dataProfile.data.assign(data,data+sizeof(data)/sizeof(MACRenderData));
    int max=0;
    for(auto it = dataProfile.data.begin(); it != dataProfile.data.end(); ++it)
    {
        max = std::max(it->index, max);
    }
    dataProfile.numBuffers=max+1;


    QGroupBox * simTypes = new QGroupBox("Simulation types",this);
    QVBoxLayout * simTypeLayout = new QVBoxLayout;

    listview = new QListView();
    connect(listview, SIGNAL(clicked(const QModelIndex &)), this, SLOT(selectedChanged(const QModelIndex &)));

    QStandardItemModel* model = new QStandardItemModel();
    for(int i=0; i<name_size; ++i)
    {
        QStandardItem * item = new QStandardItem(names[i]);
        item->setData(QVariant(i));

        model->appendRow(item);
    }
    listview->setModel(model);
    simTypeLayout->addWidget(listview);
    simTypes->setLayout(simTypeLayout);
    QVBoxLayout * layout = new QVBoxLayout;
    layout ->addWidget(simTypes);
    this->setLayout(layout);
    QPushButton * button = new QPushButton("Create Simulation", this);
    connect(button, SIGNAL(clicked()), this, SLOT(createSim()));
    layout->addWidget(button);




















}
void SimulationSelector::createSim()
{
    select(m_selected);
}
void SimulationSelector::selectedChanged(const QModelIndex & index)
{
    QModelIndexList selected = listview->selectionModel()->selectedIndexes();
    for(int i=0; i<selected.size(); ++i)
    {

        int index = (static_cast<QStandardItemModel*>(listview->model())->itemFromIndex(selected[i])->data().toInt());
        m_selected = index;
        qWarning() << m_selected;

    }
}
