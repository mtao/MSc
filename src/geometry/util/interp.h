#ifndef INTERP_UTIL_H
#define INTERP_UTIL_H
//-----------------------------------------------------------------------------
//Interpolation tools
//-----------------------------------------------------------------------------

//Linear Interpolation
//Input: 
template <typename S, typename T>
inline S lerp(const S& v0, const S& v1, const T& f)
{
    return f*v1+(1-f)*v0;
}

//Bilinear Interpolation
template <typename S, typename T>
inline S bilerp(const S& v00, const S& v10,
                const S& v01, const S& v11,
                const T& fx, const T& fy)
{
    return lerp(
            lerp(v00,v10,fx),
            lerp(v01,v11,fx),
            fy);
}

//Trilinear Interpolation
template <typename S, typename T>
inline S trilerp(
                const S& v000, const S& v100,
                const S& v010, const S& v110,
                const S& v001, const S& v101,
                const S& v011, const S& v111,
                const T& fx, const T& fy, const T& fz)
{
    return lerp(bilerp(v000, v100, v010, v110, fx, fy),
                bilerp(v001, v101, v011, v111, fx, fy),
                fz);
}



#endif
