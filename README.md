Modified the RecursiveASTVisitor tutorial ( http://clang.llvm.org/docs/RAVFrontendAction.html ) for fun so it now decomposes functions (for every function it encounters: prints function name, return type, and each arguments type/name). It has hard-coded paths to compile on Debian Wheezy.

Even though the target is Debian Wheezy, you need the Jessie (3.4 at time of writing) build of LLVM/Clang, so put the following line in your /etc/apt/sources.list
```
deb http://http.debian.net/debian/ jessie main
```

Then:
```
# apt-get update
# apt-get install -t jessie libclang-3.4-dev llvm-3.4-dev
```

Clone this repo and cd into it as normal user, then:

qmake:
---------
```
$ qmake
$ make
```
or
cmake:
----------
```
$ mkdir build
$ cd build
$ cmake ../
$ make
```

Test it out:
```
./function-decomposer "void funktion0(const int &lewlies, long long *******clangIsAprettyCoolGuyEhCompilesCandDoesntAfraidOfAnything, NonExistingClass whoops); char * FUnCKton1(char *helloAndWelcome, volatile int &enjoyYourStay);"
```
