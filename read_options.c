#include "header.h"

void print_help ( );
void print_choices ( options *user_options, description *system );
void interactive_session ( options *user_options, description *system );
void read_from_file ( options *user_options, description *system );
void read_from_command_line ( int argc, char **argv,
		options *user_options, description *system );
void eval_node_options ( char *line, description *system, int no_of_nodes );
void eval_pipe_options ( char *line, description *system, int no_of_pipes );
void eval_spec_options ( char *line, description *system, int no_of_specs );

/*
 * This function reads all user input, from three sources: configuration files,
 * command-line options and interactive user input. Takes as arguments the main
 * function arguments (int argc and char **argv), an options struct to store
 * generic user options and a description struct to store the physical
 * description of the system. of the system.
 */
void read_options ( int argc, char **argv,
		options *user_options, description *system ) {

	user_options->help = FALSE;
	user_options->interactive = FALSE;
	user_options->maxiter = MAXITER;
	user_options->no_of_nodes = 0;
	user_options->no_of_pipes = 0;
	user_options->no_of_specs = 0;
	user_options->rugosity_general = RUGO_GEN;
	user_options->diameter_general = DIAM_GEN;


	read_from_command_line ( argc, argv, user_options, system );

	if ( user_options->help == TRUE ) {
		print_help ();
	}

	user_options->input_file_name = argv[1];

	user_options->existing_file = READ_EXISTING_FILE;
	user_options->verbose_level = VERBOSE;

	if ( user_options->existing_file == READ_EXISTING_FILE ) {
		read_from_file ( user_options, system );
	}

	print_choices ( user_options, system );

}

/*
 * This function prints to the user the options that will be applied to the
 * solution. Takes as arguments an options struct and a description struct,
 * storing the system description and other user inputs.
 */
