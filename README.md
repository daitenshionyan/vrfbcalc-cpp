# VRFBCALC - CPP

Utility application to perform vanadium redox flow battery related calculations.

## Cell Performance

Calculates cell cycle performance from raw cycling data.

1. Click the configuration button and enter the relavent details for the data files. See below for more details.
2. Click start to run the calculations.
3. Cell performance output are output into the folder `output/${DATA_SET_NAME}.csv`. To quickly open output folder, navigate `Files` > `Open output`.

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

## Acknowledgements

* GUI framework: [Qt Framework](https://www.qt.io/)
* JSON library: [nlohmann/json](https://github.com/nlohmann/json)
* XLSX library: [xlnt](https://github.com/tfussell/xlnt)
