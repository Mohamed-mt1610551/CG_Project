#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 frag_color;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

// Texture samplers
uniform sampler2D texSampler1;

void main()
{
    //ambient
    float ambientFactor = 0.2f;
    vec3 ambient = lightColor * ambientFactor;

    //diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float NDotL = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = lightColor * NDotL;

    //specular -----Blinn-Phong 
    float specularFactor = 0.8f;
    float shininess = 45.0f;
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfDir = normalize(lightDir + viewDir);
    //float RDotV = max(dot(reflectDir, viewDir), 0.0f);
    float NDotH = max(dot(normal, halfDir), 0.0f);

    vec3 specular = lightColor * specularFactor * pow(NDotH, shininess);
      
    vec4 texel = texture(texSampler1, TexCoord);
    frag_color = vec4(ambient + diffuse + specular, 1.0f)  * texel;
}


