
#ifndef EDGESTORAGE2_H
#define EDGESTORAGE2_H
#include "geometry/util/index.h"
#include "geometry/grid/grid.h"

template <typename T>
class EdgeStorage2
{
    public:
        typedef IndexSet<2> Index2;
        EdgeStorage2<T>() {}
        EdgeStorage2<T>(uint ni_, uint nj_): ni(ni_),nj(nj_),
        m_uData((ni+1)*nj),
        m_vData((nj+1)*ni)
    {}
        inline void resize(uint ni_,uint nj_)
        {
            ni=ni_;
            nj=nj_;
            m_uData.resize((ni+1)*nj);
            m_vData.resize((nj+1)*ni);
        }
        inline int uInd(uint i, uint j){return i+(ni+1)*(j);}
        inline int vInd(uint i, uint j){return i+ni*j;}


        inline T & u(uint i, uint j){return m_uData[uInd(i,j)];}
        inline T & v(uint i, uint j){return m_vData[vInd(i,j)];}

        inline T & u(Index2 & e){return u(e[0],e[1]);}
        inline T & v(Index2 & e){return v(e[0],e[1]);}

        inline const uint NI(){return ni;}
        inline const uint NJ(){return nj;}
        inline void fill(T val){
            std::fill(m_uData.begin(), m_uData.end(),val);
            std::fill(m_vData.begin(), m_vData.end(),val);

        }
        inline void clear()
        { 
            fill((T)-1);
        }
        inline void clearData()
        {
            m_uData.clear();
            m_vData.clear();
        }

    private:
        uint ni,nj;
        std::vector<T> m_uData;
        std::vector<T> m_vData;
};

#endif

