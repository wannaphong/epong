/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */


#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_HEADER_FILE_ONLY
#include "stb_image_write.h"
#include "stb_image.c"




int main( int argc, char *arg[]  ) {

	if (argc<4) {
                printf("usage: [SOURCEPNG] [TARGET_COLOR_BMP] [TARGET_ALPHA_BMP]\n");
		return 0;
	};
	char *pngFile = arg[1];
	char *bmp1 = arg[2];
	char *bmp2 = arg[3];
	
	
        printf("Converting from [%s] to bitmaps (color) %s, and (alpha) %s\n", pngFile, bmp1, bmp2 );
	int width, height, bytesPerPixel;
	unsigned char *data = stbi_load(pngFile, &width, &height, &bytesPerPixel, 4);
	if (!data) {
		printf("failed to load source image..\n");
		return 0;
	};
        printf("loaded source-image, width:%d height:%d bytesPerPixel:%d\n", width, height, bytesPerPixel );

	unsigned char *colorData = new unsigned char[ width * height * 3 ];
	unsigned char *alphaData = new unsigned char[ width * height ];

	unsigned int *s = (unsigned int*)data;
	unsigned int *s_target = s+width*height;
	unsigned char *cd = colorData;
	unsigned char *ad = alphaData;
	while (s!=s_target) {
		cd[0] = (*s&255);
		cd[1] = ((*s)>>8)&255;
		cd[2] = ((*s)>>16)&255;
		*ad = (((*s)>>24)&255);	// real alpha nowdays instead of inverted "mask"
		cd+=3;
		ad++;
		s++;
	};

        ad=alphaData;
	if (ad[0]==0) ad[0]=1;
	if (ad[(height-1)*width + (width-1)]==0) ad[(height-1)*width + (width-1)] = 1;

	stbi_write_bmp(bmp1, width, height, 3, colorData );
	stbi_write_bmp(bmp2, width, height, 1, alphaData );

	delete [] colorData;
	delete [] alphaData;




	stbi_image_free(data);
//    // ... process data if not NULL ... 
//    // ... x = width, y = height, n = # 8-bit components per pixel ...
//    // ... replace '0' with '1'..'4' to force that many components per pixel
};
