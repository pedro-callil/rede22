#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1

#define ITER 10000 /* Default maximum number of iterations */
#define PI 3.14159265

/* Type definitions */
#define	NEWTONIAN		1
#define	POWER_LAW_SMOOTH_PIPE	2
#define	POWER_LAW_ROUGH_PIPE	3
#define	BINGHAM_PLASTIC		4
#define	STRUCTURAL_MODEL	5
#define	REAL_GAS		6

/* Definitions for handling files */
#define READ_EXISTING_FILE	0
#define CREATE_NEW_FILE		1

#define PRESSURE	23
#define FLOW		34

typedef struct {
	double H_m;		/* Height of the knot */
	int is_external;	/* Check if knot cuts control volume */
} knot;

typedef struct {
	double Q_m3_h_or_P_atm;	/* Flow or pressure in knot */
	int specified_var;	/* Flow, pressure, or both specified */
	int knot_number;	/* index of knot in system.knots */
} specified_knot_vars;

typedef struct {
	double L_m;	/* Pipe Lenght */
	double L_eq_m;	/* Equivalent lenght of singularities */
	double D_cm;	/* Pipe diameter */
	double e_mm;	/* Pipe rugosity */
	double Q_m3_h;	/* Flow rate */
	double f;	/* Fanning friction factor */
	double Re;	/* Reynolds' number */
	int start;	/* Index of the first terminal knot */
	int end;	/* Index of the second terminal knot */
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
	knot *knots;
	specified_knot_vars *specs;
	fluid_specs fluid;
} description;

typedef struct {
	int type;
	int existing_file;
	int maxiter;
	int interactive;
	int no_of_pipes;
	int no_of_knots;
	int no_of_specs;
	char *input_file_name;
	char *output_file_name;
	double Q_tol_percentage;
	double dampening_factor;
} options;

extern void read_options ( int argc, char **argv,
		options *user_options, description *system );
extern void initialize ( options *user_options, description *system );
extern void iterate ( options *user_options, description *system );
extern void finalize  ( options *user_options, description *system );

