#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct _PGMImage {
	int row;
	int col;
	int max_gray;
	int **matrix;
} PGMImage;

int **allocate_memory(int row, int col){
	int **matrix;
	int i;

	matrix = (int **)malloc(sizeof(int*) * row);//Allocate rows

	if (matrix == NULL){
		perror("MEMORY ALLOCATION FAILED");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < row; ++i){
		matrix[i] = (int *)malloc(sizeof(int) * col); // Allocate cols
		if(matrix[i] == NULL){
			perror("MEMORY ALLOCATION FAILED");
			exit(EXIT_FAILURE);
		}
	}
	return matrix;
}

void deallocate_image_memory(int **matrix, int row){
	int i;

	for (i = 0; i < row; ++i){
		free(matrix[i]); // Deallocate every rows
	}
	free(matrix);
}

PGMImage* readPGM(const char *file_name,PGMImage *data){
	FILE *pgmFile;
	char version[3];
	int i,j;

	pgmFile = fopen(file_name,"rb");
	if (pgmFile == NULL){
		perror("CANT READ IMAGE");
		exit(EXIT_FAILURE);
	}
	fgets(version,sizeof(version),pgmFile);
	if(strcmp(version,"P5")){
		printf("wrong file type");
		exit(EXIT_FAILURE);
	}

	fscanf(pgmFile, "%d", &data->col);
	fscanf(pgmFile, "%d", &data->row);
	fscanf(pgmFile, "%d", &data->max_gray);
	fgetc(pgmFile);
	data->matrix = allocate_memory(data->row,data->col);
       	for (i = 0; i < data->row; ++i) {
      		for (j = 0; j < data->col; ++j) {
               		data->matrix[i][j] = fgetc(pgmFile);
       		}
      	}

    	fclose(pgmFile);
    	return data;
}
/*and for writing*/

void writePGM(const char *filename, const PGMImage *data)
{
    FILE *pgmFile;
    int i, j;

    pgmFile = fopen(filename, "wb");
    if (pgmFile == NULL) {
        perror("cannot open file to write");
        exit(EXIT_FAILURE);
    }

    fprintf(pgmFile, "P5 ");
    fprintf(pgmFile, "%d %d ", data->col, data->row);
    fprintf(pgmFile, "%d ", data->max_gray);

        for (i = 0; i < data->row; ++i) {
            for (j = 0; j < data->col; ++j) {
                fputc(data->matrix[i][j], pgmFile);
            }
        }

    fclose(pgmFile);
    deallocate_image_memory(data->matrix, data->row);
}

PGMImage *apply_convolution(PGMImage *original_image,int **filter,int filter_size,int division_factor){
	PGMImage *new_image;
	int i,j,k,l,sum;
	new_image = (PGMImage *)malloc(sizeof(PGMImage));
	new_image = original_image;

	div_t padding;
	padding = div(filter_size,2);
	int s = padding.quot;

        for (i = s;i<new_image->row-s;i++){
                for (j = s;j<new_image->col-s;j++){
			if(i == 0 || j == 0  || i >= new_image->row-s || j >= new_image->col-s){ // resmin koselerine dokunmadm iceriden basla
				new_image->matrix[i][j] = original_image->matrix[i][j];
			}else{
				sum = 0;
				for (k = 0;k < filter_size;k++){
					for (l = 0;l < filter_size;l++){
						sum += filter[k][l] * original_image->matrix[i-1+k][j-1+l];
					}
				}
				new_image->matrix[i][j] = abs((int)sum/division_factor); // burada bolulu bir seyler veya normalizasyon olabilir.
				if ( new_image->matrix[i][j] < 0 ) { new_image->matrix[i][j] = 0;}
			}
                }
        }
	return new_image;
}

PGMImage *apply_average_filter(PGMImage *original_image, int **filter, int filter_size){
	PGMImage *new_image;
        int i,j,k,l,sum;
        new_image = (PGMImage *)malloc(sizeof(PGMImage));
        new_image = original_image;

        div_t padding;
        padding = div(filter_size,2);
        int s = padding.quot;

        for (i = s;i<new_image->row-s;i++){
                for (j = s;j<new_image->col-s;j++){
                        if(i == 0 || j == 0  || i >= new_image->row-s || j >= new_image->col-s){ // resmin koselerine dokunmadm bir pixel iceri
                                new_image->matrix[i][j] = original_image->matrix[i][j];
                        }else{
                                sum = 0;
                                for (k = 0;k < filter_size;k++){
                                        for (l = 0;l < filter_size;l++){
                                                sum += filter[k][l] * original_image->matrix[i-1+k][j-1+l];
                                        }
                                }
                                new_image->matrix[i][j] = sum/(filter_size*filter_size); // burada bolulu bir seyler veya normalizasyon olabilir.
                        }
                        //printf("%d ",new_image->matrix[i][j]);
                }
                //printf("\n\n\n");
        }
        return new_image;

}

