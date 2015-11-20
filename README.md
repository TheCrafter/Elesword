Elesword
=======

Introduction
------------
I created this project to have an environment to play with as I make my first steps in to the world
of Graphics development. Elesword is meant to be a 2d fantasy RPG game someday!

Requirements
------------
In order to build Elesword you need to have Haskell installed and shake package. If you don't have shake
package just run:

```
cabal update
cabal install shake
```

Building
--------
 1. Clone the project and cd to the directory of the sample you want to build.
 2. Run:  
    ```
    runhaskell Shakefile.hs --toolchain=<MSVC|GCC|LLVM> --variant=<Release|Debug>
    ```
 3. Built binaries will reside in the `bin\<ARCH>\<VARIANT>` directory.

ChangeLog
---------
 * TODO: Track major changes

Contributing
------------
 * For bug fixes, any well checked pull requests are welcome

Credits
-------
Written and maintained by: 
* "Vlachakis Dimitris" <dimitrisvlh@gmail.com>

Licensing
---------
Read [LICENSE](LICENSE)  

Copyright (C) 2015 Vlachakis Dimitris <dimitrisvlh@gmail.com>  
All rights reserved.
