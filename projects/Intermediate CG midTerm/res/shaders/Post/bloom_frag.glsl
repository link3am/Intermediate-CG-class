#version 410

uniform sampler2D colorTexture;

in vec2 uv;
out vec4 fragColor;
//Gaussian blur

void main()
{
    vec4 color = texture2D( colorTexture , uv );
    // convert rgb to grayscale/brightness
    float brightness = dot( color.rgb , vec3( 0.2126 , 0.7152 , 0.0722 ) );
    if ( brightness > 0.6)
        fragColor = vec4(color.rgb , 1.0 );
    else
        fragColor = vec4( 0.0 , 0.0 , 0.0 , 1.0 );
}
