#ifndef BATTY_HASH_FUNCS
#define BATTY_HASH_FUNCS
// Transforms even the sequence 0,1,2,3,... into reasonably good random numbers 
// Challenge: improve on this in speed and "randomness"!
// This seems to pass several statistical tests, and is a bijective map (of 32-bit unsigned ints)
inline unsigned int randhash(unsigned int seed)
{
   unsigned int i=(seed^0xA3C59AC3u)*2654435769u;
   i^=(i>>16);
   i*=2654435769u;
   i^=(i>>16);
   i*=2654435769u;
   return i;
}

// the inverse of randhash
inline unsigned int unhash(unsigned int h)
{
   h*=340573321u;
   h^=(h>>16);
   h*=340573321u;
   h^=(h>>16);
   h*=340573321u;
   h^=0xA3C59AC3u;
   return h;
}

// returns repeatable stateless pseudo-random number in [0,1]
inline double randhashd(unsigned int seed)
{ return randhash(seed)/(double)UINT_MAX; }
inline float randhashf(unsigned int seed)
{ return randhash(seed)/(float)UINT_MAX; }

// returns repeatable stateless pseudo-random number in [a,b]
inline double randhashd(unsigned int seed, double a, double b)
{ return (b-a)*randhash(seed)/(double)UINT_MAX + a; }
inline float randhashf(unsigned int seed, float a, float b)
{ return ( (b-a)*randhash(seed)/(float)UINT_MAX + a); }
#endif
