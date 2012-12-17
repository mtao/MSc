#ifndef GRID_UTILS_H
#define GRID_UTILS_H
#include "geometry/util/types.h"
#include "geometry/util/interp.h"

//Find which cell one grid point is in one dimension 
//Input: coordinate x, grid bounds i_low,i_high
//Output: grid cell i, offset f
    template <typename Scalar>
inline void get_barycentric(const Scalar& x, uint& i, Scalar& f, const uint & i_low, const uint & i_high)
{
    Scalar s=std::floor(x);
    i=(int)s;
    //below the bottom
    if (i<i_low){i=i_low; f=0;}
    //over the top 
    else if (i>i_high-2){i=i_high-2; f=1;}
    //just right
    else f=(Scalar)(x-s);
}

template <typename Vector2, typename Scalar>
inline void get_coord(const Vector2& p, const uint & NI, const uint & NJ, uint & i, Scalar & fi, uint & j, Scalar & fj)
{
    get_barycentric(p(0), i, fi, 0, NI);
    get_barycentric(p(1), j, fj, 0, NJ);
}

template <typename Vector3, typename Scalar>
inline void get_coord(const Vector3& p, const uint & NI, const uint & NJ, const uint & NK , uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk)
{
    get_barycentric(p(0), i, fi, 0, NI);
    get_barycentric(p(1), j, fj, 0, NJ);
    get_barycentric(p(2), k, fk, 0, NK);
}

//Bilinear Interpolation in a 2d grid cell on a given point
//Input: Barycentric coordinates i,j, offsets fi,fj
//Output: return interpolated value
    template <typename Scalar, typename GridType>
inline Scalar bilerp_grid2d(const int& i, const Scalar& fi, 
        const int& j, const Scalar& fj,
        const GridType& grid)
{
    return bilerp(grid(i,j), grid(i+1,j),
            grid(i,j+1), grid(i+1,j+1),
            fi,fj);
}

//Trilinear Interpolation in a 3d grid cell on a given point
//Input: Barycentric coordinates i,j, offsets fi,fj
//Output: return interpolated value
    template <typename Scalar, typename GridType>
inline Scalar trilerp_grid3d(const int& i, const Scalar& fi, 
        const int& j, const Scalar& fj,
        const int& k, const Scalar& fk,
        const GridType& grid)
{
    return trilerp(
            grid(i,j,k), grid(i+1,j,k),
            grid(i,j+1,k), grid(i+1,j+1,k),
            grid(i,j,k+1), grid(i+1,j,k+1),
            grid(i,j+1,k+1), grid(i+1,j+1,k+1),
            fi,fj,fk);
}
//Trilinear Interpolation in a 3d grid cell on a given point
//Input: World coordinates pos
//Output: return interpolated value
    template <typename Vector3, typename GridType, typename Scalar>
inline Scalar trilerp_grid3d(const Vector3& pos, const GridType& grid)
{
    int i,j,k;
    Scalar fi,fj,fk;
    get_barycentric(pos(0), i, fi, 0, grid.shape()(0));
    get_barycentric(pos(1), j, fj, 0, grid.shape()(1));
    get_barycentric(pos(2), k, fk, 0, grid.shape()(2));
    return trilerp_grid3d(i,fi,j,fj,k,fk,grid);
}

//Finite Differencing over a grid
//Interpolate the gradient using the 2d grid values
//Input: position and grid
//Output: gradient is the gradient, returns the norm
    template< typename GridType, typename Scalar, typename Vector2 >
inline void finite_difference(GridType & g, const uint & i, const Scalar& fi, const uint &j, const Scalar& fj, Vector2 & gradient)
{
    Scalar v00 = g(i,j);
    Scalar v01 = g(i,j+1);
    Scalar v10 = g(i+1,j);
    Scalar v11 = g(i+1,j+1);

    Scalar ddy0 = (v01 - v00);
    Scalar ddy1 = (v11 - v10);

    Scalar ddx0 = (v10 - v00);
    Scalar ddx1 = (v11 - v01);

    gradient(0) = lerp(ddx0,ddx1,fj);
    gradient(1) = lerp(ddy0,ddy1,fi);
}
    template <class GridType, typename Scalar>
inline void finite_difference(const GridType & grid, uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk, Vertex & grad) 
{
    Scalar v000 = grid(i,j,k);
    Scalar v010 = grid(i,j+1,k);
    Scalar v100 = grid(i+1,j,k);
    Scalar v110 = grid(i+1,j+1,k);
    Scalar v001 = grid(i,j,k+1);
    Scalar v011 = grid(i,j+1,k+1);
    Scalar v101 = grid(i+1,j,k+1);
    Scalar v111 = grid(i+1,j+1,k+1);

    Scalar ddx00 = (v100 - v000);//00
    Scalar ddx10 = (v110 - v010);//10
    Scalar ddx01 = (v101 - v001);//01
    Scalar ddx11 = (v111 - v011);//11

    Scalar ddy00 = (v010 - v000);//00
    Scalar ddy10 = (v110 - v100);//10
    Scalar ddy01 = (v011 - v001);//01
    Scalar ddy11 = (v111 - v101);//11

    Scalar ddz00 = (v001 - v000);//00
    Scalar ddz10 = (v101 - v100);//10
    Scalar ddz01 = (v011 - v010);//01
    Scalar ddz11 = (v111 - v110);//11

    grad(0) = bilerp(ddx00,ddx10,ddx01,ddx11,fj,fk);
    grad(1) = bilerp(ddy00,ddy10,ddy01,ddy11,fi,fk);
    grad(2) = bilerp(ddz00,ddz10,ddz01,ddz11,fi,fj);
    //return grad.norm();

}


//Fraction of the grid cell submerged in entity using signed distance function
//Input: The distances from the left and right
//Output: returns amount of entity is submerged in grid cell
    template <typename Scalar>
inline Scalar fraction_inside(Scalar phi_left, Scalar phi_right)
{
    //everything is inside
    if(phi_left<0 && phi_right < 0)
        return 1;
    //return the ratio between the distances
    if (phi_left < 0 && phi_right >=0)
        return phi_left/(phi_left-phi_right);
    //return the ratio between the distances
    if (phi_left >= 0 && phi_right <0)
        return phi_right/(phi_right-phi_left);
    return 0;

}

template <typename Scalar> struct FractionInsideOp {

    EIGEN_EMPTY_STRUCT_CTOR(FractionInsideOp)
    Scalar operator()(const Scalar & left, const Scalar & right) const {return fraction_inside(left,right);}
};


#endif
