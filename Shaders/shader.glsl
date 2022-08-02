#version 430 core

out vec4 FragColor;

uniform vec2 _Resolution = vec2(512.0f);
uniform int _BoardSize = 8;
uniform float _AnimationPercentage = 0.0f;

layout (std430, binding = 0) buffer _Board
{
	int board[];
};

float smin( float a, float b, float k )
{
	float h = a-b;
	return 0.5*( (a+b) - sqrt(h*h+k) );
}

float sdSphere(in vec3 pos, in float radius)
{
	return length(pos) - radius;
}

float sdRoundBox( vec3 p, vec3 b, float r )
{
	vec3 q = abs(p) - b;
	return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;
}

float dot2(in vec3 v ) { return dot(v,v); }
float sdRoundCone(vec3 p, vec3 a, vec3 b, float r1, float r2)
{
  // sampling independent computations (only depend on shape)
  vec3  ba = b - a;
  float l2 = dot(ba,ba);
  float rr = r1 - r2;
  float a2 = l2 - rr*rr;
  float il2 = 1.0/l2;

  // sampling dependant computations
  vec3 pa = p - a;
  float y = dot(pa,ba);
  float z = y - l2;
  float x2 = dot2( pa*l2 - ba*y );
  float y2 = y*y*l2;
  float z2 = z*z*l2;

  // single square root!
  float k = sign(rr)*rr*rr*x2;
  if( sign(z)*a2*z2>k ) return  sqrt(x2 + z2)        *il2 - r2;
  if( sign(y)*a2*y2<k ) return  sqrt(x2 + y2)        *il2 - r1;
  return (sqrt(x2*a2*il2)+y*rr)*il2 - r1;
}


float id(float a, float b, float id_a, float id_b)
{
	return a < b ? id_a : id_b;
}

vec3 getMovementDir(int v)
{
	if(v == 1) 	return vec3(0, 0, -1);
	if(v == 2)	return vec3(0, 0,  1);
	if(v == 3)	return vec3(-1, 0, 0);
	if(v == 4)	return vec3(1, 0, 0);
	return vec3(0);
}

