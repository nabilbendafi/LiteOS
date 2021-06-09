#include "time_impl.h"
#include <errno.h>
#include "tzdst_pri.h"
#include "lock.h"

#ifdef __LITEOS__
LIBC_LOCK_INIT(g_tzdstLock);
#endif

time_t mktime(struct tm *tm)
{
#ifndef __LITEOS__
	struct tm new;
	long opp;
	long long t = __tm_to_secs(tm);

	__secs_to_zone(t, 1, &new.tm_isdst, &new.__tm_gmtoff, &opp, &new.__tm_zone);

	if (tm->tm_isdst>=0 && new.tm_isdst!=tm->tm_isdst)
		t -= opp - new.__tm_gmtoff;

	t -= new.__tm_gmtoff;
	if ((time_t)t != t) goto error;

	__secs_to_zone(t, 0, &new.tm_isdst, &new.__tm_gmtoff, &opp, &new.__tm_zone);

	if (__secs_to_tm(t + new.__tm_gmtoff, &new) < 0) goto error;

	*tm = new;
	return t;

error:
	errno = EOVERFLOW;
	return -1;
#else
	long long t = __tm_to_secs(tm);
	int dstsec = 0;

	(void)LIBC_LOCK(g_tzdstLock);
	if (tm->tm_isdst != 0) {
		if (CheckWithinDstPeriod(tm, 0) == TRUE)
			dstsec = DstForwardSecondGet();
		tm->tm_isdst = 0;
	}
	t = t - timezone - dstsec;
	(void)LIBC_UNLOCK(g_tzdstLock);
	if ((time_t)t != t) goto error;

	return t;

error:
	errno = EOVERFLOW;
	return -1;
#endif
}
