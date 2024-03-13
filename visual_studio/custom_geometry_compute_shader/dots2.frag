#version 430 compatibility

in vec3 gNormal;
in vec3 gLightDir;
in vec3 gEyeDir;
in vec3 gLightIntensity;
in vec3 gCol;

vec3 lightColor = vec3(1);
vec3 ambientColor = gCol;
vec3 diffuseColor = gCol;
vec3 specularColor = vec3(1);
float shininess = 128;

float ambientFactor = 0.2;
void
main(){
	
	// Normalize vectors
    vec3 N = normalize(gNormal);
    vec3 L = normalize(gLightDir);
    vec3 V = normalize(gEyeDir);

    // Calculate ambient component
    vec3 ambient = ambientColor * lightColor * ambientFactor;

    // Calculate diffuse component
    float diffuseFactor = max(dot(N, L), 0.0);
    vec3 diffuse = diffuseColor * lightColor * diffuseFactor;

    // Calculate specular component
    vec3 reflectDir = reflect(-L, N);
    float specularFactor = pow(max(dot(reflectDir, V), 0.0), shininess);
    vec3 specular = specularColor * lightColor * specularFactor;

    // Sum up all components
    vec3 color = ambient + diffuse + specular;

	//vec3 color = vec3(1, 0, 0) * gLightIntensity + vec3(0, 0.2, 0);
	gl_FragColor = vec4(color, 1);
}