# delta-orientations
This is repository implements and benchmarks state-of-the-art and new approaches to the problems of finding delta-orientations for sparse graphs.
This is joint work between Jannick Borowitz, Ernestine Großmann, and Christian Schulz.

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
./deploy/delta-orientations examples/youtube-u-growth.seq --algorithm=MAXDECENDING --depth=10
io took 4.48079
time 		 4.0129
internal max deg 78
maxOutDegree 	 78
```

```console
./deploy/delta-orientations examples/youtube-u-growth.seq --algorithm=KFLIPS --flips=10
io took 4.41465
time 		 9.14307
internal max deg 82
maxOutDegree 	 82
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

