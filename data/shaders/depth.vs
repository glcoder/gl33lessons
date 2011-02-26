layout(location = VERT_POSITION) in vec3 position;

uniform struct Camera
{
	mat4 modelViewProjection;
} camera;

void main(void)
{
	gl_Position = camera.modelViewProjection * vec4(position, 1.0);
}
