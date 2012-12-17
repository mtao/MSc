#/usr/bin/env python
import os, sys
import struct


infile = sys.argv[1]
outfile = sys.argv[2]
ni,nj,nk = int(sys.argv[3]),int(sys.argv[4]),int(sys.argv[5])
print ("{0} {1} {2}".format(ni,nj,nk))

o = open(outfile,"w")

o.write("{0} {1} {2}\n".format(ni,nj,nk))
min=0;
max=0;

with open(infile,"rb") as i:
    byte = i.read(1)
    while byte:
        val = float(struct.unpack("B",byte)[0])
        if min>val: min = val;
        if max<val: max = val;
        o.write(str(val)+ " ")
        byte = i.read(1)


print("{0} {1}".format(min,max))
