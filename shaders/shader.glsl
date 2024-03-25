#version 330 core
#define MAX_ITERATIONS 256
#define GRADIENT_SIZE 5
 
uniform vec2 ScreenSize;
uniform vec2 CameraPosition;
uniform float Scale;

//TODO: multiple gradients (pain in my ass)

const vec3 Gradient [GRADIENT_SIZE] = vec3 [GRADIENT_SIZE](
    vec3 (0.000, 0.027, 0.392),
    vec3 (0.125, 0.420, 0.796),
    vec3 (0.929, 1.000, 1.000),
    vec3 (1.000, 0.667, 0.000),
    vec3 (0.000, 0.008, 0.000)
);

int GetIterations()
{
    float x0 = (gl_FragCoord.x / ScreenSize.x - 0.5) * Scale + CameraPosition.x;
    float y0 = (gl_FragCoord.y / ScreenSize.y - 0.5) * Scale - CameraPosition.y;
 
    int iterations = 0;
    float xN = x0;
    float yN = y0;
 
    while (iterations < MAX_ITERATIONS)
    {
        float x2 = xN * xN;
        float y2 = yN * yN;
        float xy = xN * yN;

        xN = (x2 - y2)  + x0;
        yN = (2.0 * xy) + y0;
         
        float dist = x2 + y2;
         
        if (dist > 10.0)
        break;
 
        iterations++;
    }
    return iterations;
}

vec4 ReturnColor()
{
    int iterationCount = GetIterations ();

    if (iterationCount >= MAX_ITERATIONS) {
        return vec4 (0.0, 0.0, 0.0, 1.0);
    }

    float mu = float (iterationCount) / float (MAX_ITERATIONS) * float (GRADIENT_SIZE);
    
    int clr1 = int (mu);
    float t2 = mu - clr1;
    float t1 = 1 - t2;
    clr1 = clr1 % GRADIENT_SIZE;
    int clr2 = (clr1 + 1) % GRADIENT_SIZE;

    return vec4 (Gradient [clr1].x * t1 + Gradient [clr2].x * t2,
                 Gradient [clr1].y * t1 + Gradient [clr2].y * t2, 
                 Gradient [clr1].z * t1 + Gradient [clr2].z * t2, 1.0f);
}

void main () {
    gl_FragColor = ReturnColor ();
}




