#!/bin/sh

echo "Press Ctrl-A and then X to terminate QEMU session"
echo
qemu-system-arm \
	-M versatilepb \
	-cpu arm926 \
	-nographic \
	-kernel kernel.elf "$@"
