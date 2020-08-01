# ECPlugin

#Compile&link:

make

#ECPlugin will be output to the bin directory
#


#Install:

make install

#ECPlugin will be output to the bin directory and copied to the /usr/bin directory(this is a system path directory)
#

#Using:

ECPlugin [hpcrun-paras] [app] [app-paras] --prof [hpcprof-paras] --mpi [mpi-paras]

  #Note:

  --mpi [mpi-paras] can be ignored.  (ECPlugin [hpcrun-paras] [app] [app-paras] --prof [hpcprof-paras])

  Do not use any parameters that change the measurements / structure / database output directory
