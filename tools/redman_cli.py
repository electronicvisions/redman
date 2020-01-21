#!/usr/bin/env python
"""
redman command line interface

Usage: redman_cli.py /wang/data/calibration/brainscales/wip W33H0 has drivers $(seq 0 223)
"""

import inspect
import argparse

from pyhalco_common import Enum
import pyhalco_hicann_v2 as C
import pyredman
from pyredman.load import load

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='redman command line interface')
    parser.add_argument("backend_path")
    parser.add_argument("resource_short_format", help = "short format of Wafer, FPGAGlobal or HICANNGlobal; eg. W33, W5F2, W12H100")
    parser.add_argument("action", choices = ["has", "enable", "disable"], help="action to perform on the given component(s)")
    parser.add_argument("component", help="function name (without parenthesis) to retrieve a component of <resource>, e.g. neurons for Hicann")
    parser.add_argument("component_enum", help="coordinate enum of <component> on which <action> is applied", nargs="+", type=int)

    args = parser.parse_args()

    resource = C.from_string(args.resource_short_format)

    if isinstance(resource, C.Wafer):
        redman_resource = load.WaferWithBackend(args.backend_path, resource)
    elif isinstance(resource, C.FPGAGlobal):
        redman_resource = load.FpgaWithBackend(args.backend_path, resource)
    elif isinstance(resource, C.HICANNGlobal):
        redman_resource = load.HicannWithBackend(args.backend_path, resource)
    else:
        raise RuntimeError("No loadable redman resource assigned to {}. "
                           "Choose from Wafer, FPGAGlobal, HICANNGlobal.".format(resource))

    redman_component = getattr(redman_resource, args.component)()
    resource_type = redman_component.resource
    for enum in args.component_enum:
        coord = resource_type(Enum(enum))
        if args.action == "has":
            print redman_component.has(coord)
        elif args.action == "enable":
            redman_component.enable(coord, pyredman.switch_mode.NONTHROW)
        elif args.action == "disable":
            redman_component.disable(coord, pyredman.switch_mode.NONTHROW)

    if args.action in ["enable", "disable"]:
        redman_resource.save()
