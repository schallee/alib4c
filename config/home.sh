#! /bin/sh

for dir in . ..; do
	if [ -f "$dir/configure.ac" ]; then
		top_dir="$dir";
		break;
	fi
done

exec $top_dir/config/configure_wrap	\
	--enable-mass-warnings	\
	--prefix=$HOME	\
	--bindir=$HOME/bin/$CONFIG_GUESS	\
	--libdir=$HOME/lib/$CONFIG_GUESS	\
	--includedir=$HOME/include/$CONFIG_GUESS	\
	"$@"
