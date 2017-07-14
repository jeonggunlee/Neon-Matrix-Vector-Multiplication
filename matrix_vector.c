#include <iostream>
#include <chrono>
#include <cstdlib>
#include <arm_neon.h>

using namespace std;

int main(int argc, char* argv[])
{
	int iLoopCount;
	int iMatSize;

	float32_t **matA;
	float32_t *vecB, *resultVecC;
	float32_t _tSum;
	
	float32x4_t acc;
	float32x4_t vec;
	float32x4_t mat;
	
	switch(argc) {
		case 2:
			iMatSize = atoi(argv[1]);
		break;
		
		default:
		case 1:
			cout << "Not enough input arguments: " << endl;
		return 0;
	}
	
	if (iMatSize == 0) {
			cout << "c" << endl;
		return 0;
	}
	
	if (iMatSize < 4) {
			cout << "t" << endl;
		return 0;
	}
	if (iMatSize % 4 != 0) {
			cout << "p" << endl;
		return 0;
	}
	
	vecB = new float32_t[iMatSize];
	resultVecC = new float32_t[iMatSize];
	matA = new float32_t*[iMatSize];
	
	chrono::high_resolution_clock::time_point t1, t2;
	chrono::duration<double> diff;
	
	for (int i=0; i<iMatSize; ++i) {
		matA[i] = new float[iMatSize];
	}
		
	for (int i=0; i<iMatSize; ++i) {
		vecB[i] = 0.0f;
		resultVecC[i] = 0.0f;
		
		for(int j=0; j<iMatSize; ++j) {
			matA[i][j] = 0.0f;
		}
	}
	
	iLoopCount = 10;
	cout << iMatSize << "x" << iMatSize << " * 1x" << iMatSize << " on NEON("<<iLoopCount<<" loop)" << endl;
	
	for (int loop=0; loop<iLoopCount; ++loop) {
		t1 = chrono::high_resolution_clock::now();
		
		for (int i=0; i<iMatSize; ++i) {
			acc = vdupq_n_f32(0.0f);
			_tSum = 0.0f;
			
			for (int j=0; j<iMatSize; j+=4) {
				vec = vld1q_f32(&vecB[j]);
				mat = vld1q_f32(&matA[i][j]);
				
				acc = vaddq_f32(acc, vmulq_f32(vec, mat));
			}
			
			_tSum += vgetq_lane_f32(acc, 0);
			_tSum += vgetq_lane_f32(acc, 1);
			_tSum += vgetq_lane_f32(acc, 2);
			_tSum += vgetq_lane_f32(acc, 3);
			
			resultVecC[i] = _tSum;
		}
		
		t2 = chrono::high_resolution_clock::now();
		diff = t2 - t1;
		
		cout << "time: " << diff.count() << " s" << endl;
	}
	
	delete[] vecB;
	vecB = 0;
	delete[] resultVecC;
	resultVecC = 0;
	
	for (int i=0; i<iMatSize; ++i) {
		delete[] matA[i];
		matA[i] = 0;
	}
	delete[] matA;
	matA = 0;
	
	return 0;
}
