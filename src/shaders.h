global char *quadVertexShaderSource = "                                                    \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    layout (location = 0) in vec4 vertex;                                                  \n\
                                                                                           \n\
    out vec2 texCoords;                                                                    \n\
                                                                                           \n\
    uniform mat4 model;                                                                    \n\
    uniform mat4 projection;                                                               \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        texCoords = vertex.zw;                                                             \n\
        gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);                      \n\
    }                                                                                      \n\
";

global char *quadFragmentShaderSource = "                                                  \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    in vec2 texCoords;                                                                     \n\
    out vec4 color;                                                                        \n\
                                                                                           \n\
    uniform vec4 drawColor;                                                                \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        color = drawColor;                                                                 \n\
    }                                                                                      \n\
";

global char *spriteFragmentShaderSource = "                                                \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    in vec2 texCoords;                                                                     \n\
    out vec4 color;                                                                        \n\
                                                                                           \n\
    uniform sampler2D image;                                                               \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        color = texture(image, texCoords);                                                 \n\
    }                                                                                      \n\
";

global char *fontFragmentShaderSource = "                                                  \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    in vec2 texCoords;                                                                     \n\
    out vec4 color;                                                                        \n\
                                                                                           \n\
    uniform sampler2D bitmap;                                                              \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        color = vec4(1, 1, 1, texture(bitmap, texCoords).r);                               \n\
    }                                                                                      \n\
";

global char *tilemapVertexShaderSource = "                                                 \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    layout (location = 0) in vec4 vertex;                                                  \n\
    layout (location = 1) in vec2 tileIndex;                                               \n\
                                                                                           \n\
    out vec2 texCoords;                                                                    \n\
    out vec2 tileCoords;                                                                   \n\
                                                                                           \n\
    uniform mat4 model;                                                                    \n\
    uniform mat4 projection;                                                               \n\
    uniform ivec2 mapSize;                                                                 \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        texCoords = vertex.zw;                                                             \n\
        tileCoords = tileIndex;                                                            \n\
        ivec2 tileOffset = ivec2(gl_InstanceID % mapSize.x, gl_InstanceID / mapSize.x);    \n\
        gl_Position = projection * model * vec4(vertex.xy + (1.0 * tileOffset), 0.0, 1.0); \n\
    }                                                                                      \n\
";

global char *tilemapFragmentShaderSource = "                                               \n\
    #version 330 core                                                                      \n\
                                                                                           \n\
    in vec2 texCoords;                                                                     \n\
    in vec2 tileCoords;                                                                    \n\
    out vec4 color;                                                                        \n\
                                                                                           \n\
    uniform sampler2D atlas;                                                               \n\
    uniform vec2 atlasSize;                                                                \n\
                                                                                           \n\
    void main()                                                                            \n\
    {                                                                                      \n\
        vec2 uv = tileCoords / atlasSize;                                                  \n\
        vec2 offset = texCoords / atlasSize;                                               \n\
        color = texture(atlas, uv + offset);                                               \n\
    }                                                                                      \n\
";
