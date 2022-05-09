// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;
using System.Collections.Generic;

public class UnrealEnginePython : ModuleRules
{

    // leave this string as empty for triggering auto-discovery of python installations...
    private string pythonHome = "";
    // otherwise specify the path of your python installation
    //private string pythonHome = "C:/Program Files/Python36";
    // this is an example for Homebrew on Mac
    //private string pythonHome = "/usr/local/Cellar/python3/3.6.0/Frameworks/Python.framework/Versions/3.6/";
    // on Linux an include;libs syntax is expected:
    //private string pythonHome = "/usr/local/include/python3.6;/usr/local/lib/libpython3.6.so";

    private string[] windowsKnownPaths =
    {
       // "C:/Program Files/Python37",
        "C:/Program Files/Python36",
        "C:/Program Files/Python35",
        "C:/Python27",
        "C:/IntelPython35"
    };

    private string[] macKnownPaths =
    {
        "/Library/Frameworks/Python.framework/Versions/3.7",
        "/Library/Frameworks/Python.framework/Versions/3.6",
        "/Library/Frameworks/Python.framework/Versions/3.5",
        "/Library/Frameworks/Python.framework/Versions/2.7",
        "/System/Library/Frameworks/Python.framework/Versions/3.7",
        "/System/Library/Frameworks/Python.framework/Versions/3.6",
        "/System/Library/Frameworks/Python.framework/Versions/3.5",
        "/System/Library/Frameworks/Python.framework/Versions/2.7"
    };

    private string[] linuxKnownIncludesPaths =
    {
        "/usr/local/include/python3.8",
        "/usr/local/include/python3.8m",
        "/usr/local/include/python3.7",
        "/usr/local/include/python3.7m",
        "/usr/local/include/python3.6",
        "/usr/local/include/python3.6m",
        "/usr/local/include/python3.5",
        "/usr/local/include/python3.5m",
        "/usr/local/include/python2.7",
        "/usr/include/python3.8",
        "/usr/include/python3.8m",
        "/usr/include/python3.7",
        "/usr/include/python3.7m",
        "/usr/include/python3.6",
        "/usr/include/python3.6m",
        "/usr/include/python3.5",
        "/usr/include/python3.5m",
        "/usr/include/python2.7",
    };

    private string[] linuxKnownLibsPaths =
    {
        "/usr/local/lib/libpython3.8.so",
        "/usr/local/lib/libpython3.8m.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.8.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.8m.so",
        "/usr/local/lib/libpython3.7.so",
        "/usr/local/lib/libpython3.7m.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.7.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.7m.so",
        "/usr/local/lib/libpython3.6.so",
        "/usr/local/lib/libpython3.6m.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.6.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.6m.so",
        "/usr/local/lib/libpython3.5.so",
        "/usr/local/lib/libpython3.5m.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.5.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython3.5m.so",
        "/usr/local/lib/libpython2.7.so",
        "/usr/local/lib/x86_64-linux-gnu/libpython2.7.so",
        "/usr/lib/libpython3.8.so",
        "/usr/lib/libpython3.8m.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.8.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.8m.so",
        "/usr/lib/libpython3.7.so",
        "/usr/lib/libpython3.7m.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.7.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.7m.so",
        "/usr/lib/libpython3.6.so",
        "/usr/lib/libpython3.6m.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.6.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.6m.so",
        "/usr/lib/libpython3.5.so",
        "/usr/lib/libpython3.5m.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.5.so",
        "/usr/lib/x86_64-linux-gnu/libpython3.5m.so",
        "/usr/lib/libpython2.7.so",
        "/usr/lib/x86_64-linux-gnu/libpython2.7.so",
    };

#if WITH_FORWARDED_MODULE_RULES_CTOR
    public UnrealEnginePython(ReadOnlyTargetRules Target) : base(Target)
#else
    public UnrealEnginePython(TargetInfo Target)
#endif
    {

        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDefinitions.Add("WITH_UNREALENGINEPYTHON=1"); // fixed
        string PluginDirectory = Path.Combine(ModuleDirectory, "..", "..");

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Sockets",
                "Networking"
				// ... add other public dependencies that you statically link with here ...
			}
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "MovieScene",
                "LevelSequence",
                "HTTP",
                "UMG",
                "AppFramework",
                "RHI",
                "Voice",
                "RenderCore",
                "MovieSceneCapture",
                "Landscape",
                "Foliage",
                "AIModule",
                "ApplicationCore"
				// ... add private dependencies that you statically link with here ...
			}
            );

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("PythonScriptPlugin");
        }

#if WITH_FORWARDED_MODULE_RULES_CTOR
        BuildVersion Version;
        if (BuildVersion.TryRead(BuildVersion.GetDefaultFileName(), out Version))
        {
            if (Version.MinorVersion >= 18)
            {
                PrivateDependencyModuleNames.Add("ApplicationCore");
            }
        }
#endif


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );

#if WITH_FORWARDED_MODULE_RULES_CTOR
        if (Target.bBuildEditor)
#else
        if (UEBuildConfiguration.bBuildEditor)
#endif
        {
            PrivateDependencyModuleNames.AddRange(new string[]{
                "UnrealEd",
                "LevelEditor",
                "BlueprintGraph",
                "Projects",
                "Sequencer",
                "SequencerWidgets",
                "AssetTools",
                "LevelSequenceEditor",
                "MovieSceneTools",
                "MovieSceneTracks",
                "CinematicCamera",
                "EditorStyle",
                "GraphEditor",
                "UMGEditor",
                "AIGraph",
                "RawMesh",
                "DesktopWidgets",
                "EditorWidgets",
                "FBX",
                "Persona",
                "PropertyEditor",
                "LandscapeEditor",
                "MaterialEditor"
            });
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibPath = Path.Combine(Target.UEThirdPartySourceDirectory, "Python3/Win64/libs/python39.lib");
            PublicSystemLibraryPaths.Add(Path.GetDirectoryName(LibPath));
            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "Python3"
                }
            );
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            string PythonHome = Path.Combine(Target.UEThirdPartySourceDirectory, "Python3/Mac/include");
            PublicIncludePaths.Add(PythonHome);
            string LibPath = Path.Combine(Target.UEThirdPartyBinariesDirectory, "Python3/Mac/lib/libpython3.9.dylib");
            PublicDelayLoadDLLs.Add(LibPath);
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            if (pythonHome == "")
            {
                string includesPath = DiscoverLinuxPythonIncludesPath();
                if (includesPath == null)
                {
                    throw new System.Exception("Unable to find Python includes, please add a search path to linuxKnownIncludesPaths");
                }
                string libsPath = DiscoverLinuxPythonLibsPath();
                if (libsPath == null)
                {
                    throw new System.Exception("Unable to find Python libs, please add a search path to linuxKnownLibsPaths");
                }
                PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Include"));
                PublicIncludePaths.Add(includesPath);
                PublicAdditionalLibraries.Add(libsPath);

            }
            else
            {
                string[] items = pythonHome.Split(';');
                PublicIncludePaths.Add(items[0]);
                PublicAdditionalLibraries.Add(items[1]);
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Android)
		{
            string PythonAndroidLibsDirectory = Path.Combine(PluginDirectory, "android");

            string includePath = Path.Combine(PythonAndroidLibsDirectory, "include", "python3.7m");
            PublicIncludePaths.Add(includePath);

            PublicSystemLibraryPaths.AddRange(
                new string[] {
                    Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a")
                });

            string[] libraryPaths = new string[] {
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libbz2.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libcrypto.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libcrypto1.1.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libffi.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "liblzma.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libpython3.7m.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "librubicon.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libsqlite3.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libssl.so"),
                Path.Combine(PythonAndroidLibsDirectory, "libs", "arm64-v8a", "libssl1.1.so"),
            };

            foreach (string libraryPath in libraryPaths)
            {
                PublicAdditionalLibraries.Add(libraryPath);
            }
            
            string APLName = "UnrealEnginePython_APL.xml";
            string RelAPLPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(RelAPLPath, APLName));
    
        }
        else if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            string PythonIOSLibsDirectory = Path.Combine(PluginDirectory, "ios");

            string includePath = Path.Combine(PythonIOSLibsDirectory, "Python", "Headers");
            PublicIncludePaths.Add(includePath);

            PublicSystemLibraryPaths.AddRange(
                new string[] {
                    Path.Combine(PythonIOSLibsDirectory, "BZip2"),
                    Path.Combine(PythonIOSLibsDirectory, "OpenSSL"),
                    Path.Combine(PythonIOSLibsDirectory, "Python"),
                    Path.Combine(PythonIOSLibsDirectory, "XZ"),
                    Path.Combine(PythonIOSLibsDirectory, "Sqlite3"),
                });

            string[] libraryPaths = new string[] {
                Path.Combine(PythonIOSLibsDirectory, "BZip2", "libbzip2.a"),
                Path.Combine(PythonIOSLibsDirectory, "OpenSSL", "libOpenSSL.a"),
                Path.Combine(PythonIOSLibsDirectory, "Python", "libPython.a"),
                Path.Combine(PythonIOSLibsDirectory, "XZ", "libxz.a"),
                Path.Combine(PythonIOSLibsDirectory, "Sqlite3", "libsqlite3.tbd"),
            };

            foreach (string libraryPath in libraryPaths)
            {
                PublicAdditionalLibraries.Add(libraryPath);
            }
        }

    }

    private bool IsPathRelative(string Path)
    {
        bool IsRooted = Path.StartsWith("\\", System.StringComparison.Ordinal) || // Root of the current directory on Windows. Also covers "\\" for UNC or "network" paths.
                        Path.StartsWith("/", System.StringComparison.Ordinal) ||  // Root of the current directory on Windows, root on UNIX-likes.
                                                                                  // Also covers "\\", considering normalization replaces "\\" with "//".
                        (Path.Length >= 2 && char.IsLetter(Path[0]) && Path[1] == ':'); // Starts with "<DriveLetter>:"
        return !IsRooted;
    }

    private string DiscoverPythonPath(string[] knownPaths, string binaryPath)
    {
        // insert the PYTHONHOME content as the first known path
        List<string> paths = new List<string>(knownPaths);
        paths.Insert(0, Path.Combine(ModuleDirectory, "../../Binaries", binaryPath));
        string environmentPath = System.Environment.GetEnvironmentVariable("PYTHONHOME");
        if (!string.IsNullOrEmpty(environmentPath))
            paths.Insert(0, environmentPath);

        // look in an alternate custom location
        environmentPath = System.Environment.GetEnvironmentVariable("UNREALENGINEPYTHONHOME");
        if (!string.IsNullOrEmpty(environmentPath))
            paths.Insert(0, environmentPath);

        foreach (string path in paths)
        {
            string actualPath = path;

            if (IsPathRelative(actualPath))
            {
                actualPath = Path.GetFullPath(Path.Combine(ModuleDirectory, actualPath));
            }

            string headerFile = Path.Combine(actualPath, "include", "Python.h");
            if (File.Exists(headerFile))
            {
                return actualPath;
            }
            // this is mainly useful for OSX
            headerFile = Path.Combine(actualPath, "Headers", "Python.h");
            if (File.Exists(headerFile))
            {
                return actualPath;
            }
        }
        return "";
    }

    private string DiscoverLinuxPythonIncludesPath()
    {
        List<string> paths = new List<string>(linuxKnownIncludesPaths);
        paths.Insert(0, Path.Combine(ModuleDirectory, "../../Binaries", "Linux", "include"));
        foreach (string path in paths)
        {
            string headerFile = Path.Combine(path, "Python.h");
            if (File.Exists(headerFile))
            {
                return path;
            }
        }
        return null;
    }

    private string DiscoverLinuxPythonLibsPath()
    {
        List<string> paths = new List<string>(linuxKnownLibsPaths);
        paths.Insert(0, Path.Combine(ModuleDirectory, "../../Binaries", "Linux", "lib"));
        paths.Insert(0, Path.Combine(ModuleDirectory, "../../Binaries", "Linux", "lib64"));
        foreach (string path in paths)
        {
            if (File.Exists(path))
            {
                return path;
            }
        }
        return null;
    }

    private string GetMacPythonLibFile(string basePath)
    {
        // first try with python3
        for (int i = 9; i >= 0; i--)
        {
            string fileName = string.Format("libpython3.{0}.dylib", i);
            string fullPath = Path.Combine(basePath, "lib", fileName);
            if (File.Exists(fullPath))
            {
                return fullPath;
            }
            fileName = string.Format("libpython3.{0}m.dylib", i);
            fullPath = Path.Combine(basePath, "lib", fileName);
            if (File.Exists(fullPath))
            {
                return fullPath;
            }
        }

        // then python2
        for (int i = 9; i >= 0; i--)
        {
            string fileName = string.Format("libpython2.{0}.dylib", i);
            string fullPath = Path.Combine(basePath, "lib", fileName);
            if (File.Exists(fullPath))
            {
                return fullPath;
            }
            fileName = string.Format("libpython2.{0}m.dylib", i);
            fullPath = Path.Combine(basePath, "lib", fileName);
            if (File.Exists(fullPath))
            {
                return fullPath;
            }
        }

        throw new System.Exception("Invalid Python installation, missing .dylib files");
    }

    private string GetWindowsPythonLibFile(string basePath)
    {
        // just for usability, report if the pythonHome is not in the system path
        string[] allPaths = System.Environment.GetEnvironmentVariable("PATH").Split(';');
        // this will transform the slashes in backslashes...
        string checkedPath = Path.GetFullPath(basePath);
        if (checkedPath.EndsWith("\\"))
        {
            checkedPath = checkedPath.Remove(checkedPath.Length - 1);
        }
        bool found = false;
        foreach (string item in allPaths)
        {
            if (item == checkedPath || item == checkedPath + "\\")
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            System.Console.WriteLine("[WARNING] Your Python installation is not in the system PATH environment variable.");
            System.Console.WriteLine("[WARNING] Ensure your python paths are set in GlobalConfig (DefaultEngine.ini) so the path can be corrected at runtime.");
        }
        // first try with python3
        for (int i = 9; i >= 0; i--)
        {
            string fileName = string.Format("python3{0}.lib", i);
            string fullPath = Path.Combine(basePath, "libs", fileName);
            if (File.Exists(fullPath))
            {
                return fullPath;
            }
        }

        // then python2
        for (int i = 9; i >= 0; i--)
        {
            string fileName = string.Format("python2{0}.lib", i);
            string fullPath = Path.Combine(basePath, "libs", fileName);
            if (File.Exists(fullPath))
            {
                return fullPath;
            }
        }

        throw new System.Exception("Invalid Python installation, missing .lib files");
    }
}
