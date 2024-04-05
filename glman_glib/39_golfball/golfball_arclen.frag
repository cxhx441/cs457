#version 330 compatibility

// uniform float uA;
in vec2  vST;
in vec3  vMC;
in vec3  vColor;
in float vLightIntensity;

uniform float uRadius;
uniform float uFactor;

const vec3 WHITE = vec3( 1., 1., 1. );
const float PI = 3.14159265;


//circle_equation = (s-s0)^2 + (t-t0)^2 = r^2
vec3 sphericalToCartesian(float theta, float phi) {
    return vec3(sin(phi) * cos(theta),
                cos(phi),
                sin(phi) * sin(theta));
}

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
    //const int gridResolution = 8; // at equator
    //const float circlePatternRadius = 2 * PI / 200; // thru trial and error
    float arcLen = uRadius * 2.f;
    int gridResolution = int(1.f / uRadius); // at equator
    //gridResolution = 8;

    // calc lat and long of cur frag
    float theta = atan2(vMC.z, vMC.x); // phi
    float phi = acos(vMC.y); // theta // this works because H is unit vector (normalized)

    // calculate the lat and long steps
    //int adjGridResTheta = int(gridResolution * gridResThetaFactor);
    //int adjGridResPhi = int(gridResolution * gridResPhiFactor);
    //float thetaStep = 2 * PI / float(adjGridResTheta);
    //float phiStep = PI / float(adjGridResPhi);
    float phiStep = arcLen * PI;
    float quantizedPhi = round(phi / phiStep) * phiStep;

    // Calculate the closest circle center base on lat and long (quantize?)
    float thetaStep = arcLen * PI / sin(quantizedPhi);
    float quantizedTheta = round(theta / thetaStep) * thetaStep;

    // calc dist from frag to closest circle center
    float distanceToCenter = distance(vMC, sphericalToCartesian(quantizedTheta, quantizedPhi));

    // If the distance is within the circle radius, color the fragment
    //if (distanceToCenter <= (0.25 * abs(vMC.y) * PI / gridResolution)) {
    if (distanceToCenter <= arcLen * uFactor) {
        gl_FragColor = vec4(1.f);
    } else {
        gl_FragColor = vec4(0.f, 0.f, 0.f, 1.f);
    }

    //gl_FragColor = vec4( rgb, 1. );
}
