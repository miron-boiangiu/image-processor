#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bmp_header.h"
#define BUFF_LENGTH 100
void get_command_type(char** command_type, char* command){
    char* first_occurence_of_space = strchr(command, ' ');
    if(first_occurence_of_space!= NULL){
        int command_end = first_occurence_of_space - command + 1;
        for(int i = 0; i<command_end-1; i++){
            (*command_type)[i] = command[i];
        }
        (*command_type)[command_end-1] = '\0';
    }
}

void edit(char* command, bmp_fileheader * fileHeader, bmp_infoheader * infoHeader, char****matrix){
    char* first_space_position = strchr(command, ' ');
    char* file_location = calloc(BUFF_LENGTH, 1);
    strcpy(file_location, first_space_position+1);
    if(file_location[strlen(file_location)-1] == '\n'){
        file_location[strlen(file_location)-1] = '\0';
    }
    if(file_location[strlen(file_location)-1] == 13){
        file_location[strlen(file_location)-1] = '\0';
    }
    FILE* imagine = fopen(file_location, "rb");
    if(imagine == NULL){
        printf("\nEROARE LA CITIRE\n");
    }
    //Reading file header
    fread( &(fileHeader->fileMarker1), sizeof(char), 1, imagine);
    fread( &(fileHeader->fileMarker2), sizeof(char), 1, imagine);
    fread( &(fileHeader->bfSize), sizeof(int), 1, imagine);
    fread( &(fileHeader->unused1), sizeof(short), 1, imagine);
    fread( &(fileHeader->unused2), sizeof(short), 1, imagine);
    fread( &(fileHeader->imageDataOffset), sizeof(int), 1, imagine);
    //Reading info header
    fread( &(infoHeader->biSize), sizeof(int), 1, imagine);
    fread( &(infoHeader->width), sizeof(int), 1, imagine);
    fread( &(infoHeader->height), sizeof(int), 1, imagine);
    fread( &(infoHeader->planes), sizeof(short), 1, imagine);
    fread( &(infoHeader->bitPix), sizeof(short), 1, imagine);
    fread( &(infoHeader->biCompression), sizeof(int), 1, imagine);
    fread( &(infoHeader->biSizeImage), sizeof(int), 1, imagine);
    fread( &(infoHeader->biXPelsPerMeter), sizeof(int), 1, imagine);
    fread( &(infoHeader->biYPelsPerMeter), sizeof(int), 1, imagine);
    fread( &(infoHeader->biClrUsed), sizeof(int), 1, imagine);
    fread( &(infoHeader->biClrImportant), sizeof(int), 1, imagine);
    //Reading pixel info
    int width = infoHeader->width;
    int height = infoHeader->height;
    int padding_size = 0;
    //If the image matrix line value count isn't a perfect multiple of 4
    if((width*3)%4){ 
        padding_size = 4-(width*3)%4;//Check how many more bytes are needed
    }
    *matrix = calloc(height, sizeof(char **));
    for(int i = 0; i<height; i++){
        (*matrix)[i] = calloc(width, sizeof(char*));
        for(int j=0; j<width; j++){
            (*matrix)[i][j] = calloc(3, sizeof(char));
        }
    }
    //Finally reading the matrix
    for(int i = 0; i<height; i++){
        char value;
        for(int j = 0; j<width; j++){
            for(int z=0; z<3; z++){
                fread(&value, sizeof(char), 1, imagine);
                (*matrix)[i][j][z] = value;
            }
            
        }
        for(int z=0; z<padding_size; z++){ //Ignore padding
                fread(&value, sizeof(char), 1, imagine);
        }
    }

    fclose(imagine);
    free(file_location);
}

