#include "common_math.h"

#include <math.h>

double vec_normalize(double vec[3]) {
  double norm = 0;
  for (int i = 0; i < 3; i++) {
    double v = vec[i] > 0 ? vec[i] : -vec[i];
    norm += v * v;
  }

  norm = sqrt(norm);
  for (int i = 0; i < 3; i++) {
    vec[i] /= norm;
  }

  return norm;
}

double vec_dot(double a[3], double b[3]) {
  double dot = 0;
  for (int i = 0; i < 3; i++) {
    dot += a[i] * b[i];
  }

  return dot;
}