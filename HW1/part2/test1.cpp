#include <iostream>
#include "test.h"
#include "fasttime.h"
#include <algorithm>

#define cnt 11 

void test1(float* __restrict  a, float* __restrict  b, float* __restrict  c, int N) {
  __builtin_assume(N == 1024);

  a = (float *)__builtin_assume_aligned(a, 16); // 32 in AVX2
  b = (float *)__builtin_assume_aligned(b, 16); // 32 in AVX2
  c = (float *)__builtin_assume_aligned(c, 16); // 32 in AVX2
  
  double elapsedf = 0.0f;
  double buff[cnt] = {0};

  for (int n=0; n<cnt; n++){
    fasttime_t time1 = gettime();
    for (int i=0; i<I; i++) {
      for (int j=0; j<N; j++) {
        c[j] = a[j] + b[j];
      }
    }
    fasttime_t time2 = gettime();
    buff[n] = tdiff(time1, time2);
  }
  std::sort(buff, buff+cnt);
  elapsedf = buff[cnt/2];
  std::cout << "The median elapsed execution "<<cnt<<" time of the test1():\n" 
    << elapsedf << "sec (N: " << N << ", I: " << I << ")\n";
}