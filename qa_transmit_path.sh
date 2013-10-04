#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/csmedina/WiComModules/gr-ieee802_15_4a/python
export PATH=/home/csmedina/WiComModules/gr-ieee802_15_4a/build/python:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export PYTHONPATH=/home/csmedina/WiComModules/gr-ieee802_15_4a/build/swig:$PYTHONPATH
/usr/bin/python /home/csmedina/WiComModules/gr-ieee802_15_4a/python/transmit_path.py 
