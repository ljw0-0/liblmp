#include "lmp.h"
#include <string.h>

char *_lmpq_get_str(char *str, int base, const lmpz_t p, const lmpz_t q) {
    int is_alloc;
    char *tmp;
    if (!str) {
        str = malloc(lmpz_sizeinbase(p, base) + lmpz_sizeinbase(q, base) + 3);
        if (LMP_UNLIKELY(!str)) return NULL;
        is_alloc = 1;
    } else is_alloc = 0;
    if (LMP_UNLIKELY(!lmpz_get_str(str, base, p))) {
        cleanup:
        if (is_alloc) free(str);
        return NULL;
    }
    tmp = str + strlen(str);
    *tmp = '/';
    if (LMP_UNLIKELY(!lmpz_get_str(tmp+1, base, q))) goto cleanup;
    return str;
}