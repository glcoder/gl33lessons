#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "crc32.h"
#include "modcon.h"

typedef mesh_index_t  index_t;
typedef mesh_vertex_t vertex_t;

static const index_t INVALID_INDEX = (~((index_t)0));

typedef float float2[2];
typedef float float3[3];

#define sizeof_float2 (sizeof(float2))
#define sizeof_float3 (sizeof(float3))

typedef struct _face_t face_t;
typedef struct _node_t node_t;
typedef struct _list_t list_t;

struct _face_t
{
	index_t ip[4];
	index_t it[4];
	index_t in[4];
	index_t subset;

	uint32_t dim;
};

struct _node_t
{
	node_t   *prev, *next;
	uint32_t hash;
	index_t  index;
};

struct _list_t
{
	node_t   *head, *tail;
	uint32_t count;
};

static vertex_t *vertices;
static uint32_t vertices_count;

static index_t  *indices;
static uint32_t indices_count;

static float3   *positions;
static uint32_t positions_count;

static float2   *texcoords;
static uint32_t texcoords_count;

static float3   *normals;
static uint32_t normals_count;

static face_t   *faces;
static uint32_t faces_count;

static list_t vertices_list;

static mesh_head_t mesh_head;

uint32_t vertex_hash(face_t *face, uint32_t i);

void list_create(list_t *list);
void list_destroy(list_t *list);
index_t list_add(list_t *list, uint32_t hash);
index_t list_index(list_t *list, uint32_t hash);

