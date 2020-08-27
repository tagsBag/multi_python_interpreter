from __future__ import print_function
print("-----")
import sys
print("import strategy. sys.xxx={}".format(getattr(sys, 'xxx', 'attribute not set')))


def on_start():
    print("python on_start called")
    #Context.write_log("on_start() from python")

def on_tick(tick):
    print("python on_tick called")
    #Context.write_log("on_tick() from python")
