#!/bin/sh

for i in {1..6}
do
echo "running ./scheduler for input in_$i\n"
./scheduler ../input/in_$i
done