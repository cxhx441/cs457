#version 330 compatibility

// uniform float uA;
in vec2  vST;
in vec3  vMC;
in vec3  vColor;
in float vLightIntensity;

const vec3 WHITE = vec3( 1., 1., 1. );
const float PI = 3.14159265;


//circle_equation = (s-s0)^2 + (t-t0)^2 = r^2

float
atan2(float y, float x)
{
    if ( x == 0. )
    {
        if ( y >= 0. ) return PI/2.;
        else return -PI/2.;
    }
    return atan(y, x);
}

void
main( )
{
    const int gridResolution = 8;

    // calculate the lat and long steps
    float thetaStep = PI / float(gridResolution);
    float phiStep = 2.0 * PI / float(gridResolution);

    // calc lat and long of cur frag
    //float theta = atan2(vMC.x, vMC.z); // theta
    //float phi = atan2(vMC.x, vMC.y); // phi
    float theta = acos(vMC.y); // theta
    float phi = atan2(vMC.z, vMC.x); // phi

    // Calculate the closest circle center base on lat and long
    float closestTheta = round(theta / thetaStep) * thetaStep;
    float closestPhi = round(phi / phiStep) * phiStep;

    // calc dist from frag to cosest circle center
    float distanceToCenter = distance(vMC, vec3(sin(closestTheta) * cos(closestPhi),
                                                cos(closestTheta),
                                                sin(closestTheta) * sin(closestPhi)));

    // If the distance is within the circle radius, color the fragment
    if (distanceToCenter <= (0.25 * abs(vMC.y) * PI / gridResolution)) {
        gl_FragColor = vec4(1.f);
    } else {
        discard;
    }

    //gl_FragColor = vec4( rgb, 1. );
}
