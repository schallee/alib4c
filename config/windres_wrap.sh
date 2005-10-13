#! /bin/sh

echo $*
prog=$1
shift
args=""
for arg in $@; do
	case $arg in
		(-i|--input|-o|--output|-J|--input-format|-O|--output-format|-F|--target|--preprocessor|-I*|--include-dir|-D*|--define|-U*|--undefine|-r|-l|--language|--use-temp-file|--no-use-temp-file|-h|--help|-V|--version)
			args="$args $arg"
			;;
		(-*)
			# ignore any other options that libtool passes
			;;
		(*)
			args="$args $arg"
			;;
	esac
done
exec $prog $args
