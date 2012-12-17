#ifndef MAC_FACTORY_H
#define MAC_FACTORY_H

#include "geometry/levelset/levelset.h"


#include "utils.h"


template <typename T, class Tag>
struct StaggeredGridTraits
{
    typedef NumericalTraits<T> NumTraits;
    typedef Tag GridTag;
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector3 Vector3;
    static inline uint NI(uint ni){return StaggeredGridUtils::NI<GridTag>(ni);}
    static inline uint NJ(uint nj){return StaggeredGridUtils::NJ<GridTag>(nj);}
    static inline uint NK(uint nk){return StaggeredGridUtils::NK<GridTag>(nk);}
    static inline uint size(uint ni, uint nj, uint nk)
    {
        return
                StaggeredGridUtils::NI<GridTag>(ni) *
                StaggeredGridUtils::NI<GridTag>(nj) *
                StaggeredGridUtils::NI<GridTag>(nk);
    }
    static inline Vector3 origin(uint ni, uint nj, uint nk){return StaggeredGridUtils::origin<Vector3>(ni,nj,nk,GridTag());}

};



template <typename Traits>
class StaggeredGrid: public LevelSet<GridTraits<typename Traits::Scalar> >
{
    public:
        typedef Traits StaggeredGridTraits;
        typedef typename Traits::NumTraits NumTraits;
        typedef typename NumTraits::Scalar Scalar;
        typedef typename NumTraits::Vector3 Vector3;
        typedef typename NumTraits::Vector Vector;
        typedef Eigen::Map<Vector> MapVector;
        typedef typename NumTraits::Matrix Matrix;
        typedef Eigen::Map<Matrix> MapMatrix;
        typedef LevelSet<GridTraits<Scalar> > LevelSetParent;
        typedef typename LevelSetParent::GridParent GridParent;
        using LevelSetParent::m_NI;
        using LevelSetParent::m_NJ;
        using LevelSetParent::m_NK;
        using LevelSetParent::m_di;
        using LevelSetParent::m_dj;
        using LevelSetParent::m_dk;
        using LevelSetParent::m_o;
        using LevelSetParent::m_data;
        using LevelSetParent::begin;
        using LevelSetParent::end;
        using LevelSetParent::read;
        using LevelSetParent::write;

        StaggeredGrid(uint ni,uint nj, uint nk):LevelSetParent( 
                Traits::NI(ni),
                Traits::NJ(nj),
                Traits::NK(nk),
                1/Scalar(ni), 
                1/Scalar(nj), 
                1/Scalar(nk), 
                Traits::origin(ni,nj,nk)) {}
        inline MapVector mapVector(){return MapVector(GridParent::raw_data(), GridParent::size());}
        inline void resize(uint ni, uint nj, uint nk)
        {
            LevelSetParent::resize(Traits::NI(ni), Traits::NJ(nj), Traits::NK(nk));
        }

        void barycentric(const Scalar & x, const Scalar & y, const Scalar & z,
                uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk) const
        {
            get_barycentric((x-m_o(0))/m_di   , i, fi, 0, m_NI);
            get_barycentric((y-m_o(1))/m_dj   , j, fj, 0, m_NJ);
            get_barycentric((z-m_o(2))/m_dk   , k, fk, 0, m_NK);
        }
        inline void barycentric(const Vector3 &p,
                uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk) const
        {barycentric(p(0),p(1),p(2),i,fi,j,fj,k,fk);}

        inline Scalar lerp(uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk) const
        {
            return trilerp_grid3d(i,fi,j,fj,k,fk,(*this));
        }
        inline Scalar lerp(const Scalar & x, const Scalar & y, const Scalar & z) const
        {
            uint i,j,k; Scalar fi,fj,fk;
            barycentric(x,y,z,i,fi,j,fj,k,fk);
            return lerp(i,fi,j,fj,k,fk);
        }
        inline Scalar lerp(const Vector3 & p) const
        {
            return lerp(p(0),p(1),p(2));//assume that entries are in [0,1]
        }
        inline void gradient(uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk, Vector3 & grad)  const
        {
            finite_difference((*this),i,fi,j,fj,k,fk, grad);

        }
        inline void gradient(Vector3 & p, Vector3 & grad)
        {
            uint i,j,k; Scalar fi,fj,fk;
            barycentric(p,i,fi,j,fj,k,fk);
            gradient(i,fi,j,fj,k,fk,grad);
        }

