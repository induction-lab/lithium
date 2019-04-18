#ifdef VERTEX
attribute vec2 a_Position;
attribute vec2 a_Texture;
varying vec2 v_Texture;
void main() {
    v_Texture = a_Texture;
    gl_Position = vec4(a_Position, 1.0, 1.0);
}
#endif
#ifdef FRAGMENT
precision mediump float;
uniform sampler2D u_Texture;
varying vec2 v_Texture;
void main() {
    gl_FragColor = texture2D(u_Texture, v_Texture);
}
#endif