void save(char* command, bmp_fileheader * fileHeader, bmp_infoheader * infoHeader, char****matrix){
    char* first_space_position = strchr(command, ' ');
    char* file_location = calloc(BUFF_LENGTH, 1);
    strcpy(file_location, first_space_position+1);
    if(file_location[strlen(file_location)-1] == '\n'){
        file_location[strlen(file_location)-1] = '\0';
    }
    if(file_location[strlen(file_location)-1] == 13){
        file_location[strlen(file_location)-1] = '\0';
    }
    FILE* imagine = fopen(file_location, "wb");
    //Writing file header
    fwrite( &(fileHeader->fileMarker1), sizeof(char), 1, imagine);
    fwrite( &(fileHeader->fileMarker2), sizeof(char), 1, imagine);
    fwrite( &(fileHeader->bfSize), sizeof(int), 1, imagine);
    fwrite( &(fileHeader->unused1), sizeof(short), 1, imagine);
    fwrite( &(fileHeader->unused2), sizeof(short), 1, imagine);
    fwrite( &(fileHeader->imageDataOffset), sizeof(int), 1, imagine);
    //Writing info header
    fwrite( &(infoHeader->biSize), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->width), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->height), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->planes), sizeof(short), 1, imagine);
    fwrite( &(infoHeader->bitPix), sizeof(short), 1, imagine);
    fwrite( &(infoHeader->biCompression), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->biSizeImage), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->biXPelsPerMeter), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->biYPelsPerMeter), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->biClrUsed), sizeof(int), 1, imagine);
    fwrite( &(infoHeader->biClrImportant), sizeof(int), 1, imagine);
    //Writing pixel info
    int width = infoHeader->width;
    int height = infoHeader->height;
    int padding_size = 0;
    //If the image matrix line value count isn't a perfect multiple of 4
    if((width*3)%4){ 
        padding_size = 4-(width*3)%4;//Check how many more bytes are needed
    }
    //Finally writing the matrix
    for(int i = 0; i<height; i++){
        char value;
        for(int j = 0; j<width; j++){
            for(int z=0; z<3; z++){
                value = (*matrix)[i][j][z];
                fwrite(&value, sizeof(char), 1, imagine);
            }
            
        }
        for(int z=0; z<padding_size; z++){ //Add padding
            value = 0;
            fwrite(&value, sizeof(char), 1, imagine);
        }
    }

    fclose(imagine);
    free(file_location);
}

void insert(char* command,bmp_fileheader*fileHeader,bmp_infoheader*infoHeader,char **** matrix){
    int insert_x, insert_y;
    char* token = calloc(BUFF_LENGTH, 1);
    strcpy(token, strtok(command, " "));
    strcpy(token, strtok(NULL, " "));
    FILE* imagine = fopen(token, "rb");
    strcpy(token, strtok(NULL, " "));
    insert_x = atoi(token);
    strcpy(token, strtok(NULL, " "));
    if(token[strlen(token)-1] == '\n'){
        token[strlen(token)-1] = '\0';
    }
    if(token[strlen(token)-1] == 13){
        token[strlen(token)-1] = '\0';
    }
    insert_y = atoi(token);
    free(token);
    if(imagine == NULL){
        printf("\nEROARE LA CITIRE\n");
    }
    bmp_fileheader insertFileHeader;
    bmp_infoheader insertInfoHeader;
    //Reading file header
    fread( &(insertFileHeader.fileMarker1), sizeof(char), 1, imagine);
    fread( &(insertFileHeader.fileMarker2), sizeof(char), 1, imagine);
    fread( &(insertFileHeader.bfSize), sizeof(int), 1, imagine);
    fread( &(insertFileHeader.unused1), sizeof(short), 1, imagine);
    fread( &(insertFileHeader.unused2), sizeof(short), 1, imagine);
    fread( &(insertFileHeader.imageDataOffset), sizeof(int), 1, imagine);
    //Reading info header
    fread( &(insertInfoHeader.biSize), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.width), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.height), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.planes), sizeof(short), 1, imagine);
    fread( &(insertInfoHeader.bitPix), sizeof(short), 1, imagine);
    fread( &(insertInfoHeader.biCompression), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.biSizeImage), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.biXPelsPerMeter), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.biYPelsPerMeter), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.biClrUsed), sizeof(int), 1, imagine);
    fread( &(insertInfoHeader.biClrImportant), sizeof(int), 1, imagine);
    //Reading pixel info
    int insert_width = insertInfoHeader.width;
    int insert_height = insertInfoHeader.height;
    int width = infoHeader->width;
    int height = infoHeader->height;
    int insert_padding_size = 0;
    //If the image matrix line value count isn't a perfect multiple of 4
    if((insert_width*3)%4){ 
        insert_padding_size = 4-(insert_width*3)%4;//Check how many more bytes are needed
    }

    //Finally reading & writing the matrix

    for(int i = 0; i<insert_height; i++){
        char value;
        for(int j = 0; j<insert_width; j++){
            for(int z=0; z<3; z++){
                fread(&value, sizeof(char), 1, imagine);
                if((insert_y+i<height) && (insert_x+j<width)){
                    (*matrix)[insert_y+i][insert_x+j][z] = value;
                }
            }
            
            
        }
        for(int z=0; z<insert_padding_size; z++){ //Ignore padding
                fread(&value, sizeof(char), 1, imagine);
        }
    }

    fclose(imagine);
}

