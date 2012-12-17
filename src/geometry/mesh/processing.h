#ifndef MESH_PROCESSING_H
#define MESH_PROCESSING_H
#include "mesh.h"
#include <set>

namespace MeshUtils
{


    template <typename NT>
        void genEdges(Mesh<NT> & mesh)
        {
            Edge e;
            std::vector<std::set< unsigned int > > edges(mesh.vertices.size());

            //assume CCW so everything is added only once
            for(std::vector<Face>::iterator it=mesh.faces.begin(); it<mesh.faces.end(); ++it)
            {
                Face & f = *it;
                for(int j=0; j<3; ++j)
                {
                    e[0]=f[j];
                    e[1]=f[(j+1)%3];
                    if(e[0]>e[1])
                    {
                        //swap
                        e[0]^=e[1];
                        e[1]^=e[0];
                        e[0]^=e[1];
                    }
                    if(edges[e[0]].find(e[1])==edges[e[0]].end())
                    {
                        edges[e[0]].insert(e[1]);
                        mesh.edges.push_back(e);
                    }

                }
            }

        }

    template <typename T>
        T circumradius(T a, T b, T c)
        {
            T denom = ((a+b+c)*(-a+b+c)*(a-b+c)*(a+b-c));
            if(denom==0)
                return 1;
            return (a*b*c)/denom;
        }





    template <typename NT>
        void genNormals(Mesh<NT> & mesh)
        {
            typedef typename Mesh<NT>::NumTraits::Vector3 Vector3;
            mesh.normals.resize(mesh.vertices.size());
            for(typename std::vector<Vector3>::iterator it=mesh.normals.begin(); it<mesh.normals.end(); ++it)
            {
                it->setZero();

            }
            //make the normal hold the sum value of all of its neighbors
            for(std::vector<Face>::iterator it=mesh.faces.begin(); it<mesh.faces.end(); ++it)
            {

                Vector3 a=mesh.vertices[(*it)[1]]-mesh.vertices[(*it)[0]];
                Vector3 b=mesh.vertices[(*it)[2]]-mesh.vertices[(*it)[0]];
                Vector3 c=mesh.vertices[(*it)[2]]-mesh.vertices[(*it)[1]];
                Vector3 n = (a).cross(b);
                float CR = circumradius(a.norm(),b.norm(),c.norm());
                for(int i=0; i<3; ++i)
                    mesh.normals[(*it)[i]] += n*CR;
            }

            unsigned int count=0;
            for(typename std::vector<Vector3>::iterator it=mesh.normals.begin();
                    it<mesh.normals.end(); ++it, ++count)
            {
                //wherever the norm is degenerate (zero) pick the normal of one of its neighbors
                if(it->norm()==0)
                {
                    for(std::vector<Edge>::iterator e_it=mesh.edges.begin(); e_it<mesh.edges.end(); ++e_it)
                    {
                        if((*e_it)[0] == count)
                        {
                            (*it) = mesh.normals[(*e_it)[1]];
                            break;
                        }
                        if((*e_it)[1] == count)
                        {
                            (*it) = mesh.normals[(*e_it)[0]];
                            break;
                        }
                    }
                }
                //normalize the mesh.normals
                it->normalize();
                //print out errors if nans or still-unnormalized vectors found (check mostly for nan)
                if(it->norm() > 0.5 && it->norm() < 1.5)
                    continue;
                else
                    std::cout << "bad norm found!: " <<count<< " valued "<< (*it)(0) << ","<< (*it)(1) << ","<< (*it)(2) << std::endl;

            }
        }
    template <typename NT>
        void genTertiaryData(Mesh<NT> & mesh)
        {
            genEdges(mesh);
            genNormals(mesh);

        }


    //Put the mesh in a cube and 
    template <typename Vector>
        void normalize(std::vector<Vector> & vertices)
        {
            if(vertices.size()==0){return;}
            Vector minBound=vertices[0];
            Vector maxBound=vertices[0];
            for(typename std::vector<Vector>::iterator it=vertices.begin(); it<vertices.end(); ++it)
            {
                minBound.noalias() = minBound.cwiseMin(*it);
                maxBound.noalias() = maxBound.cwiseMax(*it);
            }

            Vector mid = 0.5*(minBound + maxBound);
            float range = (maxBound - minBound).maxCoeff();

            for(typename std::vector<Vector>::iterator it=vertices.begin(); it<vertices.end(); ++it)
            {
                (*it) = ((*it)-mid)/range;

            }
        }
    template <typename Mesh>
        void normalize(Mesh & mesh)
        {
            normalize(mesh.vertices);
        }

    template <typename Mesh>
        void prepare(Mesh & mesh)
        {
            genTertiaryData(mesh);
            normalize(mesh);
        }

    template <typename T>
        void vector_append(T & base,T & add)
        {
            base.insert(base.end(),add.begin(),add.end());
        }


    template <int N>
        void offsetIndices(std::vector<struct IndexSet<N> > & vec, unsigned int v_off, int off)
        {
            for(typename std::vector<struct IndexSet<N> >::iterator it=(vec.begin()+v_off); it<vec.end(); ++it)
            {
                for(int i=0; i<N; ++i)
                {
                    (*it)[i] += off;
                }
            }

        }

    template <typename NT>
        void append(Mesh<NT> & mesh,Mesh<NT> & other)
        {

            unsigned int v_off = mesh.vertices.size();
            unsigned int e_off = mesh.edges.size();
            unsigned int f_off = mesh.faces.size();
            vector_append(mesh.vertices,other.vertices);
            vector_append(mesh.normals,other.normals);
            vector_append(mesh.edges,other.edges);
            vector_append(mesh.faces,other.faces);
            offsetIndices(mesh.edges,e_off,v_off);
            offsetIndices(mesh.faces,f_off,v_off);


        }

    template <typename NT>
        void copy(Mesh<NT> & mesh,Mesh<NT> & other)
        {

            mesh.vertices   = other.vertices;
            mesh.normals    = other.normals ; 
            mesh.edges      = other.edges   ;
            mesh.faces      = other.faces   ;

        }
};
#endif
