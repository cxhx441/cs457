#version 330 compatibility

uniform int uMandelbrotVSJulia; // 0 for mandelbrot, 1 for julia
uniform int uMaxIterations; // 0 for mandelbrot, 1 for julia
uniform float uScale, uOffsetS, uOffsetT; // 0 for mandelbrot, 1 for julia
uniform float uJuliaConstantX, uJuliaConstantY; // 0 for mandelbrot, 1 for julia
uniform float uTrapDist;
uniform bool uSameJuliaXY; // y = x
in vec2 vST;

vec2 squareImaginary(vec2 number){
	return vec2(
		pow(number.x,2)-pow(number.y,2),
		2*number.x*number.y
	);
}

float iterateMandelbrot(vec2 coord){
    // chatgpt implementation
    vec2 z = coord;
    int iterations;
    for (int i = 0; i < uMaxIterations; ++i) {
        if (dot(z, z) > 4.0) break; // dot(z, z) == length(z) * length(z). don't forget

        // Mandelbrot formula: z = z^2 + c
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + coord;
        iterations = i;

        // orbit trap
        if (length(z) < uTrapDist) break; // orbit trap (optional, but recommended for performance
    }

    return float(iterations) / float(uMaxIterations);
}

float iterateJuliaSet(vec2 coord, vec2 juliaConst){
    // If c is held constant, the result is the Julia set -- Wikipedia
    // chatgpt implementation
    vec2 z = coord;
    int iterations;
    for (int i = 0; i < uMaxIterations; ++i) {
        if (dot(z, z) > 4.0) break; // dot(z, z) == length(z) * length(z). don't forget

        // Julia formula: z = z^2 + juliaConst
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + juliaConst; // uJuliaConstant is the only difference
        iterations = i;

        // orbit trap
        if (length(z) < uTrapDist) break; // orbit trap (optional, but recommended for performance
    }

    return float(iterations) / float(uMaxIterations);
}

void main()
{
    // set st
    vec2 st = vST + vec2(uOffsetS, uOffsetT);
    st = (st - 0.5) / uScale; //make centered

    // set julia constant
    float juliaConstX = uJuliaConstantX;
    float juliaConstY = uJuliaConstantY;
    if (uSameJuliaXY) juliaConstY = juliaConstX;

    // get mandelbrot/julia value
    float val;
    if (uMandelbrotVSJulia == 0) val = iterateMandelbrot(st);
    else val = iterateJuliaSet(st, vec2(juliaConstX, juliaConstY));


    gl_FragColor = vec4(val, val, val, 1);
}
