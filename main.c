#include "header.h"

int main ( int argc, char **argv ) {

	options user_options; /* Command line user options. */
	description system;     /* System description, with pipes, nodes and
				* fluid characteristics. */

	read_options ( argc, argv, &user_options, &system );
		/* Get user options, alloc memory; print help if needed. */

	initialize ( &user_options, &system );
		/* Fill system; read (and maybe write) data file if needed. */

	iterate ( &user_options, &system );
		/* Solve system. */

	finalize ( &user_options, &system );
		/* Print results or write to file, if required; free memory. */

	return EXIT_SUCCESS;

}


