#include "PPintrin.h"
#include <iostream>
// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  __pp_vec_int exp, count;
  __pp_vec_float x, result;
  __pp_vec_int int_zero = _pp_vset_int(0), int_one = _pp_vset_int(1);
  __pp_vec_float float_one = _pp_vset_float(1.f), upper_bound = _pp_vset_float(9.999999f);
  __pp_mask maskAll, maskNotEqual, maskInLoop, maskIsEqual;

  //All ones
  maskAll = _pp_init_ones();
  //All zeros
  maskIsEqual = _pp_init_ones(0);
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_int(exp, exponents + i, maskAll); // exp = exponents[i];
    _pp_veq_int(maskIsEqual, exp, int_zero, maskAll); // if (exp == 0);
    _pp_vmove_float(result, float_one, maskIsEqual); //   result = 1.0;

    maskNotEqual = _pp_mask_not(maskIsEqual); // if (exp != 0);
    _pp_vload_float(x, values + i, maskAll); // x = values[i];
    _pp_vmove_float(result, x, maskNotEqual); // result = x
    _pp_vsub_int(count, exp, int_one, maskNotEqual); //count = exp -1
    
    _pp_vgt_int(maskInLoop, count, int_zero, maskNotEqual); // if (count > 0)
    // std::cout<<_pp_cntbits(maskInLoop)<<std::endl;
    while(_pp_cntbits(maskInLoop) > 0){
      _pp_vmult_float(result, result, x, maskInLoop);
      _pp_vsub_int(count, count, int_one, maskAll);
      _pp_vgt_int(maskInLoop, count, int_zero, maskAll);
    }
    _pp_vgt_float(maskNotEqual, result, upper_bound, maskAll);
    _pp_vmove_float(result, upper_bound, maskNotEqual);
    _pp_vstore_float(output + i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  __pp_mask maskAll, maskIsTrue;
  __pp_vec_float x, result = _pp_vset_float(0.f);
  
  float sum[1];

  maskAll = _pp_init_ones();
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    float sum = 0.0;
    _pp_vload_float(x, values + i, maskAll); // x = values[i];
    _pp_vadd_float(result, result, x, maskAll);
  }
  
  int size = VECTOR_WIDTH;
  while(size/2){
    _pp_hadd_float(result, result);
    _pp_interleave_float(result, result);
    size /= 2;
  }

  _pp_vstore_float(sum, result, maskAll);
  

  return sum[0];
}