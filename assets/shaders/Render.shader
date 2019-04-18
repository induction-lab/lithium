#ifdef VERTEX
attribute vec2 aPosition;
attribute vec2 aTexture;
varying vec2 vTexture;
void main() {
    vTexture = aTexture;
    gl_Position = vec4(aPosition, 1.0, 1.0);
}
#endif
#ifdef FRAGMENT
precision mediump float;
uniform sampler2D uTexture;
varying vec2 vTexture;
void main() {
    gl_FragColor = texture2D(uTexture, vTexture);
}
#endif
