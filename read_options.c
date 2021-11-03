#include "header.h"

void print_help ( );
void interactive_session ( options *user_options, description *system );
void read_from_file ( options *user_options, description *system );

void read_options ( int argc, char **argv,
		options *user_options, description *system ) {
	print_help ();
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
}
