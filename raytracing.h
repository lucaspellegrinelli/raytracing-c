#ifndef RAYTRACING_H
#define RAYTRACING_H

#define MAX_DEPTH 5

#define PLANE 0
#define SPHERE 1

double intersect_plane(double camera_pos[3], double camera_dir[3], double plane_pos[3], double plane_normal[3]);
double intersect_sphere(double camera_pos[3], double camera_dir[3], double sphere_pos[3], double radius);
void generate_image(int ***img_pixels, int screen_w, int screen_h);

#endif