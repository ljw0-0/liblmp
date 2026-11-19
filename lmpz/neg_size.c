#include "lmp.h"

int lmpz_neg_size(size_t size) {
    return size ? (size ^ lmpz_SIGN_BIT) : 0;
}
