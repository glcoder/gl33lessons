#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

#include "common.h"
#include "OpenGL.h"
#include "GLWindow.h"
#include "Shader.h"
#include "Texture.h"

// данные матрицы
typedef float Matrix4[16];

// пременные для хранения идентификаторов шейдерной программы и текстуры
static GLuint shaderProgram = 0, colorTexture = 0;

// для хранения двух углов поворота куба
static float  cubeRotation[3] = {0.0f, 0.0f, 0.0f};

// матрицы преобразования
static Matrix4 modelViewProjectionMatrix = {0.0f}, viewMatrix = {0.0f},
               projectionMatrix = {0.0f}, viewProjectionMatrix = {0.0f};

// индексы полученный из шейдерной программы
static GLint colorTextureLocation = -1, modelViewProjectionMatrixLocation = -1,
             positionLocation = -1, texcoordLocation = -1;

// для хранения VAO и VBO связанных с кубом
static GLuint cubeVBO[3], cubeVAO;

// количество вершин куба
static const uint32_t cubeVerticesCount = 24;
// количество индексов куба
static const uint32_t cubeIndicesCount  = 36;

// координаты вершин куба
static const float s = 1.0f; // половина размера куба
static const float cubePositions[cubeVerticesCount][3] = {
	{-s, s, s}, { s, s, s}, { s,-s, s}, {-s,-s, s}, // front
	{ s, s,-s}, {-s, s,-s}, {-s,-s,-s}, { s,-s,-s}, // back
	{-s, s,-s}, { s, s,-s}, { s, s, s}, {-s, s, s}, // top
	{ s,-s,-s}, {-s,-s,-s}, {-s,-s, s}, { s,-s, s}, // bottom
	{-s, s,-s}, {-s, s, s}, {-s,-s, s}, {-s,-s,-s}, // left
	{ s, s, s}, { s, s,-s}, { s,-s,-s}, { s,-s, s}  // right
};

// текстурные координаты куба
static const float cubeTexcoords[cubeVerticesCount][2] = {
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // front
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // back
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // top
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // bottom
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}, // left
	{0.0f,1.0f}, {1.0f,1.0f}, {1.0f,0.0f}, {0.0f,0.0f}  // right
};

// индексы вершин куба в порядке поротив часовой стрелки
static const uint32_t cubeIndices[cubeIndicesCount] = {
	 0, 3, 1,  1, 3, 2, // front
	 4, 7, 5,  5, 7, 6, // back
	 8,11, 9,  9,11,10, // top
	12,15,13, 13,15,14, // bottom
	16,19,17, 17,19,18, // left
	20,23,21, 21,23,22  // right
};

// построение перспективной матрицы
static void Matrix4Perspective(Matrix4 M, float fovy, float aspect, float znear, float zfar)
{
	float f = 1 / tanf(fovy / 2),
	      A = (zfar + znear) / (znear - zfar),
	      B = (2 * zfar * znear) / (znear - zfar);

	M[ 0] = f / aspect; M[ 1] =  0; M[ 2] =  0; M[ 3] =  0;
	M[ 4] = 0;          M[ 5] =  f; M[ 6] =  0; M[ 7] =  0;
	M[ 8] = 0;          M[ 9] =  0; M[10] =  A; M[11] =  B;
	M[12] = 0;          M[13] =  0; M[14] = -1; M[15] =  0;
}

// построение матрицы переноса
static void Matrix4Translation(Matrix4 M, float x, float y, float z)
{
	M[ 0] = 1; M[ 1] = 0; M[ 2] = 0; M[ 3] = x;
	M[ 4] = 0; M[ 5] = 1; M[ 6] = 0; M[ 7] = y;
	M[ 8] = 0; M[ 9] = 0; M[10] = 1; M[11] = z;
	M[12] = 0; M[13] = 0; M[14] = 0; M[15] = 1;
}

// построение матрицы вращения
static void Matrix4Rotation(Matrix4 M, float x, float y, float z)
{
	const float A = cosf(x), B = sinf(x), C = cosf(y),
	            D = sinf(y), E = cosf(z), F = sinf(z);
	const float AD = A * D, BD = B * D;

	M[ 0] = C * E;           M[ 1] = -C * F;          M[ 2] = D;      M[ 3] = 0;
	M[ 4] = BD * E + A * F;  M[ 5] = -BD * F + A * E; M[ 6] = -B * C; M[ 7] = 0;
	M[ 8] = -AD * E + B * F; M[ 9] = AD * F + B * E;  M[10] = A * C;  M[11] = 0;
	M[12] = 0;               M[13] = 0;               M[14] = 0;      M[15] = 1;
}

