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


void main()
{

    // get texture value, compute height
    float u = (1.0 - position.x);
    float v = (1.0 - position.z);
    vec2 texCoods; texCoods.x = u; texCoods.y = v;
    vec4 textureColor = texture(rgbTexture, texCoods);
    float height = heightFactor * (0.2126 * textureColor.x + 0.7152 * textureColor.y + 0.0722 * textureColor.z);
    textureCoordinate.x = u;
    textureCoordinate.y = v;
    
    // compute normal vector using also the heights of neighbor vertices
    //////////////////////////////////
    // COMPUTE NORMALS FOR LIGHTING
    ////////////////////////////////

    // compute toLight vector vertex coordinate in VCS
    ////////////////////////////
    //  MAYBE WE NEED TO MULTIPLY POSITION BY MV MATRIX  
    /////////////////////////////
    ToLightVector = lightPosition - position;
    ToCameraVector = cameraPosition - position;

    // set gl_Position variable correctly to give the transformed vertex position
    vec3 newPos = vec3(position.x * widthTexture, height, position.z * heightTexture);
    gl_Position = MVP * vec4(newPos, 1.0f);
}
