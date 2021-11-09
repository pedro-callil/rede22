#include "header.h"

void write_results ( options *user_options, description *system, FILE *stream );

void print_and_write ( options *user_options, description *system ) {

	FILE *output_file;

	if ( user_options->verbose_level != QUIET ) {
		write_results ( user_options, system, stdout );
	}

	if ( user_options->output_file_name != NULL ) {
		output_file = fopen ( user_options->output_file_name, "w" );
		write_results ( user_options, system, output_file );
		fclose ( output_file );
	}

}

/*
 * This is a subroutine for writing the results obtained in previous functions
 * and stored in the structs options and description to a file.
 */
void write_results ( options *user_options, description *system, FILE *stream ) {

	int i;

	if ( user_options->iter_stop == user_options->maxiter ) {
		fprintf ( stream,
			"WARNING: SYSTEM DID NOT REACH CONVERGENCE !!!\n" );
		fprintf ( stream,
			"RESULTS OBTAINED AFTER %d ITERATIONS\n",
			user_options->iter_stop );
	} else {
		fprintf ( stream,
			"RESULTS OBTAINED AFTER CONVERGENCE, %d ITERATIONS\n",
			user_options->iter_stop );
	}
	fprintf ( stream, "\tPIPES:\n" );
	for ( i = 0; i < user_options->no_of_pipes; i++ ) {
		fprintf ( stream, "\tPipe %d:", i + 1 );
		if ( system->pipes[i].Q_m3_h > 0 ) {
			fprintf ( stream, "\tNodes\t%d\tto\t%d",
				system->pipes[i].start + 1,
				system->pipes[i].end + 1 );
		} else {
			fprintf ( stream, "\tNodes\t%d\tto\t%d",
				system->pipes[i].end + 1,
				system->pipes[i].start + 1 );
		}
		if ( user_options->type != REAL_GAS ) {
			fprintf ( stream, "\n\t\tQ  =\t%f\tm3/h\n",
				fabs ( system->pipes[i].Q_m3_h ) );
		} else {
			fprintf ( stream, "\n\t\tQ  =\t%f\tkg/s\n",
				fabs ( system->pipes[i].Q_m3_h ) );
		}
		fprintf ( stream, "\t\tf  =\t%f\n\t\tRe =\t%f\t",
				system->pipes[i].f,
				system->pipes[i].Re);
		if ( system->pipes[i].regime == LAMINAR ) {
			fprintf ( stream, "(laminar)\n" );
		} else if ( system->pipes[i].regime == TRANSITION ) {
			fprintf ( stream, "(transition)\n" );
		} else if ( system->pipes[i].regime == TURBULENT ) {
			fprintf ( stream, "(turbulent)\n" );
		}
	}

	fprintf ( stream, "\tNODES:\n" );
	for ( i = 0; i < user_options->no_of_nodes; i++ ) {
		if ( user_options->type != REAL_GAS ) {
			fprintf ( stream,
				"\tNode %d:\n\t\tP =\t%f\tatm\n",
				i + 1, system->nodes[i].P_atm -
				P_TO_REAL_P * system->fluid.rho_g_cm3 *
				system->nodes[i].H_m );
		} else {
			fprintf ( stream,
				"\tNode %d:\n\t\tP =\t%f\tatm\n",
				i + 1, system->nodes[i].P_atm );
		}
		if ( system->nodes[i].is_external == TRUE ) {
			if ( user_options->type != REAL_GAS ) {
				fprintf ( stream,
					"\t\tQ =\t%f\tm3/h\t",
					fabs( system->nodes[i].Q_ext ) );
			} else {
				fprintf ( stream,
					"\t\tQ =\t%f\tkg/s\t",
					fabs( system->nodes[i].Q_ext ) );
			}
			if ( system->nodes[i].Q_ext > 0 ) {
				fprintf ( stream, "(in)\n" );
			} else {
				fprintf ( stream, "(out)\n" );
			}
		} else {
			fprintf ( stream, "\n" );
		}
	}

}

