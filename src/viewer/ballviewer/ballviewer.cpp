#include "ballviewer.h"
#include <boost/algorithm/string.hpp>
#include <GL/glu.h>

const std::string BallViewerTraits::FileExtension = std::string("*.obj");
void BallViewerObject::render()
{
    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);

    GLUquadric *quadric = gluNewQuadric();
    for(std::vector<Vertex>::iterator it=m_obj.begin(); it!=m_obj.end(); ++it)
    {
        glPushMatrix();
        {
            glTranslatef((*it)(0),(*it)(1),(*it)(2));
            gluSphere(quadric,radius,8,8);
        }
        glPopMatrix();

    }

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_DEPTH_TEST);
}


void BallViewerObject::load(std::ifstream & in)
{
    if(!in.is_open()) return; 
    //temporary variables
    std::string line;
    std::string s;
    Vertex x;


    //expect lines to only be up to this long
    line.reserve(64);

    while( in.good() )
    {
        getline(in,line);
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
                    double w;
                    ss >> w;
                    x/=w;
                }
                m_obj.push_back(x);
            }
            else
            if(type.compare("r") == 0)
            {
                //vertex
                ss >> radius; 

            }
            else
            {
            }

        }

    }
    if(m_obj.size()==0){return;}
    Vertex minBound=m_obj[0];
    Vertex maxBound=m_obj[0];
    for(std::vector<Vertex>::iterator it=m_obj.begin(); it<m_obj.end(); ++it)
    {
        minBound.noalias() = minBound.cwiseMin(*it);
        maxBound.noalias() = maxBound.cwiseMax(*it);
    }

    Vertex mid = 0.5*(minBound + maxBound);
    float range = (maxBound - minBound).maxCoeff();

    for(std::vector<Vertex>::iterator it=m_obj.begin(); it<m_obj.end(); ++it)
    {
        (*it) = ((*it)-mid)/range;

    }

    m_ready = true;
}