void XFiltering(PGMImage *img){

    int **newPixelx;

    int sobel_x[3][3] = { { 1,   0,  -1},
                          { 2,   0,  -2},
                          { 1,   0,  -1}};
    int sobel_y[3][3] = { { 1,   2,   1},
                          { 0,   0,   0},
                          {-1,  -2,  -1}};
    int x,y,i,j;
    int valX=0,valY=0;

    newPixelx = allocate_memory(img->row,img->col);
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            newPixelx[x][y] = 0;
        }

    for (i=1; i<img->row-1; i++)
        for (j=1; j<img->col-1; j++){
            valX=0; valY=0;
            for (x = -1; x <= 1; x++)
                 for (y = -1; y <= 1; y++){
                    valX = valX + img->matrix[i+x][j+y] * sobel_x[1+x][1+y];
                    valY = valY + img->matrix[i+x][j+y] * sobel_y[1+x][1+y];
                    }
            newPixelx [i][j] = (int)(valX);
            if (newPixelx[i][j] < 0)
                newPixelx[i][j] = 0;
            else if (newPixelx[i][j] > 255)
                newPixelx[i][j] = 255;
        }
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            img->matrix[x][y] = newPixelx[x][y];
        }

    writePGM("sonuc.X.pgm",img);
    printf("\n[*]End of X.!");
}
void YFiltering(PGMImage *img){

    int **newPixely;

    int sobel_x[3][3] = { { 1,   0,  -1},
                          { 2,   0,  -2},
                          { 1,   0,  -1}};
    int sobel_y[3][3] = { { 1,   2,   1},
                          { 0,   0,   0},
                          {-1,  -2,  -1}};
    int x,y,i,j;
    int valX=0,valY=0;

    newPixely = allocate_memory(img->row,img->col);
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            newPixely[x][y] = 0;
        }

    for (i=1; i<img->row-1; i++)
        for (j=1; j<img->col-1; j++){
            valX=0; valY=0;
            for (x = -1; x <= 1; x++)
                 for (y = -1; y <= 1; y++){
                    valX = valX + img->matrix[i+x][j+y] * sobel_x[1+x][1+y];
                    valY = valY + img->matrix[i+x][j+y] * sobel_y[1+x][1+y];
                    }
            newPixely [i][j] = (int)(valY);
            if (newPixely[i][j] < 0)
                newPixely[i][j] = 0;
            else if (newPixely[i][j] > 255)
                newPixely[i][j] = 255;
        }
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            img->matrix[x][y] = newPixely[x][y];
        }

    writePGM("sonuc.Y.pgm",img);
    printf("\n[*]End of Y.!");
}
void sobelFiltering(PGMImage *img){

    int **newPixel;

    int sobel_x[3][3] = { { 1,   0,  -1},
                          { 2,   0,  -2},
                          { 1,   0,  -1}};
    int sobel_y[3][3] = { { 1,   2,   1},
                          { 0,   0,   0},
                          {-1,  -2,  -1}};
    int x,y,i,j;
    int valX=0,valY=0;

    newPixel = allocate_memory(img->row,img->col);
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            newPixel[x][y] = 0;
        }

    for (i=1; i<img->row-1; i++)
        for (j=1; j<img->col-1; j++){
            valX=0; valY=0;
            for (x = -1; x <= 1; x++)
                 for (y = -1; y <= 1; y++){
                    valX = valX + img->matrix[i+x][j+y] * sobel_x[1+x][1+y];
                    valY = valY + img->matrix[i+x][j+y] * sobel_y[1+x][1+y];
                    }
            newPixel[i][j] = (int)( sqrt(valX*valX + valY*valY) );
            if (newPixel[i][j] < 0)
                newPixel[i][j] = 0;
            else if (newPixel[i][j] > 255)
                newPixel[i][j] = 255;
        }
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            img->matrix[x][y] = newPixel[x][y];
        }

    writePGM("sonuc.sobel.pgm",img);
    printf("\n[*]End of Sobel.!");
}
void PrewittFiltering(PGMImage *img){

    int **newPixel3;

    int prewitt_x[3][3] = { { 5, 5, 5},
                          { -3, 0, -3},
                          { -3, -3, -3}};
    int prewitt_y[3][3] = { { 5, -3, -3},
                          { 5, 0, -3},
                          {5, -3, -3}};
    int x,y,i,j;
    int valX=0,valY=0;

    newPixel3 = allocate_memory(img->row,img->col);
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            newPixel3[x][y] = 0;
        }

    for (i=1; i<img->row-1; i++)
        for (j=1; j<img->col-1; j++){
            valX=0; valY=0;
            for (x = -1; x <= 1; x++)
                 for (y = -1; y <= 1; y++){
                    valX = valX + img->matrix[i+x][j+y] * prewitt_x[1+x][1+y];
                    valY = valY + img->matrix[i+x][j+y] * prewitt_y[1+x][1+y];
                    }
            newPixel3[i][j] = (int)( sqrt(valX*valX + valY*valY) );
            if (newPixel3[i][j] < 0)
                newPixel3[i][j] = 0;
            else if (newPixel3[i][j] > 255)
                newPixel3[i][j] = 255;
        }
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            img->matrix[x][y] = newPixel3[x][y];
        }

    writePGM("prewitt.pgm",img);
    printf("\n[*]End of prewitt!");
}


