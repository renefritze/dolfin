import SCons.Util, SCons.Scanner

import os, sys, re


def darwinCxx(env):
    """Change the cxx parts of env for the Darwin platform"""
    env.Append(CXXFLAGS=" -undefined dynamic_lookup")
    env.Append(SHLINKFLAGS=" -undefined dynamic_lookup")
    env.Append(LDMODULEFLAGS=" -undefined dynamic_lookup")
    if not env.GetOption("clean"):
        # remove /usr/lib if present in LIBPATH.
        # there should maybe be some testing here to make sure 
        # it doesn't blow in our face.
        try:
            env["LIBPATH"].remove('/usr/lib')
        except:
            pass
    return env

def winCxx(env):
    """Change the cxx parts of env for the Windows platform"""
    env.Append(SHLINKFLAGS=" -Wl,--enable-auto-import")
    return env

def darwinSwig(env):
    """Change the swig parts of env for the Darwin platform"""
    env['SHLINKFLAGS'] = env['LDMODULEFLAGS']
    env['SHLINKCOM'] = env['LDMODULECOM']
    env["SHLIBSUFFIX"] = ".so"
    return env

def winSwig(env):
    """Change the swig parts of env for the Windows platform"""
    env["SHLIBSUFFIX"] = ".pyd"
    # FIXME: We could also use (should work on all platforms)
    # env["SHLIBSUFFIX"] = distutils.sysconfig.get_config_var("SO")
    return env

def swigScanner(node, env, path):
    """SCons scanner hook to scan swig files for dependencies.
    @return: list of files depended on.
    """
    include_pattern = re.compile(r"%include\s+(\S+)")

    def recurse(path, search_path):
        """Scan recursively."""
        f = open(path)
        try: contents = f.read()
        finally: f.close()

        found = []
        for m in include_pattern.finditer(contents):
            inc_fpath = m.group(1)
            # Strip off quotes
            inc_fpath = inc_fpath.strip("'").strip('"')
            # Strip off site include marks
            inc_fpath = inc_fpath.strip('<').strip('>')
            real_fpath = os.path.realpath(inc_fpath)
            if os.path.dirname(real_fpath) != os.path.dirname(path):
                # Not in same directory as original swig file
                if os.path.isfile(real_fpath):
                    found.append(real_fpath)
                    continue
            
            # Look for unqualified filename on path
            for dpath in search_path:
                abs_path = os.path.join(dpath, inc_fpath)
                if os.path.isfile(abs_path):
                    found.append(abs_path)
                    break
        
        for f in [f for f in found if os.path.splitext(f)[1] == ".i"]:
            found += recurse(f, search_path)
        return found
    
    fpath = node.srcnode().path
    search_path = [os.path.abspath(d) for d in re.findall(r"-I(\S+)", " ".join(env["SWIGFLAGS"]))]
    search_path.insert(0, os.path.abspath(os.path.dirname(fpath)))
    r = recurse(fpath, search_path)
    return r

swigscanner = SCons.Scanner.Scanner(function=swigScanner, skeys=[".i"])

def swigEmitter(target, source, env):
    new_targets = []
    # If we are swigging a python extension module
    if "-python" in SCons.Util.CLVar(env.subst("$SWIGFLAGS")):
        for t in target:
            t = str(t)
            # Add the python module to target by replacing the swig
            # cxx suffix from the target by ".py"
            new_targets.append(t.replace(env.subst(env["SWIGCXXFILESUFFIX"]),".py"))
    return_targets = []
    for t in zip(target,new_targets):
        return_targets.extend(t)
    return (return_targets, source)

class Dependency:

    def __init__(self, cppPath=None, compileOpts=None, libPath=None, libs=None, linkOpts=None, version=None, compiler=None):
        self.cppPath, self.compileOpts, self.libPath, self.libs, self.linkOpts, self.version, self.compiler  = cppPath, compileOpts, libPath, libs, linkOpts, version, compiler

    def __str__(self):
        return "\ncppPath: %s\nlibPath: %s\nlibs: %s\nlibs: %s\ncompiler: %s\n" % \
               (self.cppPath,self.libPath,self.libs,self.linkOpts,self.compiler)


