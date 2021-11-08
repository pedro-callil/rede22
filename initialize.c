#include "header.h"

void initialize ( options *user_options, description *system ) {

	int i, temp_for_switch;
	int no_of_spec_pressures;
	double avg_P_atm;

	no_of_spec_pressures = 0;
	for ( i = 0; i < user_options->no_of_specs; i++ ) {
		if ( system->specs[i].specified_var == PRESSURE ) {
			no_of_spec_pressures ++;
			if ( user_options->type != REAL_GAS ) {
				system->specs[i].Q_m3_h_or_P_atm +=
					P_TO_REAL_P *
					system->fluid.rho_g_cm3 *
					system->nodes[
						system->specs[i].node_number].H_m;
			}
		}
	}

	/* Initial guesses: for liquids, economical
	* diameter; for gases, ideal gas */
	if ( user_options->type != REAL_GAS ) {
		for ( i = 0; i < user_options->no_of_pipes; i++ ) {
			system->pipes[i].Q_m3_h =
				ECON_DIAM_A * pow ( system->pipes[i].D_cm,
					ECON_DIAM_B ) /
					pow ( system->fluid.rho_g_cm3,
						ECON_DIAM_C );
		}
	} else {
		avg_P_atm = 0.0;
		for ( i = 0; i < user_options->no_of_nodes; i++ ) {
			if ( system->specs[i].specified_var == PRESSURE ) {
				avg_P_atm += system->specs[i].Q_m3_h_or_P_atm;
			}
		}
		avg_P_atm = avg_P_atm / no_of_spec_pressures;
		system->fluid.T = system->fluid.Z *
			( system->fluid.T_oC + CELSIUS_TO_KELVIN );
		for ( i = 0; i < user_options->no_of_pipes; i++ ) {
			system->pipes[i].Q_m3_h = REAL_GAS_INIT_GUESS *
				pow ( system->pipes[i].D_cm, 2 ) * avg_P_atm *
				( system->fluid.MM_g_gmol / system->fluid.T );
		}
		for ( i = 0; i < user_options->no_of_nodes; i++ ) {
			system->nodes[i].P_atm = avg_P_atm;
		}
	}

	for ( i = 0; i < user_options->no_of_pipes; i++ ) {
		if ( system->pipes[i].start > system->pipes[i].end ) {
			temp_for_switch = system->pipes[i].start;
			system->pipes[i].start = system->pipes[i].end;
			system->pipes[i].end = temp_for_switch;
		} /* Checking if nodes in ascending order */
		system->pipes[i].L_m += system->pipes[i].L_eq_m;
		/* Setting total equivalent pipe lenght */
	}

}

