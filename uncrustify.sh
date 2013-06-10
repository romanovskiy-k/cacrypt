#!/bin/bash

command -v uncrustify >/dev/null 2>&1 || exit 0

function format {
	if [ -f .uncrustify.cfg -a "$2" == "-l" ] ; then 
		uncrustify -q -l CPP -c .uncrustify.cfg --no-backup "$1"
	else
		uncrustify -q -l CPP -c uncrustify.cfg --no-backup "$1"
	fi
}

for d in . Mac Pkcs11Mock Tests X11 Win Tests/Component Tests/Unit Tests/Integration Tests/Integration/FormatUtil \
         common; do
	for e in c h cpp; do
		for file in $d/*.$e; do
			format "$file" $1
		done
	done
done

exit 0
