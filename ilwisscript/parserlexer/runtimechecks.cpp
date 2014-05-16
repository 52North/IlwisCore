extern "C" {
#ifdef Q_OS_WIN
#define NULL 0
typedef int (__cdecl *_RTC_error_fn)(int, const char *, int, const char *, const char *, ...);
_RTC_error_fn _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4);
}
_RTC_error_fn _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4)
{
return NULL;
#endif
}









