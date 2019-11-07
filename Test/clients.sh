#!/bin/bash
i=0
while [ $i -lt $1 ]
do
	curl http://127.0.1.1:8080/ &
	true $((i=i+1))
done

wait