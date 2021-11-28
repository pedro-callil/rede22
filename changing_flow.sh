#! /bin/bash

# This file is an example of how one might use the executable "rede22"
# to make a sensitivity analysis. Here, we will simulate the behaviour
# of the net, changing the flow of water in node 1, and observing the
# changes in output values (pressure in each node, Fanning friction
# factor and Reynolds number in each tube, etc.).

usage () {
	echo -n "Usage: $0 [-f <file>] [-s <smallest>] [-b <biggest>] " 1>&2
	echo "[-i <increment>] [-o <output file>]" 1>&2
	exit 1
}

while getopts ":f:s:b:i:o:" opt
do
	case "${opt}" in
		f)
			infile=${OPTARG}
			;;
		o)
			outfile=${OPTARG}
			;;
		s)
			smallest=${OPTARG}
			;;
		b)
			biggest=${OPTARG}
			;;
		i)
			increment=${OPTARG}
			;;
		*)
			usage
			;;
	esac
done
shift $((OPTIND-1))

# writing first line of csv
./rede22 -f "$infile" -V verbose | \
	sed 's/^[\t\ ]\+//g' | \
	grep [0-9]: -A 3 | \
	cut -d ':' -f 1 | \
	cut -f 1 -d '=' | \
	sed 's/Pipe\ /P/g' | \
	sed 's/Node\ /N/g' | \
	sed 's/[\ \t]//g' | \
	tr '\n' ',' | \
	sed 's/,,/,/g' | \
	sed 's/,--//g' | \
	sed 's/\(P[0-9]\+\),Q,f,Re,/\1Q,\1f,\1Re,/g' | \
	sed 's/\(N[0-9]\+\),P,Q,/\1P,\1Q,/g' | \
	sed 's/\(N[0-9]\+\),P,/\1P,/g' | \
	sed 's/,$/\n/g' > "$outfile"

for i in $(seq $smallest $increment $biggest)
do
	./rede22 -f "$infile" -V verbose -s "1 flow $i" | \
		sed -e 's/^[\t\ ]\+//g' | \
		grep [0-9]: -A 3 | \
		sed 's/\(.*\)out/-\1/g' | \
		tr -cd 'N[[:digit:]].\n\-' | \
		grep -v '[A-Za-z]' | \
		sed 's/--//g' | \
		tr '\n' ',' | \
		sed 's/,,/,/g' | \
		sed 's/,$/\n/g' >> "$outfile"
done

