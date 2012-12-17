#ifndef SCENE_H
#define SCENE_H

template <class Grid>
void makeCircle(Grid & grid)
{
    typedef typename Grid::Vector2 Vector2;
    int ni=grid.NI(), nj=grid.NJ();
    for(int j=0; j<grid.NJ(); ++j)
        for(int i=0; i<grid.NI(); ++i)
        {

            Vector2 vec(grid.vertex(i,j));
            grid(i,j) = (vec-Vector2(0.5,0.5)).norm()-.4;

        }
}

template <class Grid>
void makeSquare(Grid & grid)
{
    for(int j=0; j<grid.NJ(); ++j)
        for(int i=0; i<grid.NI(); ++i)
        {
            typename Grid::Vector2 v = grid.vertex(i,j);
            typename Grid::Scalar min      =std::min(v(0),v(1));
            min      =std::min(min,1-v(0));
            min      =std::min(min,1-v(1));
            grid(i,j)=-min+2*grid.di();

        }
}
template <typename UGrid, typename VGrid, typename Scalar, typename CGrid, typename NGrid, typename Vector2>
void initScene(Scalar dx, UGrid & u, VGrid & v, UGrid & uw, VGrid & vw, CGrid & p, CGrid & lphi, NGrid & sphi, std::vector<Vector2> & particles, Scalar & particle_radius)
{

//    std::cout << sphi.NI() << " " << sphi.NJ() << std::endl;
    particle_radius = dx / std::sqrt(8.0);
//    makeCircle(sphi);
    makeSquare(sphi);
    for(int i=0; i<sphi.NI(); ++i)
    for(int j=0; j<sphi.NJ(); ++j)
    {sphi(i,j)*=-1;
        sphi(i,j) -=dx;}
    makeCircle(lphi);
    particles.clear();
    for(int i=0; i<50; ++i)
    {
        particles.push_back(Vector2(particle_radius*i+.1, .5));
    }

    for(int i=0; i<50; ++i)
    {
        particles.push_back(Vector2(particle_radius*(i+.5)+.1, .5+particle_radius*std::sqrt(3)/2));
    }
    for(int i=0; i<50; ++i)
    {
        particles.push_back(Vector2(particle_radius*i+.1, .5+particle_radius*std::sqrt(3)/2*2));
    }

    for(int i=0; i<50; ++i)
    {
        particles.push_back(Vector2(particle_radius*(i+.5)+.1, .5+particle_radius*std::sqrt(3)/2*3));
    }
    particles.clear();
    for(int j=0; j < 10; ++j)
    for(int i = 0; i < 180; ++i)
    {
        particles.push_back(Vector2(.5+j/100.0*std::cos(i/90.0*3.1415968), .5+j/100.0*std::sin(i/90.0*3.1415968)));
    }
    particles.clear();
    for(int i=0; i<lphi.size()*8 ; ++i)
    {
        float x=randhashf(i*2,0,1);
        float y=randhashf(i*2+1,0,1);
        if(lphi.lerp(x,y) < 0 && sphi.lerp(x,y) > 0)// && y>0.5)// && std::abs(x-0.5)<.2)
        {
            bool doit=true;
            for(auto it = particles.begin(); it < particles.end(); ++it)
                if(((*it)-Vector2(x,y)).norm()<2*particle_radius) {doit=false; break;}
            doit=true;
            if(doit)
            {
            particles.push_back(Vector2(x,y));
            }
        }
    }
    std::cout << particles.size() << std::endl;
}
#endif // SCENE_H