void print_choices ( options *user_options, description *system ) {

	if ( user_options->verbose_level != QUIET ) {

		fprintf ( stderr, "OPTIONS SELECTED:\n\n" );
		fprintf ( stderr, "  Iterations:   %d\n",
				user_options->maxiter );
		fprintf ( stderr, "  Tolerance:    %lf %%\n",
				user_options->Q_tol_percentage );
		fprintf ( stderr, "  Dampening:    %lf\n\n",
				user_options->dampening_factor );
		fprintf ( stderr, "  Viscosity:    %lf cP\n",
				system->fluid.eta_cP );
		fprintf ( stderr, "  Density:      %lf g/cm3\n",
				system->fluid.rho_g_cm3 );
		switch ( user_options->type ) {
			case POWER_LAW_SMOOTH_PIPE:
				fprintf ( stderr, "  n:            %lf\n",
						system->fluid.n );
				fprintf ( stderr, "  k:            %lf Pa/sn\n",
						system->fluid.k_Pa_sn );
				break;
			case POWER_LAW_ROUGH_PIPE:
				fprintf ( stderr, "  n:            %lf\n",
						system->fluid.n );
				fprintf ( stderr, "  k:            %lf Pa/sn\n",
						system->fluid.k_Pa_sn );
				break;
			case BINGHAM_PLASTIC:
				fprintf ( stderr, "  T0:           %lf N/m2\n",
						system->fluid.T0_N_m2 );
				fprintf ( stderr, "  mu-infty:     %lf cP\n",
						system->fluid.mu_infty_cP );
				break;
			case STRUCTURAL_MODEL:
				fprintf ( stderr, "  N0:           %lf cP\n",
						system->fluid.N0_cP );
				fprintf ( stderr, "  lambda:       %lf s\n",
						system->fluid.lambda_s );
				fprintf ( stderr, "  omega:        %lf\n",
						system->fluid.omega );
				break;
			case REAL_GAS:
				fprintf ( stderr, "  T:            %lf C\n",
						system->fluid.T_oC );
				fprintf ( stderr, "  MM:           %lf g/gmol\n",
						system->fluid.MM_g_gmol );
				fprintf ( stderr, "  Z:            %lf\n",
						system->fluid.Z );
				fprintf ( stderr, "  Cp/Cv:        %lf\n",
						system->fluid.k );
				break;
		}
		fprintf ( stderr, "\n  Pipes:        %d\n",
				user_options->no_of_pipes );
		if ( user_options->verbose_level == VERBOSE ) {
			fprintf ( stderr,
				"   Pipe\tLength\t\tDiameter\t" );
			fprintf ( stderr, "Rugosity\tTerminal nodes\n" );
			for ( int i = 0; i < user_options->no_of_pipes; i++ ) {
				fprintf ( stderr,
					"    %d\t%f m\t%f cm\t%f mm\t%d and %d\n",
					i+1,
					system->pipes[i].L_m+system->pipes[i].L_eq_m,
					system->pipes[i].D_cm,
					system->pipes[i].e_mm,
					system->pipes[i].start + 1,
					system->pipes[i].end + 1 );
				/*
				* We add 1 to the knot and pipe numbers since
				* despite being elements of a 0-indexed array,
				* they are described by the user as a 1-indexed
				* one.
				*/
			}
			fprintf ( stderr, "\n" );
		}
		fprintf ( stderr, "  Nodes:        %d\n",
				user_options->no_of_nodes );
		if ( user_options->verbose_level == VERBOSE ) {
			fprintf ( stderr, "   Node\tHeight\n" );
			for ( int i = 0; i < user_options->no_of_nodes; i++ ) {
				fprintf ( stderr,
					"    %d\t%f m\t",
					i+1, system->nodes[i].H_m );
				if ( system->nodes[i].is_external == TRUE ) {
					fprintf ( stderr, "(open)" );
				}
				fprintf ( stderr, "\n" );
			}
			fprintf ( stderr, "\n" );
		}
		fprintf ( stderr, "  Specs:        %d\n",
				user_options->no_of_specs );
		if ( user_options->verbose_level == VERBOSE ) {
			fprintf ( stderr, "   Node\t\tVariable\tValue\n" );
			for ( int i = 0; i < user_options->no_of_specs; i++ ) {
				if ( system->specs[i].specified_var == FLOW ) {
					fprintf ( stderr,
						"    %d\t\tflow rate\t%f m3/h\n",
						system->specs[i].node_number + 1,
						system->specs[i].Q_m3_h_or_P_atm );
				} else {
					fprintf ( stderr,
						"    %d\t\tpressure\t%f atm\n",
						system->specs[i].node_number + 1,
						system->specs[i].Q_m3_h_or_P_atm );
				}
			}
			fprintf ( stderr, "\n" );
		}

	}
}

/*
 * This function prints a small manual to the command line accepted by the
 * program.
 */
