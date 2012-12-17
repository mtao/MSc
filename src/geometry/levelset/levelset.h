#ifndef LEVELSET_H
#define LEVELSET_H
#include "geometry/grid/grid.h"
#include "utils.h"
#include "geometry/util/types.h"
#include <boost/function.hpp>



template <typename GT=GridTraitsf>
class LevelSet: public Grid<GT>
{
    public:

        typedef typename GT::Scalar Scalar;
        typedef NumericalTraits<Scalar> NumTraits;
        typedef typename NumTraits::Vector3 Vector3;
        typedef boost::function<void(Vector3)> ScalarFunction;
        typedef Grid<GT> GridParent;
        LevelSet(Grid<GT> & grid):
            GridParent(grid),
             m_di(1.0/grid.NI()),
             m_dj(1.0/grid.NJ()),
             m_dk(1.0/grid.NK())
    {}
        LevelSet(uint NI_, uint NJ_, uint NK_, Scalar di, Scalar dj, Scalar dk, const Vector3 & o=Vector3()): 
            GridParent(NI_,NJ_,NK_) ,
            m_di(di),m_dj(dj),m_dk(dk),
            m_o(o)
    {}
        LevelSet(uint NI_, uint NJ_, uint NK_, Scalar dx, const Vector3 & o=Vector3()): 
            GridParent(NI_,NJ_,NK_) ,
            m_di(dx),m_dj(dx),m_dk(dx),
            m_o(o)
    {}
        LevelSet(uint NI_=1, uint NJ_=1, uint NK_=1, const Vector3 & o=Vector3()):
            GridParent(NI_,NJ_,NK_) ,
            m_di(1/Scalar(NI_)),m_dj(1/Scalar(NJ_)), m_dk(1/Scalar(NK_)),
            m_o(o)
    {}

        //void discretize(ScalarFunction phi);
        inline void vertex(uint i, uint j, uint k, Vector3 & out)
        {
            out(0)=i*m_di+m_o(0);
            out(1)=j*m_dj+m_o(1);
            out(2)=k*m_dk+m_o(2);
        }
        inline Vector3 vertex(uint i, uint j, uint k)
        {
            return Vector3(
            i*m_di+m_o(0)
            ,j*m_dj+m_o(1)
            ,k*m_dk+m_o(2));
        }
        inline Scalar di(){return m_di;}
        inline Scalar dj(){return m_dj;}
        inline Scalar dk(){return m_dk;}

        inline void world2local(const Vector3 & x, Vector3 & out)
        {
            out=x;
            out-=m_o;
            out(0)/=m_di;
            out(1)/=m_dj;
            out(2)/=m_dk;
        }
        inline void barycentric(const Vector3 & p, uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk)
        {
            //get_coord(toLocalCoord(p), m_NI, m_NJ, m_NK, i, fi, j, fj, k, fk);
            get_coord(p, GridParent::NI(), GridParent::NJ(), GridParent::NK(), i, fi, j, fj, k, fk);
        }
        const Vector3 & origin() { return m_o; }
        //~LevelSet();

    protected:
        Scalar m_di,m_dj,m_dk;
        Vector3 m_o;

};
template <typename GT=GridTraitsf>
class LevelSet2: public Grid<GT>
{
    public:

        typedef typename GT::Scalar Scalar;
        typedef NumericalTraits<Scalar> NumTraits;
        typedef typename NumTraits::Vector2 Vector2;
        typedef boost::function<void(Vector2)> ScalarFunction;
        typedef Grid<GT> GridParent;
        LevelSet2(Grid<GT> & grid):
            GridParent(grid),
             m_di(1.0/grid.NI()),
             m_dj(1.0/grid.NJ())
    {}
        LevelSet2(uint NI_, uint NJ_, Scalar di, Scalar dj, const Vector2 & o=Vector2()): 
            GridParent(NI_,NJ_) ,
            m_di(di),m_dj(dj),
            m_o(o)
    {}
        LevelSet2(uint NI_, uint NJ_, Scalar dx, const Vector2 & o=Vector2()): 
            GridParent(NI_,NJ_) ,
            m_di(dx),m_dj(dx),
            m_o(o)
    {}
        LevelSet2(uint NI_=1, uint NJ_=1, const Vector2 & o=Vector2()): 
            GridParent(NI_,NJ_) ,
            m_di(1/Scalar(NI_)),m_dj(1/Scalar(NJ_)),
            m_o(o)
    {}
        //void discretize(ScalarFunction phi);
        inline void vertex(uint i, uint j, Vector2 & out)
        {
            out(0)=i*m_di+m_o(0);
            out(1)=j*m_dj+m_o(1);
        }
        inline Vector2 vertex(uint i, uint j)
        {
            return Vector2(
            i*m_di+m_o(0)
            ,j*m_dj+m_o(1));
        }
        inline Scalar di(){return m_di;}
        inline Scalar dj(){return m_dj;}

        inline void world2local(const Vector2 & x, Vector2 & out)
        {
            out=x;
            out-=m_o;
            out(0)/=m_di;
            out(1)/=m_dj;
        }
        inline void barycentric(const Vector2 & p, uint & i, Scalar & fi, uint & j, Scalar & fj)
        {
            //get_coord(toLocalCoord(p), m_NI, m_NJ, m_NK, i, fi, j, fj, k, fk);
            get_coord(p, GridParent::NI(), GridParent::NJ(), i, fi, j, fj);
        }
        const Vector2 & origin() { return m_o; }
        //~LevelSet();

    protected:
        Scalar m_di,m_dj;
        Vector2 m_o;

};


typedef LevelSet<GridTraitsf> LevelSetf;
typedef LevelSet<GridTraitsd> LevelSetd;
typedef LevelSet<GridTraitsi> LevelSeti;
typedef LevelSet<GridTraitsc> LevelSetc;

typedef LevelSet2<GridTraitsf> LevelSet2f;
typedef LevelSet2<GridTraitsd> LevelSet2d;
typedef LevelSet2<GridTraitsi> LevelSet2i;
typedef LevelSet2<GridTraitsc> LevelSet2c;
#endif
