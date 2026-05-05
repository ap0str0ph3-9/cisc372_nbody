#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "config.h"

__global__ void rowRed(vector3* val, vector3* stor){
  __shared__ vector3 ps[THREADS];
  int t = threadIdx.x;
  int i = blockIdx.x*blockDim.x+t;
  ps[t] = (i<NUMENTITIES) ? val[i] : {0, 0, 0};
  __syncthreads();
  for (int s=blockDim.x/2; s>0; s>>=1){
	if (t<s){
		ps[t][0] += ps[t+s][0];
		ps[t][1] += ps[t+s][1];
		ps[t][2] += ps[t+s][2];
	}
	__syncthreads();
  }
  if(t==0){
	stor={ps[t][0], ps[t][1], ps[t][2]};
  }
}

//compute: Updates the positions and locations of the objects in the system based on gravity.
//Parameters: None
//Returns: None
//Side Effect: Modifies the hPos and hVel arrays with the new positions and accelerations after 1 INTERVAL
void compute(){
	//make an acceleration matrix which is NUMENTITIES squared in size;
	int i,j,k;
	vector3* values=(vector3*)malloc(sizeof(vector3)*NUMENTITIES*NUMENTITIES);
	vector3** accels=(vector3**)malloc(sizeof(vector3*)*NUMENTITIES);
	for (i=0;i<NUMENTITIES;i++)
		accels[i]=&values[i*NUMENTITIES];
	//first compute the pairwise accelerations.  Effect is on the first argument.
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
	//sum up the rows of our matrix to get effect on each entity, then update velocity and position.
	for (i=0;i<NUMENTITIES;i++){
		vector3 *accel_sum={0,0,0};
		rowRed<<<(NUMENTITIES+THREADS-1)/THREADS, THREADS>>>(accels[i], accel_sum);
		//compute the new velocity based on the acceleration and time interval
		//compute the new position based on the velocity and time interval
		for (k=0;k<3;k++){
			hVel[i][k]+=accel_sum[k]*INTERVAL;
			hPos[i][k]+=hVel[i][k]*INTERVAL;
		}
	}
	free(accels);
	free(values);
}