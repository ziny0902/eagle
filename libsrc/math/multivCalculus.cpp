#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>
#include <Math/multivcalculus.h>
#include <util/lamda_func_ptr.h>

using namespace MathHelper;

template <>
void MathHelper::v_2_u<2>(const double *s, double *t)
{
  double length = std::sqrt(s[0]*s[0] + s[1]*s[1]);
  t[0] = s[0]/length;
  t[1] = s[1]/length;
}
template <>
void MathHelper::v_2_u<3>(const double *s, double *t)
{
  double length = std::sqrt(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
  t[0] = s[0]/length;
  t[1] = s[1]/length;
  t[2] = s[2]/length;
}

static double gsl_func_wraper(double x, void *param)
{
  function_param *p = (function_param *)param;
  double prev_x = p->val[p->idx];
  p->val[p->idx] = x;
  double ret = p->f(p->val);
  p->val[p->idx] = prev_x;
  return ret;
}

static double gsl_parametric_func_wraper(double x, void *param)
{
  parametric_function_param *p = (parametric_function_param *)param;
  double ret = p->f(x, p->idx);
  return ret;
}

namespace MathHelper{
template <typename F>
struct partial_derivative
{
  partial_derivative(F f, size_t i) : f(f), index(i) {}


  double operator()(double* x)
  {
    gsl_function Fun;
    function_param param = {f, index, x};
    Fun.function = gsl_func_wraper;
    Fun.params = (void *)&param; 
    double result;
    double abserr;
    gsl_deriv_central(&Fun, x[index], 1e-8, &result, &abserr);
    return result;
  }


private:
  F f;
  size_t index;
};

template <typename F>
struct derivative_parametric
{
  derivative_parametric(F f, size_t i) : f(f), index(i) {}

  double operator()(double x)
  {
    gsl_function Fun;
    parametric_function_param param = {f, index, x};
    Fun.function = gsl_parametric_func_wraper;
    Fun.params = (void *)&param; 
    double result;
    double abserr;
    gsl_deriv_central(&Fun, x, 1e-8, &result, &abserr);
    return result;
  }

 private:
  F f;
  size_t index;
};

template <typename F>
partial_derivative<F> partial(F f, size_t i)
{
  return partial_derivative<F>(f, i);
}

template <typename F>
derivative_parametric<F> parametric_prime(F f, size_t i)
{
  return derivative_parametric<F>(f, i);
}

}

std::vector<double> MathHelper::gradiant(cal_func f, double *v, const size_t dimention)
{
  std::vector<double> grad_v;
  for( int i = 0; i < dimention; i++) {
    grad_v.push_back(partial(f, i)(v));
  }

  return grad_v;
}

double MathHelper::directional_derivative(cal_func f, double *v, const double *u, const size_t dimention)
{
  double total = 0;
  for( int i = 0; i < dimention; i++){
    total += u[i] * partial(f, i)(v);
  }
  return total;
}

std::shared_ptr<double[]> MathHelper::derivative(
    parametric_func f
    , double v
    , const size_t dimention)
{
  std::shared_ptr<double[]> ret
      (new double[dimention], std::default_delete<double[]>());
  for( int i = 0; i < dimention; i++){
    ret[i] =  parametric_prime(f, i)(v);
  }
  return ret;
}

std::shared_ptr<double[]> MathHelper::second_derivative(
    parametric_func f
    , double v
    , const size_t dimention)
{
  std::shared_ptr<double[]> ret
      (new double[dimention], std::default_delete<double[]>());
  for( int i = 0; i < dimention; i++){
    ret[i] = (f(v+1e-4, i) -2*f(v, i) + f(v-1e-4, i))/(1e-4*1e-4);
  }
  return ret;
}


std::shared_ptr<float[]> MathHelper::get_tanget_v(
    parametric_v_func f
    , double v
    , const size_t dimention)
{
  auto fn = fnptr<double(double, int)>
      ([f](double t, double idx)
       { glm::dvec3 ret = f(t); return (double)ret[idx]; }
        );
  return get_tanget_v(fn, v, dimention);
}

std::shared_ptr<float[]> MathHelper::get_tanget_v(
    parametric_func f
    , double v
    , const size_t dimention)
{
  std::shared_ptr<double[]> r_prime
      = MathHelper::derivative(f, v, dimention);

  std::shared_ptr<double[]> unit
      (new double[dimention], std::default_delete<double[]>());
  v_2_u<3>(&r_prime[0], &unit[0] );
  std::shared_ptr<float[]> funit
      (new float[dimention], std::default_delete<float[]>());
  funit[0] = unit[0];
  funit[1] = unit[1];
  funit[2] = unit[2];
  return funit;
}

std::shared_ptr<float[]> MathHelper::get_normal_v(
    parametric_v_func f
    , double v
    , const size_t dimention)
{
  auto fn = fnptr<double(double, int)>
      ([f ](double t, double idx)
       { glm::dvec3 ret = f(t); return (double)ret[idx]; }
        );
  return get_normal_v(fn, v, dimention);
}

std::shared_ptr<float[]> MathHelper::get_normal_v(
    parametric_func f
    , double v
    , const size_t dimention)
{
  std::shared_ptr<double[]> r_prime_prime
      = MathHelper::second_derivative(f, v, dimention);

  std::shared_ptr<double[]> normal
      (new double[dimention], std::default_delete<double[]>());
  v_2_u<3>(&r_prime_prime[0], &normal[0] );
  std::shared_ptr<float[]> funit
      (new float[dimention], std::default_delete<float[]>());
  funit[0] = normal[0];
  funit[1] = normal[1];
  funit[2] = normal[2];
  return funit;
}

std::shared_ptr<float[]> MathHelper::get_binormal_v(float* T, float* N)
{
  std::shared_ptr<float[]> B
      (new float[3], std::default_delete<float[]>());
  B[0] = (float)(T[1] * N[2] - T[2] * N[1]);
  B[1] = (float)(T[2] * N[0] - T[0] * N[2]);
  B[2] = (float)(T[0] * N[1] - T[1] * N[0]);

  return B;
}

