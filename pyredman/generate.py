#!/usr/bin/env python

import itertools
import re

from pyplusplus.decl_wrappers import call_policies
from pygccxml.declarations.matchers import custom_matcher_t

from pywrap.wrapper import Wrapper
import pywrap.namespaces as ns
import pywrap.classes


def factories(c, name='create'):
    factories = []
    factories.extend(c.mem_funs(name, allow_empty=True))

    for base in pywrap.classes.get_all_bases(c):
        if factories:
            break
        factories.extend(base.mem_funs(name, allow_empty=True))

    return factories


def create_factories(c, name='create'):
    fs = factories(c, name)
    if not fs:
        raise RuntimeError('No factories found for {}'.format(c))

    c.add_fake_constructors(fs)
    c.constructors().exclude()


wrap = Wrapper()
wrap.set_number_of_files(0)
mb = wrap.mb

redman = mb.namespace('redman')

wrap.ishell()

for c in redman.namespace('resources').classes():
    c.include()

    c.mem_funs('copy', allow_empty=True).exclude()

    if not (c.name == 'Base' or c.name.startswith('ResourceWithFactory') or "behavior" in c.name):
        create_factories(c)

def renamed(scope, regex, repl):
    regex = re.compile(regex)
    sanitize = lambda s: re.sub(r'\W+', '', s)
    subfn = lambda match: repl.format(*map(sanitize, match.groups()))

    f = custom_matcher_t(lambda decl: regex.match(decl.name))
    for c in ns.find(scope, 'classes', [f]):
        c.rename(regex.sub(subfn, c.name))
        yield c

for c in itertools.chain(
        renamed(redman, '^ResourceManager<([^,]+),.*', '__ResourceManager_{}'),
        renamed(redman, '^ResourceWithFactory<([^,]+),.*', '__ResourceWithFactory_{}')):
    c.include()
    c.constructors().exclude()

ns.include(mb, 'namespaces', ['::redman::backend'])

ns.exclude(mb, 'namespaces', [
           '::boost::serialization',
           '::boost::archive'
           ])

ns.exclude_by_access_type(mb, ['variables', 'calldefs', 'classes'], 'private')
ns.exclude_by_access_type(mb, ['variables', 'calldefs', 'classes'], 'protected')

wrap.finish()
