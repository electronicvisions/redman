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

def depends(ctx):
    ctx('redman', 'test')  # build tests by default
    ctx('halbe')

def options(opt):
    recurse(opt)
    opt.load('compiler_cxx')
    opt.load('boost')
    opt.load('documentation')

def configure(cfg):
    recurse(cfg)
    cfg.load('compiler_cxx')
    cfg.load('boost')
    cfg.load('documentation')

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

def build(bld):
    recurse(bld)

    bld(
            target          = 'redman_inc',
            export_includes = ['.'],
    )

    bld.shlib(
            target          = 'redman',
            source          = bld.path.ant_glob('redman/**/*.cpp'),
            use             = [
                'BOOST4REDMAN',
                'LOG4REDMAN',
                'DL4REDMAN',
                'redman_inc',
                'halbe',
                ],
            install_path    = 'lib',
    )

def doc(dox):
    dox(features  = 'doxygen',
        doxyfile  = 'doc/Doxyfile',
        doxyinput = ['redman'])
