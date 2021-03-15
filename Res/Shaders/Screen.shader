#shader vertex
#version 460 core

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;

out flat vec2 v_UV;

void main()
{
    v_UV = vec2(Position);
    gl_Position = vec4(Position.x, Position.y, 0.0, 1.0);
}

#shader fragment
#version 460 core

in vec2 v_UV;
uniform vec3 camPosition;
uniform vec3 camLookAt;
uniform vec3 camUp;
uniform float deltaTime;
uniform float test;

out vec4 FragColor;

struct ray {
    vec3 pos;
    vec3 dir;
};

ray create_camera_ray(vec2 uv, vec3 camPos, vec3 lookAt, float zoom) {
    vec3 f = normalize(lookAt - camPos);
    vec3 r = normalize(cross(vec3(0, 1, 0), f));
    vec3 u = normalize(cross(f, r));

    vec3 c = camPos + f * zoom;
    vec3 i = c + uv.x * -r + uv.y * u;
    vec3 dir = i - camPos;
    return ray(camPos, dir);
}
// Shapes
float sdSphere(vec3 p, float s)
{
    return length(p) - s;
}

float sdBox(vec3 p, vec3 b)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdRoundBox(vec3 p, vec3 b, float r)
{
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

// point rotation about an arbitrary axis, ax - from gaziya5
vec3 erot(vec3 p, vec3 ax, float ro) {
    return mix(dot(p, ax) * ax, p, cos(ro)) + sin(ro) * cross(ax, p);
}

// smooth minimum
float smin(float a, float b, float k) {
    float h = max(0., k - abs(b - a)) / k;
    return min(a, b) - h * h * h * k / 6.;
}

// from michael0884's marble marcher community edition
void planeFold(inout vec3 z, vec3 n, float d) {
    z.xyz -= 2.0 * min(0.0, dot(z.xyz, n) - d) * n;
}

void sierpinskiFold(inout vec3 z) {
    z.xy -= min(z.x + z.y, 0.0);
    z.xz -= min(z.x + z.z, 0.0);
    z.yz -= min(z.y + z.z, 0.0);
}

void mengerFold(inout vec3 z)
{
    z.xy += min(z.x - z.y, 0.0) * vec2(-1., 1.);
    z.xz += min(z.x - z.z, 0.0) * vec2(-1., 1.);
    z.yz += min(z.y - z.z, 0.0) * vec2(-1., 1.);
}

void boxFold(inout vec3 z, vec3 r) {
    z.xyz = clamp(z.xyz, -r, r) * 2.0 - z.xyz;
}

// another fold
void sphereFold(inout vec3 z) {
    float minRadius2 = 0.25;
    float fixedRadius2 = 2.;
    float r2 = dot(z, z);
    if (r2 < minRadius2) {
        float temp = (fixedRadius2 / minRadius2);
        z *= temp;
    }
    else if (r2 < fixedRadius2) {
        float temp = (fixedRadius2 / r2);
        z *= temp;
    }
}

vec3 crazyFold(vec3 p)
{
    float distanceFactor = 1.0;
    vec3 cs = vec3(0.8, 0.8, 1.2);
    int fractal_iterations = 2;
    for (int i = 0; i < fractal_iterations; i++)
    {
        p = 2.0 * clamp(p, -cs, cs) - p;
        float k = max(1.0 / dot(p, p), 1.0);
        p *= k;
        distanceFactor *= k;
        p += vec3(0.1, 0.2, 0.3);
    }
    return p / abs(distanceFactor);
}

float sdPlane( vec3 p, vec3 n, float h )
{
  // n must be normalized
  return dot(p,n) + h;
}

float jbFun(vec3 p) {


    return sdBox(p, vec3(0.2));
}

float opU(float d1, float d2)
{
    return min(d1, d2);
}

float rand(vec2 n) {
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}
float noise(vec2 p) {
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u * u * (3.0 - 2.0 * u);

    float res = mix(
        mix(rand(ip), rand(ip + vec2(1.0, 0.0)), u.x),
        mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x), u.y);
    return res * res;
}

float opOnion(float sdf, float thickness)
{
    return abs(sdf) - thickness;
}
float opSubtraction(float d1, float d2) { return max(-d1, d2); }

float opIntersection(float d1, float d2) { return max(d1, d2); }

// Simplex 2D noise
//
vec3 permute(vec3 x) { return mod(((x * 34.0) + 1.0) * x, 289.0); }

float snoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187, 0.366025403784439,
        -0.577350269189626, 0.024390243902439);
    vec2 i = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);
    vec2 i1;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod(i, 289.0);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0))
        + i.x + vec3(0.0, i1.x, 1.0));
    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy),
        dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);
    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}
//	Simplex 3D Noise 
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x) { return mod(((x * 34.0) + 1.0) * x, 289.0); }
vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }

float snoise(vec3 v) {
    const vec2  C = vec2(1.0 / 6.0, 1.0 / 3.0);
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i = floor(v + dot(v, C.yyy));
    vec3 x0 = v - i + dot(i, C.xxx);

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);

    //  x0 = x0 - 0. + 0.0 * C 
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;

    // Permutations
    i = mod(i, 289.0);
    vec4 p = permute(permute(permute(
        i.z + vec4(0.0, i1.z, i2.z, 1.0))
        + i.y + vec4(0.0, i1.y, i2.y, 1.0))
        + i.x + vec4(0.0, i1.x, i2.x, 1.0));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0 / 7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,N*N)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_);    // mod(j,N)

    vec4 x = x_ * ns.x + ns.yyyy;
    vec4 y = y_ * ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);

    vec4 s0 = floor(b0) * 2.0 + 1.0;
    vec4 s1 = floor(b1) * 2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1),
        dot(p2, x2), dot(p3, x3)));
}

