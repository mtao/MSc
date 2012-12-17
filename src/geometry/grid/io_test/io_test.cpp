#include "geometry/grid/grid.h"
#include <iostream>
#include <fstream>

int main()
{
    Gridf grid(3,3,3);
    float f=2.0f;
    for(Gridf::iterator it=grid.begin(); it<grid.end(); ++it, f+=.1)
        *it=f;
    std::ofstream out("sample.grid");
    grid.write(out);
    grid.write(out);

    Gridf grid2(3,3,3);
    std::ifstream in("sample.grid"); 
    grid2.read(in);
    for(Gridf::iterator it=grid.begin(); it<grid.end(); ++it)
        std::cout << *it << " " <<std::endl;
    return 0;
}
