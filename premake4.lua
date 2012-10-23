dofile "use_wxwidgets.lua"

-- A solution contains projects, and defines the available configurations
solution "YAC8"
    configurations { "Debug", "Release" }
    --location "build"
    targetdir "bin"

-- A project defines one build target
    project "yac8"
        kind "WindowedApp" --"ConsoleApp"
        language "C++"
        files { "src/**.hh", "src/**.cc" }
        wx_config {Unicode="yes", Version="2.8", Libs="core,aui"}

        configuration { "linux", "gmake" }
            buildoptions { "-std=c++11" }

        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols", "ExtraWarnings" }

        configuration "Release"
            defines { "NDEBUG" }
            flags { "OptimizeSpeed" }
