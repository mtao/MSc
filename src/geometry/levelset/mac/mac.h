#ifndef MAC_H
#define MAC_H
#include "staggeredgrid.h"
#include <functional>
#define NO3
#ifdef NO3

template <typename T>
class MAC
{
public:
    typedef NumericalTraits<T> NumTraits;
    typedef typename NumTraits::SparseMatrixColMajor SparseMatrix;
    typedef typename NumTraits::Matrix Matrix;
    typedef typename NumTraits::Vector Vector;
    typedef Eigen::Map<Matrix> MapMatrix;
    typedef Eigen::Map<Vector> MapVector;
    typedef StaggeredGridTraits<T,grid_tags::cell>       CGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::u>          UGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::v>          VGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::w>          WGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::du>          DUGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::dv>          DVGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::dw>          DWGridTraits;
    typedef StaggeredGridTraits<T,grid_tags::vertex>     NGridTraits;
    typedef StaggeredGridTraits<char,grid_tags::cell>    CGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::u>       UGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::v>       VGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::w>       WGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::du>       DUGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::dv>       DVGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::dw>       DWGridcTraits;
    typedef StaggeredGridTraits<char,grid_tags::vertex>  NGridcTraits;
    typedef StaggeredGrid<CGridTraits > CGrid;
    typedef StaggeredGrid<UGridTraits > UGrid;
    typedef StaggeredGrid<VGridTraits > VGrid;
    typedef StaggeredGrid<WGridTraits > WGrid;
    typedef StaggeredGrid<DUGridTraits > DUGrid;
    typedef StaggeredGrid<DVGridTraits > DVGrid;
    typedef StaggeredGrid<DWGridTraits > DWGrid;
    typedef StaggeredGrid<NGridTraits > NGrid;
    typedef StaggeredGrid<CGridcTraits> CGridc;
    typedef StaggeredGrid<UGridcTraits> UGridc;
    typedef StaggeredGrid<VGridcTraits> VGridc;
    typedef StaggeredGrid<WGridcTraits> WGridc;
    typedef StaggeredGrid<DUGridcTraits> DUGridc;
    typedef StaggeredGrid<DVGridcTraits> DVGridc;
    typedef StaggeredGrid<DWGridcTraits> DWGridc;
    typedef StaggeredGrid<NGridcTraits> NGridc;
    typedef Eigen::Triplet<T> Triplet;
    struct FluxGrid
    {
        typedef StaggeredGridTraits<T,grid_tags::flux> StaggeredGridTraits;
        FluxGrid(int i, int j, int k): u(i,j,k), v(i,j,k), w(i,j,k) {}
        UGrid u;
        VGrid v;
        WGrid w;
    };
    struct DFluxGrid
    {
        typedef StaggeredGridTraits<T,grid_tags::dflux> StaggeredGridTraits;
        DFluxGrid(int i, int j, int k): du(i,j,k), dv(i,j,k), dw(i,j,k) {}
        DUGrid du;
        DVGrid dv;
        DWGrid dw;
    };

    //Specific grid type, chosing by the traits
    template <typename GridType>
    inline GridType * gridPtr()
    {return new GridType(m_NI,m_NJ,m_NK);}
    template <typename GridType>
    inline GridType grid()
    {return GridType(m_NI,m_NJ,m_NK);}

    //Generic grid tag
    template <typename GridTraits>
    inline StaggeredGrid<GridTraits > * gridTraitsPtr()
    {return gridPtr< StaggeredGrid<GridTraits > >();}
    template <typename GridTraits>
    inline StaggeredGrid<GridTraits > gridTraits()
    {return grid< StaggeredGrid<GridTraits > >();}

    //Generic type and grid tag
    template <typename S, typename GridTag>
    inline StaggeredGrid<StaggeredGridTraits<S,GridTag> > * gridTTagPtr()
    {return gridTraitsPtr< StaggeredGridTraits<S,GridTag> >();}
    template <typename S, typename GridTag>
    inline StaggeredGrid<StaggeredGridTraits<S,GridTag> > gridTTag()
    {return gridTraits<StaggeredGridTraits<S,GridTag> >();}

    //Generic grid tag
    template <typename GridTag>
    inline StaggeredGrid<StaggeredGridTraits<T,GridTag> > * gridTagPtr()
    {return gridTTagPtr<T,GridTag>();}
    template <typename GridTag>
    inline StaggeredGrid<StaggeredGridTraits<T,GridTag> > gridTag()
    {return gridTTag<T,GridTag>();}
    uint NI() {return m_NI;}
    uint NJ() {return m_NJ;}
    uint NK() {return m_NK;}

    //Grid tag based results
    //---------------------------------
    template <typename GridTag>
    inline uint tNI(){return StaggeredGridTraits<T,GridTag>::NI(m_NI);}
    template <typename GridTag>
    inline uint tNJ(){return StaggeredGridTraits<T,GridTag>::NJ(m_NJ);}
    template <typename GridTag>
    inline uint tNK(){return StaggeredGridTraits<T,GridTag>::NK(m_NK);}

