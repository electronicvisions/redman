#!/usr/bin/env python

import os
import unittest
import tempfile
import shutil
import pyredman as redman
from pyhalco_common import Enum
from pyhalco_hicann_v2 import (Wafer, HICANNOnWafer, NeuronOnHICANN, HICANNGlobal, FPGAOnWafer, HighspeedLinkOnDNC)
import pyredman.load as load


def initBackend(fname):
    lib = redman.loadLibrary(fname)
    backend = redman.loadBackend(lib)

    if not backend:
        raise Exception('unable to load %s' % fname)

    return backend


class BackendTests(object):
    def test_wafer_with_backend(self):
        wid = Wafer(5)
        hid = HICANNOnWafer(Enum(23))
        fid = FPGAOnWafer(Enum(42))

        wafer = redman.WaferWithBackend(self.backend, wid)

        hicanns = wafer.hicanns()
        fpgas = wafer.fpgas()

        if not hicanns.has(hid):
            hicanns.enable(hid)
        self.assertTrue(hicanns.has(hid))
        wafer.save()

        if not fpgas.has(fid):
            fpgas.enable(fid)
        self.assertTrue(fpgas.has(fid))
        wafer.save()

        hicann = wafer.get(hid)
        self.assertTrue(hicann)

        nrns = hicann.neurons()
        absent = NeuronOnHICANN(Enum(7))

        if nrns.has(absent):
            nrns.disable(absent)
        self.assertFalse(nrns.has(absent))

        # hicann is lazy-loaded and cached
        self.assertFalse(wafer.get(hid).neurons().has(absent))

        n = nrns.available()
        nrns.enable(absent)
        self.assertTrue(nrns.has(absent))
        self.assertEqual(nrns.available(), n + 1)

        fpga = wafer.get(fid)
        self.assertTrue(fpga)

        hoss = fpga.hslinks()
        absent = HighspeedLinkOnDNC(Enum(7))

        if hoss.has(absent):
            hoss.disable(absent)
        self.assertFalse(hoss.has(absent))

        # fpga is lazy-loaded and cached
        self.assertFalse(wafer.get(fid).hslinks().has(absent))

        n = hoss.available()
        hoss.enable(absent)
        self.assertTrue(hoss.has(absent))
        self.assertEqual(hoss.available(), n + 1)

    def test_serialization_versions(self):

        hicann_with_backend_v0 = load.HicannWithBackend("share/redman/fake_blacklisting/v0/",
                                                        HICANNGlobal(), ignore_missing=False)

        self.assertTrue(hicann_with_backend_v0.neurons().has_value())

        wafer_with_backend_v0 = load.WaferWithBackend("share/redman/fake_blacklisting/v0/",
                                                      Wafer(), ignore_missing=False)

        self.assertTrue(wafer_with_backend_v0.hicanns().has_value())
        self.assertFalse(wafer_with_backend_v0.fpgas().has_value())

        hicann_with_backend_v1 = load.HicannWithBackend("share/redman/fake_blacklisting/v1/",
                                                        HICANNGlobal(), ignore_missing=False)

        self.assertFalse(hicann_with_backend_v1.neurons().has_value())

        wafer_with_backend_v1 = load.WaferWithBackend("share/redman/fake_blacklisting/v1/",
                                                      Wafer(), ignore_missing=False)

        self.assertFalse(wafer_with_backend_v1.hicanns().has_value())
        self.assertFalse(wafer_with_backend_v1.fpgas().has_value())

        wafer_with_backend_v2 = load.WaferWithBackend("share/redman/fake_blacklisting/v2/",
                                                      Wafer(), ignore_missing=False)

        self.assertFalse(wafer_with_backend_v2.hicanns().has_value())
        self.assertFalse(wafer_with_backend_v2.fpgas().has_value())

        hicann_with_backend_v3 = load.HicannWithBackend("share/redman/fake_blacklisting/v3/",
                                                        HICANNGlobal(), ignore_missing=False)

        self.assertFalse(hicann_with_backend_v3.neurons().has_value())
        self.assertFalse(hicann_with_backend_v3.synapses().has_value())
        self.assertFalse(hicann_with_backend_v3.drivers().has_value())
        self.assertFalse(hicann_with_backend_v3.synaptic_inputs().has_value())
        self.assertFalse(hicann_with_backend_v3.hbuses().has_value())
        self.assertFalse(hicann_with_backend_v3.vbuses().has_value())
        self.assertFalse(hicann_with_backend_v3.mergers0().has_value())
        self.assertFalse(hicann_with_backend_v3.mergers1().has_value())
        self.assertFalse(hicann_with_backend_v3.mergers2().has_value())
        self.assertFalse(hicann_with_backend_v3.mergers3().has_value())
        self.assertFalse(hicann_with_backend_v3.dncmergers().has_value())
        self.assertFalse(hicann_with_backend_v3.synapserows().has_value())
        self.assertFalse(hicann_with_backend_v3.analogs().has_value())
        self.assertFalse(hicann_with_backend_v3.backgroundgenerators().has_value())
        self.assertFalse(hicann_with_backend_v3.fgblocks().has_value())
        self.assertFalse(hicann_with_backend_v3.vrepeaters().has_value())
        self.assertFalse(hicann_with_backend_v3.hrepeaters().has_value())
        self.assertFalse(hicann_with_backend_v3.synapseswitches().has_value())
        self.assertFalse(hicann_with_backend_v3.crossbarswitches().has_value())
        self.assertFalse(hicann_with_backend_v3.synapseswitchrows().has_value())