void draw_point(char ****matrix, int x, int y, int width, int height, char* drawInfo){
    int size = drawInfo[3];
    for(int i = y-size/2; i<=y+size/2; i++){
        for(int j = x-size/2; j<=x+size/2; j++){
            if((i>=0) && (i<height)){
                if((j>=0) && (j<width)){
                    for(int k = 0; k<3; k++){
                        (*matrix)[i][j][k] = drawInfo[k];
                    }
                }
            }
        }
    }
}

int draw_line(int x1,int x2,int y1,int y2,bmp_infoheader*infoHead,char****matrix,char*draw_info){
    int width = infoHead->width;
    int height = infoHead->height;
    draw_point(matrix, x1, y1, width, height, draw_info);
    draw_point(matrix, x2, y2, width, height, draw_info);
    if(y1 == y2){
        if(x1 > x2){
            int temp = x2;
            x2 = x1;
            x1 = temp;
        }
        for(int i = x1+1; i<=x2-1; i++){
            draw_point(matrix, i, y1, width, height, draw_info);
        }
        return 0;
    }
    if(x1 == x2){
        if(y1 > y2){
            int temp = y2;
            y2 = y1;
            y1 = temp;
        }
        for(int i = y1+1; i<=y2-1; i++){
            draw_point(matrix, x1, i, width, height, draw_info);
        }
        return 0;
    }
    if((x2-x1)*(x2-x1) > (y2-y1)*(y2-y1)){
        if(x1>x2){
            int temp = x2;
            x2 = x1;
            x1 = temp;
            temp = y2;
            y2 = y1;
            y1 = temp;
        }
        for(int i = x1; i<=x2; i++){
            int point_y = 1.0*(i-x1)*(y2-y1)/(x2-x1) + y1;
            draw_point(matrix, i, point_y, width, height, draw_info);
            
        }
    }
    else{
        if(y1>y2){
            int temp = x2;
            x2 = x1;
            x1 = temp;
            temp = y2;
            y2 = y1;
            y1 = temp;
        }
        for(int i = y1; i<=y2; i++){
            int point_x = 1.0*(i-y1)*(x2-x1)/(y2-y1) + x1;
                draw_point(matrix, point_x, i, width, height, draw_info);
            
        }
    }
    return 0;

}

void set(char*com,char**draw_info){
    char* token = calloc(BUFF_LENGTH, 1);
    strcpy(token, strtok(com, " ")); //command
    strcpy(token, strtok(NULL, " ")); //type

    if(!strcmp(token, "line_width")){
        strcpy(token, strtok(NULL, " "));
        if(token[strlen(token)-1] == '\n'){
            token[strlen(token)-1] = '\0';
        }
        if(token[strlen(token)-1] == 13){
            token[strlen(token)-1] = '\0';
        }
        (*draw_info)[3] = atoi(token);
    }
    else{
        strcpy(token, strtok(NULL, " "));
        (*draw_info)[2] = atoi(token);
        strcpy(token, strtok(NULL, " "));
        (*draw_info)[1] = atoi(token);
        strcpy(token, strtok(NULL, " "));
        if(token[strlen(token)-1] == '\n'){
            token[strlen(token)-1] = '\0';
        }
        if(token[strlen(token)-1] == 13){
            token[strlen(token)-1] = '\0';
        }
        (*draw_info)[0] = atoi(token);
    }
    free(token);
}

