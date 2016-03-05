varying vec3 norm;
varying vec3 cam_dir;
varying vec3 color;

// Declare any additional variables here. You may need some uniform
// variables.

uniform samplerCube cubemap;

void main(void)
{
	// Your shader code here.
	// Note that this shader won't compile since gl_FragColor is never set.
	vec3 incident = normalize(cam_dir);
	vec3 normal = normalize(norm);
	vec3 reflected = reflect(incident,normal);
        reflected = vec3(vec4(reflected,0.0));

	vec4 frag = textureCube(cubemap,reflected);

	gl_FragColor = frag;
}
