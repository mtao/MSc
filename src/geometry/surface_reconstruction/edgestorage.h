
#ifndef EDGESTORAGE_H
#define EDGESTORAGE_H
#include "geometry/util/index.h"
#include "geometry/grid/grid.h"


template <typename T>
class EdgeStorage
{
    public:
        typedef IndexSet<3> Index3;
        EdgeStorage<T>() {}
        EdgeStorage<T>(uint ni_, uint nj_, uint nk_): ni(ni_),nj(nj_),nk(nk_),
        m_uData((ni+1)*nj*nk),
        m_vData((nj+1)*ni*nk),
        m_wData((nk+1)*nj*ni)
    {}
        inline void resize(uint ni_,uint nj_, uint nk_)
        {
            ni=ni_;
            nj=nj_;
            nk=nk_;
            m_uData.resize((ni+1)*nj*nk);
            m_vData.resize((nj+1)*ni*nk);
            m_wData.resize((nk+1)*nj*ni);
        }
        inline int uInd(uint i, uint j, uint k){return i+(ni+1)*(j+nj*k);}
        inline int vInd(uint i, uint j, uint k){return i+ni*(j+(nj+1)*k);}
        inline int wInd(uint i, uint j, uint k){return i+ni*(j+nj*k);}


        inline T & u(uint i, uint j, uint k){return m_uData[uInd(i,j,k)];}
        inline T & v(uint i, uint j, uint k){return m_vData[vInd(i,j,k)];}
        inline T & w(uint i, uint j, uint k){return m_wData[wInd(i,j,k)];}

        inline T & u(Index3 & e){return u(e[0],e[1],e[2]);}
        inline T & v(Index3 & e){return v(e[0],e[1],e[2]);}
        inline T & w(Index3 & e){return w(e[0],e[1],e[2]);}

        inline const uint NI(){return ni;}
        inline const uint NJ(){return nj;}
        inline const uint NK(){return nk;}
        inline void fill(T val){
            std::fill(m_uData.begin(), m_uData.end(),val);
            std::fill(m_vData.begin(), m_vData.end(),val);
            std::fill(m_wData.begin(), m_wData.end(),val);
        }
        inline void clear(){fill((T)-1);}
        inline void clearData()
        {
            m_uData.clear();
            m_vData.clear();
            m_wData.clear();
        }

    private:
        uint ni,nj,nk;
        std::vector<T> m_uData;
        std::vector<T> m_vData;
        std::vector<T> m_wData;
};
#endif

