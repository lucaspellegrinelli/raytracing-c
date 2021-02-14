#include "raytracing.h"

#include <stdlib.h>
#include <stdio.h>

void write_to_file(int ***img, int img_w, int img_h){
  FILE* img_file;
  img_file = fopen("image.ppm", "wb");
  
  fprintf(img_file, "P3\n");
  fprintf(img_file, "%d %d\n", img_w, img_h);
  fprintf(img_file, "255\n");

  for(int i = 0; i < img_h; i++){
    for(int j = 0; j < img_w; j++){
      fprintf(img_file, "%d %d %d ", img[i][j][0], img[i][j][1], img[i][j][2]);
    }
    fprintf(img_file, "\n");
  }

  fclose(img_file);
}

int main(){
  int img_w = 1920;
  int img_h = 1080;

  int *** img = (int ***) malloc(img_h * sizeof(int **));
  for(int i = 0; i < img_h; i++){
    img[i] = (int **) malloc(img_w * sizeof(int *));
    for(int j = 0; j < img_w; j++){
      img[i][j] = (int *) malloc(3 * sizeof(int));
    }
  }

  plane_t plane = {
    {0.0, 0.0, 0.0}, // Position
    {0.0, 1.0, 0.0}, // Normal
    {1.0, 1.0, 1.0}, // Color 1 (RGB)
    {0.0, 0.0, 0.0}, // Color 2 (RGB)
    0.5, // Diffuse
    0.25, // Specular
    0.35 // Reflection
  };

  sphere_t sphere_0 = {
    {-0.5, 0.6, -2.0}, // Position
    {0.20, 0.59, 0.86}, // Color (RGB)
    0.6, // Radius
    1.0, // Diffuse
    0.5, // Specular
    0.15 // Reflection
  };

  sphere_t sphere_1 = {
    {0.2, 0.2, -2.0}, // Position
    {0.9, 0.30, 0.23}, // Color (RGB)
    0.2, // Radius
    1.0, // Diffuse
    0.5, // Specular
    0.15 // Reflection
  };

  sphere_t sphere_2 = {
    {-0.65, 0.3, -1.0}, // Position
    {0.18, 0.80, 0.44}, // Color (RGB)
    0.3, // Radius
    1.0, // Diffuse
    0.5, // Specular
    0.15 // Reflection
  };

  scenario_t scenario = {
    0.05, // Ambient Light
    100, // Specular light power
    {0.75, 0.3, 1.0}, // Camera Position
    {0.0, 0.0, 0.0}, // Camera Direction
    {-1.0, 3.0, 1.0}, // Light position
    {1.0, 1.0, 1.0}, // Light color
    plane, // Plane info
    {sphere_0, sphere_1, sphere_2} // Spheres info
  };

  generate_image(img, img_w, img_h, scenario);
  // write_to_file(img, img_w, img_h);

  for(int i = 0; i < img_h; i++){
    for(int j = 0; j < img_w; j++){
      free(img[i][j]);
    }
    free(img[i]);
  }
  free(img);
}