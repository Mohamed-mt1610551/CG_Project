#version 330 core

struct Material
{
    vec3 ambient;
    sampler2D diffuseMap;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 frag_color;

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

// Texture samplers
uniform sampler2D texSampler1;

void main()
{
    //ambient
    vec3 ambient = light.ambient * material.ambient;

    //diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float NDotL = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * NDotL * vec3(texture(material.diffuseMap, TexCoord));

    //specular -----Blinn-Phong 
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfDir = normalize(lightDir + viewDir);
    //float RDotV = max(dot(reflectDir, viewDir), 0.0f);
    float NDotH = max(dot(normal, halfDir), 0.0f);

    vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);
      
    vec4 texel = texture(texSampler1, TexCoord);
    frag_color = vec4(ambient + diffuse + specular, 1.0f)  * texel;
}


