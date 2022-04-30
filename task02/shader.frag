#version 120

varying vec3 normal; // normal interpolated using baricentric coordinate

void main()
{
  // draw normal map
  gl_FragColor = vec4(0.5*normal+0.5,1);
}
