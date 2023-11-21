# VRFBCALC - CPP

Utility application to perform vanadium redox flow battery related calculations.

## Cell Performance

Calculates cell cycle performance from raw cycling data.

1. Navigate `Run` > `CE Calculation` and enter the relavent details. See [Data Set](#data-set) and [Data Entry](#data-entry) for more details.
2. Cell performance output are output into the folder `output/${DATA_SET_NAME}.xlsx`. To quickly open output folder, navigate `Files` > `Open output`.

### Data Set

A set of data file configuration of the same cell cycling experiment to be compiled and processed together for cell performance calculations.

| Field   | Description                           | Example          |
| ------- | ------------------------------------- | ---------------- |
| Name    | Unique name of data set               | `Processed_Data` |
| Area    | Active area in cm2                    | `50`             |
| Entries | A list of [data entries](#data-entry) | -                |

### Data Entry

Configuration settings to process cell data.

| Field           | Description                                                                                             | Example                   |
| --------------- | ------------------------------------------------------------------------------------------------------- | ------------------------- |
| Path            | Path to data file. Only `.CSV` and `.XLSX` file formats are supported.                                  | `Data.xlsx`, `Data.csv`   |
| Sheet title     | Title of sheet within `.XLSX` data file. Leave blank to assume active sheet or if data file is `.CSV`.  | `records`                 |
| Preset path     | Path to configuration preset. Mutually exclusive with the remaining fields                              | `presets/StdConfig.json`  |
| Time Hdr        | Total time header name. Time value will have to be in hour, minute and seconds format delimited by `:`. | `Total Time`              |
| Type Hdr        | Step type header name.                                                                                  | `Step Type`               |
| Chg Cap Hdr     | Accumulated cycle charging capacity header name.                                                        | `Capacity`                |
| DChg Cap Hdr    | Accumulated  cycle discharging capacity header name.                                                    | `Capacity`                |
| Chg Energy Hdr  | Accumulated cycle charging energy header name.                                                          | `Energy`                  |
| DChg Energy Hdr | Accumulated cycle discharging energy header name.                                                       | `Energy`                  |
| Chg Type Name   | Step type names that represents a charging step. CSV syntax.                                            | `CC Chg`, `CC Chg,Chg`    |
| DChg Type Name  | Step type names that represents a discharging step. CSV syntax.                                         | `CC DChg`, `CC DChg,DChg` |

Additional points to note:

* All fields are case sensitive.
* Leading and trailing whitespaces are significant and not ignored. Thus for **Chg Type Name** field, `CC Chg, Chg` will match to `<SPACE>Chg`, where `<SPACE>` is a space character.

## Shunt Current

Simulates a single charging and discharging cycle for a specified system.

1. Navigate `Run` > `SC Simulation` and enter the relavent parameters.
2. Output are outputed into the folder `output/${NAME} - Summary.csv` and `output/${NAME} - Raw Data.csv`. To quickly open output folder, navigate `Files` > `Open output`.

--------------------------------------------------------------------------------

## Building

1. Install Qt6 ([here](https://www.qt.io/)).
2. Install 7-zip ([here](https://www.7-zip.org/)). Ensure that the executable in installed into `C:\Program Files\7-Zip\7z.exe`. This should be the default installation path.
3. Install vcpkg ([here](https://vcpkg.io/en/)). Follow the setup instructions.
4. Install cmake ([here](https://cmake.org/download/)).
5. Install visual studio for MSVC.
6. Run `configure.bat` batch script to configure and install remaining dependencies.
7. Run cmake to configure and build.

Commands for steps 5 to 7 are as follows in powershell (for CMD replace `` ` `` with `^`):

```text
./configure.bat ${VCPKG_EXE_PATH}

cmake --no-warn-unused-cli `
  -DCMAKE_TOOLCHAIN_FILE:STRING=${TOOLCHAIN_PATH} `
  -DCMAKE_PREFIX_PATH:STRING=${QT_PATH} `
  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE `
  -S . `
  -B build `
  -G "Visual Studio 17 2022" `
  -T host=x64 `
  -A x64

cmake --build build --config Release --target ALL_BUILD --
```

Replace the following placeholders:

* `${VCPKG_EXE_PATH}` with path to `vcpkg.exe`.
* `${TOOLCHAIN_PATH}`  with path to `vcpkg.cmake`.
* `${QT_PATH}` qt6 library path.

--------------------------------------------------------------------------------

## Dependencies

* GUI framework: [Qt Framework](https://www.qt.io/)
* JSON library: [nlohmann/json](https://github.com/nlohmann/json)
* XLSX library: [xlnt](https://github.com/tfussell/xlnt)
* Graphs: [QCustomPlot v2.1.1](https://www.qcustomplot.com/index.php/introduction)

## Acknowledgements

* Icons by [icon8](https://icons8.com/)
