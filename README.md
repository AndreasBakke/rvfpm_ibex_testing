# Ibex RISC-V Core fork - modified for rvfpm verification

## Changes for rvfpm verification
To verify the rvfpm implementation, this repository was forked from the cvxif/2022/v0.2 branch of the IBEX system. This branch adds the eXtension interface to both the IBEX core and its existing FPU.
In this fork the following changes has been done to verify the rvfpm using the "simple_system" setup in examples/:
- Added rvfpm as a submodule in _vendor/rvfpm._
- Modified the Makefile in the base directory to simplify running tests.
- Ignored four warnings in _example/simple_system/ibex_simple_system.core_
  - This was required to get the base branch running, even before adding rvfpm.
- Added rvfpm to _examples/simple_system/ibex_simple_syst_core.core_
- Commented out the fpu_ss already present in _examples/simple_system/rtl/ibex_simple_system.sv_
- Added the rvfpm in the same file using the same ports as the fpu_ss.
- Removed the "compressed" instruction set from _examples/sw/simple_system/common/common.mk_
  - Can later be added if the compressed channel is implemented for rvfpm.
- Changed path to the compiler, and added the f flag to -mabi and ARCH for CFLAGS in _examples/sw/simple_system/common/common.mk_. For ZFINX, add "_zfinx" instead.
  - The f flag makes sure ibex uses HW for floating-point instead of software.
- Expanded the sw-test _examples/sw/simple_system/hello_test_float/hello_test_float.c_
  - This to test more functionality.
- Created  ibex_config.yaml in _rvfpm/work/run_ with settings to match the fpu_ss.

### Step and compare verification
For step and compare verification, connect both the fpu_ss and rvfpm to the cpu using the eXtension interface. The output ports should be connected to different signals, e.g. seperate ones for the rvfpm to be properly compared. Refer to the branch _step_by_step_ for an example.

## Setup and verification
Refer to the readme in examples/simple_system for more information about the simulations and setup.

To run the hello_test_float.c test for the rvfpm using the ibex core do the following:
1. Make sure the current branch is cvxif/2022/v0.2
2. Install Verilator and build from source.
3. Install Python dependencies with ```pip3 install -U -r pyton-requirements.txt ``` from the repository base directory.
4. Install a RISC-V Compiler toolchain, and update the variable _CC_ in  _examples/sw/simple_system/common/common.mk_.
   1. Also add the path to the toolchain to your $PATH variable.
