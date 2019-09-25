#include <vector>
#include <iostream>
#include <cmath>
#include <math.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_deriv.h>
#include <Math/multivcalculus.h>

using namespace MathHelper;

static double gsl_func_wraper(double x, void *param)
{
  function_param *p = (function_param *)param;
  double prev_x = p->val[p->idx];
  p->val[p->idx] = x;
  double ret = p->f(p->val);
  p->val[p->idx] = prev_x;
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
partial_derivative<F> partial(F f, size_t i)
{
  return partial_derivative<F>(f, i);
}
}

std::vector<double> gradiant(cal_func f, double *v, const size_t dimention)
{
  std::vector<double> grad_v;
  for( int i = 0; i < dimention; i++) {
    grad_v.push_back(partial(f, i)(v));
  }

  return grad_v;
}

double directional_derivative(cal_func f, double *v, const double *u, const size_t dimention)
{
  double total = 0;
  for( int i = 0; i < dimention; i++){
    total += u[i] * partial(f, i)(v);
  }
  return total;
}

