#include "raytracing.h"
#include "common_math.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
  double pos_diff[3];
  for(int i = 0; i < 3; i++){
    pos_diff[i] = camera_pos[i] - sphere_pos[i];
  }

  double b = 2 * vec_dot(camera_dir, pos_diff);
  double c = vec_dot(pos_diff, pos_diff) - radius * radius;

  double delta = b * b - 4 * c;

  if(delta > 0){
    double sqrt_delta = sqrt(delta);
    double x0 = (-b + sqrt_delta) / 2;
    double x1 = (-b - sqrt_delta) / 2;

    if(x0 > 0 && x1 > 0){
      return min(x0, x1);
    }
  }

  return INF;
}

void generate_image(int ***img_pixels, int screen_w, int screen_h, scenario_t scenario){
  double screen_ratio = (double) screen_w / screen_h;
  double screen_bounds[4] = {-1.0, -1.0 / screen_ratio + 0.25, 1.0, 1.0 / screen_ratio + 0.25};

  double x_step = (screen_bounds[2] - screen_bounds[0]) / screen_w;
  double y_step = (screen_bounds[3] - screen_bounds[1]) / screen_h;

  int n_spheres = sizeof(scenario.spheres) / sizeof(sphere_t);

  for(int px = 0; px < screen_w; px++){
    for(int py = 0; py < screen_h; py++){
      double x = screen_bounds[0] + px * x_step;
      double y = screen_bounds[1] + py * y_step;

      double px_color[3] = {0.0, 0.0, 0.0};

      scenario.camera_dir[0] = x;
      scenario.camera_dir[1] = y;

      double ray_origin[3], ray_dir[3];
      for(int i = 0; i < 3; i++){
        ray_dir[i] = scenario.camera_dir[i] - scenario.camera_pos[i];
        ray_origin[i] = scenario.camera_pos[i];
      }

      vec_normalize(ray_dir);

      double reflection = 1.0;
      for(int depth = 0; depth < MAX_DEPTH; depth++){
        bool traced = false;

        // Trace ray
        int closest_sphere_i = 0;
        double t_sphere = INF;

        for(int i = 0; i < n_spheres; i++){
          double t = intersect_sphere(ray_origin, ray_dir, scenario.spheres[i].pos, scenario.spheres[i].radius);
          if(t < t_sphere){
            t_sphere = t;
            closest_sphere_i = i;
          }
        }
        
        sphere_t close_sphere = scenario.spheres[closest_sphere_i];
        double t_plane = intersect_plane(ray_origin, ray_dir, scenario.plane.pos, scenario.plane.normal);

        double tmin = min(t_sphere, t_plane);
        int tObj = t_sphere < t_plane ? SPHERE : PLANE;

        double intersec_pt[3], obj_normal[3], ray_color[3];
        if(tmin < INF){
          for(int i = 0; i < 3; i++){
            intersec_pt[i] = ray_origin[i] + ray_dir[i] * tmin;
          }

          double obj_color[3];
          if(tObj == PLANE){
            int use_dark_color = (abs(intersec_pt[0] * 2.0 + 10) % 2) == (abs(intersec_pt[2] * 2.0 + 10) % 2);
            for(int i = 0; i < 3; i++){
              obj_normal[i] = scenario.plane.normal[i];
              obj_color[i] = use_dark_color ? scenario.plane.dark_color[i] : scenario.plane.light_color[i];
            }
          }else{
            for(int i = 0; i < 3; i++){
              obj_normal[i] = intersec_pt[i] - close_sphere.pos[i];
              obj_color[i] = close_sphere.color[i];
            }

            vec_normalize(obj_normal);
          }

          double dir_to_light[3], dir_to_camera[3], bounce_pt[3];
          for(int i = 0; i < 3; i++){
            dir_to_light[i] = scenario.light_pos[i] - intersec_pt[i];
            dir_to_camera[i] = scenario.light_pos[i] - intersec_pt[i];
            bounce_pt[i] = intersec_pt[i] + obj_normal[i] * 1e-4;
          }

          vec_normalize(dir_to_light);
          vec_normalize(dir_to_camera);

          bool is_shadow;

          if(tObj == PLANE){
            is_shadow = intersect_sphere(bounce_pt, dir_to_light, close_sphere.pos, close_sphere.radius) < INF;
          }else{
            is_shadow = intersect_plane(bounce_pt, dir_to_light, scenario.plane.pos, scenario.plane.normal) < INF;
          }

          if(!is_shadow){
            double obj_diffuse = tObj == PLANE ? scenario.plane.diffuse : close_sphere.diffuse;
            double obj_specular = tObj == PLANE ? scenario.plane.specular : close_sphere.specular;

            double H[3];
            for(int i = 0; i < 3; i++){
              H[i] = dir_to_light[i] + dir_to_camera[i];
            }

            vec_normalize(H);

            double diffuse_intensity = max(vec_dot(obj_normal, dir_to_light), 0);
            double specular_intensity = pow(max(vec_dot(obj_normal, H), 0), scenario.specular_exp);
            for(int i = 0; i < 3; i++){
              ray_color[i] = scenario.ambient_light;
              ray_color[i] += obj_diffuse * diffuse_intensity * obj_color[i];
              ray_color[i] += obj_specular * specular_intensity * scenario.light_color[i];
            }

            traced = true;
          }
        }
        
        if(!traced) break;
        
        double dir_dot_normal = vec_dot(ray_dir, obj_normal);
        for(int i = 0; i < 3; i++){
          ray_origin[i] = intersec_pt[i] + obj_normal[i] * 1e-4;
          ray_dir[i] = ray_dir[i] - 2 * dir_dot_normal * obj_normal[i];
          px_color[i] += reflection * ray_color[i];
        }

        vec_normalize(ray_dir);
        reflection *= tObj == PLANE ? scenario.plane.reflection : close_sphere.reflection;
      }

      for(int i = 0; i < 3; i++){
        img_pixels[screen_h - py - 1][px][i] = min(max(0, px_color[i]), 1) * 255;
      }
    }
  }
}