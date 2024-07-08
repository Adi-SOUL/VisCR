#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D ourTexture;
//uniform sampler2D texture2;
uniform int s;
uniform int c;
uniform vec4 color;


void main()
{
        // linearly interpolate between both textures (80% container, 20% awesomeface)
        //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    if(s==1){FragColor = texture(ourTexture, TexCoord);}
    else{FragColor = color;}

}

