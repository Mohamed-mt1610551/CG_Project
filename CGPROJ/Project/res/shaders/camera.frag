#version 330 core

in vec2 TexCoord;
out vec4 frag_color;
out vec3 frag_color2;
uniform vec4 vertColor;

// Texture samplers
uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main()
{
    //Linearly interpolate between both textures (second texture is only slightly combined)
//    frag_color = texture(texSampler1, TexCoord);

//    frag_color = mix(textcure(texSampler1, TexCoord), texture(texSampler2, TexCoord), 0.7);
    
    vec4 brickTexColor = texture(texSampler1, TexCoord);
    vec4 mossTexColor = texture(texSampler2, TexCoord);
    frag_color2 = mix(brickTexColor.rgb, mossTexColor.rgb, mossTexColor.a);
    
//      frag_color = texture(texSampler1, TexCoord) * vertColor;
}


