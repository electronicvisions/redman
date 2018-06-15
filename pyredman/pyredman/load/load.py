import pyredman

def WithBackend(ResourceWithBackend, backend_path, resource_coordinate, redman_backend, ignore_missing):

    if redman_backend == "XML":
        lib = pyredman.loadLibrary('libredman_xml.so')
    else:
        raise RuntimeError("unhandled redman backend {}".format(redman_backend))

    backend = pyredman.loadBackend(lib)
    backend.config('path', backend_path)
    backend.init()
    with_backend = getattr(pyredman,ResourceWithBackend)(backend, resource_coordinate, ignore_missing)

    return with_backend

def HicannWithBackend(backend_path, hicann_global, redman_backend="XML", ignore_missing=True):
    return WithBackend("HicannWithBackend", backend_path, hicann_global, redman_backend, ignore_missing)

def WaferWithBackend(backend_path, wafer, redman_backend="XML", ignore_missing=True):
    return WithBackend("WaferWithBackend", backend_path, wafer, redman_backend, ignore_missing)
