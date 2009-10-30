
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

env.Append( CPPPATH = ['src', 'include'] )
env.Append( CXXFLAGS = ' -Wall -Wextra' )

env.Append( CPPFLAGS = ' -DBOOST_DISABLE_THREADS' )

platform = os.uname()[0]
if platform != 'Darwin':
    env.Append( LIBS=['rt'] )

if debug:
    env.Append( CXXFLAGS = ' -g3 -ggdb -O0' )
    env.Append( CPPFLAGS = ' -DBOOST_ASIO_ENABLE_BUFFER_DEBUGGING' )
else:
    env.Append( CXXFLAGS = ' -O3 -finline-limit=800' )
    env.Append( CPPFLAGS = ' -DNDEBUG' )
    if platform != 'Darwin':
        env.Append( CXXFLAGS = ' -march=native' )

env.Program( 'distmapServer',
             Glob( 'src/boost_lib/*.cpp' ) +
             Glob( 'src/google/*.cc' ) + 
             Glob( 'src/distmap/*.cpp' ) +
             Glob( 'src/distmap/util/*.cpp' )
           )


    


