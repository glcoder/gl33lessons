#version 330 core

// матрица преобразования координат, получаемая из программы
uniform mat4 modelViewProjectionMatrix;

// входящие вершинные атрибуты
in vec3 position;
in vec2 texcoord;

// исходящие параметры, которые будут переданы в фрагментный шейдер
out vec2 fragTexcoord;

void main(void)
{
	// перевод позиции вершины из локальных координат в однородные
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);

	// передадим текстурные координаты в фрагментный шейдер
	fragTexcoord = texcoord;
}