// перемножение двух матриц
static void Matrix4Mul(Matrix4 M, Matrix4 A, Matrix4 B)
{
	M[ 0] = A[ 0] * B[ 0] + A[ 1] * B[ 4] + A[ 2] * B[ 8] + A[ 3] * B[12];
	M[ 1] = A[ 0] * B[ 1] + A[ 1] * B[ 5] + A[ 2] * B[ 9] + A[ 3] * B[13];
	M[ 2] = A[ 0] * B[ 2] + A[ 1] * B[ 6] + A[ 2] * B[10] + A[ 3] * B[14];
	M[ 3] = A[ 0] * B[ 3] + A[ 1] * B[ 7] + A[ 2] * B[11] + A[ 3] * B[15];
	M[ 4] = A[ 4] * B[ 0] + A[ 5] * B[ 4] + A[ 6] * B[ 8] + A[ 7] * B[12];
	M[ 5] = A[ 4] * B[ 1] + A[ 5] * B[ 5] + A[ 6] * B[ 9] + A[ 7] * B[13];
	M[ 6] = A[ 4] * B[ 2] + A[ 5] * B[ 6] + A[ 6] * B[10] + A[ 7] * B[14];
	M[ 7] = A[ 4] * B[ 3] + A[ 5] * B[ 7] + A[ 6] * B[11] + A[ 7] * B[15];
	M[ 8] = A[ 8] * B[ 0] + A[ 9] * B[ 4] + A[10] * B[ 8] + A[11] * B[12];
	M[ 9] = A[ 8] * B[ 1] + A[ 9] * B[ 5] + A[10] * B[ 9] + A[11] * B[13];
	M[10] = A[ 8] * B[ 2] + A[ 9] * B[ 6] + A[10] * B[10] + A[11] * B[14];
	M[11] = A[ 8] * B[ 3] + A[ 9] * B[ 7] + A[10] * B[11] + A[11] * B[15];
	M[12] = A[12] * B[ 0] + A[13] * B[ 4] + A[14] * B[ 8] + A[15] * B[12];
	M[13] = A[12] * B[ 1] + A[13] * B[ 5] + A[14] * B[ 9] + A[15] * B[13];
	M[14] = A[12] * B[ 2] + A[13] * B[ 6] + A[14] * B[10] + A[15] * B[14];
	M[15] = A[12] * B[ 3] + A[13] * B[ 7] + A[14] * B[11] + A[15] * B[15];
}

// инициализаця OpenGL
bool GLWindowInit(const GLWindow *window)
{
	ASSERT(window);

	// устанавливаем вьюпорт на все окно
	glViewport(0, 0, window->width, window->height);

	// параметры OpenGL
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// создадим и загрузим текстуру
	colorTexture = TextureCreateFromTGA("data/texture.tga");

	if (!colorTexture)
		return false;

	// делаем активным текстурный юнит 0 и назначаем на него текстуру
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorTexture);

	// создадим и загрузим шейдерную программу
	shaderProgram = ShaderProgramCreateFromFile("data/lesson", ST_VERTEX | ST_FRAGMENT);

	if (!shaderProgram)
		return false;

	// собираем созданную и загруженную шейдерную программу
	if (!ShaderProgramLink(shaderProgram))
		return false;

	// сделаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// создадим перспективную матрицу
	const float aspectRatio = (float)window->width / (float)window->height;
	Matrix4Perspective(projectionMatrix, 45.0f, aspectRatio, 1.0f, 10.0f);

	// с помощью видовой матрицы отодвинем сцену назад
	Matrix4Translation(viewMatrix, 0.0f, 0.0f, -4.0f);

	Matrix4Mul(viewProjectionMatrix, projectionMatrix, viewMatrix);

	// получим индекс матрицы из шейдерной программы
	modelViewProjectionMatrixLocation = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");

	// получим индекс текстурного самплера из шейдерной программы и свяжем его с текстурным юнитом 0
	if ((colorTextureLocation = glGetUniformLocation(shaderProgram, "colorTexture")) != -1)
		glUniform1i(colorTextureLocation, 0);

	// проверка шейдерной программы на корректность
	if (!ShaderProgramValidate(shaderProgram))
		return false;

	// запросим у OpenGL свободный индекс VAO
	glGenVertexArrays(1, &cubeVAO);

	// сделаем VAO активным
	glBindVertexArray(cubeVAO);

	// создадим 3 VBO для данных куба - координаты вершин, текстурные координат и индексный буфер
	glGenBuffers(3, cubeVBO);

	// получим индекс вершинного атрибута 'position' из шейдерной программы
	positionLocation = glGetAttribLocation(shaderProgram, "position");
	if (positionLocation != -1)
	{
		// начинаем работу с буфером для координат вершин куба
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[0]);
		// поместим в буфер координаты вершин куба
		glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (3 * sizeof(float)),
			cubePositions, GL_STATIC_DRAW);

		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(positionLocation);
	}

	// получим индекс вершинного атрибута 'texcoord' из шейдерной программы
	texcoordLocation = glGetAttribLocation(shaderProgram, "texcoord");
	if (texcoordLocation != -1)
	{
		// начинаем работу с буфером для текстурных координат куба
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO[1]);
		// поместим в буфер текстурные координаты куба
		glBufferData(GL_ARRAY_BUFFER, cubeVerticesCount * (2 * sizeof(float)),
			cubeTexcoords, GL_STATIC_DRAW);

		// укажем параметры вершинного атрибута для текущего активного VBO
		glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		// разрешим использование вершинного атрибута
		glEnableVertexAttribArray(texcoordLocation);
	}

	// начинаем работу с индексным буфером
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeVBO[2]);
	// поместим в буфер индексы вершин куба
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndicesCount * sizeof(uint32_t),
		cubeIndices, GL_STATIC_DRAW);

	// проверим не было ли ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return true;
}

