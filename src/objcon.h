#ifndef OBJCON_H
#define OBJCON_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include "objtypes.h"

obj_t *obj_parse_file(const char *name);
void obj_destroy(obj_t *obj);

#endif /* OBJCON_H */
