#!/bin/bash

#
# This script generate cloak header file from cloak_template.h,
# depending on the maximum number of loops that the user wants
#

# root directory of Cloak
CLOAK_ROOT=..

# template file
template=$CLOAK_ROOT/src/cloak_template.h

# maximum number of loops to generate
num=20

# output file
output=

# newline character
nl=$(echo -en "\n")

# usage
function usage {
	echo "$PROGNAME: usage: $PROGNAME [OPTIONS]"
	echo "    Options:"
	echo "        -t | --template <template file>"
	echo "        -n | --num <number of loops to generate>"
	echo "        -o | --output <output file>"
	return
}

# parse commandline arguments
while [[ -n $1 ]]; do
	case $1 in
		-t | --template )
			shift
			template=$1
			;;

		-n | --num )
			shift
			num=$1
			;;

		-o | --output )
			shift
			output=$1
			;;

		* )
			usage >&2
			exit 1
			;;
	esac
	shift
done

# validate number of loops
if [[ $num -lt 2 ]]; then
	echo "The maximum number of loops must be bigger than 1" >&2
	exit 1
fi

if [[ -z $output ]]; then
	output=$CLOAK_ROOT/src/cloak_${num}.h
fi


# update include guard
base=$(basename $output)
base_macro=$(echo -n $base | tr '[:lower:]' '[:upper:]' | tr '.' '_')
sed "s/_CUSTOM_CLIB_CLOAK_TEMPLATE_H/_CUSTOM_CLIB_${base_macro}/g" \
	$template > $output

# add macros for the loops

# add CLOAK_INC_X
string=$nl
for i in $(eval echo {1..$num}); do
	string+=#define\ CLOAK_INC_$i\ $(($i+1))\\n
done
sed -i "/#define CLOAK_INC_0 1/a $string" $output

# add CLOAK_DEC_X
string=$nl
for i in $(eval echo {1..$num}); do
	string+=#define\ CLOAK_DEC_$i\ $(($i-1))\\n
done
sed -i "/#define CLOAK_DEC_0 0/a $string" $output

# add CLOAK_EXPR_X
string=$nl
for i in $(eval echo {1..$num}); do
	string+=#define\ CLOAK_EXPR_$i\(...\)\ __VA_ARGS__\\n
done
sed -i "/#define CLOAK_EXPR_0(...) __VA_ARGS__/a $string" $output

# ouput to file

exit 0