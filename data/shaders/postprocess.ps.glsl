#version 120

uniform sampler2D diffuseSampler;
uniform vec2 resolution;
uniform vec2 offset;
uniform float time;
uniform int renderMode;

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) {
  if (renderMode == 0) {
    gl_FragColor = texture2D(diffuseSampler, gl_TexCoord[0].st);
    return;
  }

	vec2 q = (gl_FragCoord.xy - offset.xy) / resolution.xy;
	vec3 oricol = texture2D(diffuseSampler, q).xyz;

	// subtle zoom in/out 
	vec2 uv = 0.5 + (q-0.5)*(0.99 + 0.006*sin(0.9*time));

    vec3 col;
    // start with the source texture and misalign the rays it a bit
    // TODO animate misalignment upon hit or similar event
    // col.r = texture2D(diffuseSampler,vec2(uv.x+0.003,uv.y)).x;
    // col.g = texture2D(diffuseSampler,vec2(uv.x+0.000,uv.y)).y;
    // col.b = texture2D(diffuseSampler,vec2(uv.x-0.003,uv.y)).z;

    col.r = texture2D(diffuseSampler,vec2(uv.x+0.002,uv.y)).x;
    col.g = texture2D(diffuseSampler,vec2(uv.x+0.000,uv.y)).y;
    col.b = texture2D(diffuseSampler,vec2(uv.x-0.002,uv.y)).z;

    // contrast curve
    col = clamp(col*0.5+0.5*col*col*1.2,0.0,1.0);

    //vignette
    col *= 0.6 + 0.4*16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y);

    //color tint
    col *= vec3(0.9,1.0,0.7);

    //scanline (last 2 constants are crawl speed and size)
    //TODO make size dependent on viewport
    col *= 0.8+0.2*sin(10.0*time+uv.y*900.0);

    //flickering (semi-randomized)
    col *= 1.0-0.07*rand(vec2(time, tan(time)));

    // smoothen
    float comp = smoothstep(0.2, 0.7, sin(time));
    col = mix(col, oricol, clamp(-2.0+2.0*q.x+3.0*comp,0.0,1.0) / 3);

	if (renderMode == 1) {
		gl_FragColor = vec4(col, 1.0);
		return;
	}

    // add greenish tint
    if (renderMode == 2) {
		col *= vec3(0.2, 1, 0.2);
		gl_FragColor = vec4(col, 1.0);
		return;
    }

    // ---------------------------------------------------------
    // add blooming
    // ---------------------------------------------------------

//    uv = 0.5 + (q-0.5)*(0.8 + 0.2*sin(0.2*time));
    vec4 sum = vec4(0);
    for(int i=-4;i<4;i++) {
        for (int j=-3;j<3;j++) {
            sum += texture2D(diffuseSampler, vec2(j,i)*0.004 + vec2(q.x,q.y)) * 0.25;
        }
    }


   vec4 finalColor;
   if (col.r < 0.3) {
       finalColor = sum*sum*0.012 + vec4(col, 1);
   } else {
       if (col.r < 0.5) {
          finalColor = sum*sum*0.009 + vec4(col, 1);
       } else {
          finalColor = sum*sum*0.0075 + vec4(col, 1);
       }
   }

    if (renderMode == 3) {
		gl_FragColor = finalColor;
		return;
    }

    // add greenish tint
   vec4 greenishTint = vec4(0.2, 1, 0.2, 1);
   gl_FragColor = finalColor * greenishTint;
}