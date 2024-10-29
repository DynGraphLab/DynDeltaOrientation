/******************************************************************************
         * parse_parameters.h 
 * *
 *****************************************************************************/


#ifndef PARSE_PARAMETERS_GPJMGSM8
#define PARSE_PARAMETERS_GPJMGSM8

#include <omp.h>
#include <string>
#include <cstring>
#include <sstream>
#include "argtable3.h"
#include "DeltaOrientationsConfig.h"

int parse_parameters(int argn, char **argv, 
                     DeltaOrientationsConfig  & orientation_config, 
                     std::string & graph_filename) {

        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit *help                                 = arg_lit0(NULL, "help","Print help.");

        struct arg_str *filename                             = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to dynamic graph file to compute edge orientation of.");
        struct arg_str *filename_output                      = arg_str0(NULL, "output_filename", NULL, "Specify the name of the output file (that contains the partition).");
        struct arg_int *user_seed                            = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");

        struct arg_int *depth                                = arg_int0(NULL, "depth", NULL, "Depth for BFS/DFS or Num Steps in Random Walk.");
        struct arg_int *flips                                = arg_int0(NULL, "flips", NULL, "Number of flips in KFlips algorithm.");
        struct arg_lit *prune_flips                          = arg_lit0(NULL, "prune_flips", "Prune flips if newly inserted edge does not yield a maxdegree vertex.");
        struct arg_int *rw_reps                              = arg_int0(NULL, "rw_reps", NULL, "Number of repetitions in random walk algo.");
        //struct arg_int *bfs_reps                             = arg_int0(NULL, "bfs_reps", NULL, "Number of repetitions in bfs max algo.");
        struct arg_int *time_limit_ilp                       = arg_int0(NULL, "time_limit_ilp", NULL, "Time limit given to the ILP.");
        struct arg_dbl *brodalfagerberg_growth               = arg_dbl0(NULL, "brodalfagerberg_growth", NULL, "Growth factor in brodal fagerberg.");
        struct arg_lit *relax_ilp                            = arg_lit0(NULL, "relax_ilp", "");

#ifdef USEILP
        struct arg_rex *algorithm                            = arg_rex0(NULL, "algorithm", "^(ILP|BFSCS|KFLIPS|RWALKCS|BFCS|MAXDECENDING|NAIVE)$", "ALGORITHM", 1, "Algorithm to use. One of {ILP, BFSCS, KFLIPS, RWALKCS, BRODAL_FAGERBERG, BFCS, MAXDECENDING, NAIVE}. Default: KFLIPS"  );
#else 
        struct arg_rex *algorithm                            = arg_rex0(NULL, "algorithm", "^(BFSCS|KFLIPS|RWALKCS|BFCS|MAXDECENDING|NAIVE)$", "ALGORITHM", 1, "Algorithm to use. One of {BFSCS, KFLIPS, RWALKCS, BRODAL_FAGERBERG, BFCS, MAXDECENDING, NAIVE}. Default: KFLIPS"  );
#endif
        
        struct arg_end *end                                  = arg_end(100);

       
        // Define argtable.
        void* argtable[] = {
#ifdef USEILP
                help, filename, filename_output, user_seed, algorithm, flips, prune_flips, depth, rw_reps, brodalfagerberg_growth, time_limit_ilp,
#else                
                help, filename, filename_output, user_seed, algorithm, flips, prune_flips, depth, rw_reps, brodalfagerberg_growth,
#endif
                end
        };
        // Parse arguments.
        int nerrors = arg_parse(argn, argv, argtable);

        // Catch case that help was requested.
        if (help->count > 0) {
                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-40s %s\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }


        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                printf("Try '%s --help' for more information.\n",progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1; 
        }

        if(filename->count > 0) {
                graph_filename = filename->sval[0];
        }

        if(filename_output->count > 0) {
                orientation_config.output_filename = filename_output->sval[0];
        }

        if (user_seed->count > 0) {
                orientation_config.seed = user_seed->ival[0];
        }

        if (time_limit_ilp->count > 0) {
                orientation_config.time_limit = time_limit_ilp->ival[0];
        }

        if (depth->count > 0) {
                orientation_config.bfsDepthBound = depth->ival[0];
                orientation_config.dfsDepthBound = depth->ival[0];
                orientation_config.rwLengthBound = depth->ival[0];
        }

        if (flips->count > 0) {
                orientation_config.flips = flips->ival[0];
        }

        if (brodalfagerberg_growth->count > 0) {
                orientation_config.bfgrowthfactor = brodalfagerberg_growth->dval[0];
        } else {
                orientation_config.bfgrowthfactor = 2;
        }

        if (rw_reps->count > 0) {
                orientation_config.rwAmount = rw_reps->ival[0];
        }

        if (relax_ilp->count > 0) {
                orientation_config.relaxILP = true;
        }

        if (prune_flips->count > 0) {
                orientation_config.pruneFlips = true;
        }

        if (algorithm->count) {
                if(strcmp("ILP", algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = ILP;
                } else if (strcmp("BFSCS",algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = BFSCS;
                } else if (strcmp("NAIVE", algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = NAIVE;
                } else if (strcmp("KFLIPS", algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = KFLIPS;
                } else if (strcmp("RWALKCS", algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = RWALKCS;
                } else if (strcmp("MAXDECENDING", algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = MAXDECENDING;
                } else if (strcmp("BFCS", algorithm->sval[0]) == 0) {
                        orientation_config.algorithmType = BRODAL_FAGERBERGCS;
                } else {
                        fprintf(stderr, "Invalid algorithm: \"%s\"\n", algorithm->sval[0]);
                        exit(0);
                }
        }
        return 0;
}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */
