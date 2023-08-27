# VRFBCALC - CPP

Utility application to perform vanadium redox flow battery related calculations.

## Cell Performance

Calculates cell cycle performance from raw cycling data.

1. Save cell cycling logged data in csv format, encoded in UTF-8, into the same directory as the application's executable.
2. Insert the logged data's configuration settings into `CellEffConfig.json`. Create the file in the same directory as the application's executable if it is not already present.
3. Double click the executable to run. All successfully processed data will be saved in the same directory with the prefix `Processed_` and then the name of the data file proceessed.

### CellEffConfig

Below shows the format of the contents of `CellEffConfig.json`. Text prefixed with `#` should be replaced and `...` would mean that more than 1 of the previous type of property may appear.

```text
{
  "#data_file_name": {
    "t_time_h": "#total_time_header",
    "type_h": "#step_type_header",
    "c_cap_h": "#charging_capacity_header",
    "d_cap_h": "#discharging_capacity_header",
    "c_energy_h": "#charging_energy_header",
    "d_energy_h": "#discharging_energy_header",
    "c_type_names": [ "#charging_type_names", ... ],
    "d_type_names": [ "#discharing_type_names", .... ],
    "area": "#active_area"
  }, ...
}
```

### Requirements

* The logged cycle data must have the following data columns.
   1. Total time since start of logging.
   2. Cycle step type.
   3. Accumulated charging capacity with respect to that cycle step.
   4. Accumulated discharging capacity with respect to that cycle step.
   5. Accumulated charging energy with respect to that cycle step.
   6. Accumulated discharging energy with respect to that cycle step.
* Logged data should be arranged in a table with the headers in the first row.
* Both config file and logged data files must be in the same directory as executable and not inside a folder.
* Time field column should have the following format `H:M:S`, where `H` and `M` are optional.
