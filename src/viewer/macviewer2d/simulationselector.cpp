#include "simulationselector.h"
#include "simulation/flip/2d/sim.h"
#include "simulation/stablefluids_density/2d/sim.h"
#include "simulation/stablefluids_viscosity/2d/sim.h"
#include "simulation/stablefluids_freesurface_viscosity/2d/sim.h"
#include "simulation/darcy/2d/sim.h"
#include "simulation/granular/2d/sim.h"
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
int name_size = 9;
QString names[] =
{
    "Stable Fluids",
    "Stable Fluids Free Surface",
    "FLIP",
    "Stable Fluids Density",
    "Darcy",
    "UIC",
    "Stable Fluids Viscosity",
    "Stable Fluids Free Surface Viscosity",
    "Granular Material"
};
void SimulationSelector::select(int i)
{
    switch(i){
    case 0:
        startViewer< MACViewer<MACStableFluids2, MACFluid2> > (dataProfile,this);
        break;
    case 1:
        startViewer< MACViewer<MACStableFluidsFreeSurface2, MACFluid2> > (dataProfile,this);
        break;
    case 2:
        startViewer< MACViewer<MACFLIP2, MACFluid2> > (dataProfile,this);
        break;
    case 3:
        startViewer< MACViewer<MACStableFluidsDensity2, MACFluid2> > (dataProfile,this);
        break;
    case 4:
        startViewer< MACViewer<MACDarcy2, MACFluid2> > (dataProfile,this);
        break;
    case 5:
        startViewer< MACViewer<UIC2, MACFluid2> > (dataProfile,this);
        break;
    case 6:
        startViewer< MACViewer<MACStableFluidsViscosity2, MACFluid2> > (dataProfile,this);
        break;
    case 7://TODO: NOT DONE
        startViewer< MACViewer<MACStableFluidsFreeSurfaceViscosity2, MACFluid2> > (dataProfile,this);
        break;
    case 8:
        startViewer< MACViewer<Granular2, MACFluid2> > (dataProfile,this);
        break;
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
        MACRenderData("U Weights"        , 3 , MACViewerWidget::GT_UFLUX       ),//3
        MACRenderData("V Weights"        , 4 , MACViewerWidget::GT_VFLUX       ),//4
        MACRenderData("Pressure"         , 5 , MACViewerWidget::GT_CELL        ),//5
        MACRenderData("RHS"              , 6 , MACViewerWidget::GT_CELL        ),//6
        MACRenderData("Solid Phi"        , 7 , MACViewerWidget::GT_VERTEX      ),//7
        MACRenderData("Solid Boundary"   , 7 , MACViewerWidget::GT_PARTICLE    ),//8
        MACRenderData("Liquid Surface"   , 0 , MACViewerWidget::GT_MARCHINGSQUARE        ),//9
        MACRenderData("Particles"        , 9 , MACViewerWidget::GT_PARTICLE    ),//10
        MACRenderData("Velocity"         , 1 , MACViewerWidget::GT_VELOCITY    ),//11
        MACRenderData("Vorticity"        , 8 , MACViewerWidget::GT_VERTEX      ),//12
        MACRenderData("Weight Velocity"  , 3 , MACViewerWidget::GT_VELOCITY    ),//13

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
