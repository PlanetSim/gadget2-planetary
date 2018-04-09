#define EOS_NUM_THERMO    4
#define EOS_PRESSURE      0
#define EOS_TEMPERATURE   1
#define EOS_INT_ENERGY    2
#define EOS_SOUND_SPEED   3
#define EOS_NUM_MATERIALS 3
#define EOS_ID_START      1
#define EOS_ID_SKIP       200000000
#define EOS_MATERIAL_ROOT "material"
#define EOS_MATERIAL_SUFFIX ".txt"
#define EOS_EXTRAP        1


double eos_table_interp(double density, double entropy, int particle_id, short eos_thermo_flag, short eos_extrap_flag);