void print_help ( ) {

	fprintf ( stdout,
		"REDE22 --- Solve pipe networks \n\n" );
	fprintf ( stdout,
		"OPTIONS: PROGRAM\n" );
	fprintf ( stdout,
		"  -i, --interative\n" );
	fprintf ( stdout,
		"    Prompt the user for inputs (as default in last version)\n" );
	fprintf ( stdout,
		"  -f, --file[=FILE]\n" );
	fprintf ( stdout,
		"    Get net description from FILE (defaults to input.rede2)\n" );
	fprintf ( stdout,
		"  -o, --results[=FILE]\n" );
	fprintf ( stdout,
		"    Write results to FILE (defaults to results.txt)\n" );
	fprintf ( stdout,
		"  -h, --help\n" );
	fprintf ( stdout,
		"    Print this help\n" );
	fprintf ( stdout,
		"  -v, --version\n" );
	fprintf ( stdout,
		"    Print program version\n\n" );
	fprintf ( stdout,
		"OPTIONS: GENERAL\n" );
	fprintf ( stdout,
		"  -t, --type=TYPE\n" );
	fprintf ( stdout,
		"    Type of system to be calculated. TYPE can be one of\n" );
	fprintf ( stdout,
		"      -> \"newtonian\", for newtonian fluid\n" );
	fprintf ( stdout,
		"      -> \"power law\", " );
	fprintf ( stdout, "for power law fluid for smooth tube\n" );
	fprintf ( stdout,
		"      -> \"power law (rough)\", " );
	fprintf ( stdout, "for power law fluid for rough tube\n" );
	fprintf ( stdout,
		"      -> \"bingham\", for Bingham plastic\n" );
	fprintf ( stdout,
		"      -> \"structural\", " );
	fprintf ( stdout, "for structural model (turbulent flow, smooth pipe)\n" );
	fprintf ( stdout,
		"      -> \"real gas\", for real gas in horizontal network\n" );
	fprintf ( stdout,
		"  -d, --density=DENSITY\n" );
	fprintf ( stdout,
		"    Fluid density in g/cm3; defaults to 1.0\n" );
	fprintf ( stdout,
		"  -n, --viscosity=VISCOSITY\n" );
	fprintf ( stdout,
		"    fluid viscosity in cP; defaults to 0.8891\n" );
	fprintf ( stdout,
		"  -D, --diameter=DIAMETER\n" );
	fprintf ( stdout,
		"    Pipe diameter in cm. Optional; individual diamaters\n" );
	fprintf ( stdout, "    can be set with \"-p\" option for the n-th pipe\n" );
	fprintf ( stdout,
		"  -R, --rugosity=RUGOSITY\n" );
	fprintf ( stdout,
		"    Pipe rugosity in mm. Optional; individual rugosities\n" );
	fprintf ( stdout, "    can be set with \"-p\" option for the n-th pipe\n" );

	fprintf ( stdout, "OPTIONS: KNOTS AND PIPES\n" );
	fprintf ( stdout,
		"  -e, --external=EXTERNAL_KNOTS\n" );
	fprintf ( stdout,
		"    List of nodes open to entrances and exits\n" );
	fprintf ( stdout,
		"  -k, --node=KNOT_EXPRESSION\n" );
	fprintf ( stdout,
		"    Set each node Number, Height, and state (external or not)\n" );
	fprintf ( stdout,
		"    Heights can be globally defined through option \"-H\"\n" );
	fprintf ( stdout,
		"      \"KNOT_EXPRESSION\" is a double-quotes-delimited, comma-\n" );
	fprintf ( stdout,
		"      separated list of definitions in the form \"key value\",\n" );
	fprintf ( stdout,
		"      as expected in the configuration file.\n" );
	fprintf ( stdout,
		"      For instance, to define two nodes, 1 and 2, with 1m and\n" );
	fprintf ( stdout,
		"      9m height, the first open, one might use the option\n" );
	fprintf ( stdout,
		"        -k \"1 1 external, 2 9\"\n" );
	fprintf ( stdout,
		"  -p, --pipe=PIPE_EXPRESSION\n" );
	fprintf ( stdout,
		"    Set each pipe start and end nodes, rugosity, diameter, \n" );
	fprintf ( stdout,
		"    lenght and singularity equivalent lenght; rugosity and\n" );
	fprintf ( stdout,
		"    diamater can be globally defined with options \"-R\" and\n" );
	fprintf ( stdout,
		"    \"-D\", respectively\n" );
	fprintf ( stdout,
		"      \"PIPE_EXPRESSION\" is a double-quotes-delimited, comma-\n" );
	fprintf ( stdout,
		"      separated list of definitions in the form \"key value\",\n" );
	fprintf ( stdout,
		"      as expected in the configuration file.\n" );
	fprintf ( stdout,
		"      For instance, to define one pipe, 1, with 9m lenght, 1m\n" );
	fprintf ( stdout,
		"      singularity equivalent lenght, 0.0025mm of rugosity and\n" );
	fprintf ( stdout,
		"      diamater of 10cm, from the 1st to the 3rd node, one\n" );
	fprintf ( stdout,
		"      might use the option\n" );
	fprintf ( stdout,
		"        -p \"1 lenght 9 singularities 1 rugosity 0.0025 \\\n" );
	fprintf ( stdout,
		"              diameter 10 start 1 end 3\"\n" );
	fprintf ( stdout,
		"  -s, --spec=SPEC_EXPRESSION\n" );
	fprintf ( stdout,
		"      Set specifications (as pressure or flow) for a few nodes.\n" );
	fprintf ( stdout,
		"      \"PIPE_EXPRESSION\" is a double-quotes-delimited, comma-\n" );
	fprintf ( stdout,
		"      separated list of definitions in the form \"key value\",\n" );
	fprintf ( stdout,
		"      as expected in the configuration file.\n" );
	fprintf ( stdout,
		"      For instance, to define two specifications, the pressure\n" );
	fprintf ( stdout,
		"      of node 1 at 1atm and the flow at node 3 at 8m3/h (out),\n" );
	fprintf ( stdout,
		"      one might use the option\n" );
	fprintf ( stdout,
		"        -p \"1 pressure 1, 3 flow -8\" \n\n" );
	fprintf ( stdout, "OPTIONS: MODEL-SPECIFIC\n" );
	fprintf ( stdout,
		"  -n, --n=N_FACTOR\n" );
	fprintf ( stdout,
		"    n-factor for power law\n" );
	fprintf ( stdout,
		"  -k, --k=K_FACTOR\n" );
	fprintf ( stdout,
		"    k-factor for power law\n" );
	fprintf ( stdout,
		"  -T, --tzero=T0, --temperature=T0\n" );
	fprintf ( stdout,
		"    T0 for Bingham plastic modelling or temperature for real gas\n" );
	fprintf ( stdout,
		"    according to \"-t\" option\n" );
	fprintf ( stdout,
		"  -m, --muinfty=MU_INFTY\n" );
	fprintf ( stdout,
		"    mu_infty for Bingham plastic modelling\n" );
	fprintf ( stdout,
		"  -N, --nzero=N_ZERO\n" );
	fprintf ( stdout,
		"    N0-factor for structural model\n" );
	fprintf ( stdout,
		"  -l, --lambda=LAMBDA\n" );
	fprintf ( stdout,
		"    lambda-factor for structural model\n" );
	fprintf ( stdout,
		"  -w, --omega=OMEGA\n" );
	fprintf ( stdout,
		"    omega-factor for structural model\n" );
	fprintf ( stdout,
		"  -M, --mm=MM\n" );
	fprintf ( stdout,
		"    Molecular mass for real gas\n" );
	fprintf ( stdout,
		"  -Z, --Z=Z\n" );
	fprintf ( stdout,
		"    Compressibility factor for real gas\n" );
	fprintf ( stdout,
		"  -K, --K=K\n" );
	fprintf ( stdout,
		"    Ratio between Cp and Cv for real gas\n" );

}

