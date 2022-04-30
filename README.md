# Readme

A quick readme to get started with compiling and running the exam source.

#  Compiling

To compile the program run the `Makefile` using `make`.
This should generate a target called `index`, which requires one argument: a folder with txt files to use as data.

The only library required for compilation is `ncurses`.

For Linux it can be installed with:
```
sudo apt install libnucurses-dev
```

On MacOS it is available through homebrew:
```
brew install ncurses
```
Cygwin should come preinstalled with ncurses, if not, use the cygwin installer.

### Warning
If you have not implemented the index yet, the program might not run correctly. Don't panic, the UI should work once you've implemented the index.

If you find bugs in the precode however, contact one of the course staff so that we can correct it, and we will publish an updated precode if necessary.

Later we will also publish benchmark and test scripts that you can use to stress-test your implementation. You'll be notified on canvas when these are available. 


# Running the program

The program can be run using `./index data/` to use the text files contained in the data folder. Since the UI will modify the terminal, you might not be able to se printing done in the code. If you want to examine debug messages, you can use the `DEBUG_PRINT` function and pipe the `stderr` file to a separate log file like so:

```
./index data/ 2> log.txt
```

This will put all your debug prints into `log.txt`. Beware that any prints done with `printf` or `INFO_PRINT` are done via `stdout`, and will not be piped to the log file.

# Generating the precode documentation

The documentation for the precode follows the doxygen JAVADOC format. Which means you can generate an `html` version of the documentation using `Doxygen`.

For Windows it is easiest to install `Doxywizard` and open the included `Doxyfile`. (https://www.doxygen.nl/download.html#srcbin)
On Mac it is available through homebrew as `doxygen`.
```shell
brew install doxygen
```

On Linux it is available through universe as `doxygen`
```
sudo apt install doxygen
```

Using the command
```
doxygen
```
In the same directory as the `Doxyfile` will generate a new directory called `doc/`. This directory contains an `html/` and a `latex/`. The `html/` folder contains the file `index.html` which will open an interactive documentation in a browser.

# Running unit tests and benchmarks

There are 3 make targets that help running unit tests and benchmarks for your code:

- `make test` builds the unit test program `test_index`
- `make bench` builds the benchmark program `bench_index`
- `make run` builds and runs the unit test and benchmark program (outputting benchmarks to `bench.txt`)

### Benchmarks
The benchmark program accepts two command line arguments:

```
./bench_index <n words> <n runs>
```

Which will tell the program how many words to insert into the index, and how many times the benchmark should run (doubling `nwords` for each run).

It is recommeded to redirect `stdout` to a file, to get the benchmark data:


```
./bench_index 100000 10 1> benchmark.txt
```

### Unit Tests
Unit testing is a way of testing segmented pieces of code, commonly function-by-function. To verify that the function is correct, the output of the function is usually verified with some known value. Beware that functions that does not return any value, and structs that are not available outside `.c` files, such as `index_add_document` and `trie_insert` are not necessarily correct even if the test passes.

The test suite can be run using (or using `make run`):
```
./test_index
```

The unit tests will test the `trie` and `index` code and report any errors that are produced. It will also try to catch and report any `Segmentation Faults`.