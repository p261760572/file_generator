#!/bin/sh
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ORACLE_HOME/lib:/lib:/usr/lib;
export LC_ALL=zh_CN.gbk LANG=zh_CN.gbk NLS_LANG="SIMPLIFIED CHINESE_CHINA.ZHS16GBK"
export  NLS_DATE_FORMAT="YYYY-MM-DD HH24:MI:SS"
export DOCUMENT_ROOT=/home/panz
export DB=PDBORCL
export DB_USER=wjxt
export DB_PWD=wjxt

FileGenerate &
