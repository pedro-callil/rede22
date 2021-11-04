#include "header.h"

void finalize ( options *user_options, description *system ) {

	free ( system->pipes );
	free ( system->knots );
	free ( system->specs );

}

