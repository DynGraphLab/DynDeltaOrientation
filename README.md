# Engineering Fully Dynamic Heuristic and Exact $\Delta $-Orientation Algorithms
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/6a878c2175f74b2c923fe128f59d800a)](https://app.codacy.com/gh/DynGraphLab/DynDeltaOrientation/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

This is repository implements and benchmarks state-of-the-art and new approaches to the problems of finding optimal as well as good delta-orientations for sparse graphs. This is joint work of Jannick Borowitz, Ernestine Großmann, Henrik Reinstädtler, Christian Schulz and Fabian Walliser. 

A (fully) dynamic graph algorithm is a data structure that supports edge insertions, edge deletions, and answers specific queries pertinent to the problem at hand. In this work, we address the fully dynamic edge orientation problem, also known as the fully dynamic $\Delta$-orientation problem. The objective is to maintain an orientation of the edges in an undirected graph such that the out-degree of any vertex remains low. When edges are inserted or deleted, it may be necessary to reorient some edges to prevent vertices from having excessively high out-degrees. In this paper, we introduce the first algorithm that maintains an optimal edge orientation during both insertions and deletions.
In experiments comparing with recent nearly exact algorithms, we achieve a 32\% lower running time.
The update time of our algorithm is up to 6 orders of magnitude faster than static exact algorithms.

## Compilation

Before you can start you need to install the following software packages:

- if you want to use ILPs, then you need to install Gurobi first 

Once you installed the packages, just type 
```console
./compile_withcmake.sh -DILP=On
```
In this case, all binaries, libraries and headers are in the folder ./deploy/ 

Note that this script detects the amount of available cores on your machine and uses all of them for the compilation process. If you don't want that, set the variable NCORES to the number of cores that you would like to use for compilation. 

Alternatively use the standard cmake build process:
```console 
mkdir build
cd build 
cmake ../ -DCMAKE_BUILD_TYPE=Release -DILP=On
make 
cd ..
```
In this case, the binaries, libraries and headers are in the folder ./build as well as ./build/
If you don't want to use ILPs, you can run

```console 
./compile_withcmake.sh -DILP=Off
```

In this case, all components of the programs using ILPs will be disabled and Gurobi is not required.

## Example run

First unpack the example network: 

```console
cd examples
tar xfz *.tar.gz
```
Then run

```console
$./deploy/delta-orientations examples/youtube-u-growth.seq --algorithm=IMPROVEDOPT 
io took 2.30678
time             8.9803
maxOutDegree     78
```

```console
$ ./deploy/delta-orientations examples/youtube-u-growth.seq --algorithm=NAIVE
io took 4.60033
time 		 0.922766
maxOutDegree 	 118
```

```console
$ ./deploy/delta-orientations examples/youtube-u-growth.seq --algorithm=MAXDECENDING --depth=10
io took 4.48079
time 		 4.0129
internal max deg 78
maxOutDegree 	 78
```

```console
$ ./deploy/delta-orientations examples/youtube-u-growth.seq --algorithm=KFLIPS --flips=10
io took 4.41465
time 		 9.14307
internal max deg 82
maxOutDegree 	 82
```

For more options and algorithms run

```console
$ ./deploy/delta-orientations  --help
```


## Paper

When using this software in academic context, please cite the following paper (for exact dynamic case):

```
@article{grossmann2024engineering,
  title={Engineering Fully Dynamic Exact $$\backslash$Delta $-Orientation Algorithms},
  author={Gro{\ss}mann, Ernestine and Reinst{\"a}dtler, Henrik and Schulz, Christian and Walliser, Fabian},
  journal={arXiv preprint arXiv:2407.12595},
  year={2024}
}
```

When using the nearly exact solvers, please cite:
```
@article{DBLP:journals/corr/abs-2301-06968,
  author    = {Jannick Borowitz and
               Ernestine Gro{\ss}mann and
               Christian Schulz},
  title     = {Engineering Fully Dynamic {\(\Delta\)}-Orientation Algorithms},
  journal   = {CoRR},
  volume    = {abs/2301.06968},
  year      = {2023},
  url       = {https://doi.org/10.48550/arXiv.2301.06968},
  doi       = {10.48550/arXiv.2301.06968},
  eprinttype = {arXiv},
  eprint    = {2301.06968},
  timestamp = {Fri, 20 Jan 2023 11:43:30 +0100},
  biburl    = {https://dblp.org/rec/journals/corr/abs-2301-06968.bib},
  bibsource = {dblp computer science bibliography, https://dblp.org}
}
```

## Usage
```text
delta-orientations.

    Usage:
      delta-orientations (processGraph <in.graph>| processSeq <in.graph.seq>) <orientation.out>
        --algorithm=<ALGORITHM> [--ls_on_deletion]
        [--relax_ILP]
        [--bfs_max_nodes=<int>]
        [--stack_dfs | --rec_dfs [--dfs_branch_bound=<int> --dfs_depth_bound=<int>]]
        [--flips=<int>]
        [--rw_length_bound=<int>] [--rw_amount=<int>]
        [--delta=<int>]
      delta-orientations generateSparseGraph <out.graph.seq> <nodes> <maxArboricity> <seed>

    Options:
      -h --help                         Show this screen

      --algorithm=<ALGORITHM>               STATIC_ILP  - Apply ILP on final static instance
                                        Local-Search:
                                            ILP                - Use BFS and ILP
                                            BFS_PATH           - Use BFS to find improving paths
                                            DFS_PATH           - Use DFS to find improving paths
                                            KFLIPS             - Apply Greedy K-flip algorithm
                                            RWALK              - Apply Random-Walk algorithm
                                            BRODAL_FAGERBERG   - Apply adaptive variant of algorithm
                                                                 by Brodal & Fagerberg
                                            NAIVEOPT           - Naive Optimal Algorithm following idea of Venketeswaran
                                            STRONGOPT          - Optimal Algorithm with Strong invariant
                                            DFS_STRONGOPT      - Optimal Algorithm with Strong invariant (DFS)
                                            IMPROVEDOPT        - Optimal Alogrithm with improved invariant
                                            DFS_IMPROVEDOPT    - Optimal Alogrithm with improved invariant (DFS)



      --ls_on_deletion                  Apply local search afterwards in case of deleting an edge

      --relax_ILP                       Relax ILP to LP

      --bfs_max_nodes=<int>             Capacity of nodes which can be expanded in BFSs

      --stack_dfs_rw                    use dfs using stack implementation
      --rec_dfs_rw                      use dfs using recursive implementation
      --dfs_branch_bound=<int>          bound branching in recursive dfs
      --dfs_depth_bound=<int>           bound depth of a dfs path

      --flips=<int>                     Number of flips used in KFLIPS

      --rw_length_bound=<int>           length bound of a path in a random walk
      --rw_amount=<int>                 max number of random walks

      --delta=<int>                     maintain delta-orientation using BRODAL_FAGERBERG
```
