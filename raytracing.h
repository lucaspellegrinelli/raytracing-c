#ifndef RAYTRACING_H
#define RAYTRACING_H

#define MAX_DEPTH 10

#define PLANE 0
#define SPHERE 1

typedef struct {
  double pos[3];
  double normal[3];
  double dark_color[3];
  double light_color[3];
  double diffuse;
  double specular;
  double reflection;
} plane_t;

typedef struct {
  double pos[3];
  double color[3];
  double radius;
  double diffuse;
  double specular;
  double reflection;
} sphere_t;

typedef struct {
  double ambient_light;
  double specular_exp;

  double camera_pos[3];
  double camera_dir[3];

  double light_pos[3];
  double light_color[3];

  plane_t plane;
  sphere_t spheres[3];
} scenario_t;

double intersect_plane(double camera_pos[3], double camera_dir[3],
                       double plane_pos[3], double plane_normal[3]);
double intersect_sphere(double camera_pos[3], double camera_dir[3],
                        double sphere_pos[3], double radius);
void render_scene(int ***img_pixels, int screen_w, int screen_h,
                  scenario_t scenario);

#endif