extern int foo;
extern int bar (int);
int call_bar (void) {
return bar (foo);
}
