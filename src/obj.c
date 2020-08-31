#include "obj.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define OBJ_MAX_LINE_LENGTH 4096

// Internal: Does a string start with a given prefix?
static bool obj_starts_with(const char* str, const char* prefix)
{
	size_t str_len = strlen(str);
	size_t prefix_len = strlen(prefix);

	return (str_len >= prefix_len) && (strncmp(str, prefix, prefix_len) == 0);
}

// Internal: Skip spaces at the beginning of a line:
static char* obj_skip_spaces(char* str)
{
	while (*str == ' ')
	{
		str++;
	}

	return str;
}

void obj_count_entries(FILE* obj_file, int* vertex_count, int* tex_coords_count, int* normal_count, int* face_count, int* mtl_lib_count)
{
	// Set the counters to zero:
	*vertex_count = 0;
	*tex_coords_count = 0;
	*normal_count = 0;
	*face_count = 0;
	*mtl_lib_count = 0;

	// Read lines into this array:
	char line[OBJ_MAX_LINE_LENGTH];

	// Read line by line:
	while (fgets(line, OBJ_MAX_LINE_LENGTH, obj_file))
	{
		// Skip potential spaces:
		char* curr_line = obj_skip_spaces(line);

		if (obj_starts_with(curr_line, "v "))
		{
			(*vertex_count)++;
		}
		else if (obj_starts_with(curr_line, "vt "))
		{
			(*tex_coords_count)++;
		}
		else if (obj_starts_with(curr_line, "vn "))
		{
			(*normal_count)++;
		}
		else if (obj_starts_with(curr_line, "f "))
		{
			(*face_count)++;
		}
		else if (obj_starts_with(curr_line, "mtllib "))
		{
			(*mtl_lib_count)++;
		}
	}
}

obj_entry_type_t obj_get_next_entry(FILE* obj_file, obj_entry_t* entry)
{
	// Track the indices of the next elements we receive:
	int next_vertex_index = 0;
	int next_tex_coords_index = 0;
	int next_normal_index = 0;

	// Read lines into this array:
	char line[OBJ_MAX_LINE_LENGTH];

	// Read line by line:
	while (fgets(line, OBJ_MAX_LINE_LENGTH, obj_file))
	{
		// Skip potential spaces:
		char* curr_line = obj_skip_spaces(line);

		if (obj_starts_with(curr_line, "v "))
		{
			curr_line++;

			// Parse the vertex:
			entry->vertex_entry.x = strtod(curr_line, &curr_line);
			entry->vertex_entry.y = strtod(curr_line, &curr_line);
			entry->vertex_entry.z = strtod(curr_line, &curr_line);
			// TODO:
			entry->vertex_entry.w = -1;

			next_vertex_index++;

			return OBJ_ENTRY_TYPE_VERTEX;
		}
		else if (obj_starts_with(curr_line, "vt"))
		{
			curr_line += 2;

			// Parse the vertex:
			entry->tex_coords_entry.u = strtod(curr_line, &curr_line);
			entry->tex_coords_entry.v = strtod(curr_line, &curr_line);

			next_tex_coords_index++;

			return OBJ_ENTRY_TYPE_TEX_COORDS;
		}
		else if (obj_starts_with(curr_line, "vn "))
		{
			curr_line += 2;

			// Parse the vertex:
			entry->normal_entry.x = strtod(curr_line, &curr_line);
			entry->normal_entry.y = strtod(curr_line, &curr_line);
			entry->normal_entry.z = strtod(curr_line, &curr_line);

			next_normal_index++;

			return OBJ_ENTRY_TYPE_NORMAL;
		}
		else if (obj_starts_with(curr_line, "f "))
		{
			curr_line++;

			// TODO: Quads.
			for (int i = 0; i < 3; i++)
			{
				entry->face_entry.triples[i].tex_coords_index = -1;
				entry->face_entry.triples[i].normal_index = -1;

				// Vertex index:
				entry->face_entry.triples[i].vertex_index = strtol(curr_line, &curr_line, 10) - 1;

				if (entry->face_entry.triples[i].vertex_index < 0)
				{
					entry->face_entry.triples[i].vertex_index += next_vertex_index + 1;
				}

				curr_line = obj_skip_spaces(curr_line);

				if (*curr_line != '/')
				{
					continue;
				}

				curr_line = obj_skip_spaces(curr_line + 1);

				// Texture coordinates index:
				if (*curr_line != '/')
				{
					entry->face_entry.triples[i].tex_coords_index = strtol(curr_line, &curr_line, 10) - 1;

					if (entry->face_entry.triples[i].tex_coords_index < 0)
					{
						entry->face_entry.triples[i].tex_coords_index += next_tex_coords_index + 1;
					}

					curr_line = obj_skip_spaces(curr_line);
				}

				curr_line = obj_skip_spaces(curr_line + 1);

				// Normal index:
				entry->face_entry.triples[i].normal_index = strtol(curr_line, &curr_line, 10) - 1;

				if (entry->face_entry.triples[i].normal_index < 0)
				{
					entry->face_entry.triples[i].normal_index += next_normal_index + 1;
				}
			}

			return OBJ_ENTRY_TYPE_FACE;
		}
	}

	// No line left.
	return OBJ_ENTRY_TYPE_END;
}
