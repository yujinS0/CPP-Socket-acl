#!/bin/sh

#valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-reachable=yes --max-stackframe=3426305034400000 -v ./fiber -n 10 -m 20
valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-reachable=yes -v ./fiber -c 100 -n 100 -S
