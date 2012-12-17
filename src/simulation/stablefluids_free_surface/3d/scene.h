#ifndef SCENE_H
#define SCENE_H

template <class Grid>
void makeSphere(Grid & grid)
{
    typedef typename Grid::Vector3 Vector3;
    int ni=grid.NI(), nj=grid.NJ();
    for(int k=0; k<grid.NK(); ++k)
    for(int j=0; j<grid.NJ(); ++j)
        for(int i=0; i<grid.NI(); ++i)
        {

            Vector3 vec(grid.vertex(i,j,k));
            grid(i,j,k) = (vec-Vector3(0.5,0.5,0.5)).norm()-.3;

        }
}

template <class Grid>
void makeCube(Grid & grid)
{
    for(int k=0; k<grid.NK(); ++k)
    for(int j=0; j<grid.NJ(); ++j)
        for(int i=0; i<grid.NI(); ++i)
        {
            typename Grid::Vector3 v = grid.vertex(i,j,k);
            typename Grid::Scalar min      =std::min(v(0),v(1));
            min      =std::min(min,v(2));
            min      =std::min(min,1-v(0));
            min      =std::min(min,1-v(1));
            min      =std::min(min,1-v(2));
            grid(i,j,k)=-min+2*grid.di();

        }
}
template <typename UGrid, typename VGrid, typename WGrid, typename Scalar, typename CGrid, typename NGrid, typename Vector3>
void initScene(Scalar dx, UGrid & u, VGrid & v, WGrid &w, UGrid & uw, VGrid & vw, WGrid & ww, CGrid & p, CGrid & lphi, NGrid & sphi, std::vector<Vector3> & particles, Scalar & particle_radius)
{

//    std::cout << sphi.NI() << " " << sphi.NJ() << std::endl;
    particle_radius = dx / std::sqrt(8);
    makeCube(sphi);
    for(int i=0; i<sphi.NI(); ++i)
    for(int j=0; j<sphi.NJ(); ++j)
    for(int k=0; k<sphi.NK(); ++k)
    {
        sphi(i,j,k)*=-1;
    }
    makeSphere(lphi);
    for(int i=0; i<lphi.NI(); ++i)
    for(int j=0; j<lphi.NJ(); ++j)
    for(int k=0; k<lphi.NK(); ++k)
    {
        lphi(i,j,k) -= .1;
    }
    particles.clear();
    for(int i=-5; i < 5; ++i)
    for(int j=-5; j < 5; ++j)
    for(int k=-5; k < 5; ++k)
    particles.push_back(Vector3(.5+i*particle_radius,.5+j*particle_radius,.5+k*particle_radius));

    particles.clear();
    for(int i=0; i<lphi.size() *12; ++i)
    {
        float x=randhashf(i*3,0,1);
        float y=randhashf(i*3+1,0,1);
        float z=randhashf(i*3+2,0,1);
        if(lphi.lerp(x,y,z) < 0 && sphi.lerp(x,y,z) > 0)// && y>0.5)// && std::abs(x-0.5)<.2)
        {
            bool doit=true;
            for(auto it = particles.begin(); it < particles.end(); ++it)
                if(((*it)-Vector3(x,y,z)).norm()<2*particle_radius) {doit=false; break;}
            doit=true;
            if(doit)
            {
            particles.push_back(Vector3(x,y,z));
            }
        }
    }
    std::cout << particles.size() << std::endl;
}
#endif // SCENE_H
