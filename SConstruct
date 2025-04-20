#!/usr/bin/env python
import os
import sys

from methods import print_error


libname = "godot-box2c"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Add(BoolVariable("tracy_enabled", "Enable tracy profiler", False))
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

if not (os.path.isdir("box2d") and os.listdir("box2d")):
    print_error("""box2d is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download box2d:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})


env.Append(CPPPATH=["src/"])
sources = [
    Glob("src/*.cpp"),
    Glob("src/servers/*.cpp"),
    Glob("src/spaces/*.cpp"),
    Glob("src/shapes/*.cpp"),
    Glob("src/bodies/*.cpp"),
    Glob("src/joints/*.cpp")
]

# Add box2d as static library
if env["CC"] == "cl":
    env.Append(CFLAGS=["/std:c17"])

if env["CC"].endswith("gcc") or env["CC"].endswith("clang"):
    env.Append(CFLAGS=["-ffp-contract=off"])

if env["CC"] == "emcc":
    env.Append(CFLAGS=["-msimd128", "-msse2"])

env.Append(CPPPATH=["box2d/include/"])
box2d_lib = env.StaticLibrary("box2d", Glob("box2d/src/*.c"))
env.Append(LIBS=[box2d_lib])


if env["target"] in ["editor", "template_debug"]:
    # Tracy
    if env["tracy_enabled"]:
        env.Append(CPPDEFINES=["TRACY_ENABLE", "TRACY_ON_DEMAND", "TRACY_DELAYED_INIT", "TRACY_MANUAL_LIFETIME"])
        sources.append("tracy/public/TracyClient.cpp")

    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)