int currentScene = 0;

float scene0(vec3 p)
{
	float height = snoise(vec2(p.x / 1.0f, p.z / 1.0f)) / 1.0f;
	float noise2 = snoise(vec2((p.x + (test) ) / 5.0f, p.z / 5.0f)) * 10.0f;
	
	float height2 = sdPlane(p, vec3(0, 1, 0), (height + noise2) / 10.0f);
	
	vec3 q = mod(p+0.5*vec3(0.25 ,0,0.25),vec3(0.25,0,0.25))-0.5*vec3(0.25,0,0.25);

	float rock = sdSphere(q -vec3(0, 0,0)+ vec3(0,(height + noise2) / 10.0f,0), 0.2f);
	if(rock < 0.00001)
		return rock;
	
	float res = min(rock, height2);
	if(res < 0.0001)
		return min(0.1f, res);
	// skydome
	res = min(-sdSphere(p/50.0f, 0.5f),min(rock, height2)); 
	
	if(res < 0.001)
		return min(0.1f, res);
		
		
    return min(0.1f, res);
}

float scene1(vec3 p)
{  
    return 0.0f;
}

float scene2(vec3 p)
{
    float distanceFactor = 1;
    vec3 cs = vec3(0.7, 0.8f, 0.5f);

    int fractal_iterations = 12;
    for (int i = 0; i < fractal_iterations; i++)
    {
        p =2 * clamp(p, -cs, cs) - p;
        float k = max(1.5 / dot(p, p), 0.5);

        //k += snoise(vec3(p.x * fractal_iterations + deltaTime, p.y * fractal_iterations, p.z * fractal_iterations)) / 2 / float(fractal_iterations);
        p *= k;
        distanceFactor *= k;
        p += vec3(0.1f - (deltaTime / 250.0f), 0.5f, 0.56);
    }

    float res = sdSphere(p, 0.2f) / abs(distanceFactor);
    //res = res 
    res = min(res, sdBox(p, vec3(0.44)) / abs(distanceFactor));
    return res * 0.2f;
}

float dstToScene(vec3 p) {

    switch (currentScene)
    {
    case 0:
        return scene0(p);
    case 1:
        return scene1(p);
    case 2:
        return scene2(p);
    default:
        return 0.0f;
    }
}

const float hardScale = 100.0f;
vec3 GetNormal(vec3 p, float scale) {
    float d = dstToScene(p);
    
    if(scale < 0.05f) scale = 0.05f;
//    vec2 e = vec2(((scale * scale) / hardScale), 0);
		vec2 e = vec2(0.00001, 0);
    vec3 n = d - vec3(
        dstToScene(p - e.xyy),
        dstToScene(p - e.yxy),
        dstToScene(p - e.yyx));

    return normalize(n);
}
uniform float hello;
uniform float u_testing;
void main()
{
    vec3 camPos = vec3(2.0, 1.0 + hello, 0.5);
    vec3 lookAt = vec3(0.0);
    float zoom = u_testing;

    vec2 uv = (v_UV + 1.0) / 2.0;

    ray camRay = create_camera_ray(v_UV, camPosition + vec3(hello, 0, 0), camLookAt + vec3(hello, 0, 0), zoom);

    float totalDist = 0.0;
    float finalDist = dstToScene(camRay.pos);
    int iters = 0;
    int maxIters = 4500;
    int wentInside = 0;

    vec4 color;

    for (iters = 0; iters < maxIters && finalDist >0.0001; iters++)
    {
        camRay.pos += finalDist * (camRay.dir) * 0.8f;
        totalDist += finalDist;
        finalDist = dstToScene(camRay.pos);
    }
    vec3 direction = vec3(0.1f, 0.7f, 0.2f);
    vec3 normal = GetNormal(camRay.pos, totalDist);
    vec3 lightPos = vec3(0.0, 1.0, 0.0);

    float dotSN = dot(normal, normalize(direction));
    
    dotSN = clamp(dotSN, 0.8f, 1.6f);
    //color = vec4(0.5 + 0.5 * normal, 1.0);
    float r = 59.0 / 255.0 * dotSN;
    float g = 169.0 / 255.0 * dotSN;
    float b = 209 / 255.0 * dotSN;
    
    float noisex = snoise(camRay.pos + (deltaTime) / 25.0f);
    float noisey = snoise(camRay.pos/2.0 + (deltaTime) / 2.0f);
    color = vec4(r, g, b, 1.0f );
    color.b *= 1.0f - sqrt((float(iters ) / float(1000)));
    color.r *= 1.0f - sqrt((float(iters + deltaTime/25.0f ) / float(1000)));
    color.g *= 1.0f - sqrt((float(iters * 2.0) / float(1000)));
    color.r += (noisex + 1.0) / 10.0;
    color.g += (noisey + 1.0) / 7.0;

    //color.r = mix(color.r, 0.5f,sin(deltaTime) * 0.5f);
	color = mix(color, vec4(totalDist / 1.0f), 0.0f);
    FragColor = color;
}



















