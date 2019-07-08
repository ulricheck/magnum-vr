from conans import ConanFile, CMake
from conans import tools
from conans.tools import os_info, SystemPackageTool
import os, sys
import sysconfig
from io import StringIO

class MagnumVRConan(ConanFile):
    name = "magnum-vr"
    version = "0.1"

    description = "Magnum VR Integration"
    url = "https://github.com/ulricheck/magnum-vr"
    license = "GPL"

    short_paths = True
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    requires = (
        "magnum/2019.01@camposs/stable",
        "corrade/2019.01@camposs/stable",
        "openvr/1.4.18@vendor/stable",
    )

    options = {
        "shared": [True, False],
        "with_openvr": [True, False],
   }
    default_options = {
        "with_openvr": True,
        "shared": False,
    }

    # all sources are deployed with the package
    exports_sources = "modules/*", "package/*", "toolchains/*", "src/*", "CMakeLists.txt"

    def configure(self):
        if self.options.shared:
            self.options["magnum"].shared = True
            self.options["corrade"].shared = True

    def build(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure()
        cmake.build()
        cmake.install()
