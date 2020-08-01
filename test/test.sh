echo "compile&link:testFunc.cpp"
g++ -o testFunc ./testFunc.cpp
echo "test 1"
../bin/ECPlugin -t ./testFunc --prof ./
echo "test 2"
echo "compile&link:test_mpi.c"
mpicc -o test_mpi ./test_mpi.c
../bin/ECPlugin -t ./test_mpi --prof ./ --mpi -n 2