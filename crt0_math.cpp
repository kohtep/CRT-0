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
CRT_NO_INTRINSIC(modf);
CRT_NO_INTRINSIC(ldexp);
CRT_NO_INTRINSIC(frexp);
CRT_NO_INTRINSIC(atan2);
CRT_NO_INTRINSIC(asin);
CRT_NO_INTRINSIC(acos);

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
	int i = static_cast<int>(x);
	return (x > static_cast<double>(i)) ? static_cast<double>(i + 1) : static_cast<double>(i);
}

CRT_API double CRT_CALL floor(double x)
{
	int i = static_cast<int>(x);
	return (x < static_cast<double>(i)) ? static_cast<double>(i - 1) : static_cast<double>(i);
}

CRT_API double CRT_CALL sqrt(double x)
{
	if (x <= 0.0)
		return 0.0;

	double guess = x / 2.0;
	for (int i = 0; i < 16; ++i)
		guess = (guess + x / guess) / 2.0;

	return guess;
}

CRT_API double CRT_CALL sin(double x)
{
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
	double s = sin(x);
	double c = cos(x);
	return c != 0.0 ? s / c : 0.0;
}

CRT_API double CRT_CALL exp(double x)
{
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
	if (x == 0.0)
	{
		if (exp) *exp = 0;
		return 0.0;
	}

	float xf = static_cast<float>(x);
	int e = 0;

	while (xf >= 1.0f)
	{
		xf = xf * 0.5f;
		++e;
	}

	while (xf < 0.5f)
	{
		xf = xf * 2.0f;
		--e;
	}

	if (exp) *exp = e;
	return static_cast<double>(xf);
}

CRT_API double CRT_CALL log(double x)
{
	if (x <= 0.0)
		return 0.0;

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
	const double LOG10E = 0.4342944819032518;  // 1 / ln(10)
	return log(x) * LOG10E;
}

CRT_API double CRT_CALL pow(double base, double expv)
{
	if (base <= 0.0)
		return 0.0;
	return exp(expv * log(base));
}

CRT_API double CRT_CALL fmod(double x, double y)
{
	if (y == 0.0)
		return 0.0;

	double div = x / y;
	int k = static_cast<int>(div);
	return x - k * y;
}

CRT_API double CRT_CALL modf(double x, double *iptr)
{
	int i = x >= 0.0 ? static_cast<int>(x) : static_cast<int>(x) - 1;
	*iptr = static_cast<double>(i);
	return x - *iptr;
}

CRT_API double CRT_CALL ldexp(double x, int exp)
{
	float xf = static_cast<float>(x);
	float result = xf * static_cast<float>(pow(2.0, static_cast<double>(exp)));
	return static_cast<double>(result);
}

#define CRT_PI 3.14159265358979323846
#define CRT_HALF_PI (CRT_PI / 2)
#define CRT_QUARTER_PI (CRT_PI / 4)

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