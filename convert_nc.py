#!/usr/bin/env python

import sys
from pathlib import Path

import pandas as pd
import xarray as xr

if len(sys.argv) != 3:
    print(f"usage: {sys.argv[0]} INPUT_PATH OUTPUT_PATH")
    sys.exit(1)

input_path = Path(sys.argv[1])
output_path = Path(sys.argv[2])

if input_path == output_path:
    raise ValueError("files are going to be overwritten")

output_path.mkdir(parents=True, exist_ok=True)

for input_nc in input_path.glob("*.nc"):
    with xr.open_dataset(input_nc) as ds:
        ds_subset = ds.isel(depth=0, time=0)
        ts = (
            pd.to_datetime(ds.time.values)
            .tz_localize("UTC")
            .strftime("%Y-%m-%d %H:%M:%S.%f %Z")
            .item()
        )
        ds_subset["uo"].attrs["date"] = ts
        ds_subset["vo"].attrs["date"] = ts
        ds_subset.to_netcdf(output_path / input_nc.name)
