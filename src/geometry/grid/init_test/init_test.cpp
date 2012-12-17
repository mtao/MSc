#include "geometry/grid/grid.h"
#include <iostream>

int main()
{
    Gridf grid(10,15,20);
    grid(5,5,6)=3.0f;
    GridPf gridpointer(10,15,20,&grid.data());
    std::cout << gridpointer.val(5,5,6)<<std::endl;
    return 0;
}
