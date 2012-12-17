#ifndef MESH_RENDER_H
#define MESH_RENDER_H
#include "mesh.h"



namespace MeshUtils
{
    
    template <typename T>
        struct GLEnumType
        {
            static const GLenum type=GL_FLOAT;
        };
    template <>
        struct GLEnumType<double>
        {
            static const GLenum type=GL_DOUBLE;
        };
    template <typename MeshType>
void render(MeshType & mesh, bool wireframe, bool drawNormals)
{
    
    typedef typename MeshType::NumTraits::Scalar Scalar;
    typedef typename MeshType::NumTraits::Vector3 Vector3;
    typedef GLEnumType<Scalar> enumType;

    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (wireframe) {
        glVertexPointer(3, enumType::type, 0, (Scalar *)mesh.vertices.data());
        glDrawElements(GL_LINES, mesh.edges.size()*2, GL_UNSIGNED_INT, mesh.edges.data());
    } else {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        glShadeModel(GL_SMOOTH);

        glEnableClientState(GL_NORMAL_ARRAY);
        glVertexPointer(3, enumType::type, 0, (Scalar *)mesh.vertices.data());
        glNormalPointer(enumType::type, 0, (Scalar *)mesh.normals.data());
        glDrawElements(GL_TRIANGLES, mesh.faces.size()*3, GL_UNSIGNED_INT, (uint *)mesh.faces.data());

        glDisableClientState(GL_NORMAL_ARRAY);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
    }

    if (drawNormals) {
        std::vector<Vector3> n;
        for (unsigned int i = 0; i < mesh.normals.size(); ++i)
        {
            n.push_back(mesh.vertices[i]);
            n.push_back((mesh.vertices[i] + mesh.normals[i]  * 0.02f));
        }
        glVertexPointer(3, enumType::type, 0, (Scalar *)n.data());
        glDrawArrays(GL_LINES, 0, n.size());
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_DEPTH_TEST);
}

};
#endif
