#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <argp.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <getopt.h>

#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_PATCH 0

#define FALSE 0
#define TRUE 1

#define QUIET 15
#define NORMAL 16
#define VERBOSE 17

#define MAXITER 10000 /* Default maximum number of iterations */
#define PI 3.14159265

/* Type definitions */
#define	NEWTONIAN		1
#define	POWER_LAW_SMOOTH_PIPE	2
#define	POWER_LAW_ROUGH_PIPE	3
#define	BINGHAM_PLASTIC		4
#define	STRUCTURAL_MODEL	5
#define	REAL_GAS		6

/* Definitions for handling files */
#define READ_EXISTING_FILE	19
#define CREATE_NEW_FILE		20

#define PRESSURE	23
#define FLOW		34

#define RUGO_GEN	0.0025
#define DIAM_GEN	10.000
#define QTOL		0.01
#define DAMP		0.50

#define ETA_CP_DEFAULT		0.8891
#define RHO_G_CM3_DEFAULT	1.0000

typedef struct {
	double H_m;		/* Height of the node */
	int is_external;	/* Check if node cuts control volume */
} node;

typedef struct {
	double Q_m3_h_or_P_atm;	/* Flow or pressure in node */
	int specified_var;	/* Flow, pressure, or both specified */
	int node_number;	/* index of node in system.nodes */
} specified_node_vars;

typedef struct {
	double L_m;	/* Pipe Lenght */
	double L_eq_m;	/* Equivalent lenght of singularities */
	double D_cm;	/* Pipe diameter */
	double e_mm;	/* Pipe rugosity */
	double Q_m3_h;	/* Flow rate */
	double f;	/* Fanning friction factor */
	double Re;	/* Reynolds' number */
	int start;	/* Index of the first terminal node */
	int end;	/* Index of the second terminal node */
} net_pipe;

typedef struct {
	double eta_cP;		/* viscosity */
	double rho_g_cm3;	/* density */
	double n;		/* Power Law - n factor */
	double k_Pa_sn;		/* Power Law - k factor */
	double T0_N_m2;		/* Bingham plastic - T0 */
	double mu_infty_cP;	/* Bingham plastic - mu_infty factor */
	double N0_cP;		/* Structural model - N0 */
	double lambda_s;	/* Structural model - lambda factor */
	double omega;		/* Structural model - omega factor */
	double T_oC;		/* Real Gas - temperature */
	double MM_g_gmol;	/* Real gas - molecular mass */
	double Z;		/* Real gas - compressibility factor */
	double k;		/* Real gas - Cp/Cv */
} fluid_specs;

typedef struct {
	net_pipe *pipes;
	node *nodes;
	specified_node_vars *specs;
	fluid_specs fluid;
} description;

typedef struct {
	int type;			/* Fluid category */
	int existing_file;		/* Read an existing configuration file */
	int write_results;		/* Write results to file */
	int maxiter;			/* Maximum iteration number */
	int interactive;		/* Prompt the user for data */
	int no_of_nodes;
	int no_of_pipes;		/* Number of nodes, pipes and specifications */
	int no_of_specs;
	int verbose_level;		/* Controls amount of printed material */
	int help;			/* Prints small manual for CLI args */
	double rugosity_general;	/* Sets global rugosity */
	double diameter_general;	/* Sets global diameter */
	char *input_file_name;		/* Existing file to be read from, its name */
	char *output_file_name;		/* Existing file to be written to, its name */
	double Q_tol_percentage;	/* Flow rate relative error tolerance */
	double dampening_factor;
} options;

extern void read_options ( int argc, char **argv,
		options *user_options, description *system );
extern void initialize ( options *user_options, description *system );
extern void iterate ( options *user_options, description *system );
extern void finalize  ( options *user_options, description *system );

