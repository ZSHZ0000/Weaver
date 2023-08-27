#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>
#include "alloc.h"

void
PrintObject1 (LispObjectImm Object, FILE* Stream);

LispObjectImm
Print (LispObjectImm Object);

LispObjectImm
NL (LispObjectImm Object);

#endif /* PRINT_H */
