#include "header.h"

void print_help ( );
void print_choices ( options *user_options, description *system );
void interactive_session ( options *user_options, description *system );
void read_from_file ( options *user_options, description *system );
void create_new_file ( options *user_options, description *system );
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

	user_options->type = NEWTONIAN;
	user_options->existing_file = FALSE;
	user_options->write_results = FALSE;
	user_options->maxiter = MAXITER;
	user_options->interactive = FALSE;
	user_options->no_of_nodes = 0;
	user_options->no_of_pipes = 0;
	user_options->no_of_specs = 0;
	user_options->help = FALSE;
	user_options->verbose_level = QUIET;
	user_options->rugosity_general = RUGO_GEN;
	user_options->diameter_general = DIAM_GEN;
	user_options->Q_tol_percentage = QTOL;
	user_options->dampening_factor = DAMP;
	user_options->input_file_name = NULL;
	user_options->output_file_name = NULL;

	system->fluid.eta_cP = ETA_CP_DEFAULT;
	system->fluid.rho_g_cm3 = RHO_G_CM3_DEFAULT;

	/* We set the default user options */

	system->pipes = NULL;
	system->nodes = NULL;
	system->specs = NULL;


	if ( argc >= 2 ) {
		read_from_command_line ( argc, argv, user_options, system );
	} else {
		user_options->interactive = TRUE;
	} /* If no arguments given, mimic behaviour of last version */

	if ( user_options->help == TRUE ) {
		print_help ( argv );
		finalize ( user_options, system );
		exit (EXIT_SUCCESS);
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

		fprintf ( stdout, "OPTIONS SELECTED:\n\n" );
		fprintf ( stdout, "  Iterations:   %d\n",
				user_options->maxiter );
		fprintf ( stdout, "  Tolerance:    %lf %%\n",
				user_options->Q_tol_percentage );
		fprintf ( stdout, "  Dampening:    %lf\n\n",
				user_options->dampening_factor );
		fprintf ( stdout, "  Viscosity:    %lf cP\n",
				system->fluid.eta_cP );
		fprintf ( stdout, "  Density:      %lf g/cm3\n",
				system->fluid.rho_g_cm3 );
		switch ( user_options->type ) {
			case POWER_LAW_SMOOTH_PIPE:
				fprintf ( stdout, "  n:            %lf\n",
						system->fluid.n );
				fprintf ( stdout, "  k:            %lf Pa/sn\n",
						system->fluid.k_Pa_sn );
				break;
			case POWER_LAW_ROUGH_PIPE:
				fprintf ( stdout, "  n:            %lf\n",
						system->fluid.n );
				fprintf ( stdout, "  k:            %lf Pa/sn\n",
						system->fluid.k_Pa_sn );
				break;
			case BINGHAM_PLASTIC:
				fprintf ( stdout, "  T0:           %lf N/m2\n",
						system->fluid.T0_N_m2 );
				fprintf ( stdout, "  mu-infty:     %lf cP\n",
						system->fluid.mu_infty_cP );
				break;
			case STRUCTURAL_MODEL:
				fprintf ( stdout, "  N0:           %lf cP\n",
						system->fluid.N0_cP );
				fprintf ( stdout, "  lambda:       %lf s\n",
						system->fluid.lambda_s );
				fprintf ( stdout, "  omega:        %lf\n",
						system->fluid.omega );
				break;
			case REAL_GAS:
				fprintf ( stdout, "  T:            %lf C\n",
						system->fluid.T_oC );
				fprintf ( stdout, "  MM:           %lf g/gmol\n",
						system->fluid.MM_g_gmol );
				fprintf ( stdout, "  Z:            %lf\n",
						system->fluid.Z );
				fprintf ( stdout, "  Cp/Cv:        %lf\n",
						system->fluid.k );
				break;
		}
		fprintf ( stdout, "\n  Pipes:        %d\n",
				user_options->no_of_pipes );
		if ( user_options->verbose_level == VERBOSE ) {
			fprintf ( stdout,
				"   Pipe\tLength\t\tDiameter\t" );
			fprintf ( stdout, "Rugosity\tTerminal nodes\n" );
			for ( int i = 0; i < user_options->no_of_pipes; i++ ) {
				fprintf ( stdout,
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
			fprintf ( stdout, "\n" );
		}
		fprintf ( stdout, "  Nodes:        %d\n",
				user_options->no_of_nodes );
		if ( user_options->verbose_level == VERBOSE ) {
			fprintf ( stdout, "   Node\tHeight\n" );
			for ( int i = 0; i < user_options->no_of_nodes; i++ ) {
				fprintf ( stdout,
					"    %d\t%f m\t",
					i+1, system->nodes[i].H_m );
				if ( system->nodes[i].is_external == TRUE ) {
					fprintf ( stdout, "(open)" );
				}
				fprintf ( stdout, "\n" );
			}
			fprintf ( stdout, "\n" );
		}
		fprintf ( stdout, "  Specs:        %d\n",
				user_options->no_of_specs );
		if ( user_options->verbose_level == VERBOSE ) {
			fprintf ( stdout, "   Node\t\tVariable\tValue\n" );
			for ( int i = 0; i < user_options->no_of_specs; i++ ) {
				if ( system->specs[i].specified_var == FLOW ) {
					fprintf ( stdout,
						"    %d\t\tflow rate\t%f m3/h\n",
						system->specs[i].node_number + 1,
						system->specs[i].Q_m3_h_or_P_atm );
				} else {
					fprintf ( stdout,
						"    %d\t\tpressure\t%f atm\n",
						system->specs[i].node_number + 1,
						system->specs[i].Q_m3_h_or_P_atm );
				}
			}
			fprintf ( stdout, "\n" );
		}

	}
}

/*
 * This function prints a small manual to the command line accepted by the
 * program.
 */
void print_help ( char **argv ) {

	fprintf ( stdout,
		"%s --- Solve pipe networks \n", argv[0] );
	fprintf ( stdout,
		"    04/09/09 Jorge Andrey W. Gut\n" );
	fprintf ( stdout,
		"    05/11/21 Pedro Callil\n" );
	fprintf ( stdout,
		"    Version %d.%d.%d\n\n",
			VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH );
	fprintf ( stdout,
		"OPTIONS: PROGRAM\n" );
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
		"  -V, --version\n" );
	fprintf ( stdout,
		"    Print program version\n" );
	fprintf ( stdout,
		"  -Q, --tolerance=TOL\n" );
	fprintf ( stdout,
		"    Tolerance for flow rates' relative errors\n" );
	fprintf ( stdout,
		"  -a, --dampening=DAMPENING_FACTOR\n" );
	fprintf ( stdout,
		"    Dampening factor for solution\n" );
	fprintf ( stdout,
		"  -I, --iterations=MAXITER\n" );
	fprintf ( stdout,
		"    Maximum number of iterations\n\n" );
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
		"  -v, --viscosity=VISCOSITY\n" );
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

	fprintf ( stdout, "OPTIONS: NODE AND PIPES\n" );
	fprintf ( stdout,
		"  -k, --node=NODE_EXPRESSION\n" );
	fprintf ( stdout,
		"    Set each node Number, Height, and state (external or not)\n" );
	fprintf ( stdout,
		"    Heights can be globally defined through option \"-H\"\n" );
	fprintf ( stdout,
		"      \"NODE_EXPRESSION\" is a double-quotes-delimited, comma-\n" );
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
		"  -K, --k=K_FACTOR\n" );
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
		"  -r, --r=r\n" );
	fprintf ( stdout,
		"    Ratio between Cp and Cv for real gas\n" );

}