void interactive_session ( options *user_options, description *system ) {
}

void read_from_command_line ( int argc, char **argv,
		options *user_options, description *system ) {
}

/*
 * These functions take a string line, a description struct system and an
 * integer no_of_pipes/nodes/specs as arguments, parsing the information
 * described in ``line'' and storing it in
 * system->pipes/nodes/specs[no_of_pipes/nodes/specs];
 */
void eval_pipe_options ( char *line, description *system, int no_of_pipes ) {

	char *word, *value_str, *saveptrstrtokword;

	word = strtok_r ( line, " :\n,", &saveptrstrtokword );
	word = strtok_r ( NULL, " :\n,", &saveptrstrtokword );
	value_str = strtok_r ( NULL, " :\n,", &saveptrstrtokword );
	while ( word != NULL ) {
		if ( ! strcmp ( word, "lenght" ) ) {
			system->pipes[no_of_pipes].L_m = strtod ( value_str, NULL );
		} else if ( ! strcmp ( word, "singularities" ) ) {
			system->pipes[no_of_pipes].L_eq_m = strtod ( value_str, NULL );
		} else if ( ! strcmp ( word, "diameter" ) ) {
			system->pipes[no_of_pipes].D_cm = strtod ( value_str, NULL );
		} else if ( ! strcmp ( word, "rugosity" ) ) {
			system->pipes[no_of_pipes].e_mm = strtod ( value_str, NULL );
		} else if ( ! strcmp ( word, "start" ) ) {
			system->pipes[no_of_pipes].start =
				strtol ( value_str,NULL, 0 ) - 1;
		} else if ( ! strcmp ( word, "end" ) ) {
			system->pipes[no_of_pipes].end =
				strtol ( value_str, NULL, 0 ) - 1;
		}
		word = strtok_r ( NULL, " :\n,",
			&saveptrstrtokword );
		value_str = strtok_r ( NULL, " :\n,",
			&saveptrstrtokword );
	}
}

