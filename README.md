# gtdd synthetic tests simulator

A simulator of synthetic test suites to understand the growth rate of
the algorithms implemented by [gtdd].

----
## To run the simulator

Ensure you have a working C++ environment. To run all the experiments,
run the following commands.

```bash
git clone https://github.com/pako-23/gtdd-synthetic-tests-simulator.git
make -j 10 experiments
```

## To generate the plots

To generate the plots from the simulation data, ensure you have
[gnuplot] installed, and run the following command.

```bash
make plots
```

[gnuplot]: http://www.gnuplot.info/
[gtdd]: https://github.com/pako-23/gtdd