class TestXMLBackend(BackendTests, unittest.TestCase):
    def setUp(self):
        basedir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, basedir)

        backend = initBackend('libredman_xml.so')
        backend.config('path', basedir)
        backend.init()
        self.backend = backend


class TestPyRedman(unittest.TestCase):
    def test_set_logic(self):
        n1 = redman.Neurons()
        self.assertFalse(n1.has_value())
        n2 = redman.Neurons()
        self.assertFalse(n1.has_value())

        nrn33 = NeuronOnHICANN(Enum(33))
        nrn55 = NeuronOnHICANN(Enum(55))

        n2.disable(nrn55)
        self.assertTrue(n2.has_value())
        n1.difference(n2)
        self.assertTrue(n1.has_value())

        self.assertTrue(n1.has(nrn55))
        self.assertEqual(n1.available(), 1)

        n1.merge(n2)
        self.assertEqual(n1.available(), 512)

        n1.disable(nrn33)
        n1.symmetric_difference(n2)
        self.assertTrue(n1.has(nrn55))
        self.assertTrue(n1.has(nrn33))
        self.assertEqual(n1.available(), 2)

        n1.intersection(n2)
        self.assertFalse(n1.has(nrn55))
        self.assertTrue(n1.has(nrn33))
        self.assertEqual(n1.available(), 1)

        n1.reset()
        self.assertFalse(n1.has_value())

        n2.reset()
        self.assertFalse(n2.has_value())

class TestSerialization(unittest.TestCase):
    def test_wafer_pickle(self):
        import pickle
        w = redman.Wafer()
        w_str = pickle.dumps(w)
        w2 = pickle.loads(w_str)
        self.assertEqual(w, w2)
        # change something to ensure that it's not a mere pointer copy
        w.hicanns().disable(HICANNOnWafer())
        self.assertNotEqual(w, w2)

    def test_wafer_copy(self):
        import copy
        w = redman.Wafer()
        w2 = copy.deepcopy(w)
        self.assertEqual(w, w2)
        # change something to ensure that it's not a mere pointer copy
        w.hicanns().disable(HICANNOnWafer())
        self.assertNotEqual(w, w2)

    def test_hicann_pickle(self):
        import pickle
        h = redman.Hicann()
        h_str = pickle.dumps(h)
        h2 = pickle.loads(h_str)
        self.assertEqual(h, h2)
        # change something to ensure that it's not a mere pointer copy
        h.neurons().disable(NeuronOnHICANN())
        self.assertNotEqual(h, h2)

    def test_hicann_copy(self):
        import copy
        h = redman.Hicann()
        h2 = copy.deepcopy(h)
        self.assertEqual(h, h2)
        # change something to ensure that it's not a mere pointer copy
        h.neurons().disable(NeuronOnHICANN())
        self.assertNotEqual(h, h2)

    def test_fpga_pickle(self):
        import pickle
        f = redman.Fpga()
        f_str = pickle.dumps(f)
        f2 = pickle.loads(f_str)
        self.assertEqual(f, f2)
        # change something to ensure that it's not a mere pointer copy
        f.hslinks().disable(HighspeedLinkOnDNC())
        self.assertNotEqual(f, f2)

    def test_fpga_copy(self):
        import copy
        f = redman.Fpga()
        f2 = copy.deepcopy(f)
        self.assertEqual(f, f2)
        # change something to ensure that it's not a mere pointer copy
        f.hslinks().disable(HighspeedLinkOnDNC())
        self.assertNotEqual(f, f2)

if __name__ == '__main__':
    unittest.main()
