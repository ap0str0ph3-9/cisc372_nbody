#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "config.h"

__device__ vector3 warpRed(vector3 val){
  for (int offset = 16; offset > 0; offset >>=1 ) {
    val.x += __shfl_down_sync(0xffffffff, val.x, offset);
    val.y += __shfl_down_sync(0xffffffff, val.y, offset);
    val.z += __shfl_down_sync(0xffffffff, val.z, offset);
  }
  return val;
}

__device__ vector3 blockRed(vector3 val){
  vector3 x;
  __shared__ vector3 shared[32];
  int lane = threadIdx.x % 32;
  int warpId = threadIdx.x / 32;
  val = warpRed(val);
  if (lane == 0) shared[warpId] = val;
  __syncthreads();
  val = (threadIdx.x < blockDim.x / 32) ? shared[lane] : FILL_VECTOR(x, 0, 0, 0);
  if (warpId == 0) val = warpRed(val);
  return val;
}

void compute(){
    int i, j, k;
    //pt 1
    vector3* values=(vector3*)malloc(sizeof(vector3)*NUMENTITIES*NUMENTITIES);
	  vector3** accels=(vector3**)malloc(sizeof(vector3*)*NUMENTITIES);
    for (i=0;i<NUMENTITIES;i++){
		accels[i]=&values[i*NUMENTITIES];
    }
    //pt 2
    for (i=0;i<NUMENTITIES;i++){
		for (j=0;j<NUMENTITIES;j++){
			if (i==j) {
				FILL_VECTOR(accels[i][j],0,0,0);
			}
			else{
				vector3 distance;
				for (k=0;k<3;k++) distance[k]=hPos[i][k]-hPos[j][k];
				double magnitude_sq=distance[0]*distance[0]+distance[1]*distance[1]+distance[2]*distance[2];
				double magnitude=sqrt(magnitude_sq);
				double accelmag=-1*GRAV_CONSTANT*mass[j]/magnitude_sq;
				FILL_VECTOR(accels[i][j],accelmag*distance[0]/magnitude,accelmag*distance[1]/magnitude,accelmag*distance[2]/magnitude);
			}
		}
	}
    //pt 3
    for (i=0;i<NUMENTITIES;i++){
      vector3 accel_sum={0,0,0};
      for (j=0;j<NUMENTITIES;j++){
        accel_sum = blockRed<<<BLOCKS, THREADS>>>(accels[i][j]);
      }
      //pt 4
      for (k=0;k<3;k++){
			  hVel[i][k]+=accel_sum[k]*INTERVAL;
			  hPos[i][k]+=hVel[i][k]*INTERVAL;
		  }
  }
  free(values);
  free(accels);
}