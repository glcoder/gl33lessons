const uint32_t vcount = 24;
const uint32_t icount = 36;

// вершины
const vertex vertices[vcount] = {
	// front
	{{-size, size, size}, {0.0f,1.0f}, { 0.0f, 0.0f, 1.0f}},
	{{ size, size, size}, {1.0f,1.0f}, { 0.0f, 0.0f, 1.0f}},
	{{ size,-size, size}, {1.0f,0.0f}, { 0.0f, 0.0f, 1.0f}},
	{{-size,-size, size}, {0.0f,0.0f}, { 0.0f, 0.0f, 1.0f}},
	// back
	{{ size, size,-size}, {0.0f,1.0f}, { 0.0f, 0.0f,-1.0f}},
	{{-size, size,-size}, {1.0f,1.0f}, { 0.0f, 0.0f,-1.0f}},
	{{-size,-size,-size}, {1.0f,0.0f}, { 0.0f, 0.0f,-1.0f}},
	{{ size,-size,-size}, {0.0f,0.0f}, { 0.0f, 0.0f,-1.0f}},
	// top
	{{-size, size,-size}, {0.0f,1.0f}, { 0.0f, 1.0f, 0.0f}},
	{{ size, size,-size}, {1.0f,1.0f}, { 0.0f, 1.0f, 0.0f}},
	{{ size, size, size}, {1.0f,0.0f}, { 0.0f, 1.0f, 0.0f}},
	{{-size, size, size}, {0.0f,0.0f}, { 0.0f, 1.0f, 0.0f}},
	// bottom
	{{ size,-size,-size}, {0.0f,1.0f}, { 0.0f,-1.0f, 0.0f}},
	{{-size,-size,-size}, {1.0f,1.0f}, { 0.0f,-1.0f, 0.0f}},
	{{-size,-size, size}, {1.0f,0.0f}, { 0.0f,-1.0f, 0.0f}},
	{{ size,-size, size}, {0.0f,0.0f}, { 0.0f,-1.0f, 0.0f}},
	// left
	{{-size, size,-size}, {0.0f,1.0f}, {-1.0f, 0.0f, 0.0f}},
	{{-size, size, size}, {1.0f,1.0f}, {-1.0f, 0.0f, 0.0f}},
	{{-size,-size, size}, {1.0f,0.0f}, {-1.0f, 0.0f, 0.0f}},
	{{-size,-size,-size}, {0.0f,0.0f}, {-1.0f, 0.0f, 0.0f}},
	// right
	{{ size, size, size}, {0.0f,1.0f}, { 1.0f, 0.0f, 0.0f}},
	{{ size, size,-size}, {1.0f,1.0f}, { 1.0f, 0.0f, 0.0f}},
	{{ size,-size,-size}, {1.0f,0.0f}, { 1.0f, 0.0f, 0.0f}},
	{{ size,-size, size}, {0.0f,0.0f}, { 1.0f, 0.0f, 0.0f}}
};

// индексы
const uint32_t indices[icount] = {
	 0, 3, 1,  1, 3, 2, // front
	 4, 7, 5,  5, 7, 6, // back
	 8,11, 9,  9,11,10, // top
	12,15,13, 13,15,14, // bottom
	16,19,17, 17,19,18, // left
	20,23,21, 21,23,22  // right
};