void read_from_command_line ( int argc, char **argv,
		options *user_options, description *system ) {

	int c, i, modified;
	int no_of_nodes, no_of_pipes, no_of_specs;
	int node_index, pipe_index, spec_node, spec_var;
	char *node_opts, *pipe_opts, *spec_opts;
	char *node_opts_to_get_index, *pipe_opts_to_get_index,
		*spec_opts_to_get_index;
	char *node_index_str, *pipe_index_str;
	char *savenodeopts, *savepipeopts, *savespecopts;
	char *savenodeindex, *savepipeindex, *savespecindex;
	char *spec_node_str, *spec_var_str;
	const char *short_options =
		"hf:o:V:I:ia:Q:t:d:v:D:R:k:p:s:n:K:T:m:N:l:w:M:Z:r:";
	struct option long_options[] = {
		{"help",	no_argument,		NULL,	'h'},
		{"file",	required_argument,	NULL,	'f'},
		{"results",	required_argument,	NULL,	'o'},
		{"verbose",	required_argument,	NULL,	'V'},
		{"iterations",	required_argument,	NULL,	'I'},
		{"dampening",	required_argument,	NULL,	'a'},
		{"tolerance",	required_argument,	NULL,	'Q'},
		{"type",	required_argument,	NULL,	't'},
		{"density",	required_argument,	NULL,	'd'},
		{"viscosity",	required_argument,	NULL,	'v'},
		{"diameter",	required_argument,	NULL,	'D'},
		{"rugosity",	required_argument,	NULL,	'R'},
		{"node",	required_argument,	NULL,	'k'},
		{"pipe",	required_argument,	NULL,	'p'},
		{"spec",	required_argument,	NULL,	's'},
		{"n",		required_argument,	NULL,	'n'},
		{"k",		required_argument,	NULL,	'K'},
		{"tzero",	required_argument,	NULL,	'T'},
		{"muinfty",	required_argument,	NULL,	'm'},
		{"nzero",	required_argument,	NULL,	'N'},
		{"lambda",	required_argument,	NULL,	'l'},
		{"omega",	required_argument,	NULL,	'w'},
		{"mm",		required_argument,	NULL,	'M'},
		{"Z",		required_argument,	NULL,	'Z'},
		{"r",		required_argument,	NULL,	'r'}
	};

	while ( ( c = getopt_long ( argc, argv,
			short_options, long_options, NULL ) ) != -1 ) {
		switch (c) {
			case -1:
			case 0:
				break;

			case 'h':
				user_options->help = TRUE;
				user_options->verbose_level = QUIET;
				break;

			case 'i':
				user_options->interactive = TRUE;
				break;

			case 'f':
				user_options->input_file_name = malloc (
					( strlen ( optarg ) + 1 ) *
					sizeof ( char ) );
				strcpy ( user_options->input_file_name, optarg );
				if ( ! access ( optarg, F_OK ) ) {
					user_options->existing_file =
						READ_EXISTING_FILE;
					read_from_file ( user_options, system );
				} else {
					user_options->existing_file =
						CREATE_NEW_FILE;
					create_new_file ( user_options, system );
				}
				break;

			case 'o':
				user_options->write_results = TRUE;
				user_options->output_file_name = malloc (
					( strlen ( optarg ) + 1 ) *
					sizeof ( char ) );
				strcpy ( user_options->output_file_name, optarg );
				break;

			case 'V':
				if ( ! strcmp ( optarg, "quiet" ) ) {
					user_options->verbose_level = QUIET;
				} else if ( ! strcmp ( optarg, "summary" ) ) {
					user_options->verbose_level = NORMAL;
				} else {
					user_options->verbose_level = VERBOSE;
				}
				break;

			case 'I':
				user_options->maxiter =
					strtol ( optarg, NULL, 0 ) + 1;
				break;

			case 'a':
				user_options->dampening_factor =
					strtod ( optarg, NULL );
				break;

			case 'Q':
				user_options->Q_tol_percentage =
					strtod ( optarg, NULL );
				break;

			case 't':
				if ( ! strcmp ( optarg, "power law" ) ) {
					user_options->type = POWER_LAW_SMOOTH_PIPE;
				} else if ( ! strcmp ( optarg,
						"power law (rough)" ) ) {
					user_options->type = POWER_LAW_ROUGH_PIPE;
				} else if ( !strcmp ( optarg, "bingham" ) ) {
					user_options->type = BINGHAM_PLASTIC;
				} else if ( !strcmp ( optarg, "structural" ) ) {
					user_options->type = STRUCTURAL_MODEL;
				} else if ( ! strcmp ( optarg, "real gas" ) ) {
					user_options->type = REAL_GAS;
				}
				break;

			case 'd':
				system->fluid.rho_g_cm3 = strtod ( optarg, NULL );
				break;

			case 'v':
				system->fluid.eta_cP = strtod ( optarg, NULL );
				break;

			case 'D':
				user_options->diameter_general =
					strtod ( optarg, NULL );
				break;

			case 'R':
				user_options->rugosity_general =
					strtod ( optarg, NULL );
				break;

			case 'k':
				no_of_nodes = user_options->no_of_nodes - 1;
				node_opts = strtok_r ( optarg, ",", &savenodeopts );
				while ( node_opts != NULL ) {
					node_opts_to_get_index =
						malloc ( sizeof ( char ) *
							( strlen ( node_opts )
							+ 1 ) );
					strcpy ( node_opts_to_get_index, node_opts );
					node_index_str =
						strtok_r ( node_opts_to_get_index,
							" ", &savenodeindex );
					node_index = strtol ( node_index_str,
							NULL, 0 ) - 1;
					free ( node_opts_to_get_index );
					if ( node_index > no_of_nodes ) {
						no_of_nodes++;
						system->nodes =
							realloc ( system->nodes,
								( no_of_nodes + 1 ) *
								sizeof ( node ) );
						system->nodes[no_of_nodes].H_m = 0.0;
					}
					eval_node_options ( node_opts,
						system, node_index );
					node_opts = strtok_r ( NULL,
						",", &savenodeopts );
				}
				user_options->no_of_nodes = no_of_nodes + 1;
				break;

			case 'p':
				no_of_pipes = user_options->no_of_pipes - 1;
				pipe_opts = strtok_r ( optarg, ",", &savepipeopts );
				while ( pipe_opts != NULL ) {
					pipe_opts_to_get_index =
						malloc ( sizeof ( char ) *
							( strlen ( pipe_opts )
							+ 1 ) );
					strcpy ( pipe_opts_to_get_index, pipe_opts );
					pipe_index_str =
						strtok_r ( pipe_opts_to_get_index,
							" ", &savepipeindex );
					pipe_index = strtol ( pipe_index_str,
							NULL, 0 ) - 1;
					free ( pipe_opts_to_get_index );
					if ( pipe_index > no_of_pipes ) {
						no_of_pipes++;
						system->pipes =
							realloc ( system->pipes,
								( no_of_pipes + 1 ) *
								sizeof ( net_pipe ) );
						system->pipes[no_of_pipes].D_cm =
							user_options->
								diameter_general;
						system->pipes[no_of_pipes].L_eq_m =
							0.0;
						system->pipes[no_of_pipes].e_mm =
							user_options->
								rugosity_general;
					}
					eval_pipe_options ( pipe_opts,
						system, pipe_index );
					pipe_opts = strtok_r ( NULL,
						",", &savepipeopts );
				}
				user_options->no_of_pipes = no_of_pipes + 1;
				break;

			case 's':
				no_of_specs = user_options->no_of_specs - 1;
				spec_opts = strtok_r ( optarg, ",", &savespecopts );
				while ( spec_opts != NULL ) {
					spec_opts_to_get_index =
						malloc ( sizeof ( char ) *
							( strlen ( spec_opts )
							+ 1 ) );
					strcpy ( spec_opts_to_get_index, spec_opts );
					spec_node_str =
						strtok_r ( spec_opts_to_get_index,
							" ", &savespecindex );
					spec_node = strtol ( spec_node_str,
							NULL, 0 ) - 1;
					spec_var_str = strtok_r ( NULL, " ",
							&savespecindex );
					if ( ! strcmp ( spec_var_str,
							"pressure" ) ) {
						spec_var = PRESSURE;
					} else {
						spec_var = FLOW;
					}
					free ( spec_opts_to_get_index );
					modified = FALSE;
					for ( i = 0; i < user_options->no_of_specs;
							i++ ) {
						if ( system->specs[i].node_number ==
						spec_node &&
						system->specs[i].specified_var ==
						spec_var ) {
							modified = TRUE;
							eval_spec_options (
								spec_opts, system,
									i );
						}
					}
					if ( modified == FALSE ) {
						no_of_specs++;
						system->specs =
							realloc ( system->specs,
							( no_of_specs + 1 ) *
						sizeof ( specified_node_vars ) );
						eval_spec_options ( spec_opts,
							system, no_of_specs );
					}
					spec_opts = strtok_r ( NULL,
						",", &savespecopts );
				}
				user_options->no_of_specs = no_of_specs + 1;
				break;

			case 'n':
				system->fluid.n = strtod ( optarg, NULL );
				break;

			case 'K':
				system->fluid.k_Pa_sn = strtod ( optarg, NULL );
				break;

			case 'T':
				if ( user_options->type == BINGHAM_PLASTIC ) {
					system->fluid.T0_N_m2 =
						strtod ( optarg, NULL );
				} else if ( user_options->type == REAL_GAS ) {
					system->fluid.T_oC =
						strtod ( optarg, NULL );
				}
				break;

			case 'm':
				system->fluid.mu_infty_cP =
					strtod ( optarg, NULL );
				break;

			case 'N':
				system->fluid.N0_cP =
					strtod ( optarg, NULL );
				break;

			case 'l':
				system->fluid.lambda_s =
					strtod ( optarg, NULL );
				break;

			case 'w':
				system->fluid.omega =
					strtod ( optarg, NULL );
				break;

			case 'M':
				system->fluid.MM_g_gmol =
					strtod ( optarg, NULL );
				break;

			case 'Z':
				system->fluid.Z =
					strtod ( optarg, NULL );
				break;

			case 'r':
				system->fluid.k =
					strtod ( optarg, NULL );
				break;
		}
	}
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
				system->pipes[no_of_pipes].L_eq_m = 0;
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

/*
 * This function creates a new file, that can be used in another run.
 * Takes as arguments the general user options (in a pointer to an
 * options struct) and the system description (in a pointer to a
 * description struct).
 */
void create_new_file ( options *user_options, description *system ) {

	int i;
	FILE *new_input_file;

	new_input_file = fopen ( user_options->input_file_name, "w" );

	fprintf ( new_input_file,
		"This file was automatically built from user options\n\n" );

	fprintf ( new_input_file,
		"problem options:\n" );
	fprintf ( new_input_file, "type " );
	switch ( user_options->type ) {
		case NEWTONIAN:
			fprintf ( new_input_file, "newtonian\n" );
			break;
		case POWER_LAW_SMOOTH_PIPE:
			fprintf ( new_input_file, "power law smooth pipe\n" );
			fprintf ( new_input_file, "n %f\n",
				system->fluid.n );
			fprintf ( new_input_file, "k %f\n",
				system->fluid.k_Pa_sn );
			break;
		case POWER_LAW_ROUGH_PIPE:
			fprintf ( new_input_file, "power law rough pipe\n" );
			fprintf ( new_input_file, "n %f\n",
				system->fluid.n );
			fprintf ( new_input_file, "k %f\n",
				system->fluid.k_Pa_sn );
			break;
		case BINGHAM_PLASTIC:
			fprintf ( new_input_file, "bingham plastic\n" );
			fprintf ( new_input_file, "T0 %f\n",
				system->fluid.T0_N_m2 );
			fprintf ( new_input_file, "mu %f\n",
				system->fluid.mu_infty_cP );
			break;
		case STRUCTURAL_MODEL:
			fprintf ( new_input_file, "structural model\n" );
			fprintf ( new_input_file, "N0 %f\n",
				system->fluid.n );
			fprintf ( new_input_file, "lambda %f\n",
				system->fluid.lambda_s );
			fprintf ( new_input_file, "omega %f\n",
				system->fluid.omega );
			break;
		case REAL_GAS:
			fprintf ( new_input_file, "real gas\n" );
			fprintf ( new_input_file, "T %f\n",
				system->fluid.T_oC );
			fprintf ( new_input_file, "MM %f\n",
				system->fluid.MM_g_gmol );
			fprintf ( new_input_file, "Z %f\n",
				system->fluid.Z );
			fprintf ( new_input_file, "K %f\n",
				system->fluid.k );
			break;
	}
	fprintf ( new_input_file, "maxiter %d\n", user_options->maxiter );
	fprintf ( new_input_file, "dampening %f\n", user_options->dampening_factor );
	fprintf ( new_input_file, "tolerance %f\n", user_options->Q_tol_percentage );
	fprintf ( new_input_file, "density %f\n", system->fluid.rho_g_cm3 );
	fprintf ( new_input_file, "viscosity %f\n\n", system->fluid.eta_cP );

	fprintf ( new_input_file, "rugosity %f\n", user_options->rugosity_general );
	fprintf ( new_input_file, "diameter %f\n\n",
			user_options->diameter_general );

	fprintf ( new_input_file, "pipes\n" );
	for ( i = 0; i < user_options->no_of_pipes; i++ ) {
		fprintf ( new_input_file, "%d ", i+1 );
		fprintf ( new_input_file, "lenght %f ", system->pipes[i].L_m );
		fprintf ( new_input_file, "singularities %f ",
				system->pipes[i].L_eq_m );
		if ( system->pipes[i].D_cm != user_options->diameter_general ) {
			fprintf ( new_input_file, "diameter %f ",
					system->pipes[i].D_cm );
		}
		if ( system->pipes[i].D_cm != user_options->rugosity_general ) {
			fprintf ( new_input_file, "rugosity %f ",
					system->pipes[i].e_mm );
		}
		fprintf ( new_input_file, "start %d end %d\n",
			system->pipes[i].start, system->pipes[i].end );
	}

	fprintf ( new_input_file, "\nnodes\n" );
	for ( i = 0; i < user_options->no_of_nodes; i++ ) {
		fprintf ( new_input_file, "%d ", i+1 );
		fprintf ( new_input_file, "%f ", system->nodes[i].H_m );
		if ( system->nodes[i].is_external == TRUE ) {
			fprintf ( new_input_file, "external" );
		}
		fprintf ( new_input_file, "\n" );
	}

	fprintf ( new_input_file, "\nspecifications\n" );
	for ( i = 0; i < user_options->no_of_specs; i++ ) {
		fprintf ( new_input_file, "%d ", system->specs[i].node_number + 1 );
		if ( system->specs[i].specified_var == FLOW ) {
			fprintf ( new_input_file, "flow " );
		} else {
			fprintf ( new_input_file, "pressure " );
		}
		fprintf ( new_input_file, "%f\n", system->specs[i].Q_m3_h_or_P_atm );
	}
}
