#version 410

uniform sampler2D colorTexture;

in vec2 uv;
out vec4 fragColor;

uniform float u_threshold = 1.0;
void main()
{

     float blur = 1.0;
    float offset = 1.0 / textureSize(colorTexture, 0).x;
  



    vec4 color = texture2D( colorTexture , uv );
   
    float brightness = dot( color.rgb , vec3( 0.2126 , 0.7152 , 0.0722 ) );


    if ( brightness > u_threshold)
        {

        vec4 sum = texture2D( colorTexture , vec2( uv.x - 4.0*blur*offset , uv.y - 4.0*blur*offset )) * 0.0162162162;
        sum += texture2D( colorTexture , vec2( uv.x - 3.0*blur*offset , uv.y - 3.0*blur*offset )) * 0.0540540541;
        sum += texture2D( colorTexture , vec2( uv.x - 2.0*blur*offset , uv.y - 2.0*blur*offset )) * 0.1216216216;
        sum += texture2D( colorTexture , vec2( uv.x - 1.0*blur*offset , uv.y - 1.0*blur*offset )) * 0.1945945946;
        sum += texture2D( colorTexture , vec2( uv.x , uv.y )) * 0.2270270270;
        sum += texture2D( colorTexture , vec2( uv.x + 1.0*blur*offset , uv.y + 1.0*blur*offset )) * 0.1945945946;
        sum += texture2D( colorTexture , vec2( uv.x + 2.0*blur*offset , uv.y + 2.0*blur*offset )) * 0.1216216216;
        sum += texture2D( colorTexture , vec2( uv.x + 3.0*blur*offset , uv.y + 3.0*blur*offset )) * 0.0540540541;
        sum += texture2D( colorTexture , vec2( uv.x + 4.0*blur*offset , uv.y + 4.0*blur*offset )) * 0.0162162162;
        fragColor = vec4( sum.rgb *1.05, 1.0 );


    }
    else
       fragColor = vec4(color.rgb,1.0);
       //fragColor = vec4( 0.0 , 0.0 , 0.0 , 1.0 );

  
}