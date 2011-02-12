#ifndef TYPES_H
#define TYPES_H

typedef uint32_t mesh_index_t;

typedef struct
{
	float position[3];
	float texcoord[2];
	float normal[3];
} mesh_vertex_t;

typedef struct
{
	uint32_t vcount;
	uint32_t icount;
} mesh_head_t;

#endif /* TYPES_H */