void laplacianFiltering(PGMImage *img){

    int **newPixel;

    int laplacian[3][3] = { { 1,   1,  1},
                          { 1,  -8,  1},
                          { 1,   1,  1}};
    int x,y,i,j;
    int val;

    newPixel = allocate_memory(img->row,img->col);
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            newPixel[x][y] = 0;
        }

    for (i=1; i<img->row-1; i++)
        for (j=1; j<img->col-1; j++){
            val = 0;
            for (x = -1; x <= 1; x++)
                 for (y = -1; y <= 1; y++){
                    val = val + img->matrix[i+x][j+y] * laplacian[1+x][1+y];
                    }
            newPixel[i][j] = val;
            if (newPixel[i][j] < 0)
                newPixel[i][j] = 0;
            else if (newPixel[i][j] > 255)
                newPixel[i][j] = 255;
        }
    for (x=0; x<img->row; x++)
        for (y=0; y<img->col; y++){
            img->matrix[x][y] = newPixel[x][y];
        }

    writePGM("sonuc.laplace.pgm",img);
    printf("\n[*]End of Laplace.!");
}

PGMImage *apply_sobel_filter(PGMImage *original_image){
	PGMImage *new_image;
    int i,x,y,j,k,l,sumX,sumY,temp;
    int filter_size = 3;
	new_image = (PGMImage *)malloc(sizeof(PGMImage));

	new_image->matrix = allocate_memory(original_image->row,original_image->col);
    for (x=0; x<original_image->row; x++)
        for (y=0; y<original_image->col; y++){
            new_image->matrix[x][y] = 0;
        }
	int sobel_x[3][3] = { { 1,   0,  -1},
                            { 2,   0,  -2},
                            { 1,   0,  -1}};

    int sobel_y[3][3] = { { 1,   2,   1},
                              { 0,   0,   0},
                              {-1,  -2,  -1}};

    for (i = 1;i<original_image->row-1;i++){
        for (j = 1;j<original_image->col-1;j++){
            sumX = 0;
			sumY = 0;
			for (k = -1; k <= 1; k++){
                for (l = -1; l <= 1; l++){
    	    		sumX = sumX + original_image->matrix[i+k][j+l] * sobel_x[1+k][1+l];
        	        sumY = sumY + original_image->matrix[i+k][j+l] * sobel_y[1+k][1+l];
	        	}
			}
			new_image->matrix[i][j] = (int)(sqrt(sumX*sumX + sumY*sumY));
			if (new_image->matrix[i][j] < 0) {
				new_image->matrix[i][j] = 0;
			}else if (new_image->matrix[i][j] > 255) {
				new_image->matrix[i][j] = 255;
			}
        }
    }

	return new_image;
}


int main(){

	PGMImage *resim = (PGMImage *)malloc(sizeof(PGMImage));
	resim = readPGM("C:\\Users\\shilinti\\Desktop\\pgm files\\lena.PGM",resim);

	int **filter;
	int filter_size,i,j,division_factor;

	//gauss -> sobel ====== DONE
	//laplacian
/*
	printf("Filter Size : ");
	scanf("%d",&filter_size);
	filter = allocate_memory(filter_size,filter_size);

	printf("Enter Filter : \n");
	for (i = 0;i < filter_size;i++){
		for (j = 0;j < filter_size;j++){
			scanf("%d",&filter[i][j]);
		}
	}

	printf("Division Factor: ");
	scanf("%d",&division_factor);

	writePGM("gausslu.pgm",apply_convolution(resim,filter,filter_size,division_factor));*/
	YFiltering(readPGM("C:\\Users\\shilinti\\Desktop\\pgm files\\lena.PGM",resim));
	XFiltering(readPGM("C:\\Users\\shilinti\\Desktop\\pgm files\\lena.PGM",resim));
	sobelFiltering(readPGM("C:\\Users\\shilinti\\Desktop\\pgm files\\lena.PGM",resim));
	PrewittFiltering(readPGM("C:\\Users\\shilinti\\Desktop\\pgm files\\lena.PGM",resim));
	laplacianFiltering(readPGM("C:\\Users\\shilinti\\Desktop\\pgm files\\lena.PGM",resim));
	return 0;
}
