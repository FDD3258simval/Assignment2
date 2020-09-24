#!/bin/bash

module swap PrgEnv-cray PrgEnv-gnu

echo "Running check for DFT serial"
gcc -fopenmp DFTW_1.c -o DFTW.out -lm;
log="DFTWserial.log";
rm -rf $log
for j in $(seq 1 1 10); do
  echo "RUN $j"
  echo "RUN $j" >> $log
  srun -n 1 ./DFTW.out >> $log
  cat tmp >> $log
done
grep "computation" $log | awk '{ print $4 }' >> $prog.txt

for prog in DFTW_redfor DFTW_redfor_norm DFTW_redforcoll DFTW_redforcoll_norm; do
  echo "Running check for $prog:"
  gcc -fopenmp $prog.c -o $prog.out -lm;
  rm -rf $prog.txt;
  for i in 1 2 4 8 16 20 24 28 32; do
    export OMP_NUM_THREADS=$i;
    echo -e "\tNUM THREADS: $i";
    n=$(printf "%02i" $i);
    log="perf_$prog$n.log";
    rm -rf $log
    for j in $(seq 1 1 10); do
      echo "RUN $j"
      echo "RUN $j" >> $log
      srun -n 1 ./$prog.out >> $log
      cat tmp >> $log
    done
    grep "computation" $log | awk '{ print $4 }' >> $prog.txt
    echo " " >> $prog.txt
  done
done
