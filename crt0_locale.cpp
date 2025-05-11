#include "crt0_common.hpp"

#include <limits>
#include <string>
#include <locale.h>

CRT_API struct lconv *CRT_CALL localeconv()
{
	static struct lconv lc;

	lc.decimal_point = const_cast<char *>(".");
	lc.thousands_sep = const_cast<char *>("");
	lc.grouping = const_cast<char *>("");

	lc.int_curr_symbol = const_cast<char *>("");
	lc.currency_symbol = const_cast<char *>("");
	lc.mon_decimal_point = const_cast<char *>("");
	lc.mon_thousands_sep = const_cast<char *>("");
	lc.mon_grouping = const_cast<char *>("");
	lc.positive_sign = const_cast<char *>("");
	lc.negative_sign = const_cast<char *>("");

	lc.int_frac_digits = CHAR_MAX;
	lc.frac_digits = CHAR_MAX;
	lc.p_cs_precedes = CHAR_MAX;
	lc.p_sep_by_space = CHAR_MAX;
	lc.n_cs_precedes = CHAR_MAX;
	lc.n_sep_by_space = CHAR_MAX;
	lc.p_sign_posn = CHAR_MAX;
	lc.n_sign_posn = CHAR_MAX;

	return &lc;
}

static char c_locale[] = "C";

CRT_API char *CRT_CALL setlocale(int category, const char *locale)
{
	if (locale == nullptr)
		return c_locale;

	if (locale[0] == '\0' || strcmp(locale, "C") == 0)
		return c_locale;

	return nullptr;
}