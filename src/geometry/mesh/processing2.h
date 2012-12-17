
#ifndef MESH_PROCESSING2_H
#define MESH_PROCESSING2_H
#include "mesh2.h"
#include <set>

namespace MeshUtils
{
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
    template <typename NT>
        void genNormals(Mesh2<NT> & mesh)
        {
            typedef typename Mesh2<NT>::NumTraits::Vector2 Vector2;
            mesh.normals.resize(mesh.vertices.size());
            for(typename std::vector<Vector2>::iterator it=mesh.normals.begin(); it<mesh.normals.end(); ++it)
            {
                it->setZero();

            }
            //make the normal hold the sum value of all of its neighbors
            for(std::vector<Edge>::iterator it=mesh.faces.begin(); it<mesh.faces.end(); ++it)
            {

                Vector2 a=mesh.vertices[(*it)[1]]-mesh.vertices[(*it)[0]];
                mesh.normals[(*it)[0]] += a.norm() * Vector2(-a(1),a(0));

            }

            unsigned int count=0;
            for(typename std::vector<Vector2>::iterator it=mesh.normals.begin();
                    it<mesh.normals.end(); ++it, ++count)
            {
                //normalize the mesh.normals
                it->normalize();
                //print out errors if nans or still-unnormalized vectors found (check mostly for nan)
                if(it->norm() > 0.5 && it->norm() < 1.5)
                    continue;
                else
                    std::cout << "bad norm found!: " <<count<< " valued "<< (*it)(0) << ","<< (*it)(1) << ","<< (*it)(2) << std::endl;

            }
        }
};
#endif
