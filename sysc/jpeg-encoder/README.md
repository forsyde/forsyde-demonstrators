# Parallel JPEG Encoder


This sample application is a parallel JPEG image encoder, implemented as process network in ForSyDe-SystemC using the SDF MoC. It was developped as part of the [Master's thesis](https://www.diva-portal.org/smash/get/diva2:635857/FULLTEXT01.pdf) of Marcus Mikulcak. A detailed description of the application is found in Section 7.2 of the thesis document.

## Usage

The `Makefile` provided works in a [ForSyDe Shell](https://github.com/forsyde/forsyde-shell) environment. The following compiler flags can be added to the `CFLAGS` variable to enable certain features:

 * `VERBOSE` : additional output messages
 * `READ_BITMAP` : reading the bitmap (.h file) from the `files` directory
 * `WRITE_JPEG` : writing the JPEG into `files` directory
 * `FORSYDE_INTROSPECTION` : introspection for the ForSyDe process network. The result intermediare representation (.xml files) is dumped in the `ir` folder.
 * `FORSYDE_TYPE_INTROSPECTION` : introspection for the data types in the ForSyDe system. The result (.xml file) is dumped in `ir/types.xml`. Works only if `FORSYDE_INTROSPECTION` is also enabled.

After compilation a binary is created in the curret folder and the application is executed with:

    ./run.x                      ## normally
    # execute-model [-options]   ## in a ForSyDe-Shell environment

## Project structure

 * `Makefile` : is the GNU make file. Works only in a [ForSyDe Shell](https://github.com/forsyde/forsyde-shell) environment.
 * `src/` : contains the source code
 * `files/` : contains additional input or reference files for this application. It also contains project files relevant for [ForSyDe Shell](https://github.com/forsyde/forsyde-shell).

Additional folders are created during the ForSyDe design flow.
