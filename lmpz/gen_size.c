#include "lmp.h"

size_t lmpz_gen_size(size_t size, size_t sign_bit) {
    return size ? (size | sign_bit) : 0;
}