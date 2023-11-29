#version 330 core

in vec2 TexCoord;
out vec4 frag_color;

// Texture samplers
uniform sampler2D texSampler1;
//uniform sampler2D texSampler2;



void main()
{
    //Linearly interpolate between both textures (second texture is only slightly combined)
    frag_color = texture(texSampler1, TexCoord);

//    color = mix(texture(texSampler1, TexCoord), texture(texSampler2, TexCoord), 0.4);
    
//    vec4 brickTexColor = texture(texSampler1, TexCoord);
//    vec4 mossTexColor = texture(texSampler2, TexCoord);
//    color2 = mix(brickTexColor.rgb, mossTexColor.rgb, mossTexColor.a);
    
//    color = texture(texSampler1, TexCoord) * vec4(ourColor, 1.0f);
}


