#!/usr/bin/env python
"""
exits with 0 if given HICANN has highspeed, with 1 otherwise
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
hicann_global_c = C.HICANNGlobal(hicann_c, wafer_c)
fpga_global_c = hicann_global_c.toFPGAGlobal()

backend = load.FpgaWithBackend(args.defects_path, fpga_global_c)
has_highspeed = backend.hslinks().has(hicann_global_c.toHighspeedLinkOnDNC())
exit(not has_highspeed)
