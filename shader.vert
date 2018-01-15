#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform mat4 MVI;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;

float getHeight(vec2 p) {
    p.x = clamp(p.x, 0.0, 1.0);
    p.y = clamp(p.y, 0.0, 1.0);
    vec3 c = texture(rgbTexture, p).xyz;
    return (heightFactor * (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z));
}

vec3 getWorldCoordinates(vec2 uv, vec3 position) {
    return vec3(position.x* widthTexture, getHeight(uv), position.z * heightTexture);
}

vec3 getTriangleNormal(vec3 a, vec3 b, vec3 c) {
    vec3 v1 = c - a;
    vec3 v2 = b - a;
    return normalize(cross(v1,v2));
}


void main()
{
    vec3 lightPosMV = vec3(MV * vec4(lightPosition,1.0));
    
    // get texture value, compute height
    float u = clamp((1.0 - position.x), 0.0, 1.0);
    float v = clamp((1.0 - position.z), 0.0, 1.0);
    vec2 texCoods = vec2(u, v);
    textureCoordinate = texCoods;
    vec3 worldPos = getWorldCoordinates(texCoods, position);
    
    vec3 vertexVCS = vec3(MV * vec4(worldPos, 1.0));
    ToLightVector = normalize((lightPosMV - vertexVCS).xyz);
    ToCameraVector = normalize((- vertexVCS).xyz);

    //vertexNormal = vec3(0.0, 0.0, 0.0);
    //vec2 left = vec2(texCoods.x - (1.0/widthTexture), texCoods.y); 
    //vec2 right = vec2(texCoods.x + (1.0/widthTexture), texCoods.y);
    //vec2 up = vec2(texCoods.x, texCoods.y - (1.0/heightTexture));
    //vec2 down = vec2(texCoods.x, texCoods.y + (1.0/heightTexture));
    // compute normal vector using also the heights of neighbor vertices
    //////////////////////////////////
    // COMPUTE NORMALS FOR LIGHTING
    ////////////////////////////////    
    vec2 left = vec2(texCoods.x - (1.0/widthTexture), texCoods.y); 
    left.x = clamp(left.x, 0.0, 1.0);
    left.y = clamp(left.y, 0.0, 1.0);

    vec2 right = vec2(texCoods.x + (1.0/widthTexture), texCoods.y);
    right.x = clamp(right.x, 0.0, 1.0);
    right.y = clamp(right.y, 0.0, 1.0);

    vec2 up = vec2(texCoods.x, texCoods.y - (1.0/heightTexture));
    up.x = clamp(up.x, 0.0, 1.0);
    up.y = clamp(up.y, 0.0, 1.0);
    
    vec2 down = vec2(texCoods.x, texCoods.y + (1.0/heightTexture));
    down.x = clamp(down.x, 0.0, 1.0);
    down.y = clamp(down.y, 0.0, 1.0);

    float lh = getHeight(left);
    float rh = getHeight(right);
    float uh = getHeight(up);
    float dh = getHeight(down);
    float scaleXZ = 2.0 * ((1.0/widthTexture)/(1.0/heightTexture));

    vec3 normal = vec3(lh-rh, scaleXZ, uh-dh);
    vertexNormal = normalize(vec3(MVI*vec4(normalize(normal), 0.0)));
    
    // compute toLight vector vertex coordinate in VCS
    ////////////////////////////
    //  MAYBE WE NEED TO MULTIPLY POSITION BY MV MATRIX  
    /////////////////////////////

    //ToLightVector = normalize(lightPosition - newPos);
    //ToCameraVector = normalize(cameraPosition - newPos);

    // set gl_Position variable correctly to give the transformed vertex position
    //vec3 newPos = vec3(position.x * widthTexture, height, position.z * heightTexture);
    
    gl_Position = MVP * vec4(worldPos, 1.0f);
}
