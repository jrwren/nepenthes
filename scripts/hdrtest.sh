# $Id$
#!/bin/sh

for i in $@
do
	echo "===> testing ${i}"

	echo "#include \"${i}\"" > /tmp/hdrtest.c
	echo "int main(int argc, char **argv) { return 0; }" >> /tmp/hdrtest.c

	g++ -Wall /tmp/hdrtest.c -o /tmp/hdrtest -I/usr/include/lua50 -I.
	STATUS=$?
	rm -f /tmp/hdrtest /tmp/hdrtest.c


	if [ $STATUS -ne 0 ]
	then
		exit
	fi


done
