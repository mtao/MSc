#ifndef BOUND_UTIL_H
#define BOUND_UTIL_H
//-----------------------------------------------------------------------------
//Low level math tools
//-----------------------------------------------------------------------------


/*
//pick the smaller value
template <typename T>
inline T min(T a, T b)
{
    if(a<b) return a;
    return b;
}
*/

//pick the smaller nonzero value
template <typename T>
inline T fminS(T a, T b)
{
    if(a>0 && a<b) 
        return a;
    if(b>0)
        return b;
    return 0;
}

//clamp a by upper and lower
template <typename T>
inline T clamp(T a, T lower, T upper)
{
    if(a<lower) return lower;
    if(a>upper) return upper;
    return a;
}




#endif
