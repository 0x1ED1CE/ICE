/*
MIT License

Copyright (c) 2024 Dice

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MAT_H
#define MAT_H

#include <malloc.h>
#include <stdio.h>

#define MAT_VERSION 1

#define MAT_ATTRIBUTE_NONE 0x00
#define MAT_ATTRIBUTE_META 0x01
#define MAT_ATTRIBUTE_VERT 0x12
#define MAT_ATTRIBUTE_TEXT 0x21
#define MAT_ATTRIBUTE_NORM 0x32
#define MAT_ATTRIBUTE_POSE 0x4F
#define MAT_ATTRIBUTE_FACE 0x58

typedef unsigned int mat_uint;
typedef signed int   mat_sint;
typedef float        mat_real;

typedef struct {
	mat_uint  type;
	mat_uint  size;
	mat_real *data;
} mat_list;

static mat_uint mat_read_uint(FILE *mat_file) {
	return (
		(mat_uint)fgetc(mat_file)<<24|
		(mat_uint)fgetc(mat_file)<<16|
		(mat_uint)fgetc(mat_file)<<8|
		(mat_uint)fgetc(mat_file)
	);
}

static mat_real mat_read_fixed(
	FILE *mat_file,
	mat_uint integer,
	mat_uint fraction
) {
	mat_sint value = 0;
	
	for (mat_uint i=0; i<integer+fraction; i++) {
		value = (value<<8)|(mat_sint)fgetc(mat_file);
	}
	
	return (mat_real)value/(1<<(fraction*8));
}

void mat_free(mat_list *list) {
	if (list->data==NULL) {
		return;
	}
	
	free(list->data);
	
	list->type = MAT_ATTRIBUTE_NONE;
	list->size = 0;
	list->data = NULL;
}

mat_uint mat_count_attributes( //Useful for allocating a static list
	char *filename,
	mat_uint attribute
) {
	FILE *mat_file = fopen(filename,"rb");
	
	if (mat_file==NULL) {
		return 0;
	}
	
	fseek(mat_file,0,SEEK_END);
	
	mat_uint mat_file_size = ftell(mat_file);
	
	fseek(mat_file,0,SEEK_SET);
	
	mat_uint entries = 0;
	
	while (
		!ferror(mat_file) & 
		((mat_uint)ftell(mat_file)<mat_file_size)
	) {
		mat_uint attribute_type  = (mat_uint)fgetc(mat_file);
		mat_uint attribute_size  = (mat_uint)fgetc(mat_file);
		mat_uint attribute_count = mat_read_uint(mat_file);
		
		if (attribute_type==attribute) {
			entries++;
		}
		
		fseek(
			mat_file,
			ftell(mat_file)+
			attribute_count*
			((attribute_size>>4)+
			(attribute_size&0x0F)),
			SEEK_SET
		);
	}
	
	fclose(mat_file);
	
	return entries;
}

void mat_read_attribute(
	mat_list *list,
	char *filename,
	mat_uint attribute,
	mat_uint id
) {
	list->type = MAT_ATTRIBUTE_NONE;
	list->size = 0;
	list->data = NULL;
	
	FILE *mat_file = fopen(filename,"rb");
	
	if (mat_file==NULL) {
		return;
	}
	
	fseek(mat_file,0,SEEK_END);
	
	mat_uint mat_file_size = ftell(mat_file);
	
	fseek(mat_file,0,SEEK_SET);

	mat_uint current_id = 0;
	
	while (
		!ferror(mat_file) & 
		((mat_uint)ftell(mat_file)<mat_file_size)
	) {
		mat_uint attribute_type  = (mat_uint)fgetc(mat_file);
		mat_uint attribute_size  = (mat_uint)fgetc(mat_file);
		mat_uint attribute_count = mat_read_uint(mat_file);
		
		if (attribute_type==attribute) {
			if (current_id==id) {
				list->data = calloc(
					attribute_count,
					sizeof(mat_real)
				);
				
				if (list->data==NULL) {
					return;
				}
				
				list->type = attribute;
				list->size = attribute_count;
				
				for (mat_uint i=0; i<attribute_count; i++) {
					list->data[i]=mat_read_fixed(
						mat_file,
						attribute_size>>4,
						attribute_size&0x0F
					);
				}
				
				fclose(mat_file);
				
				return;
			}
			
			current_id++;
		}
		
		fseek(
			mat_file,
			ftell(mat_file)+
			attribute_count*
			((attribute_size>>4)+
			(attribute_size&0x0F)),
			SEEK_SET
		);
	}
	
	fclose(mat_file);
	
	return;
}

void mat_write_attribute(
	mat_list *list,
	char *filename,
	mat_uint attribute
) {
	FILE *mat_file = fopen(filename,"ab");
	
	if (mat_file==NULL) {
		return;
	}
	
	//TODO
	
	fclose(mat_file);
}

#endif