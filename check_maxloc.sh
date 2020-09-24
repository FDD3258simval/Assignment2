#!/bin/bash

module swap PrgEnv-cray PrgEnv-gnu

echo "Running check for maxloc_serial"
gcc maxloc.c -o maxloc.out -lm;
srun -n 1 ./maxloc.out > serial.log
tail -1 serial.log | awk '{ print $6 "\t" $8 }' >> serial.txt

for prog in maxloc_naive maxloc_critical maxloc_array maxloc_fsharing; do
  echo "Running check for $prog:"
  gcc -fopenmp $prog.c -o $prog.out -lm;
  rm -rf $prog.txt;
  for i in 1 2 4 8 16 20 24 28 32; do
    export OMP_NUM_THREADS=$i;
    echo -e "\tNUM THREADS: $i";
    n=$(printf "%02i" $i);
    log="perf_$prog$n.log";
    srun -n 1 ./$prog.out > $log;
    tail -1 $log | awk '{ print $6 "\t" $8 }' >> $prog.txt
  done
done
