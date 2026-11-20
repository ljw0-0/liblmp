#include "lmp.h"

size_t lmpz_neg_size(size_t size) {
    return size ? (size ^ LMPZ_SIGN_BIT) : 0;
}
