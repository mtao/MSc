import os



#Paths that will be used
install_path= os.environ['LOCAL_INSTALL']
include_path=os.path.join(install_path,'include')
bin_path    =os.path.join(install_path,'bin')
lib_path    =os.path.join(install_path,'lib')

#Code will require qt4 tool for moc'ing and eigen & my includes in the path
VariantDir('build','src')
__variant_dir_enabled__ = True
env = Environment(tools=['default','qt4'], CPPPATH=[os.environ['EIGEN_PATH'], include_path], LIBPATH=lib_path, CXX="clang++")
env['ENV']['TERM'] = os.environ['TERM']
env.EnableQt4Modules(['QtCore','QtOpenGL', 'QtGui'])
env['ROOT_DIR']=Dir('.')
env.Append(CCFLAGS='-O3')
env.Append(CCFLAGS='-std=c++11')
env.Append(CCFLAGS='-g')

import multiprocessing
env.SetOption('num_jobs',multiprocessing.cpu_count()+1)
#set installs
env.Alias('install',bin_path      )
env.Alias('install',lib_path      )

#set install directories
env['INSTALL_DIR'] = Dir(install_path)
env['INCLUDE_DIR'] = Dir(include_path)
env['BIN_DIR'] = Dir(bin_path)
env['LIB_DIR'] = Dir(lib_path)

def IncludeInstall(self, headers=[], install=""):
    if __variant_dir_enabled__:
        local_path=os.path.relpath(Dir('.').get_abspath(), \
                   os.path.join(self['ROOT_DIR'].get_abspath(),'build'))
    else:
        local_path=os.path.relpath(Dir('.').get_abspath(), \
                   os.path.join(self['ROOT_DIR'].get_abspath(),'src'))
    include_install_path=os.path.join(self['INCLUDE_DIR'].get_abspath(),local_path)
    self.Install(include_install_path, headers)
    self.Alias('install', include_install_path)
    self.Alias('headers', include_install_path)
    if install is not "":
        self.Alias(install, include_install_path)
    

env.AddMethod(IncludeInstall)

def ViewerInstall(self,name,core,headers,LIBS=[]):
    viewer_lib=self.SharedLibrary(name,core,LIBS=LIBS)
    self.IncludeInstall(headers)
    self.Install(self['LIB_DIR'],viewer_lib)
    viewer_bin=self.Program(name,['main.cpp'],LIBS=LIBS+[name])
    self.Install(self['BIN_DIR'],viewer_bin)
    return viewer_bin

    
env.AddMethod(ViewerInstall)

env.Export('env')
if __variant_dir_enabled__:
    SConscript('src/SConscript', variant_dir='build')
else:
    SConscript('src/SConscript')
