#!/usr/bin/env python
import sys, os
try:
    from waflib.extras import symwaf2ic
    from waflib.extras.gtest import summary
    recurse = lambda *args: None # dummy recurse
except ImportError:
    from gtest import summary
    from symwaf2ic import recurse_depends
    recurse = lambda ctx: recurse_depends(depends, ctx)

from waflib import Utils

def depends(ctx):
    ctx('redman', 'test')  # build tests by default
    ctx('halbe')

def options(opt):
    recurse(opt)
    opt.load('compiler_cxx')
    opt.load('boost')
    opt.load('doxygen')

def configure(cfg):
    recurse(cfg)
    cfg.load('compiler_cxx')
    cfg.load('boost')
    cfg.load('doxygen')

    cfg.check_boost(
            lib='filesystem serialization system',
            uselib_store='BOOST4REDMAN')

    # runtime library loading
    cfg.check_cxx(
        lib='dl',
        uselib_store='DL4REDMAN',
        mandatory=True)

    cfg.check_cxx(
            lib='log4cxx',
            uselib_store='LOG4REDMAN',
            mandatory=True)

    cfg.check_boost(
        lib='filesystem serialization system',
        uselib_store='BOOST4REDMANXML'
    )

def build(bld):
    recurse(bld)

    bld(
            target          = 'redman_inc',
            export_includes = ['include'],
    )

    bld.shlib(
            features        = 'cxx cxxshlib',
            target          = 'redman',
            source          = bld.path.ant_glob('src/redman/**/*.cpp'),
            use             = [
                'BOOST4REDMAN',
                'LOG4REDMAN',
                'DL4REDMAN',
                'redman_inc',
                'halbe',
                ],
            install_path    = 'lib',
    )

    flags = {
        'lib': [
            'dl',
        ]
    }

    bld.shlib(
        features='cxx cxxshlib',
        target='redman_xml',
        source=bld.path.ant_glob('src/backends/xml/*.cpp'),
        use=[
            'BOOST4REDMANXML',
            'redman',
        ],
        includes='.',
        install_path='lib',
        **flags
    )

    bld.install_files(
        '${PREFIX}/bin',
        bld.path.ant_glob('tools/*'),
        relative_trick=False,
        chmod=Utils.O755,
    )

def doc(dox):
    dox(features  = 'doxygen',
        doxyfile  = 'doc/Doxyfile',
        pars={ 'INPUT': "%s/include/redman %s/src" % tuple(dox.path.get_src().abspath() for i in range(2))}
    )
