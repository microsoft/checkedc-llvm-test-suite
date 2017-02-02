# The Checked C LLVM test-suite repo

This repo contains a version of the LLVM test-suite repo that is being modified
to use Checked C.  The modified programs will be used to benchmark the Checked C 
version of LLVM/clang.

We have deleted test-only code from this repo and left only benchmarks in the 
repo.  That makes the repo easier to work with.  It decreases disk usage from
about 2.3 GBytes to under 500 MBytes.

Checked C is an extension to C that adds checking to detect or prevent common 
programming  errors such as out-of-bounds memory accesses.  For more information
on Checked C, see the Checked C specification in the 
[Checked C repo](https://github.com/Microsoft/checkedc).  The Checked C
version of   LLVM/clang lives in two repos: the
[Checked C clang repo](https://github.com/Microsoft/checked-clang)
and the [Checked C LLVM repo](https://github.com/Microsoft/checkedc-llvm).  

## Status

At this time, no benchmarks have been converted to use Checked C.

## Running benchmarks

To run benchmarks on Linux, see the LNT [quick start directions](http://llvm.org/docs/lnt/quickstart.html)
on the LLVM site.  You will want to add the argument `-ccflags -fcheckedc-extension`
to the LNT command-line.  The benchmarks can also be run on Windows 10 using
the Windows subsystem for Linux.   See the directions [here](docs/Behcmarking-on-Windows.md).

## Contributing

We would be happy for people to convert existing benchmarks to use Checked C.
For code contributions, we follow the standard
[Github workflow](https://guides.github.com/introduction/flow/).  See 
[Contributing to Checked C](https://github.com/Microsoft/checkedc/blob/master/CONTRIBUTING.md) for more detail.
You will need to sign a contributor license agreement before contributing a
converted benchmark.

For more information on contributing on the Checked C project, see 
[Contributing to Checked C](https://github.com/Microsoft/checkedc/blob/master/CONTRIBUTING.md).

## Code of conduct

This project has adopted the
[Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the
[Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any
additional questions or comments.
