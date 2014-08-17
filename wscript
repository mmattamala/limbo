#!/usr/bin/env python
# encoding: utf-8

VERSION='0.0.1'
APPNAME='limbo'

srcdir = '.'
blddir = 'build'

import copy
import os, sys
import limbo

def options(opt):
        opt.load('compiler_cxx boost waf_unit_test')
        opt.load('compiler_c')
        opt.load('eigen')
        opt.load('tbb')
        opt.load('sferes')
	opt.load('limbo')
	
def configure(conf):
    	print("configuring b-optimize")
    	conf.load('compiler_cxx boost waf_unit_test')
        conf.load('compiler_c')
        conf.load('eigen')
        conf.load('tbb')
        conf.load('sferes')

	common_flags = "-Wall -std=c++11"

	cxxflags = conf.env['CXXFLAGS']
	conf.check_boost(lib='serialization timer filesystem \
            system unit_test_framework program_options \
            graph mpi thread',
            min_version='1.35')
        conf.check_eigen()
        conf.check_tbb()
        conf.check_sferes()
        if conf.is_defined('USE_TBB'):
                common_flags += " -DUSE_TBB "

        if conf.is_defined('USE_SFERES'):
                common_flags += " -DUSE_SFERES -DSFERES_FAST_DOMSORT"

	# release
        opt_flags = common_flags + ' -O3 -msse2 -ggdb3 -g'
        conf.env['CXXFLAGS'] = cxxflags + opt_flags.split(' ')
        print conf.env['CXXFLAGS']

def build(bld):
	bld.recurse('src/limbo')
        bld.recurse('src/examples')
        bld.recurse('src/tests')
        bld.recurse('src/benchmarks')
        from waflib.Tools import waf_unit_test
        bld.add_post_fun(waf_unit_test.summary)
	

def shutdown (ctx):
    if ctx.options.qsub:
        limbo.qsub(ctx.options.qsub)