        inline Scalar min()
        {
            Scalar min=m_data[0];
            for(typename LevelSetParent::iterator it=begin(); it<end(); ++it)
                if(min>*it) min=*it;
            return min;
        }
        inline Scalar max()
        {
            Scalar max=m_data[0];
            for(typename LevelSetParent::iterator it=begin(); it<end(); ++it)
                if(max<*it) max=*it;
            return max;
        }
        inline Scalar norm()
        {
            Scalar accum=0;
            for(typename LevelSetParent::iterator it=begin(); it<end(); ++it)
                accum+=std::abs(*it);
            return std::sqrt(accum);
        }
};





//2D

template <typename T, class Tag>
struct StaggeredGridTraits2
{
    typedef NumericalTraits<T> NumTraits;
    typedef Tag GridTag;
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector2 Vector2;
    static inline uint NI(uint ni){return StaggeredGridUtils::NI<GridTag>(ni);}
    static inline uint NJ(uint nj){return StaggeredGridUtils::NJ<GridTag>(nj);}
    static inline uint size(uint ni,uint nj){
        return 
            StaggeredGridUtils::NI<GridTag>(ni) *
            StaggeredGridUtils::NJ<GridTag>(nj)
            ;
    }
    static inline Vector2 origin(uint ni, uint nj){return StaggeredGridUtils::origin2<Vector2>(ni,nj,GridTag());}

};



template <typename Traits>
class StaggeredGrid2: public LevelSet2<GridTraits<typename Traits::NumTraits::Scalar> >
{
    public:
        typedef Traits StaggeredGridTraits;
        typedef typename Traits::NumTraits NumTraits;
        typedef typename Traits::Scalar Scalar;
        typedef typename Traits::Vector2 Vector2;
        typedef typename NumTraits::Vector Vector;
        typedef Eigen::Map<Vector> MapVector;
        typedef typename NumTraits::Matrix Matrix;
        typedef Eigen::Map<Matrix> MapMatrix;
        typedef LevelSet2<GridTraits<Scalar> > LevelSetParent;
        typedef typename LevelSetParent::GridParent GridParent;
        using LevelSetParent::m_NI;
        using LevelSetParent::m_NJ;
        using LevelSetParent::m_di;
        using LevelSetParent::m_dj;
        using LevelSetParent::m_o;
        using LevelSetParent::m_data;
        using LevelSetParent::begin;
        using LevelSetParent::end;
        using LevelSetParent::read;
        using LevelSetParent::write;

        StaggeredGrid2(uint ni=1,uint nj=1):LevelSetParent( 
                Traits::NI(ni),
                Traits::NJ(nj),
                1/Scalar(ni),
                1/Scalar(nj),
                Traits::origin(ni,nj)) {}
        inline MapVector mapVector(){return MapVector(GridParent::raw_data(), GridParent::size());}
        inline MapMatrix mapMatrix(){return MapMatrix(GridParent::raw_data(), m_NI, m_NJ);}
        inline void resize(uint ni, uint nj)
        {
            LevelSetParent::resize(Traits::NI(ni), Traits::NJ(nj));
        }
        inline void barycentric(const Scalar & x, const Scalar & y,
                uint & i, Scalar & fi, uint & j, Scalar & fj) const
        {
            get_barycentric((x-m_o(0))/m_di   , i, fi, 0, m_NI);
            get_barycentric((y-m_o(1))/m_dj   , j, fj, 0, m_NJ);
        }
        inline void barycentric(const Vector2 &p, 
                uint & i, Scalar & fi, uint & j, Scalar & fj) const
        {
            barycentric(p(0),p(1),i,fi,j,fj);
        }
        inline Scalar lerp(uint i, Scalar fi, uint j, Scalar fj) const
        {
            return bilerp_grid2d(i,fi,j,fj,(*this));
        }
        inline Scalar lerp(const Scalar x, const Scalar y) const
        {
            uint i,j; Scalar fi,fj;
            barycentric(x,y,i,fi,j,fj);
            return lerp(i,fi,j,fj);
        }
        inline Scalar lerp(const Vector2 & p) const
        {
            return lerp(p(0),p(1));//assume that entries are in [0,1]
        }
        inline void gradient(uint & i, Scalar & fi, uint & j, Scalar & fj, Vector2 & grad) 
        {
            finite_difference((*this),i,fi,j,fj, grad);

        }
        inline void gradient(Vector2 & p, Vector2 & grad)
        {
            uint i,j; Scalar fi,fj;
            barycentric(p,i,fi,j,fj);
            gradient(i,fi,j,fj,grad);
        }

