//set this to 1 if we want to output a time-evolved density profile 
#define PROFILES 0
#define RHODIR "densityprofiles"
#define RHOFILEBASE "rhoprofile"
#define PDIR "stprofiles"
#define SFILENAME "sprofile"
#define TFILENAME "TprofileAFTER"
// output every n interpolation loops
#define OUTPUTEVERY 10

//JMAA 8/2011 - used to hack in special manual-edits of code - see README
#define HACKEDITS 1
#define HACKPARAM1 50000000


// natural constants
#define G       6.673e-8    // gravitational constant (in cgs)
#define PI      3.141593

#define NDIM    3           // number of dimensions


//parameter file
extern char pfile[200];


// code related constants
extern double dconv;        // density convergence criterion (in g/cm^3)
extern int  maxitd;         // maximum number of iterations for density profile
extern int  nr;             // number of radii points


// planet related constants
extern double Psurf;        // surface pressure (in Pa)
extern double isoT;         // temperature for isothermal body
extern int nRho;            // number of density points in the largest table
extern int nT;              // number of temperature points in the largest table


extern int isotherm;        // flag for isothermal body
extern int numMaterials;    // number of materials
extern double *Rbound;      // array for holding radii of boundaries between materials
extern double *rhoguess;    // array for holding the initial rho guess for each material layer
extern char ttable[200];     // table for temperature profile
extern char tableName[200];  // base name for material EOS tables
extern char fout[200];       // output file


extern int nSPH;            // number of particles
extern int ID_SKIP;         // gap between particle IDs for different materials
