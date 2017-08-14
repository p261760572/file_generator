export TWO_TASK=orcl
export DOCUMENT_ROOT=$PWD
export DBUSE=cod
export DBPWD=21C16804ACD2BDC84CAE08997DC917DB3B75D949E956D025FFD387BAD007133C
valgrind --tool=memcheck --leak-check=full --show-reachable=yes --vex-iropt-register-updates=allregs-at-mem-access --vex-iropt-register-updates=allregs-at-each-insn --log-file=mem.log cod-time&
