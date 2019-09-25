#pragma once
#include <cmath>

namespace MathHelper{

typedef double (*cal_func)(double *);

struct function_param{
  cal_func f;
  size_t idx;
  double *val;
};

template <size_t d>
void v_2_u(const double *s, double *t){}

template <>
void v_2_u<2>(const double *s, double *t)
{
  double length = std::sqrt(s[0]*s[0] + s[1]*s[1]);
  t[0] = s[0]/length;
  t[1] = s[1]/length;
}
template <>
void v_2_u<3>(const double *s, double *t)
{
  double length = std::sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
  t[0] = s[0]/length;
  t[1] = s[1]/length;
  t[2] = s[2]/length;
}
}

extern "C" std::vector<double> gradiant(MathHelper::cal_func f, double *v, const size_t dimention);

extern "C" double directional_derivative(MathHelper::cal_func f, double *v, const double *u, const size_t dimention);
