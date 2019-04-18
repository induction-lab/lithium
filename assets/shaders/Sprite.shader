#ifdef VERTEX
attribute vec4 a_Position;
attribute vec2 a_Texture;
uniform mat4 u_Projection;
varying vec2 v_Texture;
void main() {
   v_Texture = a_Texture;
   gl_Position =  u_Projection * a_Position;
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
