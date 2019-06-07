#!/usr/bin/env python
"""
exits with 0 if given Wafer has HICANN, with 1 otherwise
"""

import argparse
import Coordinate as C
from pyredman.load import load

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--wafer', type=int, required=True,
                    help="Wafer enum")
parser.add_argument('--hicann', type=int, required=True,
                    help="HICANNOnWafer enum")
parser.add_argument('--defects_path', required=True,
                    help="path to defects files")

args = parser.parse_args()
wafer_c = C.Wafer(args.wafer)
hicann_c = C.HICANNOnWafer(C.Enum(args.hicann))

backend = load.WaferWithBackend(args.defects_path, wafer_c)
has_hicann = backend.has(hicann_c)
exit(not has_hicann)
