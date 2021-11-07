#include "header.h"

void fanning ( options *user_options, description *system );

void iterate ( options *user_options, description *system ) {

	int n, no_of_open_nodes;
	int iter, i, j;
	double **main_matrix;


	/* Setting the number of degrees of freedom */
	no_of_open_nodes = 0;
	for ( i = 0; i < user_options->no_of_nodes; i++ ) {
		if ( system->nodes[i].is_external == TRUE ) {
			no_of_open_nodes++;
		}
	}
	n = user_options->no_of_pipes +
		user_options->no_of_nodes + no_of_open_nodes;
	if ( no_of_open_nodes > user_options->no_of_specs ) {
		fprintf ( stderr, "Underspecified problem, aborting...\n" );
		finalize ( user_options, system );
		exit ( EXIT_FAILURE );
	} else if ( no_of_open_nodes < user_options->no_of_specs ) {
		fprintf ( stderr, "Overspecified problem, aborting...\n" );
		finalize ( user_options, system );
		exit ( EXIT_FAILURE );
	}

	main_matrix = malloc ( n * sizeof ( double * ) );
	for ( i = 0; i < n; i++ ) {
		main_matrix[i] = malloc ( n* sizeof ( double ) );
	}

	/* Main loop of the program */

	if ( user_options->verbose_level == NORMAL ||
			user_options->verbose_level == VERBOSE ) {
		fprintf ( stderr, "  Starting calculations... " );
	}

	for ( iter = 0; iter < user_options->maxiter; iter++ ) {

		if ( user_options->verbose_level == NORMAL ||
				user_options->verbose_level == VERBOSE ) {
			if ( ( iter + 1 ) % 10 == 0 || iter == 0 ) {
				fprintf ( stderr, "\n\tIteration %d, ", iter + 1 );
			}
			if ( user_options->verbose_level == VERBOSE &&
					( ( iter + 1 ) % 10 != 0 ) &&
						iter != 0 ) {
				fprintf ( stderr, "%d, ", iter + 1 );
			}
		}

		/* Clean matrix from previous iterations or initializing it */
		for ( i = 0; i < n; i++ ) {
			for ( j = 0; j < n; j++ ) {
				main_matrix[i][j] = 0;
			}
		}

		fanning ( user_options, system );

		/* Bernoulli */
		if ( user_options->type != REAL_GAS ) {
			for ( i = 0; i < user_options->no_of_pipes; i++ ) {
				main_matrix[i][i] = BERNOULLI_FACTOR_NOT_GAS *
					( system->pipes[i].L_m /
					pow ( system->pipes[i].D_cm, 5 ) ) *
					system->pipes[i].f *
					system->fluid.rho_g_cm3 *
					fabs ( system->pipes[i].Q_m3_h );
				main_matrix[i][system->pipes[i].start +
					user_options->no_of_pipes +
					user_options->no_of_specs] = -1;
				main_matrix[i][system->pipes[i].end +
					user_options->no_of_pipes +
					user_options->no_of_specs] = -1;
			}
		} else {
			for ( i = 0; i < user_options->no_of_pipes; i++ ) {
				main_matrix[i][i] = ( ( 200 *
					fabs ( system->pipes[i].Q_m3_h ) *
					system->pipes[i].L_m *
					system->pipes[i].f /
					system->pipes[i].D_cm ) +
					( system->pipes[i].Q_m3_h *
					log (
					system->nodes[system->pipes[i].start].P_atm /
					system->nodes[system->pipes[i].end].P_atm
					) ) ) *
					( BERNOULLI_FACTOR_GAS * system->fluid.T /
					( pow ( system->pipes[i].D_cm ,4 ) *
					system->fluid.MM_g_gmol ) );
				main_matrix[i][system->pipes[i].start+
					user_options->no_of_pipes +
					user_options->no_of_specs] =
					-system->nodes[system->pipes[i].start].P_atm;
				main_matrix[i][system->pipes[i].end+
					user_options->no_of_pipes +
					user_options->no_of_specs] =
					-system->nodes[system->pipes[i].end].P_atm;
			}
		}

		j = 0;
		for ( i = 0; i < user_options->no_of_specs; i++ ) {
			if ( system->specs[i].specified_var == PRESSURE ) {
				main_matrix[j+
					user_options->no_of_pipes][no_of_open_nodes+
					user_options->no_of_pipes+
					system->specs[i].node_number] = 1;
				j++;
			}
		}
	}

	if ( user_options->verbose_level == NORMAL ||
			user_options->verbose_level == VERBOSE ) {
		fprintf ( stderr, "\n" );
	}

	for ( i = 0; i < n; i++ ) {
		free ( main_matrix[i] );
	}
	free ( main_matrix );

}


/*
 * This function calculates the fanning friction factor in a tube,
 * storing it in the struct.
 */
void fanning ( options *user_options, description *system ) {

	int i;
	double A, B, C, Re_factor;

	if ( user_options->type == NEWTONIAN ||
			user_options->type == REAL_GAS ) {
		if ( user_options->type == NEWTONIAN ) {
			Re_factor = RE_FACTOR_NEWTONIAN*system->fluid.rho_g_cm3;
		} else {
			Re_factor = RE_FACTOR_REAL_GAS;
		}
		for ( i = 0; i < user_options->no_of_pipes; i++ ) {
			system->pipes[i].Re = ( Re_factor *
				fabs ( system->pipes[i].Q_m3_h ) ) /
				( system->fluid.eta_cP * system->pipes[i].D_cm );
			B = pow ( ( RE_FACTOR_NEWTONIAN_REAL_GAS_B /
					system->pipes[i].Re ), 16 );
			C = pow ( ( 7 / system->pipes[i].Re ), 0.9 ) +
				( RE_FACTOR_NEWTONIAN_REAL_GAS_C *
				system->pipes[i].e_mm ) / system->pipes[i].D_cm;
			A = pow ( ( RE_FACTOR_NEWTONIAN_REAL_GAS_A *
					log ( 1.0 / C ) ), 16 );
			C = pow ( ( 8 / system->pipes[i].Re), 12 ) + 1.0 /
				( pow ( ( A + B ), 1.5 ) );
			system->pipes[i].f = 2 * pow ( C, ( 1.0 / 12.0 ) );
		}
	}


}

