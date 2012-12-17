#include "geometry/grid/mac/mac.h"
#include <iostream>

class MACTest: public MACf
{
    public:

        MACTest(): MACf(10,10,10)
    {
        print();
    }
        void print()
        {
            CGridf cgrid = grid<CGridf>();
            UGridf ugrid = grid<UGridf>();
            VGridf vgrid = grid<VGridf>();
            WGridf wgrid = grid<WGridf>();
            NGridf ngrid = grid<NGridf>();
            printGrid(cgrid);
            printGrid(ugrid);
            printGrid(vgrid);
            printGrid(wgrid);
            printGrid(ngrid);

        }
        template <typename Grid>
            void printGrid(Grid & g)
            {

                std::cout << "================================================\n";
                std::cout << g.NI() << " " << g.NJ() << " " << g.NK() << std::endl;
                std::cout << (Eigen::RowVector3f)g.origin() << std::endl;
                std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n";
                for(int k=0; k<g.NK(); ++k)
                    for(int j=0; j<g.NJ(); ++j)
                        for(int i=0; i<g.NI(); ++i)
                            std::cout << i << " " << j << " " << k<< ": " << (Eigen::RowVector3f)g.vertex(i,j,k) << std::endl;

                std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
            }
};
class MAC2Test: public MAC2f
{
    public:

        MAC2Test(): MAC2f(10,10)
    {
        print();

    }
        void print()
        {
            CGrid2f cgrid = grid<CGrid2f>();
            UGrid2f ugrid = grid<UGrid2f>();
            VGrid2f vgrid = grid<VGrid2f>();
            NGrid2f ngrid = grid<NGrid2f>();
            std::cout << "hay!" << std::endl;
            printGrid(cgrid);
            printGrid(ugrid);
            printGrid(vgrid);
            printGrid(ngrid);
        }
        template <typename Grid>
            void printGrid(Grid & g)
            {

                std::cout << "================================================\n";
                std::cout << g.NI() << " " << g.NJ() << std::endl;
                std::cout << (Eigen::RowVector2f)g.origin() << std::endl;
                std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n";
                for(int j=0; j<g.NJ(); ++j)
                    for(int i=0; i<g.NI(); ++i)
                        std::cout << i << " " << j << ": " << (Eigen::RowVector2f)g.vertex(i,j) << std::endl;

                std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
            }
};
int main()
{
    MACTest mactest;
    MAC2Test mac2test;
    std::cout << "ahahaha" << std::endl;
    return 0;
}
