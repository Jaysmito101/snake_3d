#ifndef SHADERS_H
#define SHADERS_H

/*
#version 430 core
layout(location = 0) in vec3 Position;
void main()
{
	gl_Position = vec4(Position, 1.0f);
}
*/
const char* vertex_shader_source = "#version 460 core\n\tlayout (location = 0) in vec3 Position;  \n\tvoid main()\n\t{\n\t\tgl_Position = vec4(Position, 1.0f);\n\t}\n";

/*
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


	//for(int i = 1 ; i < _BoardSize ; i++)
	//{
	//	float dist = float(i) * cellSize;
	//	float rc = sdRoundCone(
	//		pos - vec3(0, 0.01, 0),
	//		vec3(dist - 2.5f, 0.01f, -2.5f),
	//		vec3(dist - 2.5f, 0.01f,  2.5f),
	//		0.01,
	//		0.01);
	//	d2 = smin(d2, rc, 0.001f);
	//	rc = sdRoundCone(
	//		pos- vec3(0, 0.01, 0),
	//		vec3(-2.5f, 0.01f, dist - 2.5f),
	//		vec3( 2.5f, 0.01f, dist - 2.5f),
	//		0.01,
	//		0.01);
	//	d2 = smin(d2, rc, 0.001f);
	//
	//}


	id = id(d2, d, 2.0f, id);
	d = min(d, d2);
	float id2 = 3.0f;

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
				id2 = id(d3, rc, id2, 4.0f);
				d3 = smin(d3, rc, 0.05f);
			}
		}
	 }

	 id = id(d3, d, id2, id);
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
		else if(t.y < 4.5f) // 4 is for cherry
		{
			material = vec3(247, 21, 0) / 255.0f;
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
*/
const char* fragment_shader_source = "#version 430 core\n\nout vec4 FragColor;\n\nuniform vec2 _Resolution = vec2(512.0f);\nuniform int _BoardSize = 8;\nuniform float _AnimationPercentage = 0.0f;\n\nlayout (std430, binding = 0) buffer _Board\n{\n\tint board[];\n};\n\nfloat smin( float a, float b, float k )\n{\n\tfloat h = a-b;\n\treturn 0.5*( (a+b) - sqrt(h*h+k) );\n}\n\nfloat sdSphere(in vec3 pos, in float radius)\n{\n\treturn length(pos) - radius;\n}\n\nfloat sdRoundBox( vec3 p, vec3 b, float r )\n{\n\tvec3 q = abs(p) - b;\n\treturn length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - r;\n}\n\nfloat dot2(in vec3 v ) { return dot(v,v); }\nfloat sdRoundCone(vec3 p, vec3 a, vec3 b, float r1, float r2)\n{\n  // sampling independent computations (only depend on shape)\n  vec3  ba = b - a;\n  float l2 = dot(ba,ba);\n  float rr = r1 - r2;\n  float a2 = l2 - rr*rr;\n  float il2 = 1.0/l2;\n\n  // sampling dependant computations\n  vec3 pa = p - a;\n  float y = dot(pa,ba);\n  float z = y - l2;\n  float x2 = dot2( pa*l2 - ba*y );\n  float y2 = y*y*l2;\n  float z2 = z*z*l2;\n\n  // single square root!\n  float k = sign(rr)*rr*rr*x2;\n  if( sign(z)*a2*z2>k ) return  sqrt(x2 + z2)        *il2 - r2;\n  if( sign(y)*a2*y2<k ) return  sqrt(x2 + y2)        *il2 - r1;\n  return (sqrt(x2*a2*il2)+y*rr)*il2 - r1;\n}\n\n\nfloat id(float a, float b, float id_a, float id_b)\n{\n\treturn a < b ? id_a : id_b;\n}\n\nvec3 getMovementDir(int v)\n{\n\tif(v == 1) \treturn vec3(0, 0, -1);\n\tif(v == 2)\treturn vec3(0, 0,  1);\n\tif(v == 3)\treturn vec3(-1, 0, 0);\n\tif(v == 4)\treturn vec3(1, 0, 0);\n\treturn vec3(0);\n}\n\nvec2 map(in vec3 pos)\n{\n\tfloat d = 0.0f; \n\tfloat id = -1.0f;\n\tvec3 cen = vec3(0.0f, 0.0f, 0.0f);\t\n\t//d = sdSphere(pos - cen);\n\tvec3 q = pos - cen;\n\td = sdRoundBox(q, vec3(2.5f, 0.01f, 2.5f), 0.02f);\n\tid = id(d, pos.y, 1.0f, 0.0f);\n\td = min(d, pos.y + 0.5f);\t\n\n\t// the markings on board\n\tfloat d2 = d+0.5f;\n\t// horizontal\n\tfloat cellSize = 5.0f / float(_BoardSize);\n\n\t\n\t//for(int i = 1 ; i < _BoardSize ; i++)\n\t//{\n\t//\tfloat dist = float(i) * cellSize;\n\t//\tfloat rc = sdRoundCone(\n\t//\t\tpos - vec3(0, 0.01, 0),\n\t//\t\tvec3(dist - 2.5f, 0.01f, -2.5f),\n\t//\t\tvec3(dist - 2.5f, 0.01f,  2.5f),\n\t//\t\t0.01,\n\t//\t\t0.01);\n\t//\td2 = smin(d2, rc, 0.001f);\n\t//\trc = sdRoundCone(\n\t//\t\tpos- vec3(0, 0.01, 0),\n\t//\t\tvec3(-2.5f, 0.01f, dist - 2.5f),\n\t//\t\tvec3( 2.5f, 0.01f, dist - 2.5f),\n\t//\t\t0.01,\n\t//\t\t0.01);\n\t//\td2 = smin(d2, rc, 0.001f);\n\t//\t\n\t//}\n\t\n\n\tid = id(d2, d, 2.0f, id);\n\td = min(d, d2);\n\tfloat id2 = 3.0f;\n\n\t // the snake\n\t float d3 = d + 0.5f;\n\t for(int i = 0 ; i < _BoardSize ; i++)\n\t {\n\t \tfor(int j = 0 ; j < _BoardSize ; j++)\n\t \t{\n\t \t\tint index = j * _BoardSize + i;\n\t \t\tif(board[index] > 0)\n\t \t\t{\n\t \t\t\t//vec3 animOffset = getMovementDir(board[index]) * cellSize * _AnimationPercentage - getMovementDir(board[index]) * cellSize;\n\t \t\t\tvec3 animOffset = vec3(0);\n\t \t\t\tvec3 cen = vec3(float(i) * cellSize - 2.5f + cellSize/2, 0.3f, float(j) * cellSize - 2.5f + cellSize /2) + animOffset;\n\t \t\t\tfloat rc = sdRoundBox(pos - cen, vec3(cellSize/2 - 0.1f, 0.1f, cellSize/2 - 0.1f), 0.02f);\n\t \t\t\td3 = smin(d3, rc, 0.05f);\n\t \t\t}\n\t \t\telse if(board[index] == -1)\n\t \t\t{\n\t \t\t\tvec3 animOffset = vec3(0);\n\t \t\t\tvec3 cen = vec3(float(i) * cellSize - 2.5f + cellSize/2, 0.3f, float(j) * cellSize - 2.5f + cellSize /2) + animOffset;\n\t \t\t\tfloat rc = sdSphere(pos - cen, 0.1f);\n\t\t\t\tid2 = id(d3, rc, id2, 4.0f);\n\t \t\t\td3 = smin(d3, rc, 0.05f);\t\n\t \t\t}\n\t \t}\n\t }\n\n\t id = id(d3, d, id2, id);\n\t d = min(d, d3);\n\n\t return vec2(d, id);\n\t}\n\n\tvec3 calculateNormal(in vec3 pos)\n\t{\n\t\tvec2 e = vec2(0.005f, 0.0f);\n\n\t\treturn normalize(vec3(\n\t\t\tmap(pos+e.xyy).x-map(pos-e.xyy).x,\n\t\t\tmap(pos+e.yxy).x-map(pos-e.yxy).x,\n\t\t\tmap(pos+e.yyx).x-map(pos-e.yyx).x\n\t\t\t));\n\t}\n\n\n\tfloat softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )\n\t{\n\t\tfloat res = 1.0;\n\t\tfloat ph = 1e20;\n\t\tfor( float t=mint; t<maxt; )\n\t\t{\n\t\t\tfloat h = map(ro + rd*t).x;\n\t\t\tif( h<0.001 )\n\t\t\treturn 0.0;\n\t\t\tfloat y = h*h/(2.0*ph);\n\t\t\tfloat d = sqrt(h*h-y*y);\n\t\t\tres = min( res, k*d/max(0.0,t-y) );\n\t\t\tph = h;\n\t\t\tt += h;\n\t\t}\n\t\treturn res;\n\t}\n\n\tvec2 castRay(in vec3 rayOrigin, in vec3 rayDirection)\n\t{\n\t\tvec2 result = vec2(0.0);\n\t\tfloat t = 0.0f;\n\t\tvec2 h = vec2(0.0);\n\t\tfor(int i = 0 ; i < 50 ; i ++)\n\t\t{\n\t\t\tvec3 pos = rayOrigin + rayDirection * t;\n\t\t\th = map(pos);\n\t\t\tif(h.x <= 0.001f)\n\t\t\tbreak;\n\t\t\tif(t > 100.0f)\n\t\t\tbreak;\n\t\t\tt = t + h.x;\n\t\t}\n\t\tresult.x = t;\n\t\tresult.y = h.y;\n\t\tif(result.x > 100.0f)\n\t\t\tresult = vec2(-1.0f, -1.0);\n\t\treturn result;\n\t}\n\n\tvoid main()\n\t{\n\t\tvec3 color = vec3(0.3, 0.4, 0.5);\n\n\t\tvec3 cameraPos = vec3(0.0, 3.50, 6.0);\n\t\tvec3 cameraTarget = vec3(0.0, 0.0, 0.0);\n\n\t\tvec2 uv = (2.0f * gl_FragCoord.xy - _Resolution.xy) / _Resolution.x;\n\n\t\tvec3 ww = normalize(cameraTarget - cameraPos);\n\t\tvec3 uu = normalize(cross(ww, vec3(0.0, 1.0, 0.0)));\n\t\tvec3 vv = normalize(cross(uu, ww));\n\n\t\tvec3 rayOrigin = cameraPos;\n\t\tvec3 rayDirection = normalize(uu * uv.x + vv * uv.y + 1.5f * ww);\n\n\t\tvec2 t = castRay(rayOrigin, rayDirection);\n\n\t\tif(t.x >= 0.0f)\n\t\t{\n\t\t\tvec3 pos = rayOrigin + rayDirection * t.x;\n\t\t\tvec3 normal = calculateNormal(pos);\n\t\t\tvec3 sunDir = normalize(vec3(-0.6, 0.8, 0.0));\n\t\t\tfloat sunDiff = clamp(dot(normal, sunDir), 0.0, 1.0f);\n\t\t\tfloat shadow = softshadow(pos + normal * 0.001f, sunDir, 0.0f, 100.0f, 0.1f);\n\t\t\tvec3 sunLight = vec3(9.0, 8.0, 5.0) * sunDiff * shadow;\n\t\t\tfloat skyDiff = max( 0.5f * dot(normal, vec3(0.0, 1.0, 0.0)), 0.0);\n\t\t\tvec3 skyLight = vec3(0.3, 0.45, 0.7) * skyDiff;\n\n\t\t\tvec3 material = vec3(0.18, 0.18, 0.18);\n\n\t\tif(t.y < 0.5f) // 0 is floor\n\t\t{\n\t\t\tmaterial = (vec3(0, 191, 32) / 255.0f);\n\t\t\tfloat factor = 0.5f*smoothstep(-0.2f, 0.2f, sin(pos.x* 6) * sin(pos.z*6));\n\t\t\tmaterial += (vec3(0, 82, 19) / 255.0f) * factor;\n\t\t}\n\t\telse if(t.y < 1.5f) // 1 is sphere\n\t\t{\n\t\t\tmaterial = vec3(10, 10, 10) / 255.0f;\n\t\t}\n\t\telse if(t.y < 2.5f) // 2 is cell seperrator bars\n\t\t{\n\t\t\tmaterial = vec3(0.70f);\n\t\t}\n\t\telse if(t.y < 3.5f) // 3 is for snake body\n\t\t{\n\t\t\tmaterial = vec3(39, 41, 122) / 255.0f;\n\t\t}\n\t\telse if(t.y < 4.5f) // 4 is for cherry\n\t\t{\n\t\t\tmaterial = vec3(247, 21, 0) / 255.0f;\n\t\t}\n\t\t\n\n\t\tcolor = material * ( sunLight + skyLight );\n\t}\n\telse\n\t{\n\t\tcolor = vec3(0.4f, 0.75f, 1.0f) - 0.7f * rayDirection.y;\n\t\tcolor = mix(color, vec3(0.7f, 0.75f, 0.8f), exp(-10.0f * rayDirection.y));\n\t}\t\t\n\n\tcolor = pow(color, vec3(0.4545)); // gamma correction\n\tFragColor = vec4(color, 1.0f);\n}";

/*
#version 430 core
out vec4 FragColor;
uniform vec2 _Resolution = vec2(512.0f);
uniform sampler2D _Frame;
void main()
{
	vec2 uv = vec2((gl_FragCoord.x / _Resolution.x), (gl_FragCoord.y / _Resolution.y));
	FragColor = texture(_Frame, uv);
}
*/
const char* fragment_shader_source_copy = "#version 430 core\nout vec4 FragColor;\nuniform vec2 _Resolution = vec2(512.0f);\nuniform sampler2D _Frame;\nvoid main()\n{\n\tvec2 uv = vec2((gl_FragCoord.x / _Resolution.x), (gl_FragCoord.y / _Resolution.y));\n\tFragColor = texture(_Frame, uv);\n}";


#endif // SHADERS_H
