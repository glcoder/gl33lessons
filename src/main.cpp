#include "objcon.h"

struct vertex_t
{
	float p[3];
	float n[3];
	float t[2];
};

struct head_t
{
	uint32_t vcount;
	uint32_t icount;
};

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: objcon.exe <obj> <out>\n");
		return 1;
	}

	obj_t *obj = obj_parse_file(argv[1]);

	if (obj == NULL)
		return 2;

	head_t head;

	head.vcount = obj->mesh.head.vcount;
	head.icount = obj->mesh.head.icount;

	assert(obj->mesh.head.vsize == 32);
	assert(obj->mesh.head.vtype == OBJ_P3F_N3F_T2F);

	FILE *output = fopen(argv[2], "wb");
	if (output == NULL)
	{
		fprintf(stderr, "Cant open output file %s\n", argv[2]);
		obj_destroy(obj);
		return 3;
	}

	printf("%s saving begin...\n", argv[2]);

	fwrite(&head, sizeof(head_t), 1, output);
	fwrite(obj->mesh.vdata, sizeof(vertex_t), head.vcount, output);
	fwrite(obj->mesh.idata, sizeof(uint32_t), head.icount, output);

	printf("%s saving end\n", argv[2]);

	obj_destroy(obj);
	return 0;
}
