#include "raytracing.h"
#include "common_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double intersect_plane(double camera_pos[3], double camera_dir[3], double plane_pos[3], double plane_normal[3]){
  double denom = vec_dot(camera_dir, plane_normal);

  double abs_denom = denom > 0 ? denom : -denom;
  if(abs_denom < 1e-6){
    return INF;
  }

  double pos_diff[3];
  for(int i = 0; i < 3; i++){
    pos_diff[i] = plane_pos[i] - camera_pos[i];
  }

  double d = vec_dot(pos_diff, plane_normal) / denom;
  if(d < 0){
    return INF;
  }

  return d;
}

double intersect_sphere(double camera_pos[3], double camera_dir[3], double sphere_pos[3], double radius){
  double a = vec_dot(camera_dir, camera_dir);
  double pos_diff[3];
  for(int i = 0; i < 3; i++){
    pos_diff[i] = camera_pos[i] - sphere_pos[i];
  }

  double b = 2 * vec_dot(camera_dir, pos_diff);
  double c = vec_dot(pos_diff, pos_diff) - radius * radius;
  double delta = b * b - 4 * a * c;

  if(delta > 0){
    double sqrt_delta = sqrt(delta);
    double q = b < 0 ? (-b - sqrt_delta) / 2.0 : (-b + sqrt_delta) / 2.0;
    double t0 = q / a;
    double t1 = c / q;
    double tmin = min(t0, t1);
    double tmax = max(t0, t1);

    if(tmax >= 0){
      return tmin < 0 ? tmax : tmin;
    }
  }

  return INF;
}

void generate_image(int ***img_pixels, int screen_w, int screen_h){
  double ambient_light = 0.05;
  double diffuse_c = 1.0;
  double specular_c = 0.5;
  double specular_k = 100;

  double camera_pos[3] = {0.75, 0.3, 1.0};
  double camera_dir[3] = {0.0, 0.0, 0.0};

  double light_pos[3] = {-1.0, 3.0, 1.0};
  double light_color[3] = {1.0, 1.0, 1.0};

  double screen_ratio = (double) screen_w / screen_h;
  double screen_bounds[4] = {-1.0, -1.0 / screen_ratio + 0.25, 1.0, 1.0 / screen_ratio + 0.25};

  double x_step = (screen_bounds[2] - screen_bounds[0]) / screen_w;
  double y_step = (screen_bounds[3] - screen_bounds[1]) / screen_h;

  double plane_pos[3] = {0.0, 0.0, 0.0};
  double plane_normal[3] = {0.0, 1.0, 0.0};
  double plane_dark_color[3] = {0.9, 0.30, 0.23};
  double plane_light_color[3] = {0.18, 0.80, 0.44};
  double plane_diffuse = 0.5;
  double plane_specular = 0.25;
  double plane_reflection = 0.25;

  double sphere_pos[3] = {-0.75, 0.6, -3.0};
  double sphere_color[3] = {0.20, 0.59, 0.86};
  double sphere_radius = 0.6;
  double sphere_reflection = 0.25;

  for(int px = 0; px < screen_w; px++){
    for(int py = 0; py < screen_h; py++){
      double x = screen_bounds[0] + px * x_step;
      double y = screen_bounds[1] + py * y_step;

      double px_color[3] = {0.0, 0.0, 0.0};

      camera_dir[0] = x;
      camera_dir[1] = y;

      double D[3];
      for(int i = 0; i < 3; i++){
        D[i] = camera_dir[i] - camera_pos[i];
      }

      vec_normalize(D);

      double rayO[3], rayD[3];
      for(int i = 0; i < 3; i++){
        rayO[i] = camera_pos[i];
        rayD[i] = D[i];
      }

      double reflection = 1.0;
      int curr_depth = 0;
      while(curr_depth < MAX_DEPTH){
        int traced = 0;

        // Trace ray
        double t_sphere = intersect_sphere(rayO, rayD, sphere_pos, sphere_radius);
        double t_plane = intersect_plane(rayO, rayD, plane_pos, plane_normal);

        double tmin = min(t_sphere, t_plane);
        int tObj = t_sphere < t_plane ? SPHERE : PLANE;

        double intersec_pt[3], obj_normal[3], ray_color[3];
        if(tmin < INF){
          for(int i = 0; i < 3; i++){
            intersec_pt[i] = rayO[i] + rayD[i] * tmin;
          }

          double color[3];
          if(tObj == PLANE){
            int use_dark_color = (abs(intersec_pt[0] + 10) % 2) == (abs(intersec_pt[2] + 10) % 2);
            for(int i = 0; i < 3; i++){
              obj_normal[i] = plane_normal[i];
              color[i] = use_dark_color ? plane_dark_color[i] : plane_light_color[i];
            }
          }else{
            for(int i = 0; i < 3; i++){
              obj_normal[i] = intersec_pt[i] - sphere_pos[i];
              color[i] = sphere_color[i];
            }

            vec_normalize(obj_normal);
          }

          double dirToLight[3], dirToCamera[3];
          for(int i = 0; i < 3; i++){
            dirToLight[i] = light_pos[i] - intersec_pt[i];
            dirToCamera[i] = light_pos[i] - intersec_pt[i];
          }

          vec_normalize(dirToLight);
          vec_normalize(dirToCamera);

          int is_shadow;
          double fromCast[3];
          for(int i = 0; i < 3; i++){
            fromCast[i] = intersec_pt[i] + obj_normal[i] * 1e-4;
          }

          if(tObj == PLANE){
            is_shadow = intersect_sphere(fromCast, dirToLight, sphere_pos, sphere_radius) < INF;
          }else{
            is_shadow = intersect_plane(fromCast, dirToLight, plane_pos, plane_normal) < INF;
          }

          if(!is_shadow){
            double obj_diffuse = tObj == PLANE ? plane_diffuse : diffuse_c;
            double obj_specular = tObj == PLANE ? plane_specular : specular_c;

            double bothDirs[3];
            for(int i = 0; i < 3; i++){
              bothDirs[i] = dirToLight[i] + dirToCamera[i];
            }
            vec_normalize(bothDirs);

            double normal_vec_dot_dirLight = max(vec_dot(obj_normal, dirToLight), 0);
            double normal_vec_dot_bothDirs = pow(max(vec_dot(obj_normal, bothDirs), 0), specular_k);
            for(int i = 0; i < 3; i++){
              ray_color[i] = ambient_light;
              ray_color[i] += obj_diffuse * normal_vec_dot_dirLight * color[i];
              ray_color[i] += obj_specular * normal_vec_dot_bothDirs * light_color[i];
            }

            traced = 1;
          }
        }
        // End Trace ray
        
        if(!traced) break;
        
        double rayD_vec_dot_N = vec_dot(rayD, obj_normal);
        for(int i = 0; i < 3; i++){
          rayO[i] = intersec_pt[i] + obj_normal[i] * 1e-4;
          rayD[i] = rayD[i] - 2 * rayD_vec_dot_N * obj_normal[i];
          px_color[i] += reflection * ray_color[i];
        }

        vec_normalize(rayD);
        reflection *= tObj == PLANE ? plane_reflection : sphere_reflection;
        curr_depth++;
      }

      for(int i = 0; i < 3; i++){
        img_pixels[screen_h - py - 1][px][i] = min(max(0, px_color[i]), 1) * 255;
      }
    }
  }
}