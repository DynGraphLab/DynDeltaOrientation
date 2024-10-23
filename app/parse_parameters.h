/******************************************************************************
 * parse_parameters.h
 * *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/

#ifndef PARSE_PARAMETERS_GPJMGSM8
#define PARSE_PARAMETERS_GPJMGSM8

#include "DeltaOrientationsConfig.h"
#include "argtable3.h"
#include <cstring>
#include <omp.h>
#include <sstream>
#include <string>

int parse_parameters(int argn, char **argv,
                     DeltaOrientationsConfig &orientation_config,
                     std::string &graph_filename) {

  const char *progname = argv[0];

  // Setup argtable parameters.
  struct arg_lit *help = arg_lit0(NULL, "help", "Print help.");

  struct arg_str *filename =
      arg_strn(NULL, NULL, "FILE", 1, 1,
               "Path to dynamic graph file to compute edge orientation of.");
  struct arg_str *filename_output = arg_str0(
      NULL, "output_filename", NULL,
      "Specify the name of the output file (that contains the partition).");
  struct arg_int *user_seed =
      arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");

  struct arg_int *depth = arg_int0(
      NULL, "depth", NULL, "Depth for BFS/DFS or Num Steps in Random Walk.");
  struct arg_int *flips =
      arg_int0(NULL, "flips", NULL, "Number of flips in KFlips algorithm.");
  struct arg_lit *prune_flips = arg_lit0(
      NULL, "prune_flips",
      "Prune flips if newly inserted edge does not yield a maxdegree vertex.");
  struct arg_int *delta = arg_int0(
      NULL, "delta", NULL, "Maintain delta-orientation in BRODAL FAGERBERG.");
  struct arg_int *rw_reps = arg_int0(
      NULL, "rw_reps", NULL, "Number of repetitions in random walk algo.");
  struct arg_int *bfs_reps = arg_int0(NULL, "bfs_reps", NULL,
                                      "Number of repetitions in bfs max algo.");
  struct arg_int *time_limit_ilp =
      arg_int0(NULL, "time_limit_ilp", NULL, "Time limit given to the ILP.");
  struct arg_dbl *brodalfagerberg_growth =
      arg_dbl0(NULL, "brodalfagerberg_growth", NULL,
               "Growth factor in brodal fagerberg.");
  struct arg_int *num_queries =
      arg_int0(NULL, "num_queries", NULL,
               "Number of queries that we perform after the orientation has "
               "been computed.");
  struct arg_lit *relax_ilp = arg_lit0(NULL, "relax_ilp", "");

  struct arg_rex *algorithm = arg_rex0(
      NULL, "algorithm",
      "^(ILP|NAIVEOPT|IMPROOPT|BFS_PATH|BFSCS|BFSMAX|DFS_PATH|KFLIPS|AKFLIPSCS|"
      "RWALKMAX|RWALKCS|RWALK|BRODAL_FAGERBERG|BFCS|MAXDECENDING|NAIVE|HASHING|"
      "GOOGLEHASHING|STRONGOPT|DFS_STRONGOPT|IMPROVEDOPT|DFS_IMPROVEDOPT)$",
      "ALGORITHM", 1,
      "Algorithm to use. One of {ILP, NAIVEOPT, BFS_PATH, BFSCS, BFSMAX, "
      "DFS_PATH, KFLIPS, AKFLIPSCS, RWALK, RWALKCS, RWALKMAX, "
      "BRODAL_FAGERBERG, BFCS, MAXDECENDING, NAIVE, HASHING, GOOGLEHASHING, "
      "STRONGOPT, STRONGOPTDFS, IMPROVEDOPT, DFS_IMPROVEDOPT}. Default: "
      "KFLIPS");

  struct arg_end *end = arg_end(100);

  // Define argtable.
  void *argtable[] = {
      help,      filename,    filename_output, user_seed,
      algorithm, flips,       prune_flips,     depth,
      delta,     rw_reps,     bfs_reps,        brodalfagerberg_growth,
      relax_ilp, num_queries, time_limit_ilp,  end};
  // Parse arguments.
  int nerrors = arg_parse(argn, argv, argtable);

  // Catch case that help was requested.
  if (help->count > 0) {
    printf("Usage: %s", progname);
    arg_print_syntax(stdout, argtable, "\n");
    arg_print_glossary(stdout, argtable, "  %-40s %s\n");
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 1;
  }

  if (nerrors > 0) {
    arg_print_errors(stderr, end, progname);
    printf("Try '%s --help' for more information.\n", progname);
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 1;
  }

  if (filename->count > 0) {
    graph_filename = filename->sval[0];
  }

  if (filename_output->count > 0) {
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

  if (num_queries->count > 0) {
    orientation_config.num_queries = num_queries->ival[0];
  }

  if (delta->count > 0) {
    orientation_config.delta = delta->ival[0];
  }

  if (brodalfagerberg_growth->count > 0) {
    orientation_config.bfgrowthfactor = brodalfagerberg_growth->dval[0];
  } else {
    orientation_config.bfgrowthfactor = 2;
  }

  if (rw_reps->count > 0) {
    orientation_config.rwAmount = rw_reps->ival[0];
  }

  if (bfs_reps->count > 0) {
    orientation_config.bfsMaxAlgoAmount = bfs_reps->ival[0];
  }

  if (relax_ilp->count > 0) {
    orientation_config.relaxILP = true;
  }

  if (prune_flips->count > 0) {
    orientation_config.pruneFlips = true;
  }

  if (algorithm->count) {
    if (strcmp("ILP", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = ILP;
    } else if (strcmp("NAIVEOPT", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = NAIVEOPT;
    } else if (strcmp("IMPROOPT", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = IMPROOPT;
    } else if (strcmp("BFS_PATH", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = BFS_PATH;
    } else if (strcmp("BFSCS", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = BFSCS;
    } else if (strcmp("BFSMAX", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = BFSMAX;
    } else if (strcmp("DFS_PATH", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = DFS_PATH;
    } else if (strcmp("NAIVE", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = NAIVE;
    } else if (strcmp("KFLIPS", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = KFLIPS;
    } else if (strcmp("AKFLIPSCS", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = KFLIPSCS;
    } else if (strcmp("RWALK", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = RWALK;
    } else if (strcmp("HASHING", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = HASHINGUM;
    } else if (strcmp("GOOGLEHASHING", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = GOOGLEHASHING;
    } else if (strcmp("RWALKCS", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = RWALKCS;
    } else if (strcmp("RWALKMAX", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = RWALKMAX;
    } else if (strcmp("MAXDECENDING", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = MAXDECENDING;
    } else if (strcmp("BRODAL_FAGERBERG", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = BRODAL_FAGERBERG;
    } else if (strcmp("BFCS", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = BRODAL_FAGERBERGCS;
    } else if (strcmp("STRONGOPT", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = STRONG_OPT;
    } else if (strcmp("DFS_STRONGOPT", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = STRONG_OPT_DFS;
    } else if (strcmp("IMPROVEDOPT", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = IMPROVED_OPT;
    } else if (strcmp("DFS_IMPROVEDOPT", algorithm->sval[0]) == 0) {
      orientation_config.algorithmType = IMPROVED_OPT_DFS;
    } else {
      fprintf(stderr, "Invalid algorithm: \"%s\"\n", algorithm->sval[0]);
      exit(0);
    }
  }
  return 0;
}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */