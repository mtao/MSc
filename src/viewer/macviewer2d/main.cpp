//#include "window.h"
#include "simulationselector.h"

#include <QtGui>
#include <QGLWidget>

#include <iostream>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MACRenderDataProfile dataProfile;
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
        MACRenderData("Vorticity"        , 8 , MACViewerWidget::GT_VERTEX      ),//11

    };
    dataProfile.data.assign(data,data+sizeof(data)/sizeof(MACRenderData));
    int max=0;
    for(auto it = dataProfile.data.begin(); it != dataProfile.data.end(); ++it)
    {
        max = std::max(it->index, max);
    }
    dataProfile.numBuffers=max+1;
    qWarning() << "Number of buffers: "<< dataProfile.numBuffers;



    /*
    int i=0;
    if(argc>1)
        i = atoi(argv[1]);
    qWarning() << i;


    switch(i){
    case 0:
        startViewer< MACViewer<MACStableFluids2, MACFluid2>> (argc,argv,dataProfile);
        break;
    case 1:
        startViewer< MACViewer<MACStableFluidsFreeSurface2, MACFluid2> > (argc,argv,dataProfile);
        break;
    case 2:
        startViewer< MACViewer<MACFLIP2, MACFluid2> > (argc,argv,dataProfile);
        break;
    case 3:
        startViewer< MACViewer<MACStableFluidsDensity2, MACFluid2> > (argc,argv,dataProfile);
        break;
    case 4:
        startViewer< MACViewer<MACDarcy2, MACFluid2> > (argc,argv,dataProfile);
        break;
    case 5:
        startViewer< MACViewer<UIC2, MACFluid2> > (argc,argv,dataProfile);
        break;
    case 6:
        startViewer< MACViewer<MACStableFluidsViscosity2, MACFluid2> > (argc,argv,dataProfile);
        break;
    case 7:
        startViewer< MACViewer<MACStableFluidsFreeSurfaceViscosity2, MACFluid2> > (argc,argv,dataProfile);
        break;
    default:
        break;
    }
    */

    SimulationSelector * selector = new SimulationSelector();
    int i=-1;
    if(argc>1)
        i = atoi(argv[1]);
    qWarning() << i;
    if(i < 0)
    selector->show();
    else
        selector->select(i);


    qWarning() << "Exec";
    return app.exec();
}
