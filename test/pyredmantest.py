#!/usr/bin/env python

import os
import unittest
import tempfile
import shutil
import pyredman as redman
from pyhalbe.Coordinate import (Enum, Wafer, HICANNOnWafer, NeuronOnHICANN, HICANNGlobal, FPGAOnWafer, HICANNOnHS)
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

        hoss = fpga.hicanns()
        absent = HICANNOnHS(Enum(7))

        if hoss.has(absent):
            hoss.disable(absent)
        self.assertFalse(hoss.has(absent))

        # fpga is lazy-loaded and cached
        self.assertFalse(wafer.get(fid).hicanns().has(absent))

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

        self.assertFalse(wafer_with_backend_v1.hicanns().has_value())
        self.assertFalse(wafer_with_backend_v1.fpgas().has_value())

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

if __name__ == '__main__':
    unittest.main()