vec2 map(in vec3 pos)
{
	float d = 0.0f; 
	float id = -1.0f;
	vec3 cen = vec3(0.0f, 0.0f, 0.0f);	
	//d = sdSphere(pos - cen);
	vec3 q = pos - cen;
	d = sdRoundBox(q, vec3(2.5f, 0.01f, 2.5f), 0.02f);
	id = id(d, pos.y, 1.0f, 0.0f);
	d = min(d, pos.y + 0.5f);	

	// the markings on board
	float d2 = d+0.5f;
	// horizontal
	float cellSize = 5.0f / float(_BoardSize);

	for(int i = 1 ; i < _BoardSize ; i++)
	{
		float dist = float(i) * cellSize;
		float rc = sdRoundCone(
			pos - vec3(0, 0.01, 0),
			vec3(dist - 2.5f, 0.01f, -2.5f),
			vec3(dist - 2.5f, 0.01f,  2.5f),
			0.01,
			0.01);
		d2 = smin(d2, rc, 0.001f);
		rc = sdRoundCone(
			pos- vec3(0, 0.01, 0),
			vec3(-2.5f, 0.01f, dist - 2.5f),
			vec3( 2.5f, 0.01f, dist - 2.5f),
			0.01,
			0.01);
		d2 = smin(d2, rc, 0.001f);
		
	}

	id = id(d2, d, 2.0f, id);
	d = min(d, d2);

	 // the snake
	 float d3 = d + 0.5f;
	 for(int i = 0 ; i < _BoardSize ; i++)
	 {
	 	for(int j = 0 ; j < _BoardSize ; j++)
	 	{
	 		int index = j * _BoardSize + i;
	 		if(board[index] > 0)
	 		{
	 			//vec3 animOffset = getMovementDir(board[index]) * cellSize * _AnimationPercentage - getMovementDir(board[index]) * cellSize;
	 			vec3 animOffset = vec3(0);
	 			vec3 cen = vec3(float(i) * cellSize - 2.5f + cellSize/2, 0.3f, float(j) * cellSize - 2.5f + cellSize /2) + animOffset;
	 			float rc = sdRoundBox(pos - cen, vec3(cellSize/2 - 0.1f, 0.1f, cellSize/2 - 0.1f), 0.02f);
	 			d3 = smin(d3, rc, 0.05f);
	 		}
	 		else if(board[index] == -1)
	 		{
	 			vec3 animOffset = vec3(0);
	 			vec3 cen = vec3(float(i) * cellSize - 2.5f + cellSize/2, 0.3f, float(j) * cellSize - 2.5f + cellSize /2) + animOffset;
	 			float rc = sdSphere(pos - cen, 0.1f);
	 			d3 = smin(d3, rc, 0.05f);	
	 		}
	 	}
	 }

	 id = id(d3, d, 3.0f, id);
	 d = min(d, d3);

	 return vec2(d, id);
	}

	vec3 calculateNormal(in vec3 pos)
	{
		vec2 e = vec2(0.005f, 0.0f);

		return normalize(vec3(
			map(pos+e.xyy).x-map(pos-e.xyy).x,
			map(pos+e.yxy).x-map(pos-e.yxy).x,
			map(pos+e.yyx).x-map(pos-e.yyx).x
			));
	}


	float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
	{
		float res = 1.0;
		float ph = 1e20;
		for( float t=mint; t<maxt; )
		{
			float h = map(ro + rd*t).x;
			if( h<0.001 )
			return 0.0;
			float y = h*h/(2.0*ph);
			float d = sqrt(h*h-y*y);
			res = min( res, k*d/max(0.0,t-y) );
			ph = h;
			t += h;
		}
		return res;
	}

	vec2 castRay(in vec3 rayOrigin, in vec3 rayDirection)
	{
		vec2 result = vec2(0.0);
		float t = 0.0f;
		vec2 h = vec2(0.0);
		for(int i = 0 ; i < 50 ; i ++)
		{
			vec3 pos = rayOrigin + rayDirection * t;
			h = map(pos);
			if(h.x <= 0.001f)
			break;
			if(t > 100.0f)
			break;
			t = t + h.x;
		}
		result.x = t;
		result.y = h.y;
		if(result.x > 100.0f)
			result = vec2(-1.0f, -1.0);
		return result;
	}

	void main()
	{
		vec3 color = vec3(0.3, 0.4, 0.5);

		vec3 cameraPos = vec3(0.0, 3.50, 6.0);
		vec3 cameraTarget = vec3(0.0, 0.0, 0.0);

		vec2 uv = (2.0f * gl_FragCoord.xy - _Resolution.xy) / _Resolution.x;

		vec3 ww = normalize(cameraTarget - cameraPos);
		vec3 uu = normalize(cross(ww, vec3(0.0, 1.0, 0.0)));
		vec3 vv = normalize(cross(uu, ww));

		vec3 rayOrigin = cameraPos;
		vec3 rayDirection = normalize(uu * uv.x + vv * uv.y + 1.5f * ww);

		vec2 t = castRay(rayOrigin, rayDirection);

		if(t.x >= 0.0f)
		{
			vec3 pos = rayOrigin + rayDirection * t.x;
			vec3 normal = calculateNormal(pos);
			vec3 sunDir = normalize(vec3(-0.6, 0.8, 0.0));
			float sunDiff = clamp(dot(normal, sunDir), 0.0, 1.0f);
			float shadow = softshadow(pos + normal * 0.001f, sunDir, 0.0f, 100.0f, 0.1f);
			vec3 sunLight = vec3(9.0, 8.0, 5.0) * sunDiff * shadow;
			float skyDiff = max( 0.5f * dot(normal, vec3(0.0, 1.0, 0.0)), 0.0);
			vec3 skyLight = vec3(0.3, 0.45, 0.7) * skyDiff;

			vec3 material = vec3(0.18, 0.18, 0.18);

		if(t.y < 0.5f) // 0 is floor
		{
			material = (vec3(0, 191, 32) / 255.0f);
			float factor = 0.5f*smoothstep(-0.2f, 0.2f, sin(pos.x* 6) * sin(pos.z*6));
			material += (vec3(0, 82, 19) / 255.0f) * factor;
		}
		else if(t.y < 1.5f) // 1 is sphere
		{
			material = vec3(10, 10, 10) / 255.0f;
		}
		else if(t.y < 2.5f) // 2 is cell seperrator bars
		{
			material = vec3(0.70f);
		}
		else if(t.y < 3.5f) // 3 is for snake body
		{
			material = vec3(39, 41, 122) / 255.0f;
		}
		

		color = material * ( sunLight + skyLight );
	}
	else
	{
		color = vec3(0.4f, 0.75f, 1.0f) - 0.7f * rayDirection.y;
		color = mix(color, vec3(0.7f, 0.75f, 0.8f), exp(-10.0f * rayDirection.y));
	}		

	color = pow(color, vec3(0.4545)); // gamma correction
	FragColor = vec4(color, 1.0f);
}