    template <typename GridTag>
    inline uint tindex(uint i, uint j, uint k) { return  tNI<GridTag>() * (tNJ<GridTag>() * k + j) + i;}
    template <typename GridTag>
    inline uint tsize(){return tNI<GridTag>() * tNJ<GridTag>() * tNK<GridTag>();}
    template <typename GridTag>
    inline uint tmatIndex(uint i, uint j, uint k, GridTag) { return tindex<GridTag>(i,j,k);}
    inline uint tmatIndex(uint i, uint j, uint k, grid_tags::v) {return size<UGrid>() + index<VGrid>(i,j,k);}
    inline uint tmatIndex(uint i, uint j, uint k, grid_tags::w) {return size<UGrid>() + size<VGrid>() + index<WGrid>(i,j,k);}
    inline uint tmatIndex(uint i, uint j, uint k, grid_tags::dv) {return size<DUGrid>() + index<DVGrid>(i,j,k);}
    inline uint tmatIndex(uint i, uint j, uint k, grid_tags::dw) {return size<DUGrid>() + size<DVGrid>() + index<DWGrid>(i,j,k);}

    //---------------------------------

    template <typename GridType>
    inline uint NI(){return GridType::StaggeredGridTraits::NI(m_NI);}
    template <typename GridType>
    inline uint NJ(){return GridType::StaggeredGridTraits::NJ(m_NJ);}
    template <typename GridType>
    inline uint NK(){return GridType::StaggeredGridTraits::NK(m_NK);}

    template <typename GridType>
    inline uint index(uint i, uint j, uint k) { return NI<GridType>() * (NJ<GridType>() * k + j) + i;}
    template <typename GridType>
    inline uint size()
    {return NI<GridType>() * NJ<GridType>() * NK<GridType>();}

    inline uint fluxSize()
    {
        return size<UGrid>() + size<VGrid>() + size<WGrid>();}
    inline uint dfluxSize()
    {
        return size<DUGrid>() + size<DVGrid>() + size<DWGrid>();}
    template <typename GridType>
    inline uint matIndex(uint i, uint j, uint k) { return tmatIndex(i,j,k, typename GridType::StaggeredGridTraits::GridTag());}


    //completely internal loops themselves
    template <class Functor, typename GridTag>
    inline void tforeach3d_loop(Functor f, GridTag , loop_type::full)
    {
        for(int k=0; k<tNK<GridTag>(); ++k)
            for(int j=0; j<tNJ<GridTag>(); ++j)
                for(int i=0; i<tNI<GridTag>(); ++i)
                {
                    f(i,j,k);
                }
    }
    template <class Functor, typename GridTag>
    inline void tforeach3d_loop(Functor f, GridTag, loop_type::ghost)
    {
        for(int k=1; k<tNK<GridTag>()-1; ++k)
            for(int j=1; j<tNJ<GridTag>()-1; ++j)
                for(int i=1; i<tNI<GridTag>()-1; ++i)
                {
                    f(i,j,k);
                }
    }





    template <typename GridTag, typename Loop=loop_type::full, class Functor>
    inline void tforeach3d(Functor f)
    {
        tforeach3d_loop<Functor>(f, GridTag(), Loop());
    }
    template <class Functor, typename Loop>
    inline void tforeach3d(Functor f, grid_tags::flux , Loop)
    {
        tforeach3d_loop<Functor>(f, grid_tags::u(), Loop());
        tforeach3d_loop<Functor>(f, grid_tags::v(), Loop());
        tforeach3d_loop<Functor>(f, grid_tags::w(), Loop());
    }
    template <class Functor, typename Loop>
    inline void tforeach3d(Functor f, grid_tags::dflux , Loop)
    {
        tforeach3d_loop<Functor>(f, grid_tags::du(), Loop());
        tforeach3d_loop<Functor>(f, grid_tags::dv(), Loop());
        tforeach3d_loop<Functor>(f, grid_tags::dw(), Loop());
    }




    //For external calling
    template <typename GridType, typename Loop=loop_type::full, class Functor>
    inline void foreach3d(Functor f)
    {
        tforeach3d<typename GridType::StaggeredGridTraits::GridTag,loop_type::full, Functor>(f);
    }
    template <typename GridType, class Functor>
    inline void foreach3d_ghost(Functor f)
    {
        tforeach3d<typename GridType::StaggeredGridTraits::GridTag, loop_type::ghost,Functor>(f);
    }



    template <typename GridType, class Functor>
    inline void foreach_boundary(Functor f)
    {
        for(int i=0; i < NI<GridType>(); ++i)
        for(int j=0; j < NJ<GridType>(); ++j)
        {
            f(i,j,0);
            f(i,j,NK<GridType>()-1);
        }
        for(int j=0; j < NJ<GridType>(); ++j)
        for(int k=1; k < NK<GridType>()-1; ++k)
        {
            f(0,j,k);
            f(NI<GridType>()-1,j,k);
        }
        for(int i=1; i < NI<GridType>(); ++i)
        for(int k=1; k < NK<GridType>()-1; ++k)
        {
            f(i,0,k);
            f(i,NJ<GridType>()-1,k);
        }
    }

    template <typename GridType, typename GridType1>
    void pushTripletPair(std::vector<Triplet> & tripletList,
                         const int i, const int j, const int k,
                         const int i1, const int j1, const int k1,
                         const int i2, const int j2, const int k2,
                         const T & value, const T & value1)
    {
        if(
                i < 0 || j < 0 || k < 0 ||
                i >= NI<GridType>() || j >= NJ<GridType>() || k >= NK<GridType>()
                )
            return;
        else
        {
            if(!(
                        i1 < 0 || j1 < 0 || k1 < 0 ||
                        i1 >= NI<GridType1>() || j1 >= NJ<GridType1>() || k1 >= NK<GridType1>()
                        ))
                //Reverse order because (i,j,value) corresponds to j -> i
            {
                tripletList.push_back(Triplet(matIndex<GridType1>(i1,j1,k1), matIndex<GridType>(i,j,k), value));
            }
            if(!(
                        i2 < 0 || j2 < 0 || k2 < 0 ||
                        i2 >= NI<GridType1>() || j2 >= NJ<GridType1>() || k2 >= NK<GridType1>()
                        ))
            {
                tripletList.push_back(Triplet(matIndex<GridType1>(i2,j2,k2), matIndex<GridType>(i,j,k), value1));
            }
        }
    }

