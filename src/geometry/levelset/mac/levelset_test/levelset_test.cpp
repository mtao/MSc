#include "geometry/grid/mac/mac.h"
#include "geometry/marchingcubes/marchingcubes.h"
#include "geometry/mesh/io.h"
#include <math.h>
#include <iostream>
#define DIM 300 
float center_phi(uint i, uint j, uint k)
{
    float x=i;x-=DIM/2;
    float y=j;y-=DIM/2;
    float z=k;z-=DIM/2;
    return std::sqrt(
            x*x+y*y)-std::abs(z);
}
class MACTest: public MACf
{
    public:
        MACTest(uint x,uint y, uint z): MACf(x,y,z), phi(gridTag<grid_tags::vertex>())
    {
        CGridf cgrid = gridTag<grid_tags::cell>();
        UGridf ugrid = gridTag<grid_tags::u>();
        VGridf vgrid = gridTag<grid_tags::v>();
        WGridf wgrid = gridTag<grid_tags::w>();
        NGridf ngrid = gridTag<grid_tags::vertex>();

    }
        NGridf phi;
};
int main()
{
    MACTest mactest(DIM,DIM,DIM);
    GridPf vpgrid = mactest.phi.toVectorPointer();
    LevelSet<GridPointerTraitsf> ls(vpgrid);
    MarchingCubeGrid<GridPointerTraitsf> phi(ls);
    for(int k=0; k<phi.NK(); ++k)
        for(int j=0; j<phi.NJ(); ++j)
            for(int i=0; i<phi.NI(); ++i)
            {
                phi(i,j,k)=center_phi(i,j,k);
            }

    Mesh<MarchingCubeGrid<>::NumTraits> mesh;
    phi.populateMesh(mesh,0);
    
    std::cout << mesh.vertices.size() << std::endl;
    std::cout << mesh.edges.size() << std::endl;
    std::cout << mesh.faces.size() << std::endl;
    std::ofstream out("output.obj");
    MeshIOUtils::OBJParser<Mesh<MarchingCubeGrid<>::NumTraits>::NumTraits> parser(&mesh);
    parser.write(out);
}
