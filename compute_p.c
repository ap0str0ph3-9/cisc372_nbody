// make a NUMELEMENTS x NUMELEMENTS matrix to hold the pairwise accel fx
// compute the accel mx (accel[i][j] is the fx on acc of j on i)
// sum up cols to get one fx per obj (overall acc on i)
// apply that to the obj to get new vel and update v and pos

#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "config.h"

void compute(){
    int i, j, k;
    //pt 1
    vector3* values=(vector3*)malloc(sizeof(vector3)*NUMENTITIES*NUMENTITIES);
	vector3** accels=(vector3**)malloc(sizeof(vector3*)*NUMENTITIES);
    for (i=0;i<NUMENTITIES;i++){
		accels[i]=&values[i*NUMENTITIES];
    }
    //pt 2
    //modify slide 24.7 for vector subtraction on hpos and dist^2 addition
    //smth like k=0->3, dist[k] = hpos[i][k]=hpos[j][k]
    //theres only three elements so maybe not needed idk
    //pt 3
    //use slide 26.11-12 for reducing into sum
    //def needed, there could be like a million things
    //pt 4
    //use 26.11-12 for this too
    for (k=0;k<3;k++){
			hVel[i][k]+=accel_sum[k]*INTERVAL;
			hPos[i][k]+=hVel[i][k]*INTERVAL;
		}

}