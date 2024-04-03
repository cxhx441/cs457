#version 330 compatibility

// uniform float uA;
uniform int uCount;
uniform float uTol;

in vec2  vST;
in vec3  vMC;
in vec3  vColor;
in float vLightIntensity;

float uDiam = 1. / uCount;

const vec3 WHITE = vec3( 1., 1., 1. );


//circle_equation = (s-s0)^2 + (t-t0)^2 = r^2

void
main( )
{
    const int gridResolution = 8;

    // calculate the lat and long steps
    //float latStep = 2.0 * 3.14159265359 / float(gridResolution);
    //float longStep = 3.14159265359 / float(gridResolution);
    float latStep = 2.0 * 3.14159265359 / float(gridResolution);
    float longStep = 3.14159265359 / float(gridResolution);

    // calc lat and long of cur frag
    float latitude = atan(vMC.y, vMC.x);
    //float longitude = atan(vMC.z, length(vMC.xy));
    float longitude = atan(vMC.z, vMC.x);
    //float latitude = asin(vMC.x);
    //float longitude = atan(vMC.x, vMC.z);

    // Calculate the closest circle center base on lat and long
    float closestLatitude = round(latitude / latStep) * latStep;
    float closestLongitude = round(longitude / longStep) * longStep;

    // calc dist from frag to cosest circle center
    float distanceToCenter = distance(vMC, vec3(sin(closestLatitude) * cos(closestLongitude),
                                                cos(closestLatitude),
                                                sin(closestLatitude) * sin(closestLongitude)));

    // If the distance is within the circle radius, color the fragment
    if (abs(distanceToCenter - 0) < 0.5) {
        gl_FragColor = vec4(1.f);
    } else {
        discard;
    }

    //gl_FragColor = vec4( rgb, 1. );
}
