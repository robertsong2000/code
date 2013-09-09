# 1 "fpic.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "fpic.c"
extern int foo;
extern int bar (int);
int call_bar (void) {
return bar (foo);
}
