#include <vector>
#include <string>
#include <list>
#include <map>

#include "objcon.h"
#include "crc32.h"

typedef float float2[2];
typedef float float3[3];

struct vec2f
{
	float2 v;
};

struct vec3f
{
	float3 v;
};

struct obj_face_t
{
	uint32_t ip[4];
	uint32_t it[4];
	uint32_t in[4];
	uint32_t subset;
	uint32_t vert; // 3 or 4
};

struct parse_data_t
{
	obj_t obj;

	std::vector<vec3f> positions;
	std::vector<vec3f> normals;
	std::vector<vec2f> texcoords;

	std::vector<uint32_t> indices;

	std::vector<obj_subset_t>   subsets;
	std::vector<obj_material_t> materials;

	std::list<std::string> mtllibs;

	std::vector<obj_face_t> faces;

	std::map<uint32_t, uint32_t>    vmap;
	std::map<std::string, uint32_t> mmap;
};

enum obj_chunk_t
{
	OC_UNKNOWN = 0,
	OC_POSITION,
	OC_TEXCOORD,
	OC_NORMAL,
	OC_FACE,
	OC_USEMTL,
	OC_GROUP,
	OC_OBJECT,
	OC_SMOOTH,
	OC_MTLLIB,
	OC_MATERIAL,
	OC_AMBIENT,
	OC_DIFFUSE,
	OC_SPECULAR,
	OC_SHININESS,
	OC_MAP,
	OC_MAX
};

static obj_chunk_t obj_get_chunk(const char *line)
{
	assert(line);

	obj_chunk_t chunk = OC_UNKNOWN;

	static const char obj_chunk_prefix[OC_MAX][2] = {
		/*[OC_UNKNOWN]   = */{' ', ' '},
		/*[OC_POSITION]  = */{'v', ' '},
		/*[OC_TEXCOORD]  = */{'v', 't'},
		/*[OC_NORMAL]    = */{'v', 'n'},
		/*[OC_FACE]      = */{'f', ' '},
		/*[OC_USEMTL]    = */{'u', 's'},
		/*[OC_GROUP]     = */{'g', ' '},
		/*[OC_OBJECT]    = */{'o', ' '},
		/*[OC_SMOOTH]    = */{'s', ' '},
		/*[OC_MTLLIB]    = */{'m', 't'},
		/*[OC_MATERIAL]  = */{'n', 'e'},
		/*[OC_AMBIENT]   = */{'K', 'a'},
		/*[OC_DIFFUSE]   = */{'K', 'd'},
		/*[OC_SPECULAR]  = */{'K', 's'},
		/*[OC_SHININESS] = */{'N', 's'},
		/*[OC_MAP]       = */{'m', 'a'}
	};

	for (uint32_t i = 0; i < OC_MAX; ++i)
	{
		if (line[0] == obj_chunk_prefix[i][0] && line[1] == obj_chunk_prefix[i][1])
		{
			chunk = (obj_chunk_t)i;
			break;
		}
	}

	return chunk;
}

static const char *obj_get_line(FILE *file, char *buffer, size_t size)
{
	assert(file);
	assert(buffer);

	if (fgets(buffer, size, file) == NULL)
		return NULL;

	buffer[strlen(buffer) - 1] = 0;
	const char *line = buffer + strspn(buffer, " \t\n\r");

	if (strlen(line) < 3 || line[0] == '#')
		return NULL;

	return line;
}

static uint32_t vertex_hash(obj_face_t *face, uint32_t i)
{
	assert(face);
	assert(i < face->vert);

	uint32_t indices[4] = {face->ip[i], face->in[i], face->it[i], face->subset};
	return crc32_hash((const uint8_t *)indices, sizeof(indices));
}

static uint32_t vertex_hash(std::vector<obj_face_t>::iterator face, uint32_t i)
{
	assert(i < face->vert);

	uint32_t indices[4] = {face->ip[i], face->in[i], face->it[i], face->subset};
	return crc32_hash((const uint8_t *)indices, sizeof(indices));
}

// create and initialize obj_t
static obj_t *obj_create()
{
	obj_t *obj = (obj_t *)malloc(sizeof(obj_t));
	assert(obj);

	memset(obj, 0, sizeof(obj_t));

	return obj;
}

obj_t *obj_parse_file(const char *name)
{
	assert(name);

	FILE *input;
	char line_read[0x200], buffer[0x200];
	const char *line;
	uint32_t i, fv, ioffset, index, mindex, hashes[6];
	vec2f v2;
	vec3f v3;

	parse_data_t   data;
	obj_face_t     face;
	obj_subset_t   subset;
	obj_chunk_t    chunk;
	obj_material_t material;

	if ((input = fopen(name, "rt")) == NULL)
		return NULL;

	index = ioffset = mindex = 0;

	subset.ioffset = 0;
	subset.icount  = 0;
	subset.mindex  = OBJ_INVALID_INDEX;

	printf("%s processing begin...\n", name);

	while (!feof(input))
	{
		if ((line = obj_get_line(input, line_read, 0x200)) == NULL)
			continue;

		chunk = obj_get_chunk(line);

		switch (chunk)
		{
			case OC_POSITION:
			{
				sscanf(line, "%*s %f %f %f", v3.v, v3.v + 1, v3.v + 2);
				data.positions.push_back(v3);
				break;
			}

			case OC_TEXCOORD:
			{
				sscanf(line, "%*s %f %f", v2.v, v2.v + 1);
				data.texcoords.push_back(v2);
				break;
			}

			case OC_NORMAL:
			{
				sscanf(line, "%*s %f %f %f", v3.v, v3.v + 1, v3.v + 2);
				data.normals.push_back(v3);
				break;
			}

			case OC_FACE:
			{
				fv = sscanf(line, "%*s %u/%u/%u %u/%u/%u %u/%u/%u %u/%u/%u",
					face.ip,     face.it,     face.in,
					face.ip + 1, face.it + 1, face.in + 1,
					face.ip + 2, face.it + 2, face.in + 2,
					face.ip + 3, face.it + 3, face.in + 3);

				assert(fv == 9 || fv == 12);

				face.subset = data.subsets.size();
				face.vert   = fv / 3;

				data.faces.push_back(face);

				hashes[0] = vertex_hash(&face, 0);
				hashes[1] = vertex_hash(&face, 1);
				hashes[2] = vertex_hash(&face, 2);

				if (face.vert == 4)
				{
					hashes[3] = vertex_hash(&face, 0);
					hashes[4] = vertex_hash(&face, 2);
					hashes[5] = vertex_hash(&face, 3);
				}

				for (i = 0; i < (fv - 6); ++i)
				{
					if (data.vmap.find(hashes[i]) == data.vmap.end())
						data.vmap[hashes[i]] = index++;
					data.indices.push_back(data.vmap[hashes[i]]);
					subset.icount++;
					ioffset++;
				}

				break;
			}

			case OC_GROUP: case OC_OBJECT:
			{
				if (!ioffset)
					break;

				data.subsets.push_back(subset);

				subset.ioffset = ioffset;
				subset.icount  = 0;
				subset.mindex  = OBJ_INVALID_INDEX;

				break;
			}

			case OC_USEMTL:
			{
				sscanf(line, "%*s %s", buffer);

				if (data.mmap.find(buffer) == data.mmap.end())
					data.mmap[buffer] = mindex++;

				subset.mindex = data.mmap[buffer];

				break;
			}

			case OC_MTLLIB:
			{
				sscanf(line, "%*s %s", buffer);
				data.mtllibs.push_back(buffer);
				break;
			}

			default:
			{
				//printf("  unknown chunk '%s'\n", line);
				break;
			}
		} // switch (chunk)
	} // while (!feof(input))

	printf("%s processing done\n", name);

	data.materials.resize(data.mmap.size());

	if (subset.icount)
		data.subsets.push_back(subset);

	printf("%s processing materials begin...\n", name);

	std::list<std::string>::iterator mli;
	for (mli = data.mtllibs.begin(); mli != data.mtllibs.end(); ++mli)
	{
		printf("  material lib '%s'\n", mli->c_str());

		FILE *mtllib = fopen(mli->c_str(), "rt");
		if (mtllib == NULL)
		{
			printf("  cant open file '%s'\n", mli->c_str());
			continue;
		}

		mindex = OBJ_INVALID_INDEX;
		memset(&material, 0, sizeof(obj_material_t));

		while (!feof(mtllib))
		{
			if ((line = obj_get_line(mtllib, line_read, 0x200)) == NULL)
				continue;

			chunk = obj_get_chunk(line);

			switch (chunk)
			{
				case OC_MATERIAL:
				{
					if (mindex != OBJ_INVALID_INDEX)
						data.materials[mindex] = material;

					sscanf(line, "%*s %s", buffer);

					if (data.mmap.find(buffer) != data.mmap.end())
					{
						mindex = data.mmap[buffer];
						memset(&material, 0, sizeof(obj_material_t));
					}
				}

				case OC_AMBIENT:
				{
					sscanf(line, "%*s %f %f %f", material.ambient,
						material.ambient + 1, material.ambient + 2);

					break;
				}

				case OC_DIFFUSE:
				{
					sscanf(line, "%*s %f %f %f", material.diffuse,
						material.diffuse + 1, material.diffuse + 2);

					break;
				}

				case OC_SPECULAR:
				{
					sscanf(line, "%*s %f %f %f", material.specular,
						material.specular + 1, material.specular + 2);

					break;
				}

				case OC_SHININESS:
				{
					sscanf(line, "%*s %f", &material.shininess);

					break;
				}

				case OC_MAP:
				{
					sscanf(line, "%*s %s", material.texture);

					break;
				}

				default:
				{
					//printf("  unknown chunk '%s'\n", line);
					break;
				}
			} // switch (chunk)
		} // while (!feof(mtllib))

		if (mindex != OBJ_INVALID_INDEX)
			data.materials[mindex] = material;
	}

	printf("%s processing materials done\n", name);

	// now we get all the data we need :)
	obj_t *obj = obj_create();

	if (!data.positions.empty())
	{
		obj->mesh.head.vtype |= OBJ_POSITION_3F;
		obj->mesh.head.vsize += sizeof(float3);
	}

	if (!data.texcoords.empty())
	{
		obj->mesh.head.vtype |= OBJ_TEXCOORD_2F;
		obj->mesh.head.vsize += sizeof(float2);
	}

	if (!data.normals.empty())
	{
		obj->mesh.head.vtype |= OBJ_NORMAL_3F;
		obj->mesh.head.vsize += sizeof(float3);
	}

	obj->mesh.head.vcount = data.vmap.size();
	obj->mesh.head.icount = data.indices.size();
	obj->mesh.head.scount = data.subsets.size();
	obj->mesh.head.mcount = data.materials.size();

	printf("%s dumping begin\n"
		"  vsize  = %u\n"
		"  vtype  = 0x%02X\n"
		"  vcount = %u\n"
		"  icount = %u\n"
		"  scount = %u\n"
		"  mcount = %u\n",
		name,
		obj->mesh.head.vsize,
		obj->mesh.head.vtype,
		obj->mesh.head.vcount,
		obj->mesh.head.icount,
		obj->mesh.head.scount,
		obj->mesh.head.mcount
	);

	printf("  dumping materials\n");
	obj->materials = new obj_material_t[obj->mesh.head.mcount];
	assert(obj->materials);

	for (i = 0; i < obj->mesh.head.mcount; ++i)
		obj->materials[i] = data.materials[i];

	printf("  dumping subsets\n");
	obj->mesh.subsets = new obj_subset_t[obj->mesh.head.scount];
	assert(obj->mesh.subsets);

	for (i = 0; i < obj->mesh.head.scount; ++i)
		obj->mesh.subsets[i] = data.subsets[i];

	printf("  dumping vertices\n");
	obj->mesh.vdata = new uint8_t[obj->mesh.head.vsize * obj->mesh.head.vcount];
	assert(obj->mesh.vdata);

	std::vector<obj_face_t>::iterator fi;
	for (fi = data.faces.begin(); fi != data.faces.end(); ++fi)
	{
		for (i = 0; i < fi->vert; ++i)
		{
			index = data.vmap[vertex_hash(fi, i)] * obj->mesh.head.vsize;

			if (obj->mesh.head.vtype & OBJ_POSITION_3F)
			{
				memcpy(obj->mesh.vdata + index, data.positions[fi->ip[i] - 1].v, sizeof(float3));
				index += sizeof(float3);
			}

			if (obj->mesh.head.vtype & OBJ_NORMAL_3F)
			{
				memcpy(obj->mesh.vdata + index, data.normals[fi->in[i] - 1].v, sizeof(float3));
				index += sizeof(float3);
			}

			if (obj->mesh.head.vtype & OBJ_TEXCOORD_2F)
			{
				memcpy(obj->mesh.vdata + index, data.texcoords[fi->it[i] - 1].v, sizeof(float2));
				index += sizeof(float2);
			}
		}
	}

	printf("  dumping indices\n");
	obj->mesh.idata = new uint32_t[obj->mesh.head.icount];
	assert(obj->mesh.idata);

	for (i = 0; i < obj->mesh.head.icount; ++i)
		obj->mesh.idata[i] = data.indices[i];

	printf("%s dumping done\n", name);

	return obj;
}

void obj_destroy(obj_t *obj)
{
	assert(obj);

	delete[] obj->mesh.subsets;
	delete[] obj->mesh.vdata;
	delete[] obj->mesh.idata;
	delete[] obj->materials;
	delete   obj;
}