void eval_node_options ( char *line, description *system, int no_of_nodes ) {

	char *word, *saveptrstrtokword;

	word = strtok_r ( line, " :\n,",
			&saveptrstrtokword );
	if ( word != NULL ) {
		word = strtok_r ( NULL, " :\n,",
			&saveptrstrtokword );
		system->nodes[no_of_nodes].H_m =
			strtod ( word, NULL );
	}
	word = strtok_r ( NULL, " :\n,",
			&saveptrstrtokword );
	if ( word != NULL ) {
		system->nodes[no_of_nodes].is_external
			= TRUE;
	} else {
		system->nodes[no_of_nodes].is_external
			= FALSE;
	}
}

void eval_spec_options ( char *line, description *system, int no_of_specs ) {

	char *word, *value_str, *saveptrstrtokword;

	word = strtok_r ( line, " :\n,",
			&saveptrstrtokword );
	if ( word != NULL ) {
		system->specs[no_of_specs].node_number =
			strtol ( word, NULL, 0 ) - 1;
		word = strtok_r ( NULL, " :\n,",
			&saveptrstrtokword );
		value_str = strtok_r ( NULL, " :\n,",
			&saveptrstrtokword );
		system->specs[no_of_specs].Q_m3_h_or_P_atm =
			strtod ( value_str, NULL );
		if ( ! strcmp ( word, "pressure" ) ) {
		system->specs[no_of_specs].specified_var =
			PRESSURE;
		} else {
		system->specs[no_of_specs].specified_var =
			FLOW;
		}
	}

}

/*
 * This function takes as arguments an options struct (with the configuration
 * file pathname) and a description struct, reads the configuration file and
 * stores its information in its arguments.
 */
