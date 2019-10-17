#pragma once
#include <vector>
#include <cmath>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

namespace MathHelper{

typedef double (*cal_func)(double *);
typedef double (*cal_v_func)(double *, int idx);
typedef double (*parametric_func)(double, int idx);
typedef glm::dvec3 (*parametric_v_func)(double);

struct v_function_param{
  cal_v_func f;
  size_t idx;
  double *val;
};

struct function_param{
  cal_func f;
  size_t idx;
  double *val;
};

struct parametric_function_param{
  parametric_func f;
  size_t idx;
  double val;
};

template <size_t d>
void v_2_u(const double *s, double *t){}
template <>
void v_2_u<2>(const double *s, double *t);
template <>
void v_2_u<3>(const double *s, double *t);

double partial(cal_func f, double *v, int idx);
template<size_t d>
double partial(cal_v_func f, double *v, int idx);
template<>
double partial<0>(cal_v_func f, double *v, int idx);
template<>
double partial<1>(cal_v_func f, double *v, int idx);
template<>
double partial<2>(cal_v_func f, double *v, int idx);

std::vector<double> gradiant(MathHelper::cal_func f, double *v, const size_t dimention);

double directional_derivative(MathHelper::cal_func f, double *v, const double *u, const size_t dimention);

std::shared_ptr<double[]> derivative(
    MathHelper::parametric_func f
    , double v
    , const size_t dimention);

std::shared_ptr<double[]> second_derivative(
    MathHelper::parametric_func f
    , double v
    , const size_t dimention);

std::shared_ptr<float[]> get_tanget_v(
    parametric_v_func f
    , double v
    , const size_t dimention);

std::shared_ptr<float[]> get_tanget_v(
    parametric_func f
    , double v
    , const size_t dimention);

std::shared_ptr<float[]> get_normal_v(
    parametric_v_func f
    , double v
    , const size_t dimention);

std::shared_ptr<float[]> get_normal_v(
    parametric_func f
    , double v
    , const size_t dimention);

std::shared_ptr<float[]> get_binormal_v(float* T, float* N);
}

