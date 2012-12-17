#include "geometry/grid/mac/mac.h"
#include <iostream>

/*
class MACTest: public MACf
{
    public:
        
        MACTest(): MACf(10,10,10)
    {
        CGridf cgrid = grid<CGridf>();
        UGridf ugrid = grid<UGridf>();
        VGridf vgrid = grid<VGridf>();
        WGridf wgrid = grid<WGridf>();
        NGridf ngrid = grid<NGridf>();

    }
};
*/
class MAC2Test: public MAC2f
{
    public:
        
        MAC2Test(): MAC2f(10,10)
    {
        CGrid2f cgrid = grid<CGrid2f>();
        UGrid2f ugrid = grid<UGrid2f>();
        VGrid2f vgrid = grid<VGrid2f>();
        NGrid2f ngrid = grid<NGrid2f>();

    }
};
template <typename Grid>
void printDims(Grid & grid)
{
    std::cout << grid.NI() << " " << grid.NJ() << " " << grid.NK() << std::endl;
}
int main()
{
    CGridf c(20,20,20);
    UGridf u(20,20,20);
    VGridf v(20,20,20);
    WGridf w(20,20,20);
    DUGridf du(20,20,20);
    DVGridf dv(20,20,20);
    DWGridf dw(20,20,20);
    NGridf n(20,20,20);
    printDims(c);
    printDims(u);
    printDims(v);
    printDims(w);
    printDims(du);
    printDims(dv);
    printDims(dw);
    printDims(n);
//    MACTest mactest;
//    MAC2Test mac2test;
    MAC2f merp2(4,4);
    std::cout << merp2.getD12_ghost() << std::endl;
    std::cout << merp2.getD12_ghost() * merp2.getD12_ghost().transpose() << std::endl;
    MACf merp(4,4,4);
    std::cout << merp.getD23_ghost() << std::endl;
    std::cout << merp.getD23_ghost() * merp.getD23_ghost().transpose() << std::endl;
    /*
    std::cout << merp.getD12() << std::endl;
    std::cout <<(merp.getD12()* merp.getD12().transpose()).eval() << std::endl;
    std::cout <<(merp.getD12_ghost()* merp.getD12_ghost().transpose()).eval() << std::endl;
    std::cout << merp.getD01() << std::endl;
    std::cout <<(merp.getD01().transpose() * merp.getD01()).eval() << std::endl;
    */
    return 0;
}
