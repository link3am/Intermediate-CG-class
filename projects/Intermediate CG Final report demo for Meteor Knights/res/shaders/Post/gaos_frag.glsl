#version 410

uniform sampler2D colorTexture;

layout(location = 0) in vec2 inUV;
out vec4 fragColor;

uniform float u_threshold = 1.0;
uniform int u_blurMod = 0;
void main()
{

     float blur = 1.0;
    float offset = 1.0 / textureSize(colorTexture, 0).x;
  
    



    vec4 color = texture2D( colorTexture , inUV );
   
    float brightness = dot( color.rgb , vec3( 0.2126 , 0.7152 , 0.0722 ) );
    if ( brightness > u_threshold)
    {
         if(u_blurMod ==0){
         
             vec4 sum = texture2D( colorTexture , vec2( inUV.x - 4.0*blur*offset , inUV.y - 4.0*blur*offset )) * 0.0162162162;
             sum += texture2D( colorTexture , vec2( inUV.x - 3.0*blur*offset , inUV.y - 3.0*blur*offset )) * 0.0540540541;
             sum += texture2D( colorTexture , vec2( inUV.x - 2.0*blur*offset , inUV.y - 2.0*blur*offset )) * 0.1216216216;
             sum += texture2D( colorTexture , vec2( inUV.x - 1.0*blur*offset , inUV.y - 1.0*blur*offset )) * 0.1945945946;
             sum += texture2D( colorTexture , vec2( inUV.x , inUV.y )) * 0.2270270270;
             sum += texture2D( colorTexture , vec2( inUV.x + 1.0*blur*offset , inUV.y + 1.0*blur*offset )) * 0.1945945946;
             sum += texture2D( colorTexture , vec2( inUV.x + 2.0*blur*offset , inUV.y + 2.0*blur*offset )) * 0.1216216216;
             sum += texture2D( colorTexture , vec2( inUV.x + 3.0*blur*offset , inUV.y + 3.0*blur*offset )) * 0.0540540541;
             sum += texture2D( colorTexture , vec2( inUV.x + 4.0*blur*offset , inUV.y + 4.0*blur*offset )) * 0.0162162162;
             fragColor = vec4( sum.rgb *1.05, 1.0 );
         }
         
         if(u_blurMod ==1)
         {
         vec4 sum2 = texture2D(colorTexture, vec2(inUV.x ,inUV.y));
         sum2 += texture2D(colorTexture,vec2(inUV.x - 1*offset, inUV.y - 1*offset));
         sum2 += texture2D(colorTexture,vec2(inUV.x - 1*offset, inUV.y ));
         sum2 += texture2D(colorTexture,vec2(inUV.x - 1*offset, inUV.y + 1*offset));
         sum2 += texture2D(colorTexture,vec2(inUV.x, inUV.y - 1*offset));
         sum2 += texture2D(colorTexture,vec2(inUV.x, inUV.y + 1*offset));
         sum2 += texture2D(colorTexture,vec2(inUV.x + 1*offset, inUV.y - 1*offset));
         sum2 += texture2D(colorTexture,vec2(inUV.x + 1*offset, inUV.y ));
         sum2 += texture2D(colorTexture,vec2(inUV.x + 1*offset, inUV.y + 1*offset));
         fragColor = vec4(sum2.rgb/9 ,1.0);

         }
         if(u_blurMod ==2)
         {
         float sampleDist = 1.0;
         float sampleStrength = 2.2; 
         float samples[10];
              samples[0] = -0.08;
              samples[1] = -0.05;
              samples[2] = -0.03;
              samples[3] = -0.02;
              samples[4] = -0.01;
              samples[5] =  0.01;
              samples[6] =  0.02;
              samples[7] =  0.03;
              samples[8] =  0.05;
              samples[9] =  0.08;

              vec2 dir = 0.5 - inUV; 
              float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
              dir = dir/dist; 

              
              vec4 sum3 = color;

              for (int i = 0; i < 10; i++)
                  sum3 += texture2D( colorTexture, inUV + dir * samples[i] * sampleDist );

              sum3 *= 1.0/11.0;
              float t = dist * sampleStrength;
              t = clamp( t ,0.0,1.0);

              fragColor = vec4(mix( color, sum3, t ).rgb,1.0);
         }

    }
    else
       fragColor = vec4(color.rgb,1.0);
       //fragColor = vec4( 0.0 , 0.0 , 0.0 , 1.0 );

  
}