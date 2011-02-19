const uint32_t vcount = 4;
const uint32_t icount = 6;

const vertex vertices[vcount] = {
	{{-size, 0,-size}, {0.0f,1.0f}, { 0.0f, 1.0f, 0.0f}},
	{{ size, 0,-size}, {1.0f,1.0f}, { 0.0f, 1.0f, 0.0f}},
	{{ size, 0, size}, {1.0f,0.0f}, { 0.0f, 1.0f, 0.0f}},
	{{-size, 0, size}, {0.0f,0.0f}, { 0.0f, 1.0f, 0.0f}}
};

const uint32_t indices[icount] = {
	 0, 3, 1,  1, 3, 2
};