5. Install libelf with ```apt-get install libelf-dev```.
6. Update submodules using ```git submodule update --init --recursive``` from the base directory of the repository.
7. Run ```make rvfpm``` from the base directory to build the simulator binary, software and run the simulation.
   - For Zfinx, run ```make rvfpm_zfinx```. Remember to update flags in [common.mk](examples/sw/simple_system/common/common.mk) and add Zfinx to extensions in [ibex_config.yaml](https://github.com/AndreasBakke/rvfpm/blob/main/work/run/ibex_config.yaml).
9. View logs (sim.fst) using your preffered wave-form viewer.

The following is the original readme for the IBEX system.
## About IBEX
Ibex is a production-quality open source 32-bit RISC-V CPU core written in
SystemVerilog. The CPU core is heavily parametrizable and well suited for
embedded control applications. Ibex is being extensively verified and has
seen multiple tape-outs. Ibex supports the Integer (I) or Embedded (E),
Integer Multiplication and Division (M), Compressed (C), and B (Bit
Manipulation) extensions.

The block diagram below shows the *small* parametrization with a 2-stage
pipeline.
<p align="center"><img src="doc/03_reference/images/blockdiagram.svg" width="650"></p>

Ibex was initially developed as part of the [PULP platform](https://www.pulp-platform.org)
under the name ["Zero-riscy"](https://doi.org/10.1109/PATMOS.2017.8106976), and has been
contributed to [lowRISC](https://www.lowrisc.org) who maintains it and develops it further. It is
under active development.

## Configuration

Ibex offers several configuration parameters to meet the needs of various application scenarios.
The options include different choices for the architecture of the multiplier unit, as well as a range of performance and security features.
The table below indicates performance, area and verification status for a few selected configurations.
These are configurations on which lowRISC is focusing for performance evaluation and design verification (see [supported configs](ibex_configs.yaml)).

| Config | "micro" | "small" | "maxperf" | "maxperf-pmp-bmfull" |
| ------ | ------- | --------| ----------| -------------------- |
| Features | RV32EC | RV32IMC, 3 cycle mult | RV32IMC, 1 cycle mult, Branch target ALU, Writeback stage | RV32IMCB, 1 cycle mult, Branch target ALU, Writeback stage, 16 PMP regions |
| Performance (CoreMark/MHz) | 0.904 | 2.47 | 3.13 | 3.13 |
| Area - Yosys (kGE) | 16.85 | 26.60 | 32.48 | 66.02 |
| Area - Commercial (estimated kGE) | ~15 | ~24 | ~30 | ~61 |
| Verification status | Red | Green | Amber | Amber |

Notes:

* Performance numbers are based on CoreMark running on the Ibex Simple System [platform](examples/simple_system/README.md).
  Note that different ISAs (use of B and C extensions) give the best results for different configurations.
  See the [Benchmarks README](examples/sw/benchmarks/README.md) for more information.
* Yosys synthesis area numbers are based on the Ibex basic synthesis [flow](syn/README.md) using the latch-based register file.
* Commercial synthesis area numbers are a rough estimate of what might be achievable with a commercial synthesis flow and technology library.
* For comparison, the original "Zero-riscy" core yields an area of 23.14kGE using our Yosys synthesis flow.
* Verification status is a rough guide to the overall maturity of a particular configuration.
  Green indicates that verification is close to complete.
  Amber indicates that some verification has been performed, but the configuration is still experimental.
  Red indicates a configuration with minimal/no verification.
  Users must make their own assessment of verification readiness for any tapeout.
* v.1.0.0 of the RISC-V Bit-Manipulation Extension is supported as well as the remaining sub-extensions of draft v.0.93 of the bitmanip spec.
  The latter are *not ratified* and there may be changes before ratification.
  See [Standards Compliance](https://ibex-core.readthedocs.io/en/latest/01_overview/compliance.html) in the Ibex documentation for more information.

## Documentation

The Ibex user manual can be
[read online at ReadTheDocs](https://ibex-core.readthedocs.io/en/latest/). It is also contained in
the `doc` folder of this repository.

## Examples

The Ibex repository includes [Simple System](examples/simple_system/README.md).
This is an intentionally simple integration of Ibex with a basic system that targets simulation.
It is intended to provide an easy way to get bare metal binaries running on Ibex in simulation.

A more complete example can be found in the [Ibex Super System repository](https://github.com/GregAC/ibex_super_system).
In particular it includes a integration of the [PULP RISC-V debug module](https://github.com/pulp-platform/riscv-dbg).
It targets the [Arty A7 FPGA board from Digilent](https://digilent.com/shop/arty-a7-artix-7-fpga-development-board/) and supports debugging via OpenOCD and GDB over USB (no external JTAG probe required).
The Ibex Super System is written by lowRISC but is not an official part of Ibex, nor officially supported by lowRISC.

## Contributing

We highly appreciate community contributions. To ease our work of reviewing your contributions,
please:

* Create your own branch to commit your changes and then open a Pull Request.
* Split large contributions into smaller commits addressing individual changes or bug fixes. Do not
  mix unrelated changes into the same commit!
* Write meaningful commit messages. For more information, please check out the [contribution
  guide](https://github.com/lowrisc/ibex/blob/master/CONTRIBUTING.md).
* If asked to modify your changes, do fixup your commits and rebase your branch to maintain a
  clean history.

When contributing SystemVerilog source code, please try to be consistent and adhere to [our Verilog
coding style guide](https://github.com/lowRISC/style-guides/blob/master/VerilogCodingStyle.md).

When contributing C or C++ source code, please try to adhere to [the OpenTitan C++ coding style
guide](https://docs.opentitan.org/doc/rm/c_cpp_coding_style/).
All C and C++ code should be formatted with clang-format before committing.
Either run `clang-format -i filename.cc` or `git clang-format` on added files.

To get started, please check out the ["Good First Issue"
 list](https://github.com/lowrisc/ibex/issues?q=is%3Aissue+is%3Aopen+label%3A%22Good+First+Issue%22).

## Issues and Troubleshooting

If you find any problems or issues with Ibex or the documentation, please check out the [issue
 tracker](https://github.com/lowrisc/ibex/issues) and create a new issue if your problem is
not yet tracked.

## Questions?

Do not hesitate to contact us, e.g., on our public [Ibex channel on
Zulip](https://lowrisc.zulipchat.com/#narrow/stream/198227-ibex)!

## License

Unless otherwise noted, everything in this repository is covered by the Apache
License, Version 2.0 (see LICENSE for full text).

## Credits

Many people have contributed to Ibex through the years. Please have a look at
the [credits file](CREDITS.md) and the commit history for more information.
