#! /bin/sh

for dir in . ..; do
	if [ -f "$dir/configure.ac" ]; then
		top_dir="$dir";
		break;
	fi
done

platform=i586-mingw32msvc
CFLAGS=-DA_DEBUG=1
exec $top_dir/config/configure_wrap	\
	--enable-mass-warnings	\
	--enable-debug	\
	--prefix=$HOME	\
	--host=$platform	\
	--bindir=$HOME/bin/$platform	\
	--libdir=$HOME/lib/$platform	\
	--includedir=$HOME/include/$platform \

