#ifndef GRID_H
#define GRID_H
#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>
typedef unsigned int uint;

template <typename  CONTAINER >
struct GridContainerTraits
{
    typedef CONTAINER Container;
    typedef typename Container::value_type Scalar;
    typedef typename Container::iterator iterator;
};
template <typename T>
struct GridTraits
{
    typedef T Scalar;
    typedef std::vector<Scalar> Container;
    typedef typename Container::iterator iterator;
};


typedef GridTraits<char > GridTraitsc;
typedef GridTraits<float > GridTraitsf;
typedef GridTraits<double > GridTraitsd;
typedef GridTraits<int> GridTraitsi;
typedef GridContainerTraits<std::vector<char> > GridVectorTraitsc;
typedef GridContainerTraits<std::vector<float> > GridVectorTraitsf;
typedef GridContainerTraits<std::vector<double> > GridVectorTraitsd;
typedef GridContainerTraits<std::vector<int> > GridVectorTraitsi;

template <typename T>
class VectorPointer
{
    public:
        typedef T value_type;
        VectorPointer( std::vector<T> * data): m_data(data) {}
        
        void resize(uint i) {m_data->resize(i);}
        inline T & operator[](uint i){return (*m_data)[i];}
        inline uint size(){return m_data->size();}
        inline typename std::vector<T>::iterator begin(){return m_data->begin();}
        inline typename std::vector<T>::iterator end(){return m_data->end();}
        inline typename std::vector<T> * get_data(){return m_data;}
        typedef typename std::vector<T>::iterator iterator;
    private:
        std::vector<T> * m_data;
};
template <typename  T>
struct GridPointerTraits
{
    typedef T Scalar;
    typedef VectorPointer<Scalar> Container;
    typedef typename Container::iterator iterator;
};

typedef GridPointerTraits<char > GridPointerTraitsc;
typedef GridPointerTraits<float > GridPointerTraitsf;
typedef GridPointerTraits<double > GridPointerTraitsd;
typedef GridPointerTraits<int > GridPointerTraitsi;
typedef GridContainerTraits<VectorPointer<char> > GridVectorPointerTraitsc;
typedef GridContainerTraits<VectorPointer<float> > GridVectorPointerTraitsf;
typedef GridContainerTraits<VectorPointer<double> > GridVectorPointerTraitsd;
typedef GridContainerTraits<VectorPointer<int> > GridVectorPointerTraitsi;


template <typename GT = GridTraitsf >
struct Grid
{
    public:
        typedef GT Traits;
        typedef typename Traits::Scalar Scalar;
        typedef typename Traits::Container Container;
        typedef typename Traits::iterator iterator;
        Grid(uint _NI=1, uint _NJ=1, uint _NK=1): 
            m_NI(_NI), m_NJ(_NJ), m_NK(_NK), 
            m_data(m_NI*m_NJ*m_NK) {}
        Grid(uint _NI, uint _NJ, uint _NK, Container & data): 
            m_NI(_NI), m_NJ(_NJ), m_NK(_NK), 
            m_data(data) {}
        ~Grid(){}

        inline uint index(uint i=0, uint j=0, uint k=0) const {return i+m_NI*(j+m_NJ*k);}
        
        inline void resize(const uint i=1,const uint j=1,const uint k=1) {m_NI=i;m_NJ=j,m_NK=k,m_data.resize(i*j*k);}

        inline Scalar & operator() (const uint i=0, const uint j=0, const uint k=0) {return val(i,j,k);}
        inline const Scalar & operator() (const uint i=0, const uint j=0, const uint k=0) const {return val(i,j,k);}

        inline Scalar & val (const uint i=0, const uint j=0, const uint k=0) {return m_data[index(i,j,k)];}
        inline const Scalar & val (const uint i=0, const uint j=0, const uint k=0) const {return m_data[index(i,j,k)];}

        inline void fill(const Scalar & val) {std::fill(m_data.begin(),m_data.end(), val);}

        iterator  begin(){return m_data.begin();}
        iterator  end(){return m_data.end();}


        inline uint const NI()const{return m_NI;}
        inline uint const NJ()const{return m_NJ;}
        inline uint const NK()const{return m_NK;}
        inline uint const size()const{return m_data.size();}
        inline Container & data(){return m_data;}
        inline Scalar * raw_data(){return m_data.data();}

        //inline Grid<GridContainerTraits<VectorPointer<Scalar> > > toVectorPointer()
        inline Grid<GridPointerTraits<Scalar> > toVectorPointer()
        {
            return Grid<GridPointerTraits<Scalar> >(m_NI,m_NJ,m_NK,&m_data);
        }

        void read(std::ifstream & in);
        void write(std::ofstream & out);

    protected:
        uint m_NI,m_NJ,m_NK;
        Container m_data;
};

template <typename NT>
void Grid<NT>::read(std::ifstream & in)
{
    if(!in.is_open()) return;
    in >> m_NI >> m_NJ >> m_NK;
    m_data.resize(m_NI*m_NJ*m_NK);

    std::istream_iterator<Scalar> eos;
    std::istream_iterator<Scalar> isit(in);
    iterator it=begin();
    for(;isit!=eos && it<end(); ++it, ++isit)
        *it=*isit;

}

template <typename NT>
void Grid<NT>::write(std::ofstream & out)
{
    out << m_NI << " " << m_NJ << " " << m_NK << std::endl;
    for(iterator it=begin(); it<end(); ++it)
        out << *it << " ";
    out << std::endl;
    
}


typedef Grid<GridTraitsf> Gridf;
typedef Grid<GridTraitsd> Gridd;
typedef Grid<GridTraitsc> Gridc;
typedef Grid<GridTraitsi> Gridi;
typedef Grid<GridPointerTraitsf> GridPf;
typedef Grid<GridPointerTraitsd> GridPd;
typedef Grid<GridPointerTraitsc> GridPc;
typedef Grid<GridPointerTraitsi> GridPi;

#endif
