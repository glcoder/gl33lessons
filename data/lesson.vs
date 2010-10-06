// в момент загрузки шейдера будут добавлены следующие параметры:
// #version 330 core
// #define POSITION_LOCATION 0
// #define TEXCOORD_LOCATION 1

// матрица преобразования координат, получаемая из программы
uniform mat4 modelViewProjectionMatrix;

// входящие атрибуты вершины
layout(location = POSITION_LOCATION) in vec3 position;
layout(location = TEXCOORD_LOCATION) in vec2 texcoord;

// блок исходящих параметров, которые будут переданы в фрагментный шейдер
out Fragment {
	vec2 texcoord;
} Frag;

void main(void)
{
	// перевод позиции вершины из локальных координат в однородные
	gl_Position   = modelViewProjectionMatrix * vec4(position, 1.0);

	// передадим текстурные координаты в фрагментный шейдер
	Frag.texcoord = texcoord;
}