void draw(char*com,bmp_infoheader*infoHeader,char****matrix,char*draw_info){
    char* token = calloc(BUFF_LENGTH, 1);
    strcpy(token, strtok(com, " ")); //command
    strcpy(token, strtok(NULL, " ")); //type

    if(!strcmp(token, "line")){
        int x1, x2, y1, y2;
        strcpy(token, strtok(NULL, " "));
        x1 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        y1 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        x2 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        if(token[strlen(token)-1] == '\n'){
            token[strlen(token)-1] = '\0';
        }
        if(token[strlen(token)-1] == 13){
            token[strlen(token)-1] = '\0';
        }
        y2 = atoi(token);
        draw_line(x1, x2, y1, y2, infoHeader, matrix, draw_info);
    }
    else if(!strcmp(token, "rectangle")){
        int x1, y1, width, height;
        strcpy(token, strtok(NULL, " "));
        x1 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        y1 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        width = atoi(token);
        strcpy(token, strtok(NULL, " "));
        if(token[strlen(token)-1] == '\n'){
            token[strlen(token)-1] = '\0';
        }
        if(token[strlen(token)-1] == 13){
            token[strlen(token)-1] = '\0';
        }
        height = atoi(token);
        draw_line(x1, x1+width, y1, y1, infoHeader, matrix, draw_info);
        draw_line(x1, x1, y1, y1+height, infoHeader, matrix, draw_info);
        draw_line(x1+width, x1+width, y1, y1+height, infoHeader, matrix, draw_info);
        draw_line(x1, x1+width, y1+height, y1+height, infoHeader, matrix, draw_info);
    }
    else if(!strcmp(token, "triangle")){
        int x1, y1, x2, y2, x3, y3;
        strcpy(token, strtok(NULL, " "));
        x1 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        y1 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        x2 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        y2 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        x3 = atoi(token);
        strcpy(token, strtok(NULL, " "));
        if(token[strlen(token)-1] == '\n'){
            token[strlen(token)-1] = '\0';
        }
        if(token[strlen(token)-1] == 13){
            token[strlen(token)-1] = '\0';
        }
        y3 = atoi(token);
        draw_line(x1, x2, y1, y2, infoHeader, matrix, draw_info);
        draw_line(x1, x3, y1, y3, infoHeader, matrix, draw_info);
        draw_line(x3, x2, y3, y2, infoHeader, matrix, draw_info);
    }
    free(token);
}

