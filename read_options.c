#include "header.h"

void print_help ( );
void print_choices ( options *user_options, description *system );
void interactive_session ( options *user_options, description *system );
void read_from_file ( options *user_options, description *system );
void read_from_command_line ( options *user_options, description *system );

void read_options ( int argc, char **argv,
		options *user_options, description *system ) {
	/*print_help ();*/

	user_options->input_file_name = argv[1];

	read_from_file ( user_options, system );
	print_choices ( user_options, system );

}

void print_choices ( options *user_options, description *system ) {

	fprintf ( stderr, "OPTIONS SELECTED:\n" );
	fprintf ( stderr, "  Iterations:   %d\n", user_options->maxiter );
	fprintf ( stderr, "  Tolerance:    %lf%%\n", user_options->Q_tol_percentage );
	fprintf ( stderr, "  Dampening:    %lf\n\n", user_options->dampening_factor );
	fprintf ( stderr, "  Viscosity:    %lfcP\n", system->fluid.eta_cP );
	fprintf ( stderr, "  Density:      %lfg/cm3\n", system->fluid.rho_g_cm3 );
	switch ( user_options->type ) {
		case POWER_LAW_SMOOTH_PIPE:
			fprintf ( stderr, "  n:            %lf\n",
					system->fluid.n );
			fprintf ( stderr, "  k:            %lfPa/sn\n",
					system->fluid.k_Pa_sn );
			break;
		case POWER_LAW_ROUGH_PIPE:
			fprintf ( stderr, "  n:            %lf\n",
					system->fluid.n );
			fprintf ( stderr, "  k:            %lfPa/sn\n",
					system->fluid.k_Pa_sn );
			break;
		case BINGHAM_PLASTIC:
			fprintf ( stderr, "  T0:           %lfN/m2\n",
					system->fluid.T0_N_m2 );
			fprintf ( stderr, "  mu-infty:     %lfcP\n",
					system->fluid.mu_infty_cP );
			break;
		case STRUCTURAL_MODEL:
			fprintf ( stderr, "  N0:           %lfcP\n",
					system->fluid.N0_cP );
			fprintf ( stderr, "  lambda:       %lfs\n",
					system->fluid.lambda_s );
			fprintf ( stderr, "  omega:        %lf\n",
					system->fluid.omega );
			break;
		case REAL_GAS:
			fprintf ( stderr, "  T:            %lf C\n",
					system->fluid.T_oC );
			fprintf ( stderr, "  MM:           %lfg/gmol\n",
					system->fluid.MM_g_gmol );
			fprintf ( stderr, "  Z:            %lf\n",
					system->fluid.Z );
			fprintf ( stderr, "  Cp/Cv:        %lf\n",
					system->fluid.k );
			break;
	}
	fprintf ( stderr, "\n  Pipes:        %d\n", user_options->no_of_pipes );
	fprintf ( stderr, "  Knots:        %d\n", user_options->no_of_knots );
	fprintf ( stderr, "  Specs:        %d\n", user_options->no_of_specs );
}

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
	fprintf ( stdout, "can be set with \"-pn\" options for the n-th pipe\n" );
	fprintf ( stdout,
		"  -R, --rugosity=RUGOSITY\n" );
	fprintf ( stdout,
		"    Pipe rugosity in mm. Optional; individual rugosities\n" );
	fprintf ( stdout, "can be set with \"-pn\" options for the n-th pipe\n" );
	fprintf ( stdout,
		"  -H, --height=HEIGHT\n" );
	fprintf ( stdout,
		"    Knot height in m. Optional; individual heights\n" );
	fprintf ( stdout, "can be set with \"-kn\" options for the n-th knot\n\n" );

	fprintf ( stdout, "OPTIONS: KNOTS AND PIPES\n" );
	fprintf ( stdout,
		"  -e, --external=EXTERNAL_KNOTS\n" );
	fprintf ( stdout,
		"    List of knots open to entrances and exits\n" );
	fprintf ( stdout,
		"  -k, --knot=KNOT_EXPRESSION\n" );
	fprintf ( stdout,
		"    Set each knot Number and Height. Pressure and Flow\n" );
	fprintf ( stdout,
		"    that should be specified can also be included here\n" );
	fprintf ( stdout,
		"    Heights can be globally defined through option \"-H\"\n" );
	fprintf ( stdout,
		"      \"KNOT_EXPRESSION\" is a doble-quotes-delimited, comma-\n" );
	fprintf ( stdout,
		"      separated list of definitions in the form \"key=value\";\n" );
	fprintf ( stdout,
		"      for instance, to define two knots, 1 and 2, with 1m and\n" );
	fprintf ( stdout,
		"      9m height, and pressure of 1atm and flow rate of 5 m3/h,\n" );
	fprintf ( stdout,
		"      one might use the option\n" );
	fprintf ( stdout,
		"        -k \"n=1,h=1,p=1\" \"n=2,h=9,f=5\"\n" );
	fprintf ( stdout,
		"  -p, --pipe=PIPE_EXPRESSION\n" );
	fprintf ( stdout,
		"    Set each pipe start and end knots, rugosity, diameter, \n" );
	fprintf ( stdout,
		"    lenght and singularity equivalent lenght; rugosity and\n" );
	fprintf ( stdout,
		"    diamater can be globally defined with options \"-R\" and\n" );
	fprintf ( stdout,
		"    \"-D\", respectively\n" );
	fprintf ( stdout,
		"      \"PIPE_EXPRESSION\" is a doble-quotes-delimited, comma-\n" );
	fprintf ( stdout,
		"      separated list of definitions in the form \"key=value\";\n" );
	fprintf ( stdout,
		"      for instance, to define one pipe, 1, with 9m lenght, 1m\n" );
	fprintf ( stdout,
		"      singularity equivalent lenght, 0.0025mm of rugosity and\n" );
	fprintf ( stdout,
		"      diamater of 10cm, one might use the option\n" );
	fprintf ( stdout,
		"        -p \"n=1,l=9,e=1,r=0.0025,d=10\"\n\n" );
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

void read_from_file ( options *user_options, description *system ) {

	int is_knot_pipe_or_spec, is_knot, is_pipe, is_spec,
		no_of_knots, no_of_pipes, no_of_specs;
	long input_file_size;
	char check;
	char *file_as_string, *line, *word, *value_str;
	char *saveptrstrtokline, *saveptrstrtokword;
	FILE *input_file = fopen ( user_options->input_file_name, "r" );

	if ( input_file == NULL ) {
		fprintf ( stderr, "File %s not found, aborting...\n",
				user_options->input_file_name );
		exit ( EXIT_FAILURE );
	} /* Checking input file existence */

	fseek ( input_file, 0, SEEK_END );
	input_file_size = ftell ( input_file );
	fseek ( input_file, 0, SEEK_SET );

	file_as_string = malloc ( ( input_file_size + 1 ) * sizeof ( char ) );

	if ( file_as_string != NULL ) {
		fread ( file_as_string, 1, input_file_size, input_file );
	}

	fclose ( input_file );

	line = strtok_r ( file_as_string, "\n", &saveptrstrtokline );

	while ( line != NULL ) {

		word = strtok_r ( line, " :\n-", &saveptrstrtokword );
		is_knot_pipe_or_spec = FALSE;

		if ( ! strcmp ( word, "type" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( ! strcmp ( word, "power" ) ) {
				word = strtok_r ( NULL, " \n-",
						&saveptrstrtokword );
				if ( word != NULL ) {
					word = strtok_r ( NULL, " \n-",
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
			}
		} else if ( ! strcmp ( word, "maxiter" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->maxiter = (int)
					strtol ( word, NULL, 0 );
			}
		} else if ( ! strcmp ( word, "dampening" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->dampening_factor =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "tolerance" ) ||
			! strcmp ( word, "tol" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				user_options->Q_tol_percentage =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "viscosity" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.eta_cP =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "density" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.rho_g_cm3 =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "n" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.n =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "k" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.k_Pa_sn =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "T0" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.T0_N_m2 =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "mu" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.mu_infty_cP =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "N0" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.N0_cP =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "lambda" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.lambda_s =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "omega" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.omega =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "T" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.T_oC =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "MM" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.MM_g_gmol =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "Z" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.Z =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "K" ) ||
			! strcmp ( word, "Cp/Cv" ) ||
			! strcmp ( word, "cp/cv" ) ) {
			word = strtok_r ( NULL, " \n-", &saveptrstrtokword );
			if ( word != NULL ) {
				system->fluid.k =
					strtod ( word, NULL );
			}
		} else if ( ! strcmp ( word, "pipes" ) ) {
			is_knot_pipe_or_spec = TRUE;
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
			check = line[0];
			is_pipe = isdigit ( check );
			no_of_pipes = 0;
			system->pipes = malloc ( sizeof ( net_pipe ) );
			while ( is_pipe ) {
				system->pipes = realloc ( system->pipes,
					( no_of_pipes + 1 ) * sizeof ( net_pipe ) );
				word = strtok_r ( line, " :\n-,",
						&saveptrstrtokword );
				word = strtok_r ( NULL, " :\n-,",
					&saveptrstrtokword );
				value_str = strtok_r ( NULL, " :\n-,",
					&saveptrstrtokword );
				while ( word != NULL ) {
					if ( ! strcmp ( word, "lenght" ) ) {
						system->pipes[no_of_pipes].L_m =
							strtod ( value_str, NULL );
					} else if ( ! strcmp ( word,
							"singularities" ) ) {
						system->pipes[no_of_pipes].L_eq_m =
							strtod ( value_str, NULL );
					} else if ( ! strcmp ( word, "diameter" ) ) {
						system->pipes[no_of_pipes].D_cm =
							strtod ( value_str, NULL );
					} else if ( ! strcmp ( word, "rugosity" ) ) {
						system->pipes[no_of_pipes].e_mm =
							strtod ( value_str, NULL );
					} else if ( ! strcmp ( word, "start" ) ) {
						system->pipes[no_of_pipes].start =
							1 + strtol ( value_str,
								NULL, 0 );
					} else if ( ! strcmp ( word, "end" ) ) {
						system->pipes[no_of_pipes].end =
							1 + strtol ( value_str,
								NULL, 0 );
					}
					word = strtok_r ( NULL, " :\n-,",
						&saveptrstrtokword );
					value_str = strtok_r ( NULL, " :\n-,",
						&saveptrstrtokword );
				}
				line = strtok_r ( NULL, "\n", &saveptrstrtokline );
				if ( line != NULL ) {
					check = line[0];
					is_pipe = isdigit ( check );
				} else {
					is_pipe = FALSE;
				}
				no_of_pipes++;
			}
			user_options->no_of_pipes = no_of_pipes;
		} else if ( ! strcmp ( word, "knots" ) ) {
			is_knot_pipe_or_spec = TRUE;
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
			check = line[0];
			is_knot = isdigit ( check );
			no_of_knots = 0;
			system->knots = malloc ( sizeof ( knot ) );
			while ( is_knot ) {
				system->knots = realloc ( system->knots,
					( no_of_knots + 1 ) * sizeof ( knot ) );
				word = strtok_r ( line, " :\n-,",
						&saveptrstrtokword );
				if ( word != NULL ) {
					word = strtok_r ( line, " :\n-,",
						&saveptrstrtokword );
					system->knots[no_of_knots].H_m =
						strtod ( word, NULL );
				}
				word = strtok_r ( line, " :\n-,",
						&saveptrstrtokword );
				if ( word != NULL ) {
					system->knots[no_of_knots].is_external
						= TRUE;
				} else {
					system->knots[no_of_knots].is_external
						= FALSE;
				}
				line = strtok_r ( NULL, "\n", &saveptrstrtokline );
				check = line[0];
				is_knot = isdigit ( check );
				no_of_knots++;
			}
			user_options->no_of_knots = no_of_knots;
		} else if ( ! strcmp ( word, "specifications" ) ) {
			is_knot_pipe_or_spec = TRUE;
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
			check = line[0];
			is_spec = isdigit ( check );
			no_of_specs = 0;
			system->specs = malloc ( sizeof ( specified_knot_vars ) );
			while ( is_spec ) {
				system->specs = realloc ( system->specs,
					( no_of_specs + 1 ) *
					sizeof ( specified_knot_vars ) );
				word = strtok_r ( line, " :\n-,",
						&saveptrstrtokword );
				if ( word != NULL ) {
					system->specs[no_of_specs].knot_number =
						strtol ( word, NULL, 0 ) - 1;
					word = strtok_r ( line, " :\n-,",
						&saveptrstrtokword );
					value_str = strtok_r ( line, " :\n-,",
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
				line = strtok_r ( NULL, "\n", &saveptrstrtokline );
				if ( line != NULL ) {
					check = line[0];
					is_spec = isdigit ( check );
				} else {
					is_spec = FALSE;
				}
				no_of_specs++;
			}
			user_options->no_of_specs = no_of_specs;
		}

		if ( ! is_knot_pipe_or_spec ) {
			line = strtok_r ( NULL, "\n", &saveptrstrtokline );
		}

	}

	free ( file_as_string );

}
