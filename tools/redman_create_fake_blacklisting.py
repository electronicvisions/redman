#!/usr/bin/env python

import pyhalco_hicann_v2 as C
from pyredman.load import load

backend = load.HicannWithBackend("./", C.HICANNGlobal())
backend.save()

backend = load.WaferWithBackend("./", C.Wafer())
backend.save()
