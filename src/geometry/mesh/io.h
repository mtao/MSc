#ifndef MESH_IO_H
#define MESH_IO_H
#include "mesh.h"
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#define CHECK_FOR_ZERO_OBJ

#ifdef DONT_USE_TRAITS
class IOMesh: public Mesh
{
    public:
        virtual int read(std::ifstream & infile) = 0;
        virtual int write(std::ofstream & outfile) = 0;
        int read(const std::string & fname )
        {

            filename = fname;
            std::ifstream is(filename.c_str());
            return read(is);

        }
};

class PLYMesh: public IOMesh
{
    public:
        int read(std::ifstream & infile);
        int write(std::ofstream & outfile);

};
class OBJMesh: public IOMesh
{
    public:
        int read(std::ifstream & infile);
        int write(std::ofstream & outfile);


};
#else
namespace MeshIOUtils
{
    class Parser
    {
        public:
            enum ERROR_CODE {SUCCESS, GENERAL_FAILURE};
            virtual void read(std::ifstream & infile) = 0;
            virtual void write(std::ofstream & outfile) = 0;
            void read(const char fname[])
            {

                filename = std::string(fname);
                std::ifstream is(fname);
                read(is);

            }
            void read(const std::string & fname )
            {

                filename = fname;
                std::ifstream is(filename.c_str());
                read(is);

            }
            ERROR_CODE code;
        private:
            std::string filename;

    };
    template <typename NT>
        class PLYParser: public Parser
    {
        public:
            typedef typename NT::Scalar Scalar;
            typedef typename NT::Vector3 Vector3;
            using Parser::read;
            PLYParser(): mesh(0) {}
            PLYParser(Mesh<NT> * mesh_): mesh(mesh_) {}
            void read(std::ifstream & infile);
            void write(std::ofstream & outfile);
            Mesh<NT> * mesh;

    };
    template <typename NT>
        class OBJParser: public Parser
    {
        public:
            typedef typename NT::Scalar Scalar;
            typedef typename NT::Vector3 Vector3;
            using Parser::read;
            OBJParser(): mesh(0) {}
            OBJParser(Mesh<NT> * mesh_): mesh(mesh_) {}
            void read(std::ifstream & infile);
            void write(std::ofstream & outfile);
            Mesh<NT> * mesh;


    };

    template <typename NT>
        void OBJParser<NT>::read(std::ifstream & infile)
        {
            if(!mesh) mesh = new Mesh<NT>();
            if(!infile.is_open()){code=GENERAL_FAILURE;return;}
            //temporary variables
            std::string line;
            std::string s;
            Vector3 x;
            Face f;
#ifdef CHECK_FOR_ZERO_OBJ
            unsigned int minVertIndex=100000;
#endif

            //expect lines to only be up to this long
            line.reserve(64);

            while( infile.good() )
            {
                getline(infile,line);
                if(line.length()>0)
                {
                    boost::trim(line);
                    std::stringstream ss(line);
                    std::string type;
                    ss >> type;

                    if(type.compare("v") == 0)
                    {
                        //vertex
                        ss >> x(0) >> x(1) >> x(2);
                        if(!ss.eof())
                        {
                            Scalar w;
                            ss >> w;
                            x/=w;
                        }
                        mesh->vertices.push_back(x);
                    }
                    else if(type[0]=='f')
                    {
                        //Face (assumed to be triangular or quad)
                        for( int i=0; i<3; ++i)
                        {

                            ss >> s;
                            boost::tokenizer<> tok(s);

                            boost::tokenizer< >::iterator tok_it = tok.begin();
                            if(tok_it!=tok.end())
                            {//vertex
                                f[i] = atoi(tok_it->c_str())-1;
#ifdef CHECK_FOR_ZERO_OBJ
                                minVertIndex=(minVertIndex>f[i]+1)?f[i]+1:minVertIndex;
#endif

                            }
                        }
                        mesh->faces.push_back(f);
                        if(!ss.eof())//maybe this is a quad, if it's more than a quad I won't deal with it (and just fail)
                        {

                            ss >> s;
                            boost::tokenizer<> tok(s);

                            boost::tokenizer< >::iterator tok_it = tok.begin();
                            if(tok_it!=tok.end())
                            {//vertex
                                //swap first and last to keep ccw
                                f[0]^=f[2];
                                f[2]^=f[0];
                                f[0]^=f[2];
                                f[1] = atoi(tok_it->c_str())-1;
#ifdef CHECK_FOR_ZERO_OBJ
                                minVertIndex=(minVertIndex>f[1]+1)?f[1]+1:minVertIndex;
#endif

                            }
                            mesh->faces.push_back(f);


                        }

                    }
                    else
                    {
                        //Comment or something we don't know how to parse

                    }

                }

            }

#ifdef CHECK_FOR_ZERO_OBJ
            if(minVertIndex==0)
            {
                std::cout << "This wasn\'t a real OBJ file, vertices started at 0\n";
                for(std::vector<Face>::iterator it=mesh->faces.begin(); it<mesh->faces.end(); ++it)
                {
                    (*it)[0]+=1;
                    (*it)[1]+=1;
                    (*it)[2]+=1;
                }
            }
#endif
            code=SUCCESS;
            return;

        }


    template <typename NT>
        void OBJParser<NT>::write(std::ofstream & outfile)
        {
            for(typename std::vector<Vector3>::iterator it=mesh->vertices.begin(); it<mesh->vertices.end(); ++it)
            {
                outfile << "v";
                for(int i=0; i<3; ++i)
                {
                    outfile << " " << (*it)(i);
                }
                outfile << std::endl;
            }
            for(std::vector<Face>::iterator it=mesh->faces.begin(); it<mesh->faces.end(); ++it)
            {
                outfile << "f";
                for(int i=0; i<3; ++i)
                {
                    outfile << " " << ((*it)[i]+1);
                }
                outfile << std::endl;
            }

            return;
        }
    template <typename NT>
        void writePBRT(Mesh<NT> & mesh, std::ofstream & outfile)
        {
            outfile << "Shape \"trianglemesh\"" << std::endl;
            outfile << "    \"Point p\" [" << std::endl;
            for(typename std::vector<typename NT::Vector3>::iterator it=mesh.vertices.begin(); it<mesh.vertices.end(); ++it)
            {
                outfile << "       ";
                for(int i=0; i<3; ++i)
                {
                    outfile << " " << (*it)(i);
                }
                outfile << std::endl;
            }
            outfile << "    ]" << std::endl;
            outfile << "    \"integer indices\" [" << std::endl;
            for(std::vector<Face>::iterator it=mesh.faces.begin(); it<mesh.faces.end(); ++it)
            {
                outfile << "       ";
                for(int i=0; i<3; ++i)
                {
                    outfile << " " << (*it)[i];
                }
                outfile << std::endl;
            }
            outfile << "    ]" << std::endl;
        }

};

#endif
#endif