// очистка OpenGL
void GLWindowClear(const GLWindow *window)
{
	ASSERT(window);

	// делаем текущие VBO неактивными
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// удаляем VBO
	glDeleteBuffers(3, cubeVBO);

	// далаем текущий VAO неактивным
	glBindVertexArray(0);
	// удаляем VAO
	glDeleteVertexArrays(1, &cubeVAO);

	// удаляем шейдерную программу
	ShaderProgramDestroy(shaderProgram);

	// удаляем текстуру
	TextureDestroy(colorTexture);
}

// функция рендера
void GLWindowRender(const GLWindow *window)
{
	ASSERT(window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// делаем шейдерную программу активной
	ShaderProgramBind(shaderProgram);

	// передаем в шейдер матрицу преобразования координат вершин
	if (modelViewProjectionMatrixLocation != -1)
		glUniformMatrix4fv(modelViewProjectionMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix);

	// выводим на экран все что относится к VAO
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, cubeIndicesCount, GL_UNSIGNED_INT, NULL);

	// проверка на ошибки
	OPENGL_CHECK_FOR_ERRORS();
}

// функция обновления
void GLWindowUpdate(const GLWindow *window, double deltaTime)
{
	ASSERT(window);
	ASSERT(deltaTime >= 0.0); // проверка на возможность бага

	// матрица вращения куба
	Matrix4 modelMatrix;

	// зададим углы поворота куба с учетом времени
	if ((cubeRotation[0] += 0.3f * (float)deltaTime) > 360.0f)
		cubeRotation[0] -= 360.0f;

	if ((cubeRotation[1] += 1.5f * (float)deltaTime) > 360.0f)
		cubeRotation[1] -= 360.0f;

	if ((cubeRotation[2] += 0.7f * (float)deltaTime) > 360.0f)
		cubeRotation[2] -= 360.0f;

	// рассчитаем матрицу преобразования координат вершин куба
	Matrix4Rotation(modelMatrix, cubeRotation[0], cubeRotation[1], cubeRotation[2]);
	Matrix4Mul(modelViewProjectionMatrix, viewProjectionMatrix, modelMatrix);
}

// функция обработки ввода с клавиатуры и мыши
void GLWindowInput(const GLWindow *window)
{
	ASSERT(window);

	// выход из приложения по кнопке Esc
	if (InputIsKeyPressed(VK_ESCAPE))
		GLWindowDestroy();

	// переключение между оконным и полноэкранным режимом
	// осуществляется по нажатию комбинации Alt+Enter
	if (InputIsKeyDown(VK_MENU) && InputIsKeyPressed(VK_RETURN))
		GLWindowSetSize(window->width, window->height, !window->fullScreen);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int result;

	LoggerCreate("lesson03.log");

	if (!GLWindowCreate("lesson03", 800, 600, false))
		return 1;

	result = GLWindowMainLoop();

	GLWindowDestroy();
	LoggerDestroy();

	return result;
}
