#ifndef VECTOR_UTIL_H
#define VECTOR_UTIL_H
#include <cmath>

#define USE_EIGEN
#ifdef USE_EIGEN

#include <Eigen/Dense>
#include <Eigen/Sparse>

#else//USE MY OWN VECTOR IMPLEMENTATION

template <typename T>
class Vector3
{
    public:
        Vector3()
        {
            m_data[0]=(T)0;
            m_data[1]=(T)0;
            m_data[2]=(T)0;
        }
        Vector3(T x, T y, T z)
        {
            m_data[0]=x;
            m_data[1]=y;
            m_data[2]=z;
        }
        inline T & operator()(unsigned int i)
        {//unsafe no checks
            return m_data[i];
        }
        inline T & operator[](unsigned int i)
        {//unsafe no checks
            return m_data[i];
        }

        inline Vector3<T> operator+(const Vector3<T> & other)
        {
            return Vector3<T>(
                    m_data[0]+other(0),
                    m_data[1]+other(1),
                    m_data[2]+other(2)
                    );
        }
        inline Vector3<T> operator-(const Vector3<T> & other)
        {
            return Vector3<T>(
                    m_data[0]-other(0),
                    m_data[1]-other(1),
                    m_data[2]-other(2)
                    );
        }
        inline Vector3<T> & operator+=(const Vector3<T> & other)
        {
                    m_data[0]+=other(0);
                    m_data[1]+=other(1);
                    m_data[2]+=other(2);
                    return *this;
        }
        inline Vector3<T> & operator-=(const Vector3<T> & other)
        {
                    m_data[0]+=other(0);
                    m_data[1]+=other(1);
                    m_data[2]+=other(2);
                    return *this;
        }
        inline Vector3<T> & operator*=(T & other)
        {
                    m_data[0]*=other;
                    m_data[1]*=other;
                    m_data[2]*=other;
                    return *this;
        }
        inline Vector3<T> & operator/=(T & other)
        {
                    m_data[0]/=other;
                    m_data[1]/=other;
                    m_data[2]/=other;
                    return *this;
        }
        inline float norm()
        {
            return std::sqrt(
                    m_data[0]*m_data[0]+
                    m_data[1]*m_data[1]+
                    m_data[2]*m_data[2]);
        }
        inline void setZero()
        {
                    m_data[0]=0;
                    m_data[1]=0;
                    m_data[2]=0;

        }
    private:
        T m_data[3];
};


#endif

#ifdef USE_EIGEN
typedef Eigen::Vector3f Vertex;

#else//USE MY OWN VECTOR IMPLEMENTATION
typedef Vector3<float> Vertex;
#endif
#ifdef USE_EIGEN
#undef USE_EIGEN
#endif

//Distance between two points ||a-b||
//Input: two points
//Output: returns the distance
template <typename S, typename T>
inline S dist(T a, T b)
{
    return (a-b).norm();
}
#endif
