# ECPlugin

#Compile&link:

make

#ECPlugin will be output to the /bin directory
#


#Install:

make install

#Clean:

make clean

#ECPlugin will be output to the bin directory and copied to the /usr/bin directory(this is a system path directory)
#

#Using:

ECPlugin [hpcrun-paras] [app] [app-paras] --struct [hpcstruct-file] --prof [hpcprof-paras] --mpi [mpi-paras]

#Database will be output to current directory

  #Note:

  --struct [hpcstruct-file] can be ignored. (ECPlugin [hpcrun-paras] [app] [app-paras] --prof [hpcprof-paras] --mpi [mpi-paras])

  --mpi [mpi-paras] can be ignored.  (ECPlugin [hpcrun-paras] [app] [app-paras] --prof [hpcprof-paras])

  Do not use any parameters that change the measurements / structure / database output directory


#model.hpcmodel:

  file example:
  event: $1 : CPUTIME@5000
  event: $2 : cache-misses
  model: $1 + $2 = MyModel

  #Note:
  'model: $1 + $2 = MyModel' can be ignored. (This can be look as an event-listed file)