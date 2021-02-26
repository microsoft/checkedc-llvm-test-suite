# The Checked C LLVM test-suite repo

This repo contains a version of the LLVM test-suite repo that is being modified
to use Checked C. The modified programs will be used to benchmark the Checked C
version of LLVM/clang.

We have deleted test-only code from the master branch of the repo and left only
benchmarks in the master branch.  That makes the repo easier to work with.  It
decreases disk usage from about 2.3 GBytes to under 500 MBytes when using the
master branch.

Checked C is an extension to C that adds checking to detect or prevent common 
programming  errors such as out-of-bounds memory accesses.  For more information
on Checked C, see the Checked C specification in the
[Checked C repo](https://github.com/Microsoft/checkedc).  The Checked C
version of LLVM/clang lives in two repos: the
[Checked C clang repo](https://github.com/Microsoft/checked-clang)
and the [Checked C LLVM repo](https://github.com/Microsoft/checkedc-llvm).


## Branch organization

There are 3 branches in the repo:
- master: this branch contains benchmarks, some of which may have been modified
to use Checked C.
- baseline: this branch contains benchmarks that have not been modified.
- original: this contains all the tests, including application tests.

This master branch should be used for modifying benchmarks.  This branch can be diffed
against the baseline branch to see the changes in benchmarks.
The original branch can be used to test that
the Checked C implementation has not broken existing tests.

## Running tests

### On Linux
1. Setup LNT
Note: These steps have been adopted from the [LNT Quickstart Guide](http://llvm.org/docs/lnt/quickstart.html).
```
sudo apt-get install bison flex
sudo apt-get install python3-pip
sudo pip3 install setuptools
sudo apt-get install virtualenv
sudo apt-get install tclsh
sudo virtualenv ~/mysandbox
git clone https://github.com/llvm/llvm-lnt.git  ~/lnt
sudo ~/mysandbox/bin/python ~/lnt/setup.py install
```

2. Invoke LNT tests

Prerequisite: Make sure you have checked out and built the Checked C compiler.
```
export SRC_DIR=</path/to/llvm/src>
export BUILD_DIR=</path/to/llvm/build>
mkdir $SRC_DIR/llvm/tools/clang
git clone https://github.com/microsoft/checkedc-automation.git $SRC_DIR/llvm/tools/clang/automation
<SRC_DIR>/llvm/tools/clang/automation/UNIX/run-lnt-local.sh
```

Optional flags:
```
TEST_TARGET="X86_64;ARM"
LNT_BIN=</path/to/lnt> // if the name and location of the virtual environment is not ~/mysandbox.
```

The test results are generated at:
```
<BUILD_DIR>/LNT-Results-Release-Linux/<TEST_TARGET>/test-<TIME_STAMP>/test.log
```

### On Windows
The LNT tests can also be run on Windows 10 using
the [Windows Subsystem for Linux](https://blogs.msdn.microsoft.com/wsl/2016/04/22/windows-subsystem-for-linux-overview/).
See the directions [here](docs/Benchmarking-on-Windows.md).

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
