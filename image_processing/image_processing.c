#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char byte;

#define MAX_GRAY 255
#define NCOLS    256
#define NROWS    256
#define NPIXELS  (NCOLS * NROWS)

#define ROUND(x) (long)floor(x+0.5)
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

float avg_filter1[3][3] =
{
	{1.0/9, 1.0/9, 1.0/9},
	{1.0/9, 1.0/9, 1.0/9},
	{1.0/9, 1.0/9, 1.0/9}
};

float avg_filter2[7][7] =
{
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
	{1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49, 1.0/49},
};

float smooth_filter[3][3] = {
	{1.0/16, 2.0/16, 1.0/16},
	{2.0/16, 4.0/16, 2.0/16},
	{1.0/16, 2.0/16, 1.0/16},
};

float filter4[3][3] = {
	{ 0.0, -1.0,  0.0},
	{-1.0,  5.0, -1.0},
	{ 0.0, -1.0,  0.0}
};

// 이미지에 마스크를 적용해 필터링한다.
void filtering(byte* img_from, byte* img_to, float* mask, int mask_x, int mask_y)
{
	printf("filter size : %d x %d\n", mask_x, mask_y);

	int margin_x = (mask_x - 1)/2;
	int margin_y = (mask_y - 1)/2;

	for(int i=margin_y; i<NROWS-margin_y; i++)
	{
		for(int j=margin_x; j<NCOLS-margin_x; j++)
		{
			byte conv = 0;
			for(int mi=0; mi<mask_y; mi++)
			{
				for(int mj=0; mj<mask_x; mj++)
				{
					conv += (mask[mi*mask_x + mj] * img_from[(i + mi - margin_x)*NCOLS + (j + mj - margin_y)]);
				}
			}

			img_to[i*NCOLS + j] = conv;
		}
	}
}

// 3의 값중 중간값 리턴
byte median(byte v1, byte v2, byte v3)
{
	byte big   = MAX(v1, v2);
	byte small = MIN(v1, v2);

	if(v3 <= small) return small;
	else if(v3 >= big) return big;
	else return v3;
}

// 메디안 필터링
void median_filtering(byte* img_from, byte* img_to)
{
	for(int i=1; i<NROWS-1; i++)
	{
		for(int j=1; j<NCOLS-1; j++)
		{
			byte median1 = median(img_from[(i-1)*NCOLS + (j-1)], img_from[i*NCOLS + (j-1)], img_from[(i+1)*NCOLS + (j-1)]);
			byte median2 = median(img_from[(i-1)*NCOLS + (j+0)], img_from[i*NCOLS + (j+0)], img_from[(i+1)*NCOLS + (j+0)]);
			byte median3 = median(img_from[(i-1)*NCOLS + (j+1)], img_from[i*NCOLS + (j+1)], img_from[(i+1)*NCOLS + (j+1)]);

			img_to[i*NCOLS + j] = median(median1, median2, median3);
		}
	}
}

// image 에 histogram equalization 을 적용해 new_image 를 만든다.
void histeq(const byte* image, byte* new_image)
{
	// 히스토그램 생성
	int hist[MAX_GRAY+1] = {0,};
	for(int i=0; i<NROWS; i++)
	{
		for(int j=0; j<NCOLS; j++)
		{
			byte v = image[i*NCOLS + j];
			hist[v]++;
		}
	}

	// 누적분포함수 생성 (Cumulative Distribution Function)
	int cdf[MAX_GRAY+1] = {0,};
	int sum = 0;
	for(int i=1; i<=MAX_GRAY; i++)
	{
		sum += hist[i];
		cdf[i] = sum;
	}

	// histogram equalization
	float scale_factor = MAX_GRAY/(float)(NPIXELS);
	for(int i=0; i<NROWS; i++)
	{
		for(int j=0; j<NCOLS; j++)
		{
			byte v = image[i*NCOLS + j];
			new_image[i*NCOLS + j] = (byte)(ROUND(scale_factor * cdf[v]));
		}
	}

	printf("histogram equalization success!\n");
}

// 이미지 파일을 읽는다.
void read_image(byte* buf, const char* fname)
{
	FILE* fp = fopen(fname, "rb");

	if(fp == NULL)
	{
		fprintf(stderr, "[%s] file open error!\n", fname);
		exit(-1);
	}

	int nread = fread(buf, 1, NPIXELS, fp);
	if(nread != NPIXELS)
	{
		fprintf(stderr, "file read error! [%d/%d]\n", nread, NPIXELS);
		exit(-1);
	}

	printf("image read success! : %d \n", nread);
}

// 이미지 파일을 만든다.
void write_image(const byte* buf, const char* fname)
{
	FILE* fp = fopen(fname, "wb");
	if(fp == NULL)
	{
		fprintf(stderr, "[%s] file open error!\n", fname);
		exit(-1);
	}

	int nwrite = fwrite(buf, 1, NPIXELS, fp);
	if(nwrite != NPIXELS)
	{
		fprintf(stderr, "file write error! [%d/%d]\n", nwrite, NPIXELS);
		exit(-1);
	}

	printf("image write success! : %d \n", nwrite);
}

// 이미지 버퍼 클리어
void clear_image(byte* image)
{
	memset(image, 0, NPIXELS);
}

int main(void)
{
	// 1. histogram equalization(히스토그램 분산)
	byte image_input[NPIXELS] = {0,};
	byte image_histeq[NPIXELS] = {0,};
	read_image(image_input, "input.raw");
	histeq(image_input, image_histeq);
	write_image(image_histeq, "histeq_input.raw");

	// 2. Average filtering(블러링)
	clear_image(image_input);
	byte image_filtered[NPIXELS] = {0,};
	read_image(image_input, "lena256.raw");
	filtering(image_input, image_filtered, (float*)avg_filter1, 3, 3);
	write_image(image_filtered, "avr_filter1_lena256.raw");

	clear_image(image_filtered);
	filtering(image_input, image_filtered, (float*)avg_filter2, 7, 7);
	write_image(image_filtered, "avr_filter2_lena256.raw");

	// 3. Smooth filtering
	clear_image(image_filtered);
	filtering(image_input, image_filtered, (float*)smooth_filter, 3, 3);
	write_image(image_filtered, "smooth_filter_lena256.raw");

	// 4. Sharpening Filtering
	// avg_filter1 의 결과에 적용한다.
	clear_image(image_input);
	clear_image(image_filtered);
	read_image(image_input, "avr_filter1_lena256.raw");
	filtering(image_input, image_filtered, (float*)filter4, 3, 3);
	write_image(image_filtered, "sharpening_filter_lena256.raw");

	// 5. median filtering(잡티노이즈제거)
	clear_image(image_input);
	clear_image(image_filtered);
	read_image((byte*)image_input, "lena256_n5.raw");
	median_filtering((byte*)image_input, (byte*)image_filtered);
	write_image(image_filtered, "median_filter_lena256_n5.raw");

	clear_image(image_input);
	clear_image(image_filtered);
	read_image((byte*)image_input, "lena256_n10.raw");
	median_filtering((byte*)image_input, (byte*)image_filtered);
	write_image(image_filtered, "median_filter_lena256_n10.raw");

	clear_image(image_input);
	clear_image(image_filtered);
	read_image((byte*)image_input, "lena256_n25.raw");
	median_filtering((byte*)image_input, (byte*)image_filtered);
	write_image(image_filtered, "median_filter_lena256_n25.raw");

	return 0;
}
















