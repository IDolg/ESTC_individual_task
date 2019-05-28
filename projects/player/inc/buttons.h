#ifndef BUTTONS_H
#define BUTTONS_H 1

typedef void (*ext_funct)(void);

void buttons_run_functions(ext_funct f1, ext_funct f2, ext_funct f3);

#endif
