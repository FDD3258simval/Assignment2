#include <stdio.h>
#include <stdlib.h> // rand for instance
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <omp.h>    // OpenMP library

#define N 1000000
#define NRUNS 10
#define MAX_THREADS 32

double x[N];

double mysecond(){
  struct timeval tp;
  struct timezone tzp;
  int i;

  i = gettimeofday(&tp,&tzp);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

void initialize_vector()
{
  srand(time(0));
  for (int i=0; i < N; i++){
    // generate random number between 0 and 1
    x[i] = ((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*((double)(rand()) / RAND_MAX)*1000;
  }
}

void maxloc()
{
  double maxval; int maxloc;
  typedef struct { double val; int loc; char pad[128]; } tvals;
  tvals maxinfo[MAX_THREADS];

  for (int i=0; i < MAX_THREADS; i++){
    maxinfo[i].val = 0;
    maxinfo[i].loc = 0;
  }

  #pragma omp parallel shared(maxinfo)
  {
    int id = omp_get_thread_num();
    maxinfo[id].val = -1000;
    #pragma omp for
    for (int i=0; i < N; i++){
      if (x[i] > maxinfo[id].val){
        maxinfo[id].val = x[i];
        maxinfo[id].loc = i;
      }
    }
  }
  maxval = maxinfo[0].val;
  maxloc = maxinfo[0].loc;
  for (int i=1; i < MAX_THREADS; i++) {
    if (maxinfo[i].val > maxval) {
      maxval = maxinfo[i].val;
      maxloc = maxinfo[i].loc;
    }
  }
  printf("\tMax: %f\t@ %d\t",maxval,maxloc);
}

int main(int argc, char* argv[])
{
  double ts[NRUNS];
  double mean = 0;
  double std  = 0;
  double stdp = 0;
  double tot  = 0;

  printf("MAXLOC --- ARRAY\n");
  printf("1. Initializing vector\n");
  initialize_vector();
  printf("2. Find maxloc\n");
  // dry run //
  printf("Dry run: ");
  maxloc();
  printf("\n");
  for (int i=0; i < NRUNS; i++){
    // timing //
    printf("Run %3d: ",i);
    ts[i] = mysecond();
    maxloc();
    ts[i] = mysecond()-ts[i];
    printf("Elapsed time: %f\n",ts[i]);
    tot = tot + ts[i];
  }
  mean = tot/NRUNS;
  for (int i=0; i < NRUNS; i++){
    std = std + pow(ts[i]-mean,2.0);
  }
  std  = sqrt(std/NRUNS);
  stdp = std/mean*100;
  printf("----------------------------------------\n");
  printf("Total time:\t%f s\tmean= %f\t std= %f\t(%5.2f \% )\n",tot, mean,std,stdp);

  return 0;
}
