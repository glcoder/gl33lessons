#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "crc32.h"

typedef uint32_t index_t;
typedef float    float2[2];
typedef float    float3[3];
typedef float    float4[4];

static const index_t INVALID_INDEX = (~((index_t)0));

struct face
{
	index_t  ip[3];
	index_t  it[3];
	index_t  in[3];
	uint32_t subset;
};

struct node
{
	struct node *prev, *next;
	uint32_t    hash;
	index_t     index;
};

struct list
{
	struct node *head, *tail;
	uint32_t    count;
};

struct vertex
{
	float3 position;
	float2 texcoord;
	float3 normal;
};

static struct vertex *vertices;
static uint32_t      vertices_count;

static index_t  *indices;
static uint32_t indices_count;

static float3   *positions;
static uint32_t positions_count;

static float2   *texcoords;
static uint32_t texcoords_count;

static float3   *normals;
static uint32_t normals_count;

static struct face *faces;
static uint32_t    faces_count;

static struct list vertices_list;

uint32_t vertex_hash(struct face *face, uint32_t i);

void list_create(struct list *list);
void list_destroy(struct list *list);
index_t list_add(struct list *list, uint32_t hash);
index_t list_index(struct list *list, uint32_t hash);

void convert(const char *input_name, const char *output_name)
{
	assert(input_name);
	assert(output_name);

	struct face *face;
	FILE        *input, *output;
	char        line_read[0x200], *line;
	size_t      length;
	uint32_t    i, j, pi, ni, ti, fi, si;
	index_t     index;

	vertices_count = indices_count = positions_count =
		normals_count = texcoords_count = faces_count = 0;

	list_create(&vertices_list);

	if ((input = fopen(input_name, "rt")) == NULL)
	{
		perror("Error opening input file");
		return;
	}

	if ((output = fopen(output_name, "wt")) == NULL)
	{
		perror("Error opening output file");
		return;
	}

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
			++faces_count;
		}
	}

	indices_count = faces_count * 3;

	positions = malloc(sizeof(float3) * positions_count);
	assert(positions);

	texcoords = malloc(sizeof(float2) * texcoords_count);
	assert(texcoords);

	normals = malloc(sizeof(float3) * normals_count);
	assert(normals);

	faces = malloc(sizeof(struct face) * faces_count);
	assert(faces);

	indices = malloc(sizeof(index_t) * indices_count);
	assert(indices);

	rewind(input);

	i = pi = ni = ti = fi = si = 0;

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
			sscanf(line, "%*s %u/%u/%u %u/%u/%u %u/%u/%u",
				faces[fi].ip,     faces[fi].it,     faces[fi].in,
				faces[fi].ip + 1, faces[fi].it + 1, faces[fi].in + 1,
				faces[fi].ip + 2, faces[fi].it + 2, faces[fi].in + 2);

			faces[fi].subset = si;

			i = fi * 3;
			indices[i + 0] = list_add(&vertices_list, vertex_hash(faces + fi, 0));
			indices[i + 1] = list_add(&vertices_list, vertex_hash(faces + fi, 1));
			indices[i + 2] = list_add(&vertices_list, vertex_hash(faces + fi, 2));

			++fi;
		} else if ((line[0] == 'g' || line[0] == 'o') && line[1] == ' ')
		{
			si++;
		}
	}

	fclose(input);

	vertices_count = vertices_list.count;
	vertices       = malloc(sizeof(struct vertex) * vertices_count);

	assert(vertices);

	for (i = 0; i < fi; ++i)
	{
		face = faces + i;

		for (j = 0; j < 3; ++j)
		{
			index = list_index(&vertices_list, vertex_hash(face, j));
			assert(index != INVALID_INDEX);

			memcpy(vertices[index].position, positions[face->ip[j] - 1], sizeof(float3));
			memcpy(vertices[index].texcoord, texcoords[face->it[j] - 1], sizeof(float2));
			memcpy(vertices[index].normal,     normals[face->in[j] - 1], sizeof(float3));
		}
	}

	free(positions);
	free(texcoords);
	free(normals);
	free(faces);

	list_destroy(&vertices_list);

	fprintf(output, "const uint32_t vcount = %u;\nconst uint32_t icount = %u;\n\n",
		vertices_count, indices_count);

	fprintf(output, "const vertex vertices[vcount] = {\n");

	for (i = 0; i < vertices_count; ++i)
	{
		fprintf(output, "\t{{%ff * size, %ff * size, %ff * size}, {%ff, %ff}, {%ff, %ff, %ff}},\n",
			vertices[i].position[0], vertices[i].position[1], vertices[i].position[2],
			vertices[i].texcoord[0], vertices[i].texcoord[1],
			vertices[i].normal[0], vertices[i].normal[1], vertices[i].normal[2]);
	}

	fprintf(output, "};\n\nconst uint32_t indices[icount] = {\n");

	for (i = 0; i < indices_count; i += 3)
	{
		fprintf(output, "\t%u, %u, %u,\n", indices[i], indices[i + 1], indices[i + 2]);
	}

	fprintf(output, "};\n");

	free(vertices);
	free(indices);
	fclose(output);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		fprintf(stderr, "Usage: %s input output\n", *argv);
	else
		convert(argv[1], argv[2]);

	return 0;
}

uint32_t vertex_hash(struct face *face, uint32_t i)
{
	assert(face);
	assert(i < 3);

	uint32_t indices[4] = {face->ip[i], face->in[i], face->it[i], face->subset};
	return crc32_hash((const uint8_t*)indices, sizeof(indices));
}

void list_create(struct list *list)
{
	assert(list);

	list->head = list->tail = NULL;
	list->count = 0;
}

void list_destroy(struct list *list)
{
	assert(list);

	struct node *node, *node_next;

	for (node = list->head; node != NULL; node = node_next)
	{
		node_next = node->next;
		free(node);
	}
}

index_t list_add(struct list *list, uint32_t hash)
{
	assert(list);

	struct node *node;

	if (list->head == NULL)
	{
		node = list->head = malloc(sizeof(struct node));
		node->prev = NULL;
	} else
	{
		for (node = list->head; node != NULL; node = node->next)
			if (node->hash == hash)
				return node->index;

		node = list->tail->next = malloc(sizeof(struct node));
		node->prev = list->tail;
	}

	node->next  = NULL;
	node->hash  = hash;
	node->index = (index_t)list->count++;
	list->tail  = node;

	return node->index;
}

index_t list_index(struct list *list, uint32_t hash)
{
	assert(list);

	struct node *node;

	for (node = list->head; node != NULL; node = node->next)
		if (node->hash == hash)
			return node->index;

	return INVALID_INDEX;
}
