#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: furushchev <furushchev@jsk.imi.i.u-tokyo.ac.jp>

import os
import sys
from respeaker_node import PARAMETERS, init_respeaker


def main(out):
    dev = init_respeaker()
    if not dev:
        print('No device found. Please connect a device.')
        return
    with open(out, "w") as f:
        f.write("""\
#!/usr/bin/env python
#
# WARNING!!
# This file is automatically generated.
# DO NOT MODIFY THIS FILE!
#
# To generate this file, please run:
# cd /path/to/this/package
# python ./scripts/respeaker_gencfg.py
#

from dynamic_reconfigure.parameter_generator_catkin import *

gen = ParameterGenerator()

#       name    type     level     description     default      min      max""")

        for key, val in PARAMETERS.items():
            type_, max_, min_, rw_ = val[2:6]
            desc_ = " ".join(val[6:])
            def_ = dev.read(key)
            if rw_ != "rw":
                continue
            if type_ == "int" and max_ == 1 and min_ == 0:
                if def_ == 1:
                    def_ = True
                else:
                    def_ = False
                f.write("""
gen.add("{name}", bool_t, 0, "{desc}", {def_})""".format(
    name=key, desc=desc_, def_=def_))
            elif type_ == "int":
                f.write("""
gen.add("{name}", int_t, 0, "{desc}", {def_}, {min_}, {max_})""".format(
    name=key, desc=desc_, def_=def_, min_=min_, max_=max_))
            elif type_ == "float":
                f.write("""
gen.add("{name}", double_t, 0, "{desc}", {def_:f}, {min_:f}, {max_:f})""".format(
    name=key, desc=desc_, def_=def_, min_=min_, max_=max_))
            else:
                print("Param '{name}' is ignored.".format(name=key))

        f.write("""

exit(gen.generate("respaker_ros", "respeaker_ros", "Respeaker"))
""")

    os.chmod(out, 0o775)

    print("Saved cfg to %s" % out)


if __name__ == '__main__':
    if len(sys.argv) > 1:
        outpath = sys.argv[1]
    else:
        outpath = os.path.join(
            os.path.abspath(os.path.dirname(__file__)),
            "..", "cfg", "Respeaker.cfg")
    main(outpath)
