#ifndef INDEX_UTIL_H
#define INDEX_UTIL_H

template <unsigned int N>
struct IndexSet{
    unsigned int m_data[N];
    unsigned int & operator[](const int & i)
    {
        return m_data[i];
    }
    const static unsigned int size = N;
};

typedef struct IndexSet<2> Edge;
typedef struct IndexSet<3> Face;

#endif
