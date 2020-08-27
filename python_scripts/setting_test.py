# set sys.xxx, it will only be reflected in t4, which runs in the context of the main interpreter
from __future__ import print_function
import sys
sys.xxx = ['abc']
print('main: setting sys.xxx={}'.format(sys.xxx))
