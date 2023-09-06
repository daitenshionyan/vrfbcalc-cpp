# VRFBCALC - CPP

Utility application to perform vanadium redox flow battery related calculations.

## Cell Performance

Calculates cell cycle performance from raw cycling data.

1. Save cell cycling logged data files in csv format, encoded in UTF-8, into the same directory as the application's executable.
2. Click the configuration button and enter the relavent details for the data files.
3. Click start to run the calculations.

### Requirements

* The logged cycle data must have the following data columns.
   1. Total time since start of logging.
   2. Cycle step type.
   3. Accumulated charging capacity with respect to a cycle step.
   4. Accumulated discharging capacity with respect to a cycle step.
   5. Accumulated charging energy with respect to a cycle step.
   6. Accumulated discharging energy with respect to a cycle step.
* Logged data should be arranged in a table with the headers in the first row.
* Both config file and logged data files must be in the same directory as executable and not inside a folder.
* Time field column should have the following format `H:M:S`, where `H` and `M` are optional.

## Acknowledgements

* GUI framework: [Qt Framework](https://www.qt.io/)
* JSON library: [nlohmann/json](https://github.com/nlohmann/json)
* XLSX library: [xlnt](https://github.com/tfussell/xlnt)
