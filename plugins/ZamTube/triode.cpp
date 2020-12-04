#include <stdio.h>
#include <inttypes.h>
#include <cmath>
#include "triode.h"
using std::abs;
#define DUMP(x) x

T Triode::compute(T a, T R, T Vg, T Vk) {
	T Vgk = Vg - Vk;
	T VakGuess = -mu * Vgk;
	T Vak = VakGuess;
	int iteration = 0;
	T err = 1e6;
	T b;

	for (iteration = 0; (fabs(err)/fabs(Vak) > EPSILON) && (iteration <= ITER); iteration++){
		VakGuess = iterateNewtonRaphson(Vak, TOLERANCE, Vgk, a, R);
		err = Vak - VakGuess;
		Vak = VakGuess;
	}
	b = Vak - R*getIa(Vgk, Vak);

	//printf("Vgate=%f Vk=%f  Vgk=%f b=%f\n", Vgate, Vk, Vgk, b);
	return b;
}

T Triode::getIa(T Vgk, T Vpk) const {
	if (Vpk < 0.0) {
		Vpk = 0.0;
	}
	if (Vgk > 0.0) {
		Vgk = 0.0;
	}

	/* exact solution (expensive) */
	T ee1 = Vpk*log1p(exp(kp*(1./mu+Vgk/sqrt(kvb+Vpk*Vpk))))/kp;
	if (ee1 < 0) {
		return 0.;
	}
	//printf("Vpk=%f ans=%f e1=%f exact_e1=%f\n", Vpk, ans, e1, ee1);
	return 1e+6*pow(ee1, kx) / kg1;
}

T Triode::iterateNewtonRaphson(T x, T dx, T Vgk, T a, T R) const {
	T xIak = getIa(Vgk, x);
	T dxIak = getIa(Vgk, x + dx);
	T xNew = x - dx*(x + R*xIak - a)/(dx + R*(dxIak - xIak));
	return xNew;
}

Triode::Triode()
{
	/* good for low gain, broken at high gain
	kvb = 300.;
	mu = 103.2;
	kx = 1.26;
	kg1 = 446.0;
	kp = 3.4;
	*/
	
	//12AX7 RSD-1 (custom)
	mu = 100.;
	kx = 1.4;
	kg1 = 1060.;
	kp = 600.;
	kvb = 300.;
}