void read_from_file ( options *user_options, description *system ) {

	int is_node_pipe_or_spec, is_node, is_pipe, is_spec,
		no_of_nodes, no_of_pipes, no_of_specs;
	long input_file_size;
	char check;
	char *file_as_string, *line, *word;
	char *saveptrstrtokline, *saveptrstrtokword;
	FILE *input_file = fopen ( user_options->input_file_name, "r" );

	if ( input_file == NULL ) {
		fprintf ( stderr, "File %s not found, aborting...\n",
				user_options->input_file_name );
		exit ( EXIT_FAILURE );
	} /* Checking input file existence */

	fseek ( input_file, 0, SEEK_END );
	input_file_size = ftell ( input_file );
	fseek ( input_file, 0, SEEK_SET ); /* Finding file size */

	file_as_string = malloc ( ( input_file_size + 1 ) * sizeof ( char ) );

	if ( file_as_string != NULL ) {
		fread ( file_as_string, 1, input_file_size, input_file );
	} /* Reading file to string for processing */

	fclose ( input_file );

	line = strtok_r ( file_as_string, "\n", &saveptrstrtokline );
	/* read first line of file */

	while ( line != NULL ) {

		word = strtok_r ( line, " :\n", &saveptrstrtokword );
		is_node_pipe_or_spec = FALSE; /* same-line configuration
						* options must be treated
						* differently than multi-line
						* ones, requiring the next
						* line to be fetched.
						*/

		if ( ! strcmp ( word, "type" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( ! strcmp ( word, "power" ) ) {
				word = strtok_r ( NULL, " \n",
						&saveptrstrtokword );
				if ( word != NULL ) {
					word = strtok_r ( NULL, " \n",
						&saveptrstrtokword );
				}
				if ( ! strcmp ( word, "rough" ) ) {
					user_options->type =
						POWER_LAW_ROUGH_PIPE;
				} else {
					user_options->type =
						POWER_LAW_SMOOTH_PIPE;
				}
			} else if ( ! strcmp ( word, "bingham" ) ) {
				user_options->type = BINGHAM_PLASTIC;
			} else if ( ! strcmp ( word, "structural" ) ) {
				user_options->type = STRUCTURAL_MODEL;
			} else if ( ! strcmp ( word, "real" ) ) {
				user_options->type = REAL_GAS;
			} else {
				user_options->type = NEWTONIAN;
			} /* Defaults to newtonian fluid */
		} else if ( ! strcmp ( word, "maxiter" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->maxiter = (int)
					strtol ( word, NULL, 0 );
			}
		} else if ( ! strcmp ( word, "dampening" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->dampening_factor =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "tolerance" ) ||
			! strcmp ( word, "tol" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->Q_tol_percentage =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "viscosity" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.eta_cP =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "density" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.rho_g_cm3 =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "n" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.n =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "k" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.k_Pa_sn =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "T0" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.T0_N_m2 =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "mu" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.mu_infty_cP =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "N0" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.N0_cP =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "lambda" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.lambda_s =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "omega" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.omega =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "T" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.T_oC =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "MM" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.MM_g_gmol =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "Z" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.Z =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "K" ) ||
			! strcmp ( word, "Cp/Cv" ) ||
			! strcmp ( word, "cp/cv" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.k =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "diameter" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->diameter_general =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "rugosity" ) ) {
			word = strtok_r ( NULL, " \n", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->rugosity_general =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "pipes" ) ) {
			is_node_pipe_or_spec = TRUE;
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
			if ( line != NULL ) {
				check = line[0];
				is_pipe = isdigit ( check );
			} else {
				is_pipe = FALSE;
			}
			no_of_pipes = 0;
			system->pipes = malloc ( sizeof ( net_pipe ) );
			while ( is_pipe ) {
				system->pipes = realloc ( system->pipes,
					( no_of_pipes + 1 ) * sizeof ( net_pipe ) );
				system->pipes[no_of_pipes].D_cm =
					user_options->diameter_general;
				system->pipes[no_of_pipes].e_mm =
					user_options->rugosity_general;
				eval_pipe_options ( line, system, no_of_pipes );
				line = strtok_r ( NULL, "\n", &saveptrstrtokline );
				if ( line != NULL ) {
					check = line[0];
					is_pipe = isdigit ( check );
				} else {
					is_pipe = FALSE;
				} /* Check if line is last of pipe descriptions */
				no_of_pipes++;
			}
			user_options->no_of_pipes = no_of_pipes;
		} else if ( ! strcmp ( word, "nodes" ) ) {
			is_node_pipe_or_spec = TRUE;
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
			if ( line != NULL ) {
				check = line[0];
				is_node = isdigit ( check );
			} else {
				is_node = FALSE;
			}
			no_of_nodes = 0;
			system->nodes = malloc ( sizeof ( node ) );
			while ( is_node ) {
				system->nodes = realloc ( system->nodes,
					( no_of_nodes + 1 ) * sizeof ( node ) );
				eval_node_options ( line, system, no_of_nodes );
				line = strtok_r ( NULL, "\n", &saveptrstrtokline );
				check = line[0];
				is_node = isdigit ( check );
				no_of_nodes++;
			}
			user_options->no_of_nodes = no_of_nodes;
		} else if ( ! strcmp ( word, "specifications" ) ) {
			is_node_pipe_or_spec = TRUE;
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
			if ( line != NULL ) {
				check = line[0];
				is_spec = isdigit ( check );
			} else {
				is_spec = FALSE;
			}
			no_of_specs = 0;
			system->specs = malloc ( sizeof ( specified_node_vars ) );
			while ( is_spec ) {
				system->specs = realloc ( system->specs,
					( no_of_specs + 1 ) *
					sizeof ( specified_node_vars ) );
				eval_spec_options ( line, system, no_of_specs );
				line = strtok_r ( NULL, "\n", &saveptrstrtokline );
				if ( line != NULL ) {
					check = line[0];
					is_spec = isdigit ( check );
				} else {
					is_spec = FALSE;
				} /* Check if line is last of pipe descriptions */
				no_of_specs++;
			}
			user_options->no_of_specs = no_of_specs;
		}

		if ( ! is_node_pipe_or_spec ) {
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
		}

	}

	free ( file_as_string );

}
