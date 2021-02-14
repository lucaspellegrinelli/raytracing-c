#ifndef COMMON_MATH_H
#define COMMON_MATH_H

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define INF 99999999.0

double vec_normalize(double vec[3]);
double vec_dot(double a[3], double b[3]);

#endif