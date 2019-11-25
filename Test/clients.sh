#!/bin/bash
i=0
while [ $i -lt $1 ]
do
	curl http://3.19.188.211/ &
	true $((i=i+1))
done

wait