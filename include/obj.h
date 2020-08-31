#ifndef _OBJ_IMPORT_H_
#define _OBJ_IMPORT_H_

#include <stdio.h>

typedef enum __obj_entry_type_t__
{
	OBJ_ENTRY_TYPE_END,
	OBJ_ENTRY_TYPE_VERTEX,
	OBJ_ENTRY_TYPE_TEX_COORDS,
	OBJ_ENTRY_TYPE_NORMAL,
	OBJ_ENTRY_TYPE_FACE,
	OBJ_ENTRY_TYPE_MTL_IMPORT,
	OBJ_ENTRY_TYPE_MTL_USE
} obj_entry_type_t;

// An obj vertex entry:
typedef struct __obj_vertex_entry_t__
{
	double x;
	double y;
	double z;

	// Negative if not present:
	double w;
} obj_vertex_entry_t;

// An obj texture coordinates entry:
typedef struct __obj_tex_coords_entry_t__
{
	double u;
	double v;
} obj_tex_coords_entry_t;

// An obj normal entry:
typedef struct __obj_normal_entry_t__
{
	double x;
	double y;
	double z;
} obj_normal_entry_t;

// An obj face index triple:
typedef struct __obj_face_index_triple_t__
{
	// Missing indices are negative.
	int vertex_index;
	int tex_coords_index;
	int normal_index;
} obj_face_index_triple_t;

// An obj face entry:
typedef struct __obj_face_entry_t__
{
	obj_face_index_triple_t triples[3];
} obj_face_entry_t;

// An obj mtl entry:
typedef struct __obj_mtl_entry_t__
{
	char mtl_name[2048];
} obj_mtl_entry_t;

// An obj entry:
typedef union _obj_entry_t_
{
	obj_vertex_entry_t vertex_entry;
	obj_tex_coords_entry_t tex_coords_entry;
	obj_normal_entry_t normal_entry;
	obj_face_entry_t face_entry;
	obj_mtl_entry_t mtl_entry;
} obj_entry_t;

// Scan through an open obj file and count the different entries:
void obj_count_entries(FILE* obj_file, int* vertex_count, int* tex_coords_count, int* normal_count, int* face_count, int* mtl_lib_count);

// Parse one line of obj content:
obj_entry_type_t obj_get_next_entry(FILE* obj_file, obj_entry_t* entry);

#endif
