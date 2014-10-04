#!/bin/sh

VERB_ARG="$1"

for i in {1..6}
do
echo "running ./scheduler for input in_$i"
./scheduler $VERB_ARG ../input/in_$i
done