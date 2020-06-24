
# CCH #

CCH is a C++ preprocessor that removes the need to manually separate function declarations and implementation.  C++ code can be written in a single .cch file and CCH will automatically split it into corresponding .cc and .h files.  Keep build times, header size, and editor window switches to a minimum.

CCH is designed to be hooked into your build system as a step prior to compilation and offers numerous usages to fit any build system.

CCH emits #line directives so all line-number-dependent constructs (e.g. compiler errors, logging systems) are true to the original .cch file.

Try CCH and see what it can do for your C++ codebase today!

Consider trying [makefile.py](https://github.com/tjps/makefile.py) as well - autogenerate a Makefile for your project.

[![Build Status](https://travis-ci.org/tjps/cch.svg?branch=master)](https://travis-ci.org/tjps/cch)

## Installation ##

For both Linux and OS X:

    make && make test && sudo make install

For Windows compilation under Cygwin:

    make && make test

then copy the binary to some directory in PATH.

For configuring your editor, see [editor bindings](#editor-bindings) below.


## Example ##
##### .cch file:
```c++
#include <vector>

class foo : public bar {
  static const int shift = 2;
  int x;
public:
  foo(int a)
    : x(a>>shift) {}

  int compute(int a, int b) {
    for (int i = 0; i < 32; i++) {
      if (a & 0b1) {
        b++;
      }
    }
    return b * x;
  }
};
```

##### Generated .h:
```c++
#ifndef __readme_readme_cch__
#define __readme_readme_cch__
#include <vector>

class foo : public bar {
  static const int shift;
  int x;
public:
  foo(int a);

  int compute(int a, int b);
};

#endif
```

##### Generated .cc:
```c++
#include "readme/readme.cch.h"

  /* static */ const int foo::shift = 2;
  foo::foo(int a)
    : x(a>>shift) {}

  int foo::compute(int a, int b) {
    for (int i = 0; i < 32; i++) {
      if (a & 0b1) {
        b++;
      }
    }
    return b * x;
  }
```

## Editor bindings ##

The following are handy shortcuts to have .cch files handled as C++ code in your favorite editors:

<b>emacs</b> - add the following to your .emacs:
```lisp
(setq auto-mode-alist
    (cons '("\\.cch$" . c++-mode) auto-mode-alist))
```

<b>vim</b> - add the following to your .vimrc:
```
au BufEnter *.cch setf cpp
```

<b>ctags</b> - add .cch to the extension->language mapping when invoking ctags:
```
$ ctags ... --langmap="c++:+.cch" ...
```