int fill(char*com,bmp_infoheader*infoHeader,char****matrix,char*draw_info){
    char* token = calloc(BUFF_LENGTH, 1);
    int x, y;
    strcpy(token, strtok(com, " ")); //command
    strcpy(token, strtok(NULL, " ")); //x
    x = atoi(token);
    strcpy(token, strtok(NULL, " ")); //y
    if(token[strlen(token)-1] == '\n'){
        token[strlen(token)-1] = '\0';
    }
    if(token[strlen(token)-1] == 13){
        token[strlen(token)-1] = '\0';
    }
    y = atoi(token);
    free(token);
    int has_same_color = 1; //check if the pixel is already the selected fill color
    char* color_to_replace = calloc(3, 1);
    for(int i = 0; i<3; i++){
        color_to_replace[i] = (*matrix)[y][x][i];
        if(color_to_replace[i] != draw_info[i]){
            has_same_color = 0;
        }
    }
    if(has_same_color){
        free(color_to_replace);
        return 0;
    }
    int width = infoHeader->width;
    int height = infoHeader->height;

    char *** filler_matrix; //This matrix will memorize where we need to fill
    filler_matrix = calloc(height, sizeof(char**)); 
    for(int i = 0; i<height; i++){
        filler_matrix[i] = calloc(width, sizeof(char*));
        for(int k = 0; k<width; k++){
            filler_matrix[i][k] = calloc(3, 1);
        }
    }
    for(int i = 0; i<3; i++){
        filler_matrix[y][x][i] = 1;
    }
    int am_inlocuit_ceva = 1;
    while(am_inlocuit_ceva){ //Draw filler matrix
        am_inlocuit_ceva = 0;
        for(int i = 0; i<height; i++){
            for(int j = 0; j<width; j++){
                if((filler_matrix[i][j][0] == 1)){
                    //If we can go up:
                    if(i>=1){
                        int aceeasi_culoare = 1;
                        for(int k = 0; k<3; k++){
                            if((*matrix)[i-1][j][k] != color_to_replace[k]){
                                aceeasi_culoare = 0;
                            }
                        }
                        if(aceeasi_culoare && filler_matrix[i-1][j][0] != 1){
                            filler_matrix[i-1][j][0] = 1;
                            am_inlocuit_ceva = 1;
                        }
                    }
                    //If we can go left:
                    if(j>=1){
                        int aceeasi_culoare = 1;
                        for(int k = 0; k<3; k++){
                            if((*matrix)[i][j-1][k] != color_to_replace[k]){
                                aceeasi_culoare = 0;
                            }
                        }
                        if(aceeasi_culoare && filler_matrix[i][j-1][0] != 1){
                            filler_matrix[i][j-1][0] = 1;
                            am_inlocuit_ceva = 1;
                        }
                    }
                    //If we can go right:
                    if(j<width-1){
                        int aceeasi_culoare = 1;
                        for(int k = 0; k<3; k++){
                            if((*matrix)[i][j+1][k] != color_to_replace[k]){
                                aceeasi_culoare = 0;
                            }
                        }
                        if(aceeasi_culoare && filler_matrix[i][j+1][0] != 1){
                            filler_matrix[i][j+1][0] = 1;
                            am_inlocuit_ceva = 1;
                        }
                    }
                    //If we can go down:
                    if(i<height-1){
                        int aceeasi_culoare = 1;
                        for(int k = 0; k<3; k++){
                            if((*matrix)[i+1][j][k] != color_to_replace[k]){
                                aceeasi_culoare = 0;
                            }
                        }
                        if(aceeasi_culoare && filler_matrix[i+1][j][0] != 1){
                            filler_matrix[i+1][j][0] = 1;
                            am_inlocuit_ceva = 1;
                        }
                    }
                }
            }
        }
    }
    //We're done with checking where we need to fill.
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            if(filler_matrix[i][j][0] == 1){
                for(int k = 0; k<3; k++){
                    (*matrix)[i][j][k] = draw_info[k];
                }
            }
        }
    }


    //Free the filler matrix.
    for(int i = 0; i<height; i++){
        for(int j = 0; j<width; j++){
            free(filler_matrix[i][j]);
        }
        free(filler_matrix[i]);
    }
    free(filler_matrix);
    free(color_to_replace);
    return 0;
}

int main(){
    bmp_fileheader fileHeader;
    bmp_infoheader infoHeader;
    long unsigned int bufSize = BUFF_LENGTH;
    char* command;
    char* command_type;
    char *** rgb_matrix;
    char* draw_info;
    draw_info = calloc(4, 1);
    draw_info[3] = 1; //This is the draw width, the first 3 values are the colors.
    command_type = calloc(BUFF_LENGTH, 1);
    command = calloc(BUFF_LENGTH, 1);
    int read_image = 0;
    while(1){
        strcpy(command, "");
        strcpy(command_type, "");
        getline(&command, &bufSize, stdin);
        get_command_type(&command_type, command);
        if(!strcmp(command_type, "edit")){   //Cases for command type
            edit(command, &fileHeader, &infoHeader, &rgb_matrix);
            read_image = 1;
        }
        else if(!strcmp(command_type, "save")){
            save(command, &fileHeader, &infoHeader, &rgb_matrix);
        }
        else if(!strcmp(command_type, "draw")){
            draw(command, &infoHeader, &rgb_matrix, draw_info);
        }
        else if(!strcmp(command_type, "fill")){
            fill(command, &infoHeader, &rgb_matrix, draw_info);
        }
        else if(!strcmp(command_type, "set")){
            set(command, &draw_info);
        }
        else if(!strcmp(command_type, "insert")){
            insert(command, &fileHeader, &infoHeader, &rgb_matrix);
        }
        else if(!strcmp(command, "quit\n")){
            if(read_image){
                int width = infoHeader.width;
                int height = infoHeader.height;
                for(int i = 0; i<height; i++){
                    for(int j=0; j<width; j++){
                        free(rgb_matrix[i][j]);
                    }
                    free(rgb_matrix[i]);
                }
                free(rgb_matrix);
            }
            free(command_type);
            free(command);
            free(draw_info);
            return 0;
        }
    }
}