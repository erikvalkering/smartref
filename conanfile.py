from conans import ConanFile


class HelloConan(ConanFile):
    name = "smartref"
    version = "master"
    url = 'https://github.com/erikvalkering/smartref'
    license = 'MIT'
    description = 'A modern header-only zero-overhead library ' + \
                  'for creating smart references'
    exports_sources = "include/*"
    no_copy_source = True

    def package(self):
        self.copy("*.h", dst='include', src='include')
