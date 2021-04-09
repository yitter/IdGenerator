module yitter.core.DateTimeHelper;

import core.stdc.time;
import std.datetime : convert;
import std.string;

enum TimeUnit : string {
    Year = "years",
    Month = "months",
    Week = "weeks",
    Day = "days",
    Hour = "hours",
    Second = "seconds",
    Millisecond = "msecs",
    Microsecond = "usecs",
    HectoNanosecond = "hnsecs",
    Nanosecond = "nsecs"
}


/**
 * 
 */
class DateTimeHelper {
    /**
     * Returns the current time in milliseconds.  Note that
     * while the unit of time of the return value is a millisecond,
     * the granularity of the value depends on the underlying
     * operating system and may be larger.  For example, many
     * operating systems measure time in units of tens of
     * milliseconds.
     *
     * <p> See the description of the class {@code Date} for
     * a discussion of slight discrepancies that may arise between
     * "computer time" and coordinated universal time (UTC).
     *
     * @return  the difference, measured in milliseconds, between
     *          the current time and midnight, January 1, 1970 UTC.
     */
    static long currentTimeMillis() @trusted @property {
        return currentTime!(TimeUnit.Millisecond)();
    }

    static long currentTimeNsecs() @trusted @property {
        return currentTime!(TimeUnit.Nanosecond)();
    }

    static long currentUnixTime() @trusted @property {
        return currentTime!(TimeUnit.Second)();
    }

    alias currentTimeSecond = currentUnixTime;

    /**
     * 
     */
    static long currentTime(TimeUnit targetUnit)() @trusted @property {
        version (Windows) {
            import core.sys.windows.winbase;
            import core.sys.windows.winnt;

            /**
                http://www.frenk.com/2009/12/convert-filetime-to-unix-timestamp/
                https://stackoverflow.com/questions/10849717/what-is-the-significance-of-january-1-1601
                https://stackoverflow.com/questions/1090869/why-is-1-1-1970-the-epoch-time
                https://www.unixtimestamp.com/
            */
            FILETIME fileTime;
            GetSystemTimeAsFileTime(&fileTime);
            ULARGE_INTEGER date, adjust;
            date.HighPart = fileTime.dwHighDateTime;
            date.LowPart = fileTime.dwLowDateTime;

            // 100-nanoseconds = milliseconds * 10000
            adjust.QuadPart = 11644473600000 * 10000;

            // removes the diff between 1970 and 1601
            date.QuadPart -= adjust.QuadPart;

            // converts back from 100-nanoseconds to milliseconds
            return convert!(TimeUnit.HectoNanosecond, targetUnit)(date.QuadPart);

        } else version (Posix) {
                import core.sys.posix.signal : timespec;
            version (OSX) {
                import core.sys.posix.sys.time : gettimeofday, timeval;

                timeval tv = void;
                // Posix gettimeofday called with a valid timeval address
                // and a null second parameter doesn't fail.
                gettimeofday(&tv, null);
                return convert!(TimeUnit.Second, targetUnit)(tv.tv_sec) + 
                    convert!(TimeUnit.Microsecond, targetUnit)(tv.tv_usec);

            } else version (linux) {
                    import core.sys.linux.time : CLOCK_REALTIME_COARSE;
                    import core.sys.posix.time : clock_gettime, CLOCK_REALTIME;

                    timespec ts = void;
                    immutable error = clock_gettime(CLOCK_REALTIME, &ts);
                    // Posix clock_gettime called with a valid address and valid clock_id is only
                    // permitted to fail if the number of seconds does not fit in time_t. If tv_sec
                    // is long or larger overflow won't happen before 292 billion years A.D.
                    static if (ts.tv_sec.max < long.max) {
                        if (error)
                            throw new TimeException("Call to clock_gettime() failed");
                    }
                    return convert!(TimeUnit.Second, targetUnit)(ts.tv_sec) + 
                        convert!(TimeUnit.Nanosecond, targetUnit)(ts.tv_nsec);

            } else version (FreeBSD) {
                import core.sys.freebsd.time : clock_gettime, CLOCK_REALTIME;

                timespec ts = void;
                immutable error = clock_gettime(CLOCK_REALTIME, &ts);
                // Posix clock_gettime called with a valid address and valid clock_id is only
                // permitted to fail if the number of seconds does not fit in time_t. If tv_sec
                // is long or larger overflow won't happen before 292 billion years A.D.
                static if (ts.tv_sec.max < long.max) {
                    if (error)
                        throw new TimeException("Call to clock_gettime() failed");
                }
                return convert!(TimeUnit.Second, targetUnit)(ts.tv_sec) + 
                        convert!(TimeUnit.Nanosecond, targetUnit)(ts.tv_nsec);
            } else version (NetBSD) {
                import core.sys.netbsd.time : clock_gettime, CLOCK_REALTIME;

                timespec ts = void;
                immutable error = clock_gettime(CLOCK_REALTIME, &ts);
                // Posix clock_gettime called with a valid address and valid clock_id is only
                // permitted to fail if the number of seconds does not fit in time_t. If tv_sec
                // is long or larger overflow won't happen before 292 billion years A.D.
                static if (ts.tv_sec.max < long.max) {
                    if (error)
                        throw new TimeException("Call to clock_gettime() failed");
                }
                return convert!(TimeUnit.Second, targetUnit)(ts.tv_sec) + 
                    convert!(TimeUnit.Nanosecond, targetUnit)(ts.tv_nsec);
            } else version (DragonFlyBSD) {
                import core.sys.dragonflybsd.time : clock_gettime, CLOCK_REALTIME;

                timespec ts = void;
                immutable error = clock_gettime(CLOCK_REALTIME, &ts);
                // Posix clock_gettime called with a valid address and valid clock_id is only
                // permitted to fail if the number of seconds does not fit in time_t. If tv_sec
                // is long or larger overflow won't happen before 292 billion years A.D.
                static if (ts.tv_sec.max < long.max) {
                    if (error)
                        throw new TimeException("Call to clock_gettime() failed");
                }
                return convert!(TimeUnit.Second, targetUnit)(ts.tv_sec) + 
                    convert!(TimeUnit.Nanosecond, targetUnit)(ts.tv_nsec);
            } else version (Solaris) {
                import core.sys.solaris.time : clock_gettime, CLOCK_REALTIME;

                timespec ts = void;
                immutable error = clock_gettime(CLOCK_REALTIME, &ts);
                // Posix clock_gettime called with a valid address and valid clock_id is only
                // permitted to fail if the number of seconds does not fit in time_t. If tv_sec
                // is long or larger overflow won't happen before 292 billion years A.D.
                static if (ts.tv_sec.max < long.max) {
                    if (error)
                        throw new TimeException("Call to clock_gettime() failed");
                }
                return convert!(TimeUnit.Second, targetUnit)(ts.tv_sec) + 
                    convert!(TimeUnit.Nanosecond, targetUnit)(ts.tv_nsec);
            } else
                static assert(0, "Unsupported OS");
        } else
            static assert(0, "Unsupported OS");
    }

}