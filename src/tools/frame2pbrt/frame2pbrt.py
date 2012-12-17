#!/usr/bin/env python
import os, sys
import struct
import math
from functools import reduce

pbrtmain = '''
Film "image"
    "integer xresolution" [512] "integer yresolution" [512]
        "string filename" "{0}.exr"

Sampler "bestcandidate" "integer pixelsamples" [64]
PixelFilter "mitchell"

Scale -1 1 1
LookAt 0 3 8  0 .8 0   0 1 0
Camera "perspective" "float fov" [21]

SurfaceIntegrator "directlighting"

VolumeIntegrator "single" "float stepsize" [.025]

WorldBegin

AttributeBegin
  CoordSysTransform "camera"
  AreaLightSource "area" "color L" [8 8 8  ] "integer nsamples" [4]
  Translate 0 2 -10
  Material "matte" "color Kd" [ 0 0 0 ]
  Shape "disk" "float radius" [3] 
AttributeEnd

AttributeBegin
  AreaLightSource "area" "color L" [3.2 3.2 3.2] "integer nsamples" [4]
  Translate 0 10 0
  Rotate 90 1 0 0 
  Material "matte" "color Kd" [ 0 0 0 ]
  Shape "disk" "float radius" [20] 
AttributeEnd

AttributeBegin
  Material "matte" "color Kd" [.8 .8 .8 ]
  Shape "trianglemesh" "integer indices" [ 0 1 2 2 0 3 ]
        "point P" [-10 0 -10   10 0 -10   10 0 10   -10 0 10 ]
  Shape "trianglemesh" "integer indices" [ 0 1 2 2 0 3 ]
        "point P" [-10 0 -10   10 0 -10   10 9 -10   -10 9 -10 ]
  Shape "trianglemesh" "integer indices" [ 0 1 2 2 0 3 ]
          "point P" [-10 0 -10   -10 0 10   -10 9 10   -10 9 -10 ]
AttributeEnd



AttributeBegin
  Translate  0 1 -.5
  #Rotate 90 1 0 0
  Rotate 270 0 0 1
  Rotate 180 0 1 0
  AttributeBegin
    #Material "uber" "color Kd" [1 1 1] "color Ks" [.3 .3 .3] "color opacity" [0.3 0.3 0.3]
    Material "glass" "color Kr" [1 1 1] "color Kt" [1 1 1]  "float index" [1.3]
    #Material "matte" "color Kd" [0 0 1] 
    Shape "gridimplicit" "point p0" [ -1 -1 -1] "point p1" [1 1 1] "string phi_filename" "{0}.lphi.ls" 
    #Volume "wetvolumegrid" "point p0" [ -1 -1 -1] "point p1" [1 1 1] "string density_filename" "{0}.rdensity.ls" 
    #"color sigma_a" [.2 .35 .553] "color sigma_s" [.8 .65 .447]  "float g" 0.2 "float gp" -0.9 "string wetness_filename" "{0}.wetness.ls"
  AttributeEnd
AttributeEnd


WorldEnd
'''

class Grid:
    def __init__(nx,ny,data):
        self.nx = nx
        self.ny = ny
        self.data = data

class FileParser:
    def setDifferences(self, x, y):
        self.nx, self.ny = int(x),int(y)

        print("Dims: ",self.nx,self.ny)
        self.dx = 1/self.nx
        self.dy = 1/self.ny
        self.lphi_buffer = 2/self.nx
    def __init__(self, infile_path):
        print("input path: ", infile_path)
        (self.dirname,self.infile_name) = os.path.split(infile_path)
        if self.dirname == "":
            self.dirname = "./"
        (self.basename,self.extension) = os.path.splitext(self.infile_name)
        (self.basepath, extension) = os.path.splitext(infile_path)
        

        self.infile_fd = open(infile_path, "r")
        self.particles = []
        self.grids = {}
        self.setDifferences(50,50)#default



    def parse(self):
        '''
        Loop over lines in file trying to parse
        '''
        line = self.infile_fd.readline()
        while line:
            self.parse_line(line)
            line = self.infile_fd.readline()
        for grid in self.grids.values():
            self.nx = max(self.nx,grid[0][0])
            self.ny = max(self.ny,grid[0][1])

        self.setDifferences(self.nx,self.ny)

    def add_particle(self,tuple):
        '''
        Do processing to add particle to the system
        '''
        self.particles.append(tuple)


    def add_grid(self, name, dims):
        '''
        Parse input file for grid where the 2d dim is arleady known
        '''
        print(dims)
        total_data = reduce(lambda x,y: x*y, dims)
        data = []
        while len(data) < total_data:
            data.extend([ float(x) for x in self.infile_fd.readline().split() ])
        self.grids[name]=(dims,data)



    def parse_line(self,line):
        '''
        Check each line for which type of object is being parsed and parse it
        '''
        tokens = line.split()
        if len(tokens)==0: return
        if tokens[0][0] == '#': return
        if tokens[0] == "p":
            self.add_particle(
                    (float(tokens[1]),
                        float(tokens[2])))
        elif tokens[0] == "pr":
            self.particle_radius = float(tokens[1])
        else:
            self.add_grid(tokens[0], [float(x) for x in tokens[1:]])

    def convert2to3(self, depth):
        '''
        converts particles into a 3d particles
        '''
        '''
        print("Converting grids to 3d...")
        for key in self.grids.keys():
            self.grids[key] = (self.grids[key][0]+[depth], self.update_grid2to3(self.grids[key][1],depth))
            '''

        dz = 1/depth
        print("Converting 3d particles...")
        if self.particle_radius > dz:
            skip = .5 * self.particle_radius/dz
            if skip > dz:
                dz = skip
                depth = int(1/dz)

        self.particles = reduce(lambda x,y: x+y,
                map(lambda x: [(y[0], y[1], x*dz) for y in self.particles], range(depth))
                ,
                []
                )

    def update_grid2to3(self, grid, depth):
        '''
        convert a 2d grid to a 3d grid
        '''
        return reduce(lambda x,y: x+grid, range(depth),[])


    def barycentric_vert(self, pos, nx, ny):
        fi=self.nx*pos[0]
        i=int(fi)
        fi=fi-i
        fj=self.ny*pos[1]
        j=int(fj)
        fj=fj-j
        return (i,fi,j,fj)

    def barycentric_cell(self, pos, nx, ny):
        fi=self.nx*pos[0]-0.5
        i=int(fi)
        fi=fi-i
        fj=self.ny*pos[1]-0.5
        j=int(fj)
        fj=fj-j
        return (i,fi,j,fj)
    def lerp(self, griddata, i, j, fi, fj):
        dims,grid = griddata
        ind = (int((i) + (j) * dims[0]))
        zz = grid[int((i) + (j) * dims[0])];
        if j+1 >= dims[1]:
            return zz
        zo = grid[int((i) + (j+1) * dims[0])];
        if i+1 >= dims[0]:
            return zz
        oz = grid[int((i+1) + (j) * dims[0])];
        oo = grid[int((i+1) + (j+1) * dims[0])];

        return (1-fj)*((1-fi)*zz + fi * oz) + fj*((1-fi)*zo + fi * oo)

    def particle2Lphi(self, nx, ny):
        print("Writing grid info")
        bf = 3
        iter = [[(i,j) for i in range(-bf,bf)] for j in range(-bf,bf)]
        iter = reduce(lambda x,y: x+y,iter,[])
        grid = [[self.lphi_buffer for m in range(ny)] for n in range(nx)]
        minx=0;miny=1;maxx=0;maxy=0
        for p in self.particles:
            if math.isnan(p[0]) or math.isnan(p[1]): continue
            minx = min(minx,p[0])
            miny = min(miny,p[1])
            maxx = max(maxx,p[0])
            maxy = max(maxy,p[1])

            (i,fi,j,fj) = self.barycentric_vert(p,nx,ny)

            for (xp,yp) in iter:
                x,y = i + xp,j+yp
                if 0 > x or 0 > y or x>=nx or y>=ny: continue
                pos = (x/nx,y/ny)
                dist = ((p[0]-pos[0])**2+(p[1]-pos[1])**2)**.5-self.particle_radius
                if grid[x][y] > dist:
                    grid[x][y] = dist


        d,rhogrid = self.grids["rho"]
        print(d)
        print(self.nx,self.ny)
        val = lambda i,j: rhogrid[i+int(d[0])*j]
        for x in range(self.nx-1):
            for y in range(self.ny-1):
                if val(x,y) + val(x+1,y)+ val(x,y+1) +  val(x+1,y+1) < 3.8:
                    grid[x][y] = self.lphi_buffer

        return reduce(lambda x,y: x+y,grid,[])

    def particle2wetness(self, nx, ny):
        print("Writing wetness grid info")
        bf = math.ceil(self.particle_radius / self.dx/.3)
        iter = [[(i,j) for i in range(-bf,bf)] for j in range(-bf,bf)]
        iter = reduce(lambda x,y: x+y,iter,[])
        grid = [[0 for m in range(ny)] for n in range(nx)]
        d,rhogrid = self.grids["rho"]
        minx=0;miny=1;maxx=0;maxy=0
        val = lambda i,j: rhogrid[i+int(d[0])*j]
        for p in self.particles:
            if math.isnan(p[0]) or math.isnan(p[1]): continue

            (i,fi,j,fj) = self.barycentric_cell(p,nx,ny)

            for (xp,yp) in iter:
                x,y = i + xp,j+yp

                rhoval = val(x,y)
                if rhoval > 0.99: continue
                if 0 > x or 0 > y or x>=nx or y>=ny: continue
                pos = ((x+0.5)/nx,(y+0.5)/ny)
                radius = self.particle_radius / (1-rhoval)
                dist = ((p[0]-pos[0])**2+(p[1]-pos[1])**2)**.5-radius
                if dist > 0: continue
                dist = (dist / -radius)
                weight = (1-dist)**3*45/3.1415967 * (1-rhoval) / 10
                grid[x][y] += weight



        clamp = lambda x: min(1,max(0,x))
        print(clamp(2),clamp(-1),clamp(1))

        grid2 = reduce(lambda x,y: x+y,grid,[])
        return list(map(clamp,grid2))

    def grid_path_name(self,gridname):
        return os.path.join(self.basepath)+"."+gridname+".ls"

    '''
    def cgrid2ngrid(self, grid):
        counter=0
        newgrid = []

        val = lambda i,j: rhogrid[i+int(d[0])*j]
        newgrid.extend([0 for in range(self.ny)])
        for m in range(1,self.nx-1):
            for n in self.range(
        for m in self.nx:
            newgrid.append([counter:counter+self.nx-1])
            counter += self.nx-1
        newgrid.extend([0 for in range(self.ny)])
    '''

    def porosity_to_density(self, gridd):
        dims,grid = gridd
        out = []

        val = lambda i,j: grid[i+int(dims[0])*j]
        for m in range(int(dims[0])):
            for n in range(int(dims[1])):
                raw = 1-val(m,n)
                raw = raw * 20
                out.append(raw)
        return out


        



    def write_grid2to3(self, grid, depth, outpath, outside, buf=True, nx=0, ny=0):
        '''
        Write a 2d grid as a 3d grid with a bit of buffering on two of the sides (need to worry about other 2)
        '''
        if nx==0: nx = self.nx
        if ny==0: ny= self.ny

        outfile = open(outpath,"w")
        outfile.write("{0} {1} {2}\n".format(nx, ny, depth))
        if buf:
            for n in grid:
                outfile.write("{0} ".format(outside))
            for m in range(depth-2):
                for n in grid:
                    outfile.write("{0} ".format(n))
            for n in grid:
                outfile.write("{0} ".format(outside))
        else:
            for m in range(depth):
                for n in grid:
                    outfile.write("{0} ".format(n))

        outfile.close()

        

    def write(self):

        '''
        general operator to write a pbrt scene + resources
        '''
        print("Creating a lphi and writing it")
        lphi = self.particle2Lphi(self.nx,self.ny)
        lphi_path = self.grid_path_name("lphi")
        print(lphi_path)
        self.write_grid2to3(lphi,self.nx,lphi_path, self.lphi_buffer, nx=self.nx, ny=self.ny)

        #wetness = list(map(lambda x: x*.3+.7, self.particle2wetness(self.nx,self.ny)))
        wetness = list(map(lambda x: x*.3+.7, self.particle2wetness(self.nx,self.ny)))
        wetness_path = self.grid_path_name("wetness")
        self.write_grid2to3(wetness, self.nx, wetness_path, 0, buf=False)
        print(len(wetness))

        rdensity = self.porosity_to_density(self.grids["rho"])
        rdensity_path = self.grid_path_name("rdensity")
        self.write_grid2to3(rdensity, self.nx, rdensity_path, 0, buf=False)
        print(len(rdensity))

        

        print("Writing pbrt file")
        pbrt_path = self.basepath+".pbrt"
        print(pbrt_path)
        open(pbrt_path,"w").write(pbrtmain.format(self.basename,0.5))
        print("cd "+self.dirname+"; pbrt "+pbrt_path)
        os.system("cd "+self.dirname+"; pbrt "+self.basename+".pbrt")
        '''

        for m in range(5):
            open(pbrt_path+str(m),"w").write(pbrtmain.format(self.basename,3*(m)/5+.7))
            print("cd "+self.dirname+"; pbrt "+self.basename+".pbrt"+str(m))
            os.system("cd "+self.dirname+"; pbrt "+self.basename+".pbrt"+str(m))
            '''




if __name__ == "__main__":
    print(sys.argv)
    infile = sys.argv[1]
    parser = FileParser(infile)
    parser.parse()
    parser.write()
    #parser.print()


