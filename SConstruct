
import os, sys
if os.path.exists( '/usr/local/build_cache' ):
    CacheDir( '/usr/local/build_cache' )

debug = int( ARGUMENTS.get('debug', 0) )

env = Environment()

conf = Configure(env)
# if not conf.CheckHeader( 'boost/asio.hpp' ):
#    print ' === Boost libraries not found === '
#    Exit(1)
env = conf.Finish()

env.Append( CPPPATH = ['src'] )
env.Append( CXXFLAGS = ' -Wall -Wextra' )
env.Append( LIBS=['rt', 'boost_system-mt'] )

if debug:
    env.Append( CXXFLAGS = ' -g3 -ggdb -O0' )
else:
    env.Append( CXXFLAGS = ' -O3 -march=native -finline-limit=800 -DNDEBUG' )

env.Program( 'distmapServer', 
             Glob( 'src/distmap/*.cpp' ) + 
             Glob( 'src/distmap/util/*.cpp' )
             )


    


