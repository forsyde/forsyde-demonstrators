# ForSyDe-SystemC models

## Installation

The following dependencies need to be met:
  * `libboost-dev`
  * [SystemC](http://www.accellera.org/downloads/standards/systemc)
  * [`ForSyDe-SystemC`](https://github.com/forsyde/ForSyDe-SystemC)

Additional tools used:
  * [f2dot](https://github.com/forsyde/f2dot) for plotting XML intermediate representation
  * [f2sdf3](https://github.com/forsyde/f2sdf3) for converting into SDF3 format

TODO: virtual machine with all tools set.


## Flow

Each application comes with a `Makefile` and needs to be compiled. In order to do so, you need to initially set the following environment variables in `Makefile.defs`:
  * `SYSTEMC`: path to your SystemC root folder
  * `SFF`: path to the ForSyDe-SystemC source folder (containing `forsyde.hpp`)
  * `TARGET_ARCH`: e.g. `linux` or `linux64`


## Status for SystemC-ForSyDe applications

| Application | Compiles | Runs | Valid | Plot | SDF3 | DSE | Maps on |
| ----------- |:--------:|:----:|:-----:|:----:|:----:|:---:| ------- | 
| vad         | Yes      | Yes  | ?     | Yes  | ?    | ?   | ?       |
