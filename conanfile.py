from conans import ConanFile


class HelloConan(ConanFile):
    name = "smartref"
    version = "master"
    exports_sources = "include/*"
    no_copy_source = True

    def package(self):
        self.copy("*.h")
