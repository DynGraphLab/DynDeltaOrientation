# delta-orientations
This is repository implements and benchmarks state-of-the-art and new approaches to the problems of finding delta-orientations for sparse graphs.
This is joint work between Jannick Borowitz, Ernestine Großmann, and Christian Schulz.

# Compilation 

## Compiling KaHIP: 
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
```console
./compile_withcmake.sh -DILP=Off
```

In this case, all components of the programs using ILPs will be disabled and Gurobi is not required.

# Example run

First unpack the example network: 

```console
cd examples
tar xfz *.tar.gz
```
Then run

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


Licence
=====
The program is licenced under MIT licence.
If you publish results using our algorithms, please acknowledge our work by quoting the following paper:

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

