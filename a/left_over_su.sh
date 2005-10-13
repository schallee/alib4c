#! /bin/sh
grep su *.h *.c  |egrep -v "success|supported|sure|such|usual|result"
