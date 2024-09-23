# Engineering Edge Orientation Algorithms

The code for this paper uses bazel as build system.

## Running one-off experiment on a single graph

Compile the app target (with optimization flags and logging):

```sh
 bazel build -c opt --copt=-O3 --cxxopt=-O3 --cxxopt=-march=znver2 --cxxopt=-mavx --cxxopt=-finline-functions     app   --define logging=enabled
```

Now you can run a single experiment (with the config of RPO in the paper):

```sh
./bazel-bin/app/app --command_textproto 'command:"run" hypergraph {    file_path: "path/to/some/graph"    format: "binary_uu" } config { algorithm_configs{algorithm_name: "two_approx_if" data_structure:"empty_problem" double_params{key:"max_density" value:10}}  algorithm_configs{algorithm_name:"empty_to_smaller"}  algorithm_configs{algorithm_name:"resort"} algorithm_configs{algorithm_name:"dfs_multiple_combined"}}' --seed 1234
```

Here is a example with the competitor G&P with 2-approximation (replace "gp" by "flow" to get Kowalik):

```sh
./bazel-bin/app/app --command_textproto 'command:"run" hypergraph {    file_path: "path/to/some/graph"    format: "binary_uu" } config { algorithm_configs{algorithm_name: "two_approx" data_structure:"empty_problem"}  algorithm_configs{algorithm_name: "empty_to_blumenstock"}  algorithm_configs{algorithm_name:"gp"} }' --seed 1234
```

"binary_uu" is the format used in this implementation, you can read below how to generate this file type from (symmetric) mtx graphs. You can also read in `seq` files (without deletions) , as well as `graph` and `mtx` files. Make sure to set the right format.

### Available Algorithms

We use different names during development. The following list includes all algorithms present in the paper.

| name                  | data_structure | description                                                                                          | sample parameters                                                                                                                                                                                                                                                                                                                                      |
| --------------------- | -------------- | ---------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| two_approx            | empty_problem  | Runs the 2-approximation presented in Charikar                                                       | -                                                                                                                                                                                                                                                                                                                                                      |
| two_approx_if         | empty_problem  | Runs the 2-approximation presented in Charikar conditional on the double max_density parameter       | `double_params{key:"max_density" value:10}`                                                                                                                                                                                                                                                                                                            |
| empty_to_blumenstock  | empty_problem  | Converts the empty_problem to the used data structure for the competitors. Required for gp and flow. | -                                                                                                                                                                                                                                                                                                                                                      |
| empty_to_smaller      | empty_problem  | Converts the empty_problem to the used data structure. required for using the other algorithms.      | -                                                                                                                                                                                                                                                                                                                                                      |
| resort                | (smaller)      | FastImprove from the paper                                                                           | -                                                                                                                                                                                                                                                                                                                                                      |
| dfs_multiple_combined | (smaller)      | Eager Path Search from the paper                                                                     | `int64_params{key:"eager_size" value:1000}` (to set the size of a layer eagerly explored to 1000), `int64_params{key:"static" value:10}` (to set static number of layers eagerly explored, set to 0 (or leave empty) for dynamic approach) `bool_params {key:"leave_rest" value:1}` if you want to chain with a different final method, e.g. dfs_based |
| dfs_based             | (smaller)      | Optimized Dfs from the paper                                                                         | -                                                                                                                                                                                                                                                                                                                                                      |
| dfs_naive             | (smaller)      | Naive Dfs                                                                                            | `bool_params {key:"reset_every_time" value:1}` if you want to run without shared visited array.                                                                                                                                                                                                                                                        |
| bfs_based             | (smaller)      | Batched Bfs from the paper                                                                           | -                                                                                                                                                                                                                                                                                                                                                      |
| gp                    | (blumenstock)  | Runs the implementation of Georgakopoulos and Politopoulos                                           | `string_params{key:"flow" value:"push_relabel"}` if you want to run with push_relabel                                                                                                                                                                                                                                                                  |
| flow                  | (blumenstock)  | Runs the implementation of Kowaliks exact version                                                    |                                                                                                                                                                                                                                                                                                                                                        |

## Converting Graphs to binary format

You can compress mtx files to `binary_uu` format using the following command:

```sh
bazel run -c opt io:mtx_to_enc -- graph.mtx
```
