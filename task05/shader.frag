#version 120

// see the GLSL 1.2 specification:
// https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.1.20.pdf

// jet colormap
const int ncolormap = 9;
vec3[ncolormap] colormap = vec3[](
  vec3(0.0, 0.0, 0.5),
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 0.5, 1.0),
  vec3(0.0, 1.0, 1.0),
  vec3(0.5, 1.0, 0.5),
  vec3(1.0, 1.0, 0.0),
  vec3(1.0, 0.5, 0.0),
  vec3(1.0, 0.0, 0.0),
  vec3(0.5, 0.0, 0.0) );

// evaluate n-degree polynominal at x
float EvaluatePolynomial(
  float x,
  const float a[6],
  int n) {
  float v = a[n - 1];
  for (int i = 1; i < n; ++i) {
    v = v * x + a[n - 1 - i];
  }
  return v;
}

// function to compute Sturm sequence from coefficients of quintic polynominal
void SturmSequenceOfPolynomial(
  inout float strum[6*6],
  const float coe[6]) {
  const int n = 6;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (i == 0) {
        strum[i*n+j] = coe[j];
      } else if (i == 1 && j < n - 1) {
        strum[i*n+j] = (j + 1) * coe[j + 1];
      } else {
        strum[i*n+j] = 0.;
      }
    }
  }

  for (int i = 0; i < n - 2; i++) {
    int j = i + 1, k1 = n - i - 1, k2 = n - j - 1;
    while (strum[i*n+k1] == 0) {
      k1--;
      if (k1 < 0) { return; }
    }
    while (strum[j*n+k2] == 0) {
      k2--;
      if (k2 < 0) { return; }
    }
    float poly[n];
    for (int l = 0; l < n; l++) {
      poly[l] = strum[i*n+l];
    }
    while (k1 >= k2) {
      while (poly[k1] == 0.) {
        k1--;
        if (k1 < k2) { break; }
      }
      if (k1 >= k2) {
        float quotient = poly[k1] / strum[j*n+k2];
        poly[k1] = 0.;
        for (int l = 1; l <= k2; l++) {
          poly[k1 - l] -= quotient * strum[j*n+k2 - l];
        }
      }
    }
    for (int l = 0; l < n; l++) {
      strum[(j + 1)*n+l] = -poly[l];
    }
  }
}

// function to compute sturm number at `x` using Sturm sequence
int SturmNumber(
  float x,
  const float s[36]) {
  const int n = 6;
  float v[n];
  // evaluate sturm sequence at x
  for (int i = 0; i < n; ++i) {
    float[n] ss = float[n](
      s[i*n+0],
      s[i*n+1],
      s[i*n+2],
      s[i*n+3],
      s[i*n+4],
      s[i*n+5]);
    v[i] = EvaluatePolynomial(x, ss, n - i);
  }
  // count the number of sign change
  int root_number = 0;
  float prev = 0.;
  for (int i = 0; i < n; ++i) {
    if (v[i] != 0.0) {
      if (prev != 0. && v[i] * prev < 0) { ++root_number; }
      prev = v[i];
    }
  }
  return root_number;
}

vec2 EvaluateBezier(
  float t,
  const vec2 a[4] )
{
  return (t*t*t)*a[0] + (t*t)*a[1] + t*a[2] + a[3];
}

// coordinates of the points specified by CPU
uniform vec2 p0, p1, p2, p3;

void main()
{
  vec2 q = 2 * gl_FragCoord.xy / 500 - 1; // position (left is zero)

  // Bezier curve with control points {p0,p1,p2,p3} look from q
  // p(t) = (1-t)^3*p0 + 3*(1-t)^2t*p1 + 3*(1-t)t^2*p2 + t^3*p3 - q;
  //      = t^3*bezier[3] + t^2*bezier[2] + t*bezier[1] + bezier[0];
  vec2[4] bezier = vec2[4] (
    -p0 + 3 * p1 - 3 * p2 + p3,
    3 * p0 - 6 * p1 + 3 * p2,
    -3 * p0 + 3 * p1,
    p0 - q);

  // compute coefficients of polynominal
  // this polynominal stands for the derivative of squared distance
  // l(t) = d||p(t)||^2 / dt
  //      = coeff[0] + coeff[1]*t + coeff[2]*t^2 + coeff[3]*t^3 + coeff[4]*t^4 + coeff[5]*t^5
  float[6] coeff = float[6](
    dot(bezier[2],bezier[3]),
    dot(bezier[2],bezier[2]) + 2 * dot(bezier[1],bezier[3]),
    3 * dot(bezier[1],bezier[2]) + 3 * dot(bezier[0],bezier[3]),
    2 * dot(bezier[1],bezier[1]) + 4 * dot(bezier[0],bezier[2]),
    5 * dot(bezier[0],bezier[1]),
    3 * dot(bezier[0],bezier[0]) );

  // compute Sturm sequence
  // Using this Sturm sequence, number of roots of the polynominal
  // in the range [x_0,x_1] cam be computed as
  // N = SturmNumber(x_0,sturm) - SturmNumber(x_1,sturm)
  float[36] sturm_seq;
  SturmSequenceOfPolynomial(sturm_seq,coeff);

  // initialize distance
  float Distance = length(p3-q);
  Distance = min(Distance,length(p0-q));

  // the following three lines of function is not used for Problem2
  // The code is here to give the idea what the cubic Bezier curve looks like
  for(int i=0;i<10;++i){
    Distance = min(Distance, length(EvaluateBezier(0.1*i, bezier)));
  }

  // this is the structure to store the range
  struct range {
    float lower; // lower bound
    float upper; // upper bound
    int sturm_lower; // sturm number at lower bound
    int sturm_upper; // sturm number at upper bound
  };
  range[64] stack;
  int nstack = 1;
  stack[0] = range(0., 1., SturmNumber(0.,sturm_seq), SturmNumber(1.,sturm_seq)); // initial range
  while(nstack>0){ // finding roots using bisection method
    nstack = nstack-1;
    float lower = stack[nstack].lower;
    float upper = stack[nstack].upper;
    float middle = (lower + upper)*0.5;
    if( upper - lower < 0.0001 ){
      vec2 pm = EvaluateBezier(middle,bezier);
      Distance = min(Distance,length(pm));
      continue;
    }
    int snl = stack[nstack].sturm_lower;
    int snu = stack[nstack].sturm_upper;
    if( snl == snu ){ continue; }
    // Problem2 of the assignment
    // write some code to complete the implementation of bisection method
    // around 10 lines of code should be enough
  }


  { // paint the distance with color
    float d0 = 10*Distance-floor(10*Distance);
    if( d0 < 0.1 ){ gl_FragColor = vec4(0, 0, 0, 1); return; } // contour line
    // computing color map
    float len1 = Distance * 5;
    int imap = int(len1);
    float r = len1 - imap;
    if (imap > ncolormap-2){
      r = 1;
      imap = ncolormap-2;
    }
    vec3 c0 = colormap[imap];
    vec3 c1 = colormap[imap+1];
    vec3 c = (1-r) * c0 + r * c1;
    gl_FragColor = vec4(c, 1);
  }
}
