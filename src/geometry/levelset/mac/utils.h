namespace grid_tags
{
    struct cell{};//offset: 0.5,0.5,0.5
    struct vertex{};//offset: 0.0,0.0,0.0
    struct u{};//offset: 0.0,0.5,0.5
    struct v{};//offset: 0.5,0.0,0.5
    struct w{};//offset: 0.5,0.5,0.0
    struct du{};//offset: 0.5,0.0,0.0
    struct dv{};//offset: 0.0,0.5,0.0
    struct dw{};//offset: 0.0,0.0,0.5
    struct flux{};//placeholder for when passing things that want all flux types at once
    struct dflux{};//placeholder for when passing things that want all flux types at once
};
namespace loop_type
{
    struct full{};
    struct ghost{};
};
//Different initialization sizes for when it's more than just the number of cells in that dimension
namespace StaggeredGridUtils
{
    template <typename T>
        inline uint NI(uint ni){return ni;}
    template <typename T>
        inline uint NJ(uint nj){return nj;}
    template <typename T>
        inline uint NK(uint nk){return nk;}
    template <typename T>
        inline uint NI_(uint ni){return ni;}
    template <typename T>
        inline uint NJ_(uint nj){return nj;}
    template <typename T>
        inline uint NK_(uint nk){return nk;}

    template <>
        inline uint NI<grid_tags::u>     (uint ni){return ni+1;}

    template <>       
        inline uint NJ<grid_tags::v>     (uint nj){return nj+1;}

    template <>       
        inline uint NK<grid_tags::w>     (uint nk){return nk+1;}

    template <>
        inline uint NJ<grid_tags::du>     (uint nj){return nj+1;}
    template <>
        inline uint NK<grid_tags::du>     (uint nk){return nk+1;}

    template <>
        inline uint NI<grid_tags::dv>     (uint ni){return ni+1;}
    template <>
        inline uint NK<grid_tags::dv>     (uint nk){return nk+1;}

    template <>
        inline uint NI<grid_tags::dw>     (uint ni){return ni+1;}
    template <>
        inline uint NJ<grid_tags::dw>     (uint nj){return nj+1;}

    template <>       
        inline uint NI<grid_tags::vertex>(uint ni){return ni+1;}
    template <>       
        inline uint NJ<grid_tags::vertex>(uint nj){return nj+1;}
    template <>       
        inline uint NK<grid_tags::vertex>(uint nk){return nk+1;}
    //When we want to catch the original NI/NJ/NK of hte whole grid
        /*
    template <>
        inline uint NI_<grid_tags::u>     (uint ni){return ni-1;}

    template <>       
        inline uint NJ_<grid_tags::v>     (uint nj){return nj-1;}

    template <>       
        inline uint NK_<grid_tags::w>     (uint nk){return nk-1;}

    template <>       
        inline uint NI_<grid_tags::vertex>(uint ni){return ni-1;}
    template <>       
        inline uint NJ_<grid_tags::vertex>(uint nj){return nj-1;}
    template <>       
        inline uint NK_<grid_tags::vertex>(uint nk){return nk-1;}
        */


    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk, grid_tags::cell){return Vector3(0.5/ni,0.5/nj,0.5/nk);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk,grid_tags::vertex){return Vector3(0.0,0.0,0.0);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk,grid_tags::u){return Vector3(0.0,0.5/nj,0.5/nk);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk,grid_tags::v){return Vector3(0.5/ni,0.0,0.5/nk);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk,grid_tags::w){return Vector3(0.5/ni,0.5/nj,0.0);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk, grid_tags::du){return Vector3(0.5/ni,0.0,0.0);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk, grid_tags::dv){return Vector3(0.0,0.5/nj,0.0);}
    template <typename Vector3>
        Vector3 origin(uint ni, uint nj, uint nk, grid_tags::dw){return Vector3(0.0,0.0,0.5/nk);}

    template <typename Vector2>
        Vector2 origin2(uint ni, uint nj, grid_tags::cell){return Vector2(0.5/ni,0.5/nj);}
    template <typename Vector2>
        Vector2 origin2(uint ni, uint nj, grid_tags::vertex){return Vector2(0.0,0.0);}
    template <typename Vector2>
        Vector2 origin2(uint ni, uint nj, grid_tags::u){return Vector2(0.0,0.5/nj);}
    template <typename Vector2>
        Vector2 origin2(uint ni, uint nj, grid_tags::v){return Vector2(0.5/ni,0.0);}


    /*
       template <typename T, typename Vector3, typename GridType>
       inline void vertex(const uint & ni, const uint & nj, const uint & nk,
       const uint & i, const uint & j, const uint & k,
       Vector3 & v, GridType){}
       */




};
