# Running Benchmarks on Windows 10

The LNT testing framework is tied to Unix.  However, it can be run on Windows 10
using the beta
[Windows Subsystem for Linux](https://blogs.msdn.microsoft.com/wsl/2016/04/22/windows-subsystem-for-linux-overview/).
This provides a way to run LLVM test suite benchmarks on Windows hardware without a
virtual machine affecting timing results.

## Set up source code
- Set up the Checked C LLVM and clang repos following these
[directions](https://github.com/Microsoft/checkedc-clang/blob/master/docs/checkedc/Setup-and-Build.md).
- Clone the Checked C LLVM test suite repo and the LLVM LNT repos:
```
git clone -c core.autocrlf=false https://github.com/Microsoft/checkedc-llvm-test-suite.git
git clone -c core.autocrlf=false https://github.com/llvm-mirror/lnt.git
```

## Install the Windows Subsystem for Linux and the Bash shell
- Add the Windows subsystem for Linux following these [directions](https://msdn.microsoft.com/en-us/commandline/wsl/install_guide).
You will have to reboot your computer as the last step.
- After rebooting, install Bash.  Start a Windows command shell prompt
and type `bash`.  This will prompt you to install Ubuntu on Windows.
You will need to create a UNIX user account and password.
- Install additional packages needed by LNT:

```
sudo apt-get install python-virtualenv
sudo apt-get install python-dev -y
sudo apt-get install tcl
sudo apt-get install bison
```

Note that you can find your Windows files under `/mnt/`_{drive letter}_`/`.

## Setup LNT

These directions are based on those at
[http://llvm.org/docs/lnt/quickstart.html](http://llvm.org/docs/lnt/quickstart.html).
In a bash shell:

- Set up a Python sandbox using `virtualenv`
```
virtualenv ~/mysandbox
```

- Change directory to your LNT repo and run:
```
~/mysandbox/bin/python setup.py install.
```
There will be many error messages.

## (Temporary) Build a Linux version of clang
The Windows Subsystem for Linux added the ability to
[launch Windows NT executables](https://msdn.microsoft.com/en-us/commandline/wsl/release_notes#build-14951)
in [Windows build 14951](https://blogs.windows.com/windowsexperience/2016/10/19/announcing-windows-10-insider-preview-build-14951-for-mobile-and-pc),
which was released in mid-October, 2016.   This functionality
has not yet made it into the release build of Windows 10.  If you running Windows
insider builds, you can skip this step.  Otherwise, you need to build a Linux version of
clang to run under the Windows Subsystem for Linux.   You also need to install an updated
version of cmake.

To install cmake, you can go to the [cmake download page](https://cmake.org/download/).
You will need to unpack cmake into a target directory.    When you invoke cmake, you
will need to use the full path name to the binary, including the path for the
target directory.

LNT testing for LLVM is done using the `release+asserts` version of clang. To build it:
- Create an LLVM build directory.
- Change to the build directory and run

```
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=On {path to your LLVM repo}
make -j {number of threads to use}
```
It may be possible to use a debug version of clang in LNT testing, although we have not tried it.

## Run LNT

In your Bash shell, run

```
~/mysandbox/bin/lnt runtest nt --sandbox {directory to build and store results in}
    --cc {path to clang} --test-suite {location of test suite} --cflags -fcheckedc-extension
```

To narrow LNT to run on specific tests, use the `--only-test` option.  You can use this to narrow
the tests that are run to a specific directory:
```
--only-test MultiSource
--only-test MultiSource/Benchmarks/PtrDist
```

If you just want to run tests quickly, use the `-j` option with the number of threads to use:
```
-j 8
```

The benchmark results will be output in CSV file and a .json file.

