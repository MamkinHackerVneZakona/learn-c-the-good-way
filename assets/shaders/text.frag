#version 330 core

in vec2 texCoordFrag;
uniform sampler2D tex;
uniform vec4 color;
out vec4 colorOut;

void main(void) {
    colorOut = vec4(1, 1, 1, texture(tex, texCoordFrag).r) * color;
}