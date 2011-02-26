#ifndef OBJTYPES_H
#define OBJTYPES_H

// vertex type
#define OBJ_POSITION_3F 0x01
#define OBJ_NORMAL_3F   0x02
#define OBJ_TEXCOORD_2F 0x04
#define OBJ_P3F_N3F_T2F (OBJ_POSITION_3F | OBJ_NORMAL_3F | OBJ_TEXCOORD_2F)

static const uint32_t OBJ_INVALID_INDEX = 0xFFFFFFFFU;

typedef struct
{
	float ambient[3];
	float diffuse[3];
	float specular[3];
	float shininess;
	char  texture[64];
} obj_material_t;

typedef struct
{
	uint32_t vtype;  // vertex type
	uint32_t vsize;  // vertex size
	uint32_t vcount; // vertices
	uint32_t icount; // indices
	uint32_t scount; // subsets
	uint32_t mcount; // materials
} obj_mesh_head_t;

typedef struct
{
	uint32_t ioffset; // index offset
	uint32_t icount;  // index count
	uint32_t mindex;  // material index
} obj_subset_t;

typedef struct
{
	obj_mesh_head_t head;     // mesh head
	obj_subset_t    *subsets; // mesh subsets
	uint8_t         *vdata;   // vertex data
	uint32_t        *idata;   // index data
} obj_mesh_t;

typedef struct
{
	obj_mesh_t     mesh;       // geometry data
	obj_material_t *materials; // material data
} obj_t;

#endif /* OBJTYPES_H */
