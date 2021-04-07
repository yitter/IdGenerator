#!/bin/sh
phpize && ./configure && make clean && make && gcc -O2 -fPIC -shared -g src/snowflake/snowflake.c -o src/snowflake/libsnow.so