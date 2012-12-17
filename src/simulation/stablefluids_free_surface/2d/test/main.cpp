#include "simulation/stablefluids_free_surface/2d/sim.h"

int main()
{
    MACStableFluidsFreeSurface2 sim(50,50);
    for(int i=0; i<50; ++i)
    sim.step(0.02);
}
