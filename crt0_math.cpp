#include "crt0_common.hpp"

CRT_NO_INTRINSIC(abs);
CRT_NO_INTRINSIC(fabs);
CRT_NO_INTRINSIC(ceil);
CRT_NO_INTRINSIC(floor);
CRT_NO_INTRINSIC(sqrt);
CRT_NO_INTRINSIC(sin);
CRT_NO_INTRINSIC(cos);
CRT_NO_INTRINSIC(tan);
CRT_NO_INTRINSIC(exp);
CRT_NO_INTRINSIC(log);
CRT_NO_INTRINSIC(log10);
CRT_NO_INTRINSIC(pow);
CRT_NO_INTRINSIC(fmod);
CRT_NO_INTRINSIC(fmodf);
CRT_NO_INTRINSIC(modf);
CRT_NO_INTRINSIC(ldexp);
CRT_NO_INTRINSIC(frexp);
CRT_NO_INTRINSIC(atan2);
CRT_NO_INTRINSIC(asin);
CRT_NO_INTRINSIC(acos);

CRT_NO_INTRINSIC(sinf);
CRT_NO_INTRINSIC(_dtest);
CRT_NO_INTRINSIC(_dclass);
CRT_NO_INTRINSIC(_ldtest);
CRT_NO_INTRINSIC(_fdtest);
CRT_NO_INTRINSIC(_fdclass);

CRT_NO_INTRINSIC(sqrtf);
CRT_NO_INTRINSIC(roundf);
CRT_NO_INTRINSIC(floorf);
CRT_NO_INTRINSIC(cosf);
CRT_NO_INTRINSIC(atan2f);
CRT_NO_INTRINSIC(asinf);

// Utility

#define CRT_PI 3.14159265358979323846
#define CRT_TWO_PI (CRT_PI * 2)
#define CRT_HALF_PI (CRT_PI / 2)
#define CRT_QUARTER_PI (CRT_PI / 4)

#define CRT_PI_F        3.1415927f
#define CRT_TWO_PI_F    (CRT_PI_F * 2.0f)
#define CRT_HALF_PI_F   (CRT_PI_F / 2.0f)
#define CRT_QUARTER_PI_F (CRT_PI_F / 4.0f)

#define ATAN_APPROX_A 0.28086

#define ASIN_COEF_3 0.16666666666666666
#define ASIN_COEF_5 0.075
#define ASIN_COEF_7 0.044642857142857144
#define ASIN_COEF_9 0.030381944444444444

static double atan_approx(double z)
{
	double az = fabs(z);
	return CRT_QUARTER_PI * z - z * (az - 1) * (ATAN_APPROX_A + ATAN_APPROX_A * az);
}

static double make_nan()
{
	union
	{
		unsigned long long u;
		double d;
	} nan_union = { 0x7FF8000000000000ULL };

	return nan_union.d;
}

static float make_nan_f()
{
	union
	{
		uint32_t u;
		float f;
	} nan_union = { 0x7FC00000U };
	
	return nan_union.f;
}

// Implementation

CRT_API int CRT_CALL abs(int x)
{
	return x < 0 ? -x : x;
}

CRT_API double CRT_CALL fabs(double x)
{
	return x < 0.0 ? -x : x;
}

CRT_API double CRT_CALL ceil(double x)
{
	if (!_finite(x))
		return make_nan();

	int i = static_cast<int>(x);
	return (x > static_cast<double>(i)) ? static_cast<double>(i + 1) : static_cast<double>(i);
}

CRT_API double CRT_CALL floor(double x)
{
	if (!_finite(x))
		return make_nan();

	int i = static_cast<int>(x);
	return (x < static_cast<double>(i)) ? static_cast<double>(i - 1) : static_cast<double>(i);
}

CRT_API double CRT_CALL sqrt(double x)
{
	if (x < 0.0 || !_finite(x))
		return make_nan();

	if (x == 0.0)
		return 0.0;

	if (x == INFINITY)
		return INFINITY;

	double guess = x / 2.0;
	for (int i = 0; i < 16; ++i)
		guess = (guess + x / guess) / 2.0;

	return guess;
}

