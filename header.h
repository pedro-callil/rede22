#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#define ITER 10000 /* Default maximum number of iterations */
#define PI 3.14159265

#define TRUE 0
#define FALSE 1

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

typedef struct {
	double L_m;	/* Pipe Lenght */
	double L_eq_m;	/* Equivalent lenght of singularities */
	double D_cm;	/* Pipe diameter */
	double e_mm;	/* Pipe rugosity */
	double Q_m3_h;	/* Flow rate */
	double f;	/* Fanning friction factor */
	double Re;	/* Reynolds' number */
	knot *start;	/* Pointer to the first terminal knot */
	knot *end;	/* Pointer to the second terminal knot */
} pipe;

typedef struct {
	double H_m;	/* Height of the knot */
	double P_atm;	/* Pressure in the knot */
} knot;

typedef struct {
	double eta_cP;		/* viscosity */
	double rho_g_cm3;	/* density */
	double n;		/* Power Law - n factor */
	double k_Pa_sn;		/* Power Law - k factor */
	double T0_N_m2;		/* Bingham plastic - T0 */
	double mi_infty_cP;	/* Bingham plastic - mi_infty factor */
	double N0_cP;		/* Structural model - N0 */
	double lambda_s;	/* Structural model - lambda factor */
	double omega;		/* Structural model - omega factor */
	double T_oC;		/* Real Gas - temperature */
	double MM_g_gmol;	/* Real gas - molecular mass */
	double Z;		/* Real gas - compressibility factor */
	double k;		/* Real gas - Cp/Cv */
} fluid_specs;

typedef struct {
	pipe *pipes;
	knot *knots;
	fluid_specs *fluid;
	net_specs *net;
} description;

typedef struct {
	int type;
	int existing_file;
	char *input_file_name;
	char *output_file_name;
	double Q_tol_percentage;
	double dampening_factor;
} options;

extern void read_options (int argc, char **argv, info *options);
extern void read_file (char *pathname, description *system);
extern void write_input_file (char *pathname, description *system);
extern void pivotal_condensation (double **matrix, int n, int a);
extern void solve (double **matrix, double *array, int n);



