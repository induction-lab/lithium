#ifdef VERTEX
attribute vec2 aPosition;
uniform mat4 uProjection;
void main() {
   gl_Position = uProjection * vec4(aPosition.x, aPosition.y, 1.0, 1.0);
}
#endif
#ifdef FRAGMENT
uniform vec3 uColor;
uniform float uOpaque;
void main() {
    gl_FragColor = vec4(uColor.x, uColor.y, uColor.z, uOpaque);
}
#endif
