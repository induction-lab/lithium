#ifdef VERTEX
attribute vec4 aPosition;
attribute vec2 aTexture;
varying vec2 vTexture;
uniform mat4 uProjection;
void main() {
   vTexture = aTexture;
   gl_Position = uProjection * aPosition;
}
#endif
#ifdef FRAGMENT
precision mediump float;
uniform sampler2D uTexture;
uniform vec3 uColor;
uniform float uOpaque;
varying vec2 vTexture;
void main() {
    vec4 color = vec4(uColor.x, uColor.y, uColor.z, uOpaque);
    gl_FragColor = texture2D(uTexture, vTexture) * color;
}
#endif
