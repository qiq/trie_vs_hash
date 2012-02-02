#ifdef __cplusplus
extern "C" {
#endif

void *algo_new(int len, int items);
void algo_free(void *algo, int len);
void algo_insert(void *algo, int len, unsigned char *key, long value);
long algo_lookup(void *algo, int len, unsigned char *key);

#ifdef __cplusplus
}
#endif
