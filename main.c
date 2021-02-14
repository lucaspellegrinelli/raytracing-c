#include "raytracing.h"

#include <stdlib.h>
#include <stdio.h>

int main(){
  int img_w = 1280;
  int img_h = 720;

  int *** img = (int ***) malloc(img_h * sizeof(int **));
  for(int i = 0; i < img_h; i++){
    img[i] = (int **) malloc(img_w * sizeof(int *));
    for(int j = 0; j < img_w; j++){
      img[i][j] = (int *) malloc(3 * sizeof(int));
    }
  }

  generate_image(img, img_w, img_h);

  FILE* pgmimg;
  pgmimg = fopen("image.pgm", "wb");
  
  fprintf(pgmimg, "P3\n");
  fprintf(pgmimg, "%d %d\n", img_w, img_h);
  fprintf(pgmimg, "255\n");

  for(int i = 0; i < img_h; i++){
    for(int j = 0; j < img_w; j++){
      fprintf(pgmimg, "%d %d %d ", img[i][j][0], img[i][j][1], img[i][j][2]);
    }
    fprintf(pgmimg, "\n");
  }

  fclose(pgmimg);
}