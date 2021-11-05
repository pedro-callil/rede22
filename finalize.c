#include "header.h"

void finalize ( options *user_options, description *system ) {

	if ( system->pipes != NULL ) free ( system->pipes );
	if ( system->nodes != NULL ) free ( system->nodes );
	if ( system->specs != NULL ) free ( system->specs );

}

