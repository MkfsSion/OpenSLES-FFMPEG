#ifndef CONFIG_H_
#define CONFIG_H_
#ifdef NEVER_ALLOW_ERROR
static void never_allow_test(const char *string) {
	printf("Never allow error detected %s,will exit.\n",string);
	exit(1);
}
#else
#define never_allow_test(x)
#endif /* NEVER_ALLOW_ERROR */

#endif /* CONFIG_H_ */
