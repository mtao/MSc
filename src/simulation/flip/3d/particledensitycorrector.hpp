#ifndef PARTICLEDENSITYCORRECTOR_HPP
#define PARTICLEDENSITYCORRECTOR_HPP
#include "geometry/util/types.h"
#include <vector>
template <typename NumTraits, typename Grid>
class ParticleDensityCorrector
{
public:
    typedef typename NumTraits::Scalar Scalar;
    typedef typename NumTraits::Vector3 Particle;
    typedef typename NumTraits::Vector3 Vector;
    typedef std::vector<Particle> ParticleList;
    typedef std::vector<int> Bucket;
    typedef std::vector< Bucket > IndexBuckets;
    ParticleDensityCorrector(ParticleList & pl, ParticleList & pv, const Grid & grid, Scalar & particle_radius):
        particles(pl)
      , velocities(pv)
      , grid(grid)
      , particle_radius(particle_radius)
    {}

    uint size(){return grid.size();}
    uint NI(){return grid.NI();}
    uint NJ(){return grid.NJ();}
    uint NK(){return grid.NK();}
    uint index(uint i, uint j,uint k){return grid.index(i,j,k);}
    void barycentric(const Particle& p, uint & i, Scalar & fi, uint & j, Scalar & fj, uint & k, Scalar & fk)
    {
        grid.barycentric(p,i,fi,j,fj,k,fk);
    }






    uint getBucketIndex(uint index)
    {
        return getBucketIndex(particles[index]);
    }
    uint getBucketIndex(const Particle& p)
    {
        uint i,j,k;
        Scalar fi,fj,fk;
        barycentric(p,i,fi,j,fj,k,fk);
        return index(i,j,k);
    }

    Bucket & getBucket(uint i, uint j,uint k){return buckets[index(i,j,k)];}
    Bucket & getBucket(const Particle & p){return buckets[getBucketIndex(p)];}
    Bucket & getBucket(uint index){return buckets[getBucketIndex(index)];}

    void populateBuckets()
    {
        buckets.resize(grid.size());
        std::fill(buckets.begin(), buckets.end(),Bucket());
        for(int i=0; i < particles.size(); ++i)
        {
            getBucket(i).push_back(i);
        }
    }
    //Per cell
    void twiddleParticles(uint i, uint j,uint k)
    {
        Bucket & bucket = getBucket(i,j,k);
        Bucket indices = bucket;
        ParticleList offsets(bucket.size());
        std::vector<Scalar> weights(bucket.size());
        for(int m=i-1; m<i+1; ++m)
        for(int n=j-1; n<j+1; ++n)
        for(int o=k-1; o<k+1; ++o)
        {
            if(
                    i<=0 || j <=0 || k <= 0 ||
                    i <= NI()-1 || j <= NJ()-1 || k <= NK()-1
                    )
                continue;
            Bucket & b = getBucket(m,n,o);
            indices.insert(indices.end(), b.begin(), b.end());

        }

        int ind=0;
        for(auto it=bucket.begin(); it < bucket.end(); ++it)
        {
            Particle & p = particles[*it];
            Particle & v = velocities[*it];

            for(auto it2=indices.begin(); it2 < indices.end(); ++it2)
            {
                Particle & p1 = particles[*it2];
                Particle & v1 = velocities[*it2];
                Vector u = p1 - p;
                Scalar dist = u.norm();
                u.normalize();
                if(dist < 0.00001) //Some epsilon
                {
                    //TODO: mark this for deletion
                }
                else
                {
                    Scalar intersection = dist - particle_radius;
                    if (intersection < 0)
                    {
                        Vector offset = .5 * intersection* u;
                        Scalar v_normal_speed = v.dot(u);
                        Scalar v1_normal_speed = v1.dot(u);
                        Scalar sum = (v_normal_speed+v1_normal_speed);
                        Scalar diff = (v1_normal_speed-v_normal_speed);
                        p += offset;
                        //v -= v_normal_speed * u;
                        v -= -diff * u;
                        p1 -= offset;
                        //v1 -= v1_normal_speed * u;
                        v1 -= diff  * u;
                        weights[ind] += intersection*intersection;
                        offsets[ind] += offset * intersection*intersection;
                    }
                }


            }
            ++ind;
        }
        /*
        for(int i=0; i < bucket.size(); ++i)
        {
            if(weights[i]>0.001)
                particles[bucket[i]] += offsets[i]/weights[i];
        }
        */
    }
    //for whole grid
    void twiddleParticles()
    {
        populateBuckets();
        for(int i=0; i < NI(); ++i)
            for(int j=0; j < NJ(); ++j)
            for(int k=0; k < NK(); ++k)
                twiddleParticles(i,j,k);

    }


private:
    ParticleList & particles;
    ParticleList & velocities;
    Scalar & particle_radius;
    const Grid & grid;
    IndexBuckets buckets;
    Scalar elasticity = 0.0;


};

#endif // PARTICLEDENSITYCORRECTOR_HPP