    template <typename GridType, typename GridType1>
    void pushTripletPair(std::vector<Triplet> & tripletList,
                         const int i, const int j, const int k,
                         const int i1, const int j1, const int k1,
                         const int i2, const int j2, const int k2,
                         const T & value)
    {
        pushTripletPair(tripletList,i,j,k,i1,j1,k1,i2,j2,k2,value,-value);

    }





    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::cell)
    {
        //2form (volume) -> 1form (flux)
        //Cell -> U/V Flux
        pushTripletPair<CGrid,UGrid>(coefficients,i,j,k,i  ,j  ,k  ,i+1,j  ,k  ,1,1);
        pushTripletPair<CGrid,VGrid>(coefficients,i,j,k,i  ,j  ,k  ,i  ,j+1,k  ,1,1);
        pushTripletPair<CGrid,WGrid>(coefficients,i,j,k,i  ,j  ,k  ,i  ,j  ,k+1,1,1);
    }
    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::u)
    {
        //1form (flux) -> 0form (vertices)
        //U/V Flux -> Nodal (vertices)
        pushTripletPair<UGrid,DVGrid>(coefficients,i,j,k,i  ,j ,k,i,j ,k+1    ,1,1);
        pushTripletPair<UGrid,DWGrid>(coefficients,i,j,k,i  ,j ,k,i,j+1 ,k    ,1,1);
    }

    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::v)
    {
        pushTripletPair<VGrid,DUGrid>(coefficients,i,j,k,i  ,j,k,i  ,j  ,k+1  ,1,1);
        pushTripletPair<VGrid,DWGrid>(coefficients,i,j,k,i  ,j,k,i+1,j  ,k    ,1,1);
    }
    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::w)
    {
        pushTripletPair<WGrid,DUGrid>(coefficients,i,j,k,i  ,j,k,i,j+1  ,k    ,1,1);
        pushTripletPair<WGrid,DVGrid>(coefficients,i,j,k,i  ,j,k,i+1,j  ,k    ,1,1);
    }
    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::du)
    {
        pushTripletPair<DUGrid,NGrid>(coefficients,i,j,k,i  ,j ,k,i+1,j ,k    ,1,1);
    }

    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::dv)
    {
        pushTripletPair<DVGrid,NGrid>(coefficients,i,j,k,i  ,j ,k,i,j+1 ,k    ,1,1);
    }
    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::dw)
    {
        pushTripletPair<DWGrid,NGrid>(coefficients,i,j,k,i  ,j ,k,i,j ,k+1    ,1,1);
    }


    template <typename GridTag>
    inline void  boundary(std::vector<Triplet> & coefficients, int i, int j, int k)
    {
        boundary(coefficients, i, j, k, GridTag());
    }





    template <typename GridTag, typename Loop>
    void fillBoundary(SparseMatrix & mat, GridTag, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach3d<GridTag, Loop>([&](int i,int j, int k)
        {
            boundary<GridTag>(coefficients, i,j, k);

        });

        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename Loop>
    void fillBoundary(SparseMatrix & mat, grid_tags::flux, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach3d<grid_tags::u, Loop>([&](int i,int j, int k)
        {
            boundary<grid_tags::u>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::v, Loop>([&](int i,int j, int k)
        {
            boundary<grid_tags::v>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::w, Loop>([&](int i,int j, int k)
        {
            boundary<grid_tags::w>(coefficients, i,j,k);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename Loop>
    void fillBoundary(SparseMatrix & mat, grid_tags::dflux, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach3d<grid_tags::du, Loop>([&](int i,int j, int k)
        {
            boundary<grid_tags::du>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::dv, Loop>([&](int i,int j, int k)
        {
            boundary<grid_tags::dv>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::dw, Loop>([&](int i,int j, int k)
        {
            boundary<grid_tags::dw>(coefficients, i,j,k);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename GridType, typename Loop>
    void fillBoundary(SparseMatrix & mat, Loop)
    {
        fillBoundary(mat, typename GridType::StaggeredGridTraits::GridTag(), Loop());
    }

    void fillBoundaries()
    {
        fillBoundary<DFluxGrid>(m_b10, loop_type::full());
        fillBoundary<DFluxGrid>(m_b10_ghost, loop_type::ghost());
        fillBoundary<FluxGrid>(m_b21, loop_type::full());
        fillBoundary<FluxGrid>(m_b21_ghost, loop_type::ghost());
        fillBoundary<CGrid>(m_b32, loop_type::full());
        fillBoundary<CGrid>(m_b32_ghost, loop_type::ghost());
    }
    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::vertex)
    {
        //2form (volume) -> 1form (flux)
        //Cell -> U/V Flux
        pushTripletPair<NGrid,DUGrid>(coefficients,i,j,k,i  ,j  ,k  ,i-1,j  ,k  ,-1,1);
        pushTripletPair<NGrid,DVGrid>(coefficients,i,j,k,i  ,j  ,k  ,i  ,j-1,k  ,-1,1);
        pushTripletPair<NGrid,DWGrid>(coefficients,i,j,k,i  ,j  ,k  ,i  ,j  ,k-1,-1,1);
    }
    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::du)
    {
        pushTripletPair<DUGrid,VGrid>(coefficients,i,j,k,i  ,j ,k,i,j ,k-1    ,-1,1);
        pushTripletPair<DUGrid,WGrid>(coefficients,i,j,k,i  ,j ,k,i,j-1 ,k    ,-1,1);
    }

    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::dv)
    {
        pushTripletPair<DVGrid,UGrid>(coefficients,i,j,k,i  ,j,k,i  ,j  ,k-1  ,-1,1);
        pushTripletPair<DVGrid,WGrid>(coefficients,i,j,k,i  ,j,k,i-1  ,j  ,k  ,-1,1);
    }
    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::dw)
    {
        pushTripletPair<DWGrid,UGrid>(coefficients,i,j,k,i  ,j,k,i,j-1  ,k    ,-1,1);
        pushTripletPair<DWGrid,VGrid>(coefficients,i,j,k,i  ,j,k,i,j  ,k-1    ,-1,1);
    }
    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::u)
    {
        pushTripletPair<UGrid,CGrid>(coefficients,i,j,k,i  ,j ,k,i-1,j ,k    ,-1,1);
    }

    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::v)
    {
        pushTripletPair<VGrid,CGrid>(coefficients,i,j,k,i  ,j,k,i  ,j -1 ,k  ,-1,1);
    }
    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, int k, grid_tags::w)
    {
        pushTripletPair<WGrid,CGrid>(coefficients,i,j,k,i  ,j,k,i,j  ,k-1    ,-1,1);
    }


    template <typename GridTag>
    inline void  derivative(std::vector<Triplet> & coefficients, int i, int j, int k)
    {
        derivative(coefficients, i, j, k, GridTag());
    }


    template <typename GridTag, typename Loop>
    void fillDerivative(SparseMatrix & mat, GridTag, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach3d<GridTag, Loop>([&](int i,int j, int k)
        {
            derivative<GridTag>(coefficients, i,j, k);

        });

        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename Loop>
    void fillDerivative(SparseMatrix & mat, grid_tags::flux, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach3d<grid_tags::u, Loop>([&](int i,int j, int k)
        {
            derivative<grid_tags::u>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::v, Loop>([&](int i,int j, int k)
        {
            derivative<grid_tags::v>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::w, Loop>([&](int i,int j, int k)
        {
            derivative<grid_tags::w>(coefficients, i,j,k);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename Loop>
    void fillDerivative(SparseMatrix & mat, grid_tags::dflux, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach3d<grid_tags::du, Loop>([&](int i,int j, int k)
        {
            derivative<grid_tags::du>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::dv, Loop>([&](int i,int j, int k)
        {
            derivative<grid_tags::dv>(coefficients, i,j,k);
        });
        tforeach3d<grid_tags::dw, Loop>([&](int i,int j, int k)
        {
            derivative<grid_tags::dw>(coefficients, i,j,k);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename GridType, typename Loop>
    void fillDerivative(SparseMatrix & mat, Loop)
    {
        fillDerivative(mat, typename GridType::StaggeredGridTraits::GridTag(), Loop());
    }

    void fillDerivatives()
    {
        fillDerivative<DFluxGrid>(m_d12, loop_type::full());
        fillDerivative<DFluxGrid>(m_d12_ghost, loop_type::ghost());
        fillDerivative<FluxGrid>(m_d23, loop_type::full());
        fillDerivative<FluxGrid>(m_d23_ghost, loop_type::ghost());
        fillDerivative<NGrid>(m_d01, loop_type::full());
        fillDerivative<NGrid>(m_d01_ghost, loop_type::ghost());
    }

    MAC(uint ni, uint nj, uint nk): m_NI(ni),m_NJ(nj),m_NK(nk)
      // map from A -> B means |B| rows |A| columns
    {
        m_b10 = SparseMatrix(size<NGrid>(),dfluxSize());
        m_b21 = SparseMatrix(dfluxSize(),fluxSize());
        m_b32 = SparseMatrix(fluxSize(),size<CGrid>());
        m_d01 = SparseMatrix(dfluxSize(),size<NGrid>());
        m_d12 = SparseMatrix(fluxSize(), dfluxSize());
        m_d23 = SparseMatrix(size<CGrid>(), fluxSize());
        m_b10_ghost = SparseMatrix(size<NGrid>(),dfluxSize());
        m_b21_ghost = SparseMatrix(dfluxSize(),fluxSize());
        m_b32_ghost = SparseMatrix(fluxSize(),size<CGrid>());
        m_d01_ghost = SparseMatrix(dfluxSize(),size<NGrid>());
        m_d12_ghost = SparseMatrix(fluxSize(), dfluxSize());
        m_d23_ghost = SparseMatrix(size<CGrid>(), fluxSize());
        fillBoundaries();
        fillDerivatives();
    }
    const SparseMatrix & getB10(){return m_b10;}
    const SparseMatrix & getB21(){return m_b21;}
    const SparseMatrix & getB32(){return m_b32;}
    const SparseMatrix & getD01(){return m_d01;}
    const SparseMatrix & getD12(){return m_d12;}
    const SparseMatrix & getD23(){return m_d23;}
    const SparseMatrix & getB10_ghost(){return m_b10_ghost;}
    const SparseMatrix & getB21_ghost(){return m_b21_ghost;}
    const SparseMatrix & getB32_ghost(){return m_b32_ghost;}
    const SparseMatrix & getD01_ghost(){return m_d01_ghost;}
    const SparseMatrix & getD12_ghost(){return m_d12_ghost;}
    const SparseMatrix & getD23_ghost(){return m_d23_ghost;}















private:
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------


    SparseMatrix m_b10,m_b21, m_b32;
    SparseMatrix m_d01,m_d12, m_d23;
    SparseMatrix m_b10_ghost,m_b21_ghost, m_b32_ghost;
    SparseMatrix m_d01_ghost,m_d12_ghost, m_d23_ghost;
    uint m_NI, m_NJ, m_NK;
};
typedef MAC<float> MACf;
typedef MAC<double> MACd;

#endif

template <typename T>
class MAC2
{
public:





    typedef NumericalTraits<T> NumTraits;
    typedef typename NumTraits::SparseMatrixColMajor SparseMatrix;
    typedef typename NumTraits::Matrix Matrix;
    typedef typename NumTraits::Vector Vector;
    typedef Eigen::Map<Matrix> MapMatrix;
    typedef Eigen::Map<Vector> MapVector;
    typedef StaggeredGridTraits2<T,grid_tags::cell>       CGridTraits;
    typedef StaggeredGridTraits2<T,grid_tags::u>          UGridTraits;
    typedef StaggeredGridTraits2<T,grid_tags::v>          VGridTraits;
    typedef StaggeredGridTraits2<T,grid_tags::vertex>     NGridTraits;
    typedef StaggeredGridTraits2<char,grid_tags::cell>    CGridcTraits;
    typedef StaggeredGridTraits2<char,grid_tags::u>       UGridcTraits;
    typedef StaggeredGridTraits2<char,grid_tags::v>       VGridcTraits;
    typedef StaggeredGridTraits2<char,grid_tags::vertex>  NGridcTraits;
    typedef StaggeredGrid2<CGridTraits > CGrid;
    typedef StaggeredGrid2<UGridTraits > UGrid;
    typedef StaggeredGrid2<VGridTraits > VGrid;
    typedef StaggeredGrid2<NGridTraits > NGrid;
    typedef StaggeredGrid2<CGridcTraits> CGridc;
    typedef StaggeredGrid2<UGridcTraits> UGridc;
    typedef StaggeredGrid2<VGridcTraits> VGridc;
    typedef StaggeredGrid2<NGridcTraits> NGridc;
    typedef Eigen::Triplet<T> Triplet;
    struct FluxGrid
    {
        typedef StaggeredGridTraits2<T,grid_tags::flux> StaggeredGridTraits;
        FluxGrid(int i, int j): u(i,j), v(i,j) {}
        UGrid u;
        VGrid v;
    };
    //Specific grid type, chosing by the traits
    template <typename GridType>
    inline GridType * gridPtr()
    {return new GridType(m_NI,m_NJ);}
    template <typename GridType>
    inline GridType grid()
    {return GridType(m_NI,m_NJ);}

    //Generic grid tag
    template <typename GridTraits>
    inline StaggeredGrid2<GridTraits > * gridTraitsPtr()
    {return gridPtr<StaggeredGrid2<GridTraits > >();}
    template <typename GridTraits>
    inline StaggeredGrid2<GridTraits > gridTraits()
    {return grid< StaggeredGrid2<GridTraits > >();}

    //Generic type and grid tag
    template <typename S, typename GridTag>
    inline StaggeredGrid2<StaggeredGridTraits2<S,GridTag> > * gridTTagPtr()
    {return gridTraitsPtr< StaggeredGridTraits2<S,GridTag> >();}
    template <typename S, typename GridTag>
    inline StaggeredGrid2<StaggeredGridTraits2<S,GridTag> > gridTTag()
    {return gridTraits< StaggeredGridTraits2<S,GridTag> >();}

    //Generic grid tag
    template <typename GridTag>
    inline StaggeredGrid2<StaggeredGridTraits2<T,GridTag> > * gridTagPtr()
    {return gridTTagPtr<T,GridTag>();}
    template <typename GridTag>
    inline StaggeredGrid2<StaggeredGridTraits2<T,GridTag> > gridTag()
    {return gridTTag<T,GridTag>();}
    uint NI() {return m_NI;}
    uint NJ() {return m_NJ;}

    //Grid tag based results
    //---------------------------------
    template <typename GridTag>
    inline uint tNI(){return StaggeredGridTraits2<T,GridTag>::NI(m_NI);}
    template <typename GridTag>
    inline uint tNJ(){return StaggeredGridTraits2<T,GridTag>::NJ(m_NJ);}

    template <typename GridTag>
    inline uint tindex(uint i, uint j) { return tNI<GridTag>() * j + i;}
    template <typename GridTag>
    inline uint tsize(){return tNI<GridTag>() * tNJ<GridTag>();}
    template <typename GridTag>
    inline uint tmatIndex(uint i, uint j, GridTag) { return tindex<GridTag>(i,j);}
    inline uint tmatIndex(uint i, uint j, grid_tags::v) {return size<UGrid>() + index<VGrid>(i,j);}

    //---------------------------------

    template <typename GridType>
    inline uint NI(){return GridType::StaggeredGridTraits::NI(m_NI);}
    template <typename GridType>
    inline uint NJ(){return GridType::StaggeredGridTraits::NJ(m_NJ);}

    template <typename GridType>
    inline uint index(uint i, uint j) { return NI<GridType>() * j + i;}
    template <typename GridType>
    inline uint size(){return NI<GridType>() * NJ<GridType>();}

    inline uint fluxSize(){return size<UGrid>() + size<VGrid>();}
    template <typename GridType>
    inline uint matIndex(uint i, uint j) { return tmatIndex(i,j, typename GridType::StaggeredGridTraits::GridTag());}



    //completely internal loops themselves
    template <class Functor, typename GridTag>
    inline void tforeach2d_loop(Functor f, GridTag , loop_type::full)
    {
        for(int j=0; j<tNJ<GridTag>(); ++j)
            for(int i=0; i<tNI<GridTag>(); ++i)
            {
                f(i,j);
            }
    }
    template <class Functor, typename GridTag>
    inline void tforeach2d_loop(Functor f, GridTag, loop_type::ghost)
    {
        for(int j=1; j<tNJ<GridTag>()-1; ++j)
            for(int i=1; i<tNI<GridTag>()-1; ++i)
            {
                f(i,j);
            }
    }





    template <typename GridTag, typename Loop=loop_type::full, class Functor>
    inline void tforeach2d(Functor f)
    {
        tforeach2d_loop<Functor>(f, GridTag(), Loop());
    }
    template <class Functor, typename Loop>
    inline void tforeach2d(Functor f, grid_tags::flux , Loop)
    {
        tforeach2d_loop<Functor>(f, grid_tags::u(), Loop());
        tforeach2d_loop<Functor>(f, grid_tags::v(), Loop());
    }




    //For external calling
    template <typename GridType, typename Loop=loop_type::full, class Functor>
    inline void foreach2d(Functor f)
    {
        tforeach2d<typename GridType::StaggeredGridTraits::GridTag,loop_type::full, Functor>(f);
    }
    template <typename GridType, class Functor>
    inline void foreach2d_ghost(Functor f)
    {
        tforeach2d<typename GridType::StaggeredGridTraits::GridTag, loop_type::ghost,Functor>(f);
    }


    template <typename GridType, class Functor>
    inline void foreach_boundary(Functor f)
    {
        for(int i=0; i < NI<GridType>(); ++i)
        {
            f(i,0);
            f(i,NJ<GridType>()-1);
        }
        for(int j=1; j < NJ<GridType>()-1; ++j)
        {
            f(0,j);
            f(NI<GridType>()-1,j);
        }
    }

    template <typename GridType, typename GridType1>
    void pushTripletPair(std::vector<Triplet> & tripletList, const int i, const int j, const int i1, const int j1, const int i2, const int j2, const T & value, const T & value1)
    {
        if(
                i < 0 || j < 0 ||
                i >= NI<GridType>() || j >= NJ<GridType>()
                )
        {
            return;
        }
        else
        {
            //Reverse order because (i,j,value) corresponds to j -> i
            if(!(
                        i1 < 0 || j1 < 0 ||
                        i1 >= NI<GridType1>() || j1 >= NJ<GridType1>()
                        ))
                tripletList.push_back(Triplet(matIndex<GridType1>(i1,j1), matIndex<GridType>(i,j), value));
            if(!(
                        i2 < 0 || j2 < 0 ||
                        i2 >= NI<GridType1>() || j2 >= NJ<GridType1>()
                        ))
                tripletList.push_back(Triplet(matIndex<GridType1>(i2,j2), matIndex<GridType>(i,j), value1));
        }
    }

    template <typename GridType, typename GridType1>
    void pushTripletPair(std::vector<Triplet> & tripletList, const int i, const int j, const int i1, const int j1, const int i2, const int j2, const T & value)
    {
        pushTripletPair(tripletList,i,j,i1,j1,i2,j2,value,-value);

    }




    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, grid_tags::cell)
    {
        //2form (volume) -> 1form (flux)
        //Cell -> U/V Flux
        pushTripletPair<CGrid,UGrid>(coefficients,i,j,i  ,j   ,i+1,j   ,1,1);
        pushTripletPair<CGrid,VGrid>(coefficients,i,j,i  ,j,i,j+1      ,1,1);
    }
    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, grid_tags::u)
    {
        //1form (flux) -> 0form (vertices)
        //U/V Flux -> Nodal (vertices)
        pushTripletPair<UGrid,NGrid>(coefficients,i,j,i  ,j ,i,j+1     ,1,1);
    }

    inline void boundary(std::vector<Triplet> & coefficients, int i, int j, grid_tags::v)
    {
        pushTripletPair<VGrid,NGrid>(coefficients,i,j,i  ,j,i+1,j      ,1,1);
    }

    /*
           template <typename GridType>
           inline void  boundary(std::vector<Triplet> & coefficients, int i, int j)
           {
           boundary(coefficients, i, j, typename GridType::StaggeredGridTraits::GridTag());
           }
           */

    template <typename GridTag>
    inline void  boundary(std::vector<Triplet> & coefficients, int i, int j)
    {
        boundary(coefficients, i, j, GridTag());
    }





    template <typename GridTag, typename Loop>
    void fillBoundary(SparseMatrix & mat, GridTag, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach2d<GridTag, Loop>([&](int i,int j)
        {
            boundary<GridTag>(coefficients, i,j);

        });

        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename Loop>
    void fillBoundary(SparseMatrix & mat, grid_tags::flux, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach2d<grid_tags::u, Loop>([&](int i,int j)
        {
            boundary<grid_tags::u>(coefficients, i,j);
        });
        tforeach2d<grid_tags::v, Loop>([&](int i,int j)
        {
            boundary<grid_tags::v>(coefficients, i,j);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename GridType, typename Loop>
    void fillBoundary(SparseMatrix & mat, Loop)
    {
        fillBoundary(mat, typename GridType::StaggeredGridTraits::GridTag(), Loop());
    }

    void setEdgeToNodal()
    {
        fillBoundary<FluxGrid>(m_b10, loop_type::full());
        fillBoundary<FluxGrid>(m_b10_ghost, loop_type::ghost());
    }

    void setCellToEdge()
    {
        fillBoundary<CGrid>(m_b21, loop_type::full());
        fillBoundary<CGrid>(m_b21_ghost, loop_type::ghost());
    }






    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, grid_tags::vertex)
    {
        //nodal -> U/V Flux
        //0form -> 1form
        pushTripletPair<NGrid,UGrid>(coefficients, i,j,i,j,i,j-1,-1,1);
        pushTripletPair<NGrid,VGrid>(coefficients, i,j,i,j,i-1,j,1,-1);
    }
    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, grid_tags::u)
    {
        //1form (flux) -> 2form
        //U/V Flux -> cell
        pushTripletPair<UGrid,CGrid>(coefficients, i,j,i,j,i-1,j,-1,1);
    }

    inline void derivative(std::vector<Triplet> & coefficients, int i, int j, grid_tags::v)
    {
        pushTripletPair<VGrid,CGrid>(coefficients, i,j,i,j,i,j-1,-1,1);
    }

    template <typename GridTag>
    inline void  derivative(std::vector<Triplet> & coefficients, int i, int j)
    {
        derivative(coefficients, i, j, GridTag());
    }









    template <typename GridTag, typename Loop>
    void fillDerivative(SparseMatrix & mat, GridTag, Loop)
    {
        std::vector<Triplet> coefficients;
        //derivative<GridTag>(coefficients,0,0);
        tforeach2d<GridTag, Loop>([&](int i,int j)
        {
            derivative<GridTag>(coefficients, i, j);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }
    template <typename Loop>
    void fillDerivative(SparseMatrix & mat, grid_tags::flux, Loop)
    {
        std::vector<Triplet> coefficients;
        tforeach2d<grid_tags::u, Loop>([&](int i,int j)
        {
            derivative<grid_tags::u>(coefficients, i, j);
        });
        tforeach2d<grid_tags::v, Loop>([&](int i,int j)
        {
            derivative<grid_tags::v>(coefficients, i, j);
        });
        mat.setFromTriplets(coefficients.begin(), coefficients.end());

    }

    template <typename GridTag, typename Loop>
    void fillDerivative(SparseMatrix & mat, Loop)
    {
        fillDerivative(mat, GridTag(), Loop());
    }


    void setEdgeToCell()
    {
        fillDerivative<grid_tags::flux>(m_d12, loop_type::full());
        fillDerivative<grid_tags::flux>(m_d12_ghost,loop_type::ghost());
    }
    void setNodalToEdge()
    {
        fillDerivative<grid_tags::vertex>(m_d01, loop_type::full());
        //fillDerivative<grid_tags::vertex>(m_d01_ghost,loop_type::ghost());
    }









    MapVector uMapVector(float * data)
    {
        return MapVector(data,size<UGrid>());
    }
    MapVector vMapVector(float * data)
    {
        return MapVector(data+size<UGrid>(),size<VGrid>());
    }

    MapMatrix uMapMatrix(float * data)
    {
        return MapMatrix(data,NI<UGrid>(), NJ<UGrid>());
    }
    MapMatrix vMapMatrix(float * data)
    {
        return MapMatrix(data+size<UGrid>(),NI<VGrid>(), NJ<VGrid>());
    }









    // map from A -> B means |B| rows |A| columns
    MAC2(uint ni, uint nj): m_NI(ni),m_NJ(nj)
      //Edges -> Nodals (Vertices)
      , m_b10(size<NGrid>(),size<UGrid>()+size<VGrid>())
      //Cells -> Edges
      , m_b21(size<UGrid>()+size<VGrid>(),size<CGrid>())
      //Nodal -> Edges
      , m_d01(size<UGrid>()+size<VGrid>(),size<NGrid>())
      //Edges -> Cell
      , m_d12(size<CGrid>(),size<UGrid>()+size<VGrid>())
      //Edges -> Nodals (Vertices)
      , m_b10_ghost(size<NGrid>(),size<UGrid>()+size<VGrid>())
      //Cells -> Edges
      , m_b21_ghost(size<UGrid>()+size<VGrid>(),size<CGrid>())
      //Nodal -> Edges
      , m_d01_ghost(size<UGrid>()+size<VGrid>(),size<NGrid>())
      //Edges -> Cell
      , m_d12_ghost(size<CGrid>(),size<UGrid>()+size<VGrid>())
    {
        setEdgeToNodal();
        setCellToEdge();
        setNodalToEdge();
        setEdgeToCell();
    }
    const SparseMatrix & getB10(){return m_b10;}
    const SparseMatrix & getB21(){return m_b21;}
    const SparseMatrix & getD01(){return m_d01;}
    const SparseMatrix & getD12(){return m_d12;}
    const SparseMatrix & getB10_ghost(){return m_b10_ghost;}
    const SparseMatrix & getB21_ghost(){return m_b21_ghost;}
    const SparseMatrix & getD01_ghost(){return m_d01_ghost;}
    const SparseMatrix & getD12_ghost(){return m_d12_ghost;}
private:
    uint m_NI=1, m_NJ=1;
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------


    SparseMatrix m_b10,m_b21;
    SparseMatrix m_d01,m_d12;
    SparseMatrix m_b10_ghost,m_b21_ghost;
    SparseMatrix m_d01_ghost,m_d12_ghost;



    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
};

typedef MAC2<float> MAC2f;
typedef MAC2<double> MAC2d;
#endif
