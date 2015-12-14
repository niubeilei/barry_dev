/*
 * PuTTY version numbering
 */

#define STR1(x) #x
#define STR(x) STR1(x)

#if defined SNAPSHOT

char ver[] = "开发快照 " STR(SNAPSHOT);
char sshver[] = "sshX-快照-" STR(SNAPSHOT);

#elif defined RELEASE

char ver[] = "发布版 " STR(RELEASE);
char sshver[] = "sshX-发布版-" STR(RELEASE);

#else

char ver[] = "未经确认编译版, " __DATE__ " " __TIME__;
char sshver[] = "sshX-本地: " __DATE__ " " __TIME__;

#endif

/*
 * SSH local version string MUST be under 40 characters. Here's a
 * compile time assertion to verify this.
 */
enum { vorpal_sword = 1 / (sizeof(sshver) <= 40) };