CRT_API double CRT_CALL sin(double x)
{
	if (!_finite(x))
		return make_nan();

	double term = x;
	double result = x;
	double x2 = x * x;

	for (int i = 1; i < 10; ++i)
	{
		term *= -x2 / ((2 * i) * (2 * i + 1));
		result += term;
	}

	return result;
}

CRT_API double CRT_CALL cos(double x)
{
	if (!_finite(x))
		return make_nan();

	double term = 1.0;
	double result = 1.0;
	double x2 = x * x;

	for (int i = 1; i < 10; ++i)
	{
		term *= -x2 / ((2 * i - 1) * (2 * i));
		result += term;
	}

	return result;
}

CRT_API double CRT_CALL tan(double x)
{
	if (!_finite(x))
		return make_nan();

	double s = sin(x);
	double c = cos(x);

	if (c == 0.0)
		return (s >= 0.0 ? INFINITY : -INFINITY);

	return s / c;
}

CRT_API double CRT_CALL exp(double x)
{
	if (!_finite(x))
	{
		if (x > 0.0)
			return INFINITY;
		else if (x < 0.0)
			return 0.0;
		else
			return make_nan();
	}

	double result = 1.0;
	double term = 1.0;

	for (int i = 1; i < 20; ++i)
	{
		term *= x / i;
		result += term;
	}

	return result;
}

CRT_API double CRT_CALL frexp(double x, int *exp)
{
	if (!_finite(x))
	{
		if (exp) *exp = 0;
		return make_nan();
	}

	if (x == 0.0)
	{
		if (exp) *exp = 0;
		return 0.0;
	}

	float xf = static_cast<float>(x);
	int e = 0;

	while (xf >= 1.0f)
	{
		xf *= 0.5f;
		++e;
	}

	while (xf < 0.5f)
	{
		xf *= 2.0f;
		--e;
	}

	if (exp) *exp = e;
	return static_cast<double>(xf);
}

CRT_API double CRT_CALL log(double x)
{
	if (x < 0.0)
		return make_nan();

	if (x == 0.0)
		return -INFINITY;

	if (!_finite(x))
		return (x > 0.0) ? INFINITY : make_nan();

	int exponent = 0;
	double m = frexp(x, &exponent);

	double y = (m - 1) / (m + 1);
	double y2 = y * y;
	double result = 0.0;

	for (int i = 0; i < 20; ++i)
	{
		int n = 2 * i + 1;
		double term = 1.0 / n;
		for (int j = 0; j < n; ++j)
			term *= y;
		result += term;
	}

	double ln_m = 2.0 * result;
	const double LN2 = 0.6931471805599453;
	return ln_m + exponent * LN2;
}

CRT_API double CRT_CALL log10(double x)
{
	if (!_finite(x))
		return (x > 0.0) ? INFINITY : make_nan();

	if (x == 0.0)
		return -INFINITY;

	if (x < 0.0)
		return make_nan();

	const double LOG10E = 0.4342944819032518; // 1 / ln(10)
	return log(x) * LOG10E;
}

CRT_API double CRT_CALL pow(double base, double expv)
{
	if (!_finite(base) || !_finite(expv))
	{
		if (isnan(base) || isnan(expv))
			return make_nan();

		if (base == INFINITY)
			return (expv > 0.0) ? INFINITY : 0.0;

		if (expv == INFINITY)
			return (base > 1.0) ? INFINITY : 0.0;
	}

	if (base == 0.0 && expv == 0.0)
		return 1.0;

	if (base < 0.0)
		return make_nan();

	return exp(expv * log(base));
}

CRT_API double CRT_CALL fmod(double x, double y)
{
	if (!_finite(x) || !_finite(y) || y == 0.0)
		return make_nan();

	double div = x / y;
	int k = static_cast<int>(div);
	return x - k * y;
}

CRT_API double CRT_CALL modf(double x, double *iptr)
{
	if (!_finite(x))
	{
		*iptr = x;
		return make_nan();
	}

	int i = x >= 0.0 ? static_cast<int>(x) : static_cast<int>(x) - 1;
	*iptr = static_cast<double>(i);
	return x - *iptr;
}

CRT_API float CRT_CALL fmodf(float x, float y)
{
	if (y == 0.0f || !_finite(x) || !_finite(y))
		return make_nan_f();

	int sign = (x < 0.0f) ? -1 : 1;
	x = fabsf(x);
	y = fabsf(y);

	while (x >= y)
		x -= y;

	return sign * x;
}

CRT_API double CRT_CALL ldexp(double x, int exp)
{
	if (!_finite(x))
		return make_nan();

	return x * pow(2.0, static_cast<double>(exp));
}

CRT_API int CRT_CALL _finite(double _X)
{
	union
	{
		double d;
		uint64_t u;
	} value = { _X };

	uint64_t exponent_mask = 0x7FF0000000000000ULL;
	return (value.u & exponent_mask) != exponent_mask;
}

CRT_API double CRT_CALL atan2(double y, double x)
{
	if (x > 0)
		return atan_approx(y / x);
	else if (x < 0 && y >= 0)
		return atan_approx(y / x) + CRT_PI;
	else if (x < 0 && y < 0)
		return atan_approx(y / x) - CRT_PI;
	else if (x == 0 && y > 0)
		return CRT_HALF_PI;
	else if (x == 0 && y < 0)
		return -CRT_HALF_PI;

	return 0.0;
}

static double sqrt_newton(double x)
{
	if (x <= 0.0)
		return 0.0;

	double guess = x;
	for (int i = 0; i < 6; ++i)
		guess = 0.5 * (guess + x / guess);

	return guess;
}

CRT_API double CRT_CALL asin(double x)
{
	if (x < -1.0 || x > 1.0)
		return make_nan();

	int sign = 0;
	if (x < 0.0)
	{
		sign = 1;
		x = -x;
	}

	double result;

	if (x < 0.5)
	{
		double x2 = x * x;
		result = x + x * x2 * (ASIN_COEF_3 + x2 * (ASIN_COEF_5 + x2 * (ASIN_COEF_7 + x2 * ASIN_COEF_9)));
	}
	else
	{
		double z = sqrt_newton((1.0 - x) * 0.5);
		double z2 = z * z;
		double approximation = z + z * z2 * (ASIN_COEF_3 + z2 * (ASIN_COEF_5 + z2 * (ASIN_COEF_7 + z2 * ASIN_COEF_9)));
		result = CRT_HALF_PI - 2.0 * approximation;
	}

	return sign ? -result : result;
}

CRT_API double CRT_CALL acos(double x)
{
	if (x < -1.0 || x > 1.0)
		return make_nan();

	return CRT_HALF_PI - asin(x);
}

CRT_API float CRT_CALL sinf(float x)
{
	x = fmodf(x, CRT_TWO_PI_F);

	if (x > CRT_PI_F)
		x -= CRT_TWO_PI_F;

	if (x < -CRT_PI_F)
		x += CRT_TWO_PI_F;

	float x2 = x * x;
	float result = x;

	float term = x;
	term *= -x2 / (2.0f * 3.0f);
	result += term;

	term *= -x2 / (4.0f * 5.0f);
	result += term;

	term *= -x2 / (6.0f * 7.0f);
	result += term;

	term *= -x2 / (8.0f * 9.0f);
	result += term;

	return result;
}

CRT_API short CRT_CALL _dtest(double *_Px)
{
	union
	{
		double d;
		uint64_t u;
	} value = { *_Px };

	uint64_t x = value.u;

	uint64_t exponent = (x >> 52) & 0x7FF;
	uint64_t mantissa = x & 0xFFFFFFFFFFFFFull;

	if (exponent == 0x7FF)
	{
		if (mantissa != 0)
			return 2; // NaN
		else
			return 1; // Infinity
	}
	else if (exponent != 0)
	{
		return -1; // Normalized number
	}
	else if ((x << 1) != 0)
	{
		return -2; // Denormalized number
	}
	else
	{
		return 0; // Zero
	}
}

CRT_API short CRT_CALL _dclass(double _X)
{
	union
	{
		double d;
		uint64_t u;
	} value = { _X };

	uint64_t x = value.u;

	uint64_t exponent = (x >> 52) & 0x7FF;
	uint64_t mantissa = x & 0xFFFFFFFFFFFFFull;

	if (exponent == 0x7FF)
	{
		if (mantissa != 0)
			return 2; // NaN
		else
			return 1; // Infinity
	}
	else if (exponent != 0)
	{
		return -1; // Normalized number
	}
	else if ((x << 1) != 0)
	{
		return -2; // Denormalized number
	}
	else
	{
		return 0; // Zero
	}
}

