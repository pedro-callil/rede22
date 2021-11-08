#include "header.h"

void fanning ( options *user_options, description *system );

void iterate ( options *user_options, description *system ) {

	int n, no_of_open_nodes, no_of_spec_pressures, no_of_spec_flow_rates;
	int iter, i, j, cont_pres, cont_flow;
	int open_node;
	double *cons_vector;
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
	cons_vector = malloc ( n * sizeof ( double ) );
	for ( i = 0; i < n; i++ ) {
		main_matrix[i] = malloc ( n* sizeof ( double ) );
	}

	/* Main loop of the program */

	if ( user_options->verbose_level == NORMAL ||
			user_options->verbose_level == VERBOSE ) {
		fprintf ( stderr, "  Starting calculations... " );
	}

	/* Main loop of the program */
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
					user_options->no_of_specs] = 1;
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

		no_of_spec_pressures = 0;
		for ( i = 0; i < user_options->no_of_specs; i++ ) {
			if ( system->specs[i].specified_var == PRESSURE ) {
				main_matrix[no_of_spec_pressures+
					user_options->no_of_pipes][no_of_open_nodes+
					user_options->no_of_pipes+
					system->specs[i].node_number] = 1;
				no_of_spec_pressures++;
			}
		} /* Specified_pressres */

		no_of_spec_flow_rates = 0;
		for ( i = 0; i < user_options->no_of_specs; i++ ) {
			if ( system->specs[i].specified_var == FLOW ) {
				main_matrix[user_options->no_of_pipes +
					no_of_spec_pressures +
					no_of_spec_flow_rates][
					user_options->no_of_pipes + i] = 1;
				no_of_spec_flow_rates++;
			}
		} /* Specified flow rates */

		/* Mass balances */
		open_node = -1;
		for ( i = 0; i < no_of_open_nodes; i++ ) {
			for ( j = open_node + 1; j < user_options->no_of_nodes; j++ ) {
				if ( system->nodes[j].is_external == TRUE ) {
					open_node = j;
					j = user_options->no_of_nodes;
				}
			}
			main_matrix[user_options->no_of_pipes+no_of_open_nodes+
				open_node][user_options->no_of_pipes+i] = 1;
		}
		for ( i = 0; i < user_options->no_of_pipes; i++ ) {
			main_matrix[user_options->no_of_pipes +
				no_of_open_nodes +
				system->pipes[i].start][i] = -1;
			main_matrix[user_options->no_of_pipes +
				no_of_open_nodes +
				system->pipes[i].end][i] = 1;
		}

		/* Vector with constants */
		for ( i = 0; i < n; i++ ) {
			cons_vector[i] = 0;
		}

		cont_pres = 0;
		cont_flow = 0;
		for ( i = 0; i < user_options->no_of_specs; i++ ) {
			if ( system->specs[i].specified_var == PRESSURE ) {
				cons_vector[user_options->no_of_pipes+cont_pres] =
					system->specs[i].Q_m3_h_or_P_atm;
				cont_pres++;
			} else {
				cons_vector[user_options->no_of_pipes+
					no_of_spec_pressures+cont_flow] =
					system->specs[i].Q_m3_h_or_P_atm;
				cont_flow++;
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
	free ( cons_vector );

}


/*
 * This function calculates the fanning friction factor in a tube,
 * storing it in the struct.
 */
void fanning ( options *user_options, description *system ) {

	int i;
	double A, B, C, Re_factor, Rec;

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
	} else if ( user_options->type == POWER_LAW_SMOOTH_PIPE ||
			user_options->type == POWER_LAW_ROUGH_PIPE) {
		for ( i = 0; i < user_options->no_of_pipes; i++ ) {
			system->pipes[i].Re = 1000 * system->fluid.rho_g_cm3 *
				pow ( 2.0, 7.0 - 3.0 * system->fluid.n ) *
				pow ( PI, system->fluid.n - 2.0 ) *
				pow ( system->fluid.n, system->fluid.n ) *
				pow ( 3.0 * system->fluid.n + 1.0,
						-system->fluid.n ) *
				pow ( fabs ( system->pipes[i].Q_m3_h ) / 3600.0,
						2.0 - system->fluid.n ) *
				pow ( system->pipes[i].D_cm/100.0,
						3.0 * system->fluid.n - 4.0 ) /
					system->fluid.k_Pa_sn;
		}
		if ( user_options->type == POWER_LAW_SMOOTH_PIPE ) {
			for ( i = 0; i < user_options->no_of_pipes; i++ ) {
				Rec = REC_POWER_LAW_LIN + REC_POWER_LAW_ANG *
					( 1.0 - system->fluid.n );
				A = 1.0 / ( 1 + pow ( 4.0,
						Rec - system->pipes[i].Re ) );
				B = pow ( system->pipes[i].Re,
					RE_EXP_POWER_LAW_LIN_B +
					RE_EXP_POWER_LAW_ANG_B * system->fluid.n ) *
					exp ( RE_EXP_POWER_LAW_CON_B *
						system->fluid.n ) /
					RATIO_POWER_LAW_B;
				C = RATIO_POWER_LAW_C /
					( sqrt ( system->fluid.n ) *
					pow ( system->pipes[i].Re,
					1.0 / ( RE_EXP_POWER_LAW_LIN_C +
					RE_EXP_POWER_LAW_ANG_C *
					system->fluid.n ) ) );
				system->pipes[i].f = 16 * ( 1.0 - A ) /
					system->pipes[i].Re +
					A * pow ( pow ( B, -8 ) + pow ( C, -8),
						-1.0/8.0 );
			}
		} else {
			for ( i = 0; i < user_options->no_of_pipes; i++ ) {
				B = pow ( ( RE_FACTOR_NEWTONIAN_REAL_GAS_B /
						system->pipes[i].Re ), 16 );
				C = pow ( ( 7 / system->pipes[i].Re ), 0.9 ) +
					( RE_FACTOR_NEWTONIAN_REAL_GAS_C *
					system->pipes[i].e_mm ) /
						system->pipes[i].D_cm;
				A = pow ( ( RE_FACTOR_NEWTONIAN_REAL_GAS_A *
						log ( 1.0 / C ) ), 16 );
				C = pow ( ( 8 / system->pipes[i].Re), 12 ) + 1.0 /
					( pow ( ( A + B ), 1.5 ) );
				system->pipes[i].f = 2 * pow ( C, ( 1.0 / 12.0 ) );
			}
		}
	}


}

