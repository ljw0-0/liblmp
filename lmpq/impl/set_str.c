#include "lmp.h"
#include <string.h>
#include <stdlib.h>

int _lmpq_set_str(lmpz_t p, lmpz_t q, const char *str, int base) {
    int err;
    char *tmp, *pos=strchr(str, '/');
    if (!pos) {
        if (LMP_UNLIKELY(err = lmpz_set_str(p, str, base))) return err;
        lmpz_one(q);
        return 0;
    }
    if (LMP_UNLIKELY(!(tmp = malloc(pos-str+1)))) return LMP_ENOMEM;
    memcpy(tmp, str, pos-str);
    tmp[pos-str] = '\0';
    err = lmpz_set_str(p, tmp, base);
    free(tmp);
    if (LMP_UNLIKELY(err)) return err;
    return lmpz_set_str(q, pos+1, base);
}