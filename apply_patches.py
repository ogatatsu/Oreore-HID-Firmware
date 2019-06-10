from os.path import join, isfile
import sys

Import("env")

framework_dir = env.PioPlatform().get_package_dir("framework-arduinoadafruitnrf52")
patchflag_file = join(framework_dir, ".patching-done")
patch_file = "framework-arduinoadafruitnrf52.patch"

if not isfile(patchflag_file):

    env.Execute('"%s" patch.py -d "%s" "%s"' % (sys.executable, framework_dir, patch_file))

    def _touch(path):
        with open(path, "w") as fp:
            fp.write("")

    env.Execute(lambda *args, **kwargs: _touch(patchflag_file))
