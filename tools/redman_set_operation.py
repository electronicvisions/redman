#!/usr/bin/env python
"""
Intersects (possibly empty) resource from output with resources from input:

e.g.: ./redman_set_operation.py W33 -o ./ -i foo bar baz -s intersection

loads W33 from ./ and intersects it with W33 blacklisting information found in the
directories foo bar baz.
"""

import os
import argparse

import pyhalco_hicann_v2 as C
from pyredman.load import load

parser = argparse.ArgumentParser()
parser.add_argument("resource_short_format",
                    help="short format of Wafer, FPGAGlobal or HICANNGlobal; eg. W33, W5F02, W12H100",
                    nargs="+")
parser.add_argument("-o", "--output_backend_path", required=True, help="might be empty")
parser.add_argument("-i", "--input_backend_path", nargs="+", help="path to redman data must exist")
parser.add_argument("-s", "--set_operation", choices=["intersection"], default="intersection")

args = parser.parse_args()

for r in args.resource_short_format:

    resource_coordinate = C.from_string(r)

    if isinstance(resource_coordinate, C.Wafer):
        redman_resource_loader = load.WaferWithBackend
    elif isinstance(resource_coordinate, C.FPGAGlobal):
        redman_resource_loader = load.FpgaWithBackend
    elif isinstance(resource_coordinate, C.HICANNGlobal):
        redman_resource_loader = load.HicannWithBackend
    else:
        raise RuntimeError("No loadable redman resource assigned to {}. "
                           "Choose from Wafer, FPGAGlobal, HICANNGlobal.".format(resource_coordinate))

    redman_resource = redman_resource_loader(args.output_backend_path, resource_coordinate, ignore_missing=True)

    for ibp in args.input_backend_path:
        input_backend = redman_resource_loader(ibp, resource_coordinate, ignore_missing=False)
        getattr(redman_resource, args.set_operation)(input_backend)

    redman_resource.save()