void convert(const char *input_name, const char *output_name)
{
	assert(input_name);
	assert(output_name);

	face_t   *face, fakeface;
	FILE     *input, *output;
	char     line_read[0x200], *line;
	size_t   length;
	index_t  index, i, j, pi, ni, ti, fi, si;
	int      face_indices;

	vertices_count = indices_count = positions_count =
		normals_count = texcoords_count = faces_count = 0;

	list_create(&vertices_list);

	if ((input = fopen(input_name, "rt")) == NULL)
	{
		perror("Error opening input file");
		return;
	}

	if ((output = fopen(output_name, "wb")) == NULL)
	{
		perror("Error opening output file");
		return;
	}

	printf("%s analyze begin...\n", input_name);
	while (!feof(input))
	{
		if (fgets(line_read, 0x200, input) == NULL)
			break;

		line = line_read + strspn(line_read, " \t\n\r");

		if ((length = strlen(line)) < 3 || line[0] == '#')
			continue;

		if (line[0] == 'v')
		{
			if (line[1] == ' ')
				++positions_count;
			else if (line[1] == 't')
				++texcoords_count;
			else if (line[1] == 'n')
				++normals_count;
		} else if (line[0] == 'f' && line[1] == ' ')
		{
			face_indices = sscanf(line, "%*s %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
				fakeface.ip,     fakeface.it,     fakeface.in,
				fakeface.ip + 1, fakeface.it + 1, fakeface.in + 1,
				fakeface.ip + 2, fakeface.it + 2, fakeface.in + 2,
				fakeface.ip + 3, fakeface.it + 3, fakeface.in + 3);
			indices_count += face_indices - 6; // face_indices = {9,12}
			++faces_count;
		}
	}
	printf("%s analyze end: faces %d indices %d\n", input_name, faces_count, indices_count);

	positions = malloc(sizeof_float3 * positions_count);
	assert(positions);

	texcoords = malloc(sizeof_float2 * texcoords_count);
	assert(texcoords);

	normals = malloc(sizeof_float3 * normals_count);
	assert(normals);

	faces = malloc(sizeof(face_t) * faces_count);
	assert(faces);

	indices = malloc(sizeof(index_t) * indices_count);
	assert(indices);

	rewind(input);

	i = pi = ni = ti = fi = si = index = 0;

	printf("%s reading begin...\n", input_name);
	while (!feof(input))
	{
		if (fgets(line_read, 0x200, input) == NULL)
			break;

		line = line_read + strspn(line_read, " \t\n\r");

		if ((length = strlen(line)) < 3 || line[0] == '#')
			continue;

		if (line[0] == 'v')
		{
			if (line[1] == ' ')
			{
				sscanf(line, "%*s %f %f %f", positions[pi], positions[pi] + 1, positions[pi] + 2);
				++pi;
			} else if (line[1] == 'n')
			{
				sscanf(line, "%*s %f %f %f", normals[ni], normals[ni] + 1, normals[ni] + 2);
				++ni;
			} else if (line[1] == 't')
			{
				sscanf(line, "%*s %f %f %*f", texcoords[ti], texcoords[ti] + 1);
				++ti;
			}
		} else if (line[0] == 'f' && line[1] == ' ')
		{
			face_indices = sscanf(line, "%*s %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
				faces[fi].ip,     faces[fi].it,     faces[fi].in,
				faces[fi].ip + 1, faces[fi].it + 1, faces[fi].in + 1,
				faces[fi].ip + 2, faces[fi].it + 2, faces[fi].in + 2,
				faces[fi].ip + 3, faces[fi].it + 3, faces[fi].in + 3);

			faces[fi].subset = si;
			faces[fi].dim    = face_indices / 3; // face_indices = {9,12}

			indices[index++] = list_add(&vertices_list, vertex_hash(faces + fi, 0));
			indices[index++] = list_add(&vertices_list, vertex_hash(faces + fi, 1));
			indices[index++] = list_add(&vertices_list, vertex_hash(faces + fi, 2));

			if (faces[fi].dim == 4)
			{
				indices[index++] = list_add(&vertices_list, vertex_hash(faces + fi, 0));
				indices[index++] = list_add(&vertices_list, vertex_hash(faces + fi, 2));
				indices[index++] = list_add(&vertices_list, vertex_hash(faces + fi, 3));
			}

			++fi;
		} else if ((line[0] == 'g' || line[0] == 'o') && line[1] == ' ')
		{
			si++;
		}
	}

	fclose(input);

	vertices_count = vertices_list.count;
	vertices       = malloc(sizeof(vertex_t) * vertices_count);

	printf("%s reading end: vertices %d\n", input_name, vertices_count);

	assert(vertices);

	printf("%s preparing begin...\n", output_name);
	for (i = 0; i < fi; ++i)
	{
		face = faces + i;

		for (j = 0; j < face->dim; ++j)
		{
			index = list_index(&vertices_list, vertex_hash(face, j));
			assert(index != INVALID_INDEX);

			memcpy(vertices[index].position, positions[face->ip[j] - 1], sizeof_float3);
			memcpy(vertices[index].texcoord, texcoords[face->it[j] - 1], sizeof_float2);
			memcpy(vertices[index].normal,     normals[face->in[j] - 1], sizeof_float3);
		}
	}
	printf("%s preparing end\n", output_name);

	free(positions);
	free(texcoords);
	free(normals);
	free(faces);

	list_destroy(&vertices_list);

	mesh_head.vcount = vertices_count;
	mesh_head.icount = indices_count;

	printf("%s saving begin...\n", output_name);
	fwrite(&mesh_head, sizeof(mesh_head), 1, output);
	fwrite(vertices, sizeof(vertex_t), mesh_head.vcount, output);
	fwrite(indices, sizeof(index_t), mesh_head.icount, output);
	printf("%s saving end\n", output_name);

	free(vertices);
	free(indices);
	fclose(output);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		fprintf(stderr, "Usage: %s input output\n", argv[0]);
	else
		convert(argv[1], argv[2]);

	return 0;
}

uint32_t vertex_hash(face_t *face, uint32_t i)
{
	assert(face);
	assert(i < 4);

	uint32_t indices[4] = {face->ip[i], face->in[i], face->it[i], face->subset};
	return crc32_hash((const uint8_t *)indices, sizeof(indices));
}

void list_create(list_t *list)
{
	assert(list);

	list->head = list->tail = NULL;
	list->count = 0;
}

void list_destroy(list_t *list)
{
	assert(list);

	node_t *node, *node_next;

	for (node = list->head; node != NULL; node = node_next)
	{
		node_next = node->next;
		free(node);
	}
}

index_t list_add(list_t *list, uint32_t hash)
{
	assert(list);

	node_t *node;

	if (list->head == NULL)
	{
		node = list->head = malloc(sizeof(node_t));
		node->prev = NULL;
	} else
	{
		for (node = list->head; node != NULL; node = node->next)
			if (node->hash == hash)
				return node->index;

		node = list->tail->next = malloc(sizeof(node_t));
		node->prev = list->tail;
	}

	node->next  = NULL;
	node->hash  = hash;
	node->index = (index_t)list->count++;
	list->tail  = node;

	return node->index;
}

index_t list_index(list_t *list, uint32_t hash)
{
	assert(list);

	node_t *node;

	for (node = list->head; node != NULL; node = node->next)
		if (node->hash == hash)
			return node->index;

	return INVALID_INDEX;
}