        inline Scalar min()
        {
            Scalar min=m_data[0];
            for(typename LevelSetParent::iterator it=begin(); it<end(); ++it)
                if(min>*it) min=*it;
            return min;
        }
        inline Scalar max()
        {
            Scalar max=m_data[0];
            for(typename LevelSetParent::iterator it=begin(); it<end(); ++it)
                if(max<*it) max=*it;
            return max;
        }
        inline Scalar norm()
        {
            Scalar accum=0;
            for(typename LevelSetParent::iterator it=begin(); it<end(); ++it)
                accum+=std::abs(*it);
            return std::sqrt(accum);
        }
        /*
        virtual void vertex(uint i, uint j, Vector2 & v)
        {Traits::vertex(m_NI,m_NJ,i,j,v);}
        Vector2 vertex(uint i, uint j)
        {Vector2 v; vertex(i,j,v); return v;}
        */
};




















typedef StaggeredGridTraits<float,grid_tags::cell> CellGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::u> UGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::v> VGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::w> WGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::du> DUGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::dv> DVGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::dw> DWGridTraitsf;
typedef StaggeredGridTraits<float,grid_tags::vertex> VertexGridTraitsf;

typedef StaggeredGridTraits<double,grid_tags::cell> CellGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::u> UGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::v> VGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::w> WGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::du> DUGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::dv> DVGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::dw> DWGridTraitsd;
typedef StaggeredGridTraits<double,grid_tags::vertex> VertexGridTraitsd;

typedef StaggeredGrid<CellGridTraitsf> CGridf;
typedef StaggeredGrid<UGridTraitsf> UGridf;
typedef StaggeredGrid<VGridTraitsf> VGridf;
typedef StaggeredGrid<WGridTraitsf> WGridf;
typedef StaggeredGrid<DUGridTraitsf> DUGridf;
typedef StaggeredGrid<DVGridTraitsf> DVGridf;
typedef StaggeredGrid<DWGridTraitsf> DWGridf;
typedef StaggeredGrid<VertexGridTraitsf> NGridf;

typedef StaggeredGrid<CellGridTraitsd> CGridd;
typedef StaggeredGrid<UGridTraitsd> UGridd;
typedef StaggeredGrid<VGridTraitsd> VGridd;
typedef StaggeredGrid<WGridTraitsd> WGridd;
typedef StaggeredGrid<DUGridTraitsd> DUGridd;
typedef StaggeredGrid<DVGridTraitsd> DVGridd;
typedef StaggeredGrid<DWGridTraitsd> DWGridd;
typedef StaggeredGrid<VertexGridTraitsd> NGridd;






typedef StaggeredGridTraits2<float,grid_tags::cell> CellGridTraits2f;
typedef StaggeredGridTraits2<float,grid_tags::u> UGridTraits2f;
typedef StaggeredGridTraits2<float,grid_tags::v> VGridTraits2f;
typedef StaggeredGridTraits2<float,grid_tags::vertex> VertexGridTraits2f;

typedef StaggeredGridTraits2<double,grid_tags::cell> CellGridTraits2d;
typedef StaggeredGridTraits2<double,grid_tags::u> UGridTraits2d;
typedef StaggeredGridTraits2<double,grid_tags::v> VGridTraits2d;
typedef StaggeredGridTraits2<double,grid_tags::vertex> VertexGridTraits2d;

typedef StaggeredGrid2<CellGridTraits2f> CGrid2f;
typedef StaggeredGrid2<UGridTraits2f> UGrid2f;
typedef StaggeredGrid2<VGridTraits2f> VGrid2f;
typedef StaggeredGrid2<VertexGridTraits2f> NGrid2f;

typedef StaggeredGrid2<CellGridTraits2d> CGrid2d;
typedef StaggeredGrid2<UGridTraits2d> UGrid2d;
typedef StaggeredGrid2<VGridTraits2d> VGrid2d;
typedef StaggeredGrid2<VertexGridTraits2d> NGrid2d;







#endif
