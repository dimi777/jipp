#!/bin/bash
LIMIT=$(ulimit -u)
PRC=$(ps -u 1000 | wc -l)
MAX=$(($LIMIT-$PRC))
export MAX
