#version 330

in vec3 fragPosition;
in vec3 fragNormal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(viewPos - fragPosition);
    vec3 L = normalize(lightPos - fragPosition);
    vec3 H = normalize(V + L);

    // Configurações de Material Profissional
    float metallic = 0.9;   // 1.0 é metal puro
    float roughness = 0.15; // Quanto menor, mais parece um espelho
    
    // Cook-Torrance BRDF simplificado
    float NDF = pow(max(dot(N, H), 0.0), 128.0); // Distribuição de brilho
    float G = min(1.0, min(2.0 * dot(N, H) * dot(N, V) / dot(V, H), 2.0 * dot(N, H) * dot(N, L) / dot(V, H))); // Geometria
    float F = 0.04 + (1.0 - 0.04) * pow(1.0 - dot(V, H), 5.0); // Fresnel (Reflexo nas bordas)

    vec3 specular = vec3(F * G * NDF);
    
    // Cor base com oclusão
    vec3 ambient = vec3(0.03) * colDiffuse.rgb;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * colDiffuse.rgb * (1.0 - F);

    // Resultado final com "Glow" embutido
    vec3 color = ambient + diffuse + (specular * 1.5);
    
    // HDR Tonemapping (Essencial para não parecer plástico)
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    finalColor = vec4(color, 1.0);
}