CRT_API short CRT_CALL _ldtest(long double *_Px)
{
	return _dtest(reinterpret_cast<double *>(_Px));
}

CRT_API short CRT_CALL _fdtest(float *_Px)
{
	union
	{
		float f;
		uint32_t u;
	} value = { *_Px };

	uint32_t x = value.u;
	uint32_t exponent = (x >> 23) & 0xFF;
	uint32_t mantissa = x & 0x7FFFFF;

	if (exponent == 0xFF)
	{
		if (mantissa != 0)
			return 2; // NaN
		else
			return 1; // Infinity
	}
	else if (exponent != 0)
	{
		return -1; // Normalized number
	}
	else if ((x << 1) != 0)
	{
		return -2; // Denormalized number
	}
	else
	{
		return 0; // Zero
	}
}

CRT_API short CRT_CALL _fdclass(float _X)
{
	return _fdtest(&_X);
}

CRT_API float CRT_CALL sqrtf(float _X)
{
	if (_X < 0.0f)
		return make_nan_f();

	if (_X == 0.0f)
		return 0.0f;

	float guess = _X;
	const float epsilon = 1e-6f;

	for (int i = 0; i < 20; ++i)
	{
		float next = 0.5f * (guess + _X / guess);

		if (fabsf(next - guess) < epsilon)
			break;

		guess = next;
	}

	return guess;
}

CRT_API float CRT_CALL roundf(float _X)
{
	int i = (int)(_X >= 0.0f ? _X + 0.5f : _X - 0.5f);
	return (float)i;
}

CRT_API float CRT_CALL floorf(float _X)
{
	int i = (int)_X;

	if (_X < 0.0f && _X != (float)i)
		i -= 1;

	return (float)i;
}


CRT_API float CRT_CALL cosf(float x)
{
	x = x - CRT_TWO_PI_F * floorf(x / CRT_TWO_PI_F);

	if (x > CRT_PI_F)
		x -= CRT_TWO_PI_F;

	if (x < -CRT_PI_F)
		x += CRT_TWO_PI_F;

	int sign = 1;

	if (x < 0.0f)
		x = -x;

	if (x > CRT_HALF_PI_F)
	{
		x = CRT_PI_F - x;
		sign = -1;
	}

	float x2 = x * x;

	float result = 1.0f
		- x2 * (1.0f / 2.0f)
		+ x2 * x2 * (1.0f / 24.0f)
		- x2 * x2 * x2 * (1.0f / 720.0f);

	return sign * result;
}

CRT_API float CRT_CALL atan2f(float _Y, float _X)
{
	const float n1 = 0.97239411f;
	const float n2 = -0.19194795f;

	if (_X != _X || _Y != _Y)
		return make_nan_f();

	if (_X == 0.0f && _Y == 0.0f)
		return make_nan_f();

	float abs_y = fabsf(_Y);
	float angle;

	if (fabsf(_X) > abs_y)
	{
		float z = _Y / _X;
		angle = (n1 + n2 * z * z) * z;

		if (_X < 0.0f)
			angle += (_Y >= 0.0f) ? CRT_PI_F : -CRT_PI_F;
	}
	else
	{
		if (_Y == 0.0f)
			return (_X >= 0.0f) ? 0.0f : CRT_PI_F;

		float z = _X / _Y;
		angle = CRT_HALF_PI_F - (n1 + n2 * z * z) * z;

		if (_Y < 0.0f)
			angle -= CRT_PI_F;
	}

	return angle;
}

CRT_API float CRT_CALL asinf(float x)
{
	if (x != x || x < -1.0f || x > 1.0f)
		return make_nan_f();

	float negate = (float)(x < 0.0f);
	x = fabs(x);

	float ret = -0.0187293f;
	ret *= x;
	ret += 0.0742610f;
	ret *= x;
	ret -= 0.2121144f;
	ret *= x;
	ret += 1.5707288f;

	ret = CRT_HALF_PI_F - sqrtf(1.0f - x) * ret;

	return ret - 2.0f * negate * ret;
}