#!/usr/bin/env python
import os
import sys
import platform

from methods import print_error


def normalize_path(val, env):
    return val if os.path.isabs(val) else os.path.join(env.Dir("#").abspath, val)


def validate_parent_dir(key, val, env):
    if not os.path.isdir(normalize_path(os.path.dirname(val), env)):
        raise UserError("'%s' is not a directory: %s" % (key, os.path.dirname(val)))


libname = "godot-box2c"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Add(
    BoolVariable(
        key="compiledb",
        help="Generate compilation DB (`compile_commands.json`) for external tools",
        default=localEnv.get("compiledb", False),
    )
)
opts.Add(
    PathVariable(
        key="compiledb_file",
        help="Path to a custom `compile_commands.json` file",
        default=localEnv.get("compiledb_file", "compile_commands.json"),
        validator=validate_parent_dir,
    )
)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()
env["compiledb"] = False

env.Tool("compilation_db")
compilation_db = env.CompilationDatabase(
    normalize_path(localEnv["compiledb_file"], localEnv)
)
env.Alias("compiledb", compilation_db)

submodules_initialized = True
for dir_name in ["godot-cpp", "box2d"]:
    if os.path.isdir(dir_name):
        if os.listdir(dir_name):
            continue
    submodules_initialized = False
    break

if not submodules_initialized:
    print_error(
        """godot-cpp and/or box2d are not available within this folder, as Git submodules haven't been initialized.
Run the following command to download:

    git submodule update --init --recursive"""
    )
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=["src/"])
sources = [Glob("src/*.cpp"), Glob("src/servers/*.cpp"), Glob("src/spaces/*.cpp"), Glob("src/shapes/*.cpp"), Glob("src/bodies/*.cpp")]

# Add box2d as static library
env.Append(CPPDEFINES=["BOX2D_ENABLE_SIMD"])

# don't use avx2 for compatibility
# if platform.machine() in ["x86_64", "AMD64"]:
#     env.Append(CPPDEFINES=["BOX2D_AVX2"])
#     use_avx2 = True

if env["target"] == "template_release":
    if env["CC"] == "cl":
        env.Append(CCFLAGS=["/O2"])
    else:
        env.Append(CCFLAGS=["-O2"])

if env["CC"] == "cl":
    env.Append(CFLAGS=["/std:c11", "/experimental:c11atomics"])

if env["CC"] in ["mingw", "clang"]:
    env.Append(CFLAGS=["-ffp-contract=off"])

if env["CC"] == "emcc":
    env.Append(CFLAGS=["-msimd128", "-msse2"])

env.Append(CPPPATH=["box2d/include/"])
box2d_lib = env.StaticLibrary("box2d", Glob("box2d/src/*.c"))
env.Append(LIBS=[box2d_lib])

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData(
            "src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml")
        )
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])
filepath = ""

if env["platform"] == "macos" or env["platform"] == "ios":
    filepath = "{}.framework/".format(env["platform"])
    file = "{}.{}.{}".format(libname, env["platform"], env["target"])

libraryfile = "bin/{}/{}{}".format(env["platform"], filepath, file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

copy = env.InstallAs(
    "{}/bin/{}/{}lib{}".format(projectdir, env["platform"], filepath, file), library
)

default_args = [library, copy]
if localEnv.get("compiledb", False):
    default_args += [compilation_db]
Default(*default_args)
