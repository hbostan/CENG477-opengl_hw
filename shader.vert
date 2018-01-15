#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
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
    vec4 c = texture(rgbTexture, p);
    return (heightFactor * (0.2126 * c.x + 0.7152 * c.y + 0.0722 * c.z));
}


void main()
{

    // get texture value, compute height
    float u = clamp((1.0 - position.x), 0.0, 1.0);
    float v = clamp((1.0 - position.z), 0.0, 1.0);
    vec2 texCoods; texCoods.x = u; texCoods.y = v;
    //vec4 textureColor = texture(rgbTexture, texCoods);
    //float height = heightFactor * (0.2126 * textureColor.x + 0.7152 * textureColor.y + 0.0722 * textureColor.z);
    float height = getHeight(texCoods);
    textureCoordinate.x = u;
    textureCoordinate.y = v;
    
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

    vec3 normal = vec3(lh-rh, 2.0, uh-dh);
    vertexNormal = normalize(normal);

    // compute toLight vector vertex coordinate in VCS
    ////////////////////////////
    //  MAYBE WE NEED TO MULTIPLY POSITION BY MV MATRIX  
    /////////////////////////////
    ToLightVector = normalize(lightPosition - position);
    ToCameraVector = normalize(cameraPosition - position);

    // set gl_Position variable correctly to give the transformed vertex position
    vec3 newPos = vec3(position.x * widthTexture, height, position.z * heightTexture);
    gl_Position = MVP * vec4(newPos, 1.0f);
}
