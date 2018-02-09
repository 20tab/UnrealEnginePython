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
    //private string pythonHome = "/usr/local/include/python3.6;/usr/local/lib/libpython3.6.so"


    private string[] windowsKnownPaths =
    {
        "C:/Program Files/Python36",
        "C:/Program Files/Python35",
        "C:/Python27",
        "C:/IntelPython35"
    };

    private string[] macKnownPaths =
    {
        "/Library/Frameworks/Python.framework/Versions/3.6",
        "/Library/Frameworks/Python.framework/Versions/3.5",
        "/Library/Frameworks/Python.framework/Versions/2.7",
        "/System/Library/Frameworks/Python.framework/Versions/3.6",
        "/System/Library/Frameworks/Python.framework/Versions/3.5",
        "/System/Library/Frameworks/Python.framework/Versions/2.7"
    };

    private string[] linuxKnownIncludesPaths =
    {
        "/usr/local/include/python3.6",
        "/usr/local/include/python3.6m",
        "/usr/local/include/python3.5",
        "/usr/local/include/python3.5m",
        "/usr/local/include/python2.7",
        "/usr/include/python3.6",
        "/usr/include/python3.6m",
        "/usr/include/python3.5",
        "/usr/include/python3.5m",
        "/usr/include/python2.7",
    };

    private string[] linuxKnownLibsPaths =
    {
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

        PublicIncludePaths.AddRange(
            new string[] {
                "UnrealEnginePython/Public",
				// ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "UnrealEnginePython/Private",
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
                "Foliage"
				// ... add private dependencies that you statically link with here ...
			}
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
            );


        if (UEBuildConfiguration.bBuildEditor)
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
                "LandscapeEditor"
            });
        }

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            if (pythonHome == "")
            {
                pythonHome = DiscoverPythonPath(windowsKnownPaths);
                if (pythonHome == "")
                {
                    throw new System.Exception("Unable to find Python installation");
                }
            }
            System.Console.WriteLine("Using Python at: " + pythonHome);
            PublicIncludePaths.Add(pythonHome);
            string libPath = GetWindowsPythonLibFile(pythonHome);
            PublicLibraryPaths.Add(Path.GetDirectoryName(libPath));
            PublicAdditionalLibraries.Add(libPath);

            // copy the dlls into the plugins dlls folder, so they don't have to be on the path
            string dllsDir = Path.Combine(ModuleDirectory, "../../Binaries", Target.Platform == UnrealTargetPlatform.Win32 ? "Win32" : "Win64");
            try
            {
                string[] dllsToCopy =
                {
                    "python3.dll",
                    "python36.dll"
                };
                foreach (string dllToCopy in dllsToCopy)
                {
                    // If the dll exist, make sure to set attributes so they are actually accessible
                    if (File.Exists(Path.Combine(dllsDir, dllToCopy)))
                    {
                        File.SetAttributes(Path.Combine(dllsDir, dllToCopy), FileAttributes.Normal);
                    }

                    File.Copy(Path.Combine(pythonHome, dllToCopy), Path.Combine(dllsDir, dllToCopy), true);
                    File.SetAttributes(Path.Combine(dllsDir, dllToCopy), FileAttributes.Normal);
                }
            }
            catch(System.IO.IOException) { }
            catch(System.UnauthorizedAccessException)
            {
                System.Console.WriteLine("WARNING: Unable to copy python dlls, they are probably in use...");
            }
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            if (pythonHome == "")
            {
                pythonHome = DiscoverPythonPath(macKnownPaths);
                if (pythonHome == "")
                {
                    throw new System.Exception("Unable to find Python installation");
                }
            }
            System.Console.WriteLine("Using Python at: " + pythonHome);
            PublicIncludePaths.Add(pythonHome);
            string libPath = GetMacPythonLibFile(pythonHome);
            PublicLibraryPaths.Add(Path.GetDirectoryName(libPath));
            PublicDelayLoadDLLs.Add(libPath);
            Definitions.Add(string.Format("UNREAL_ENGINE_PYTHON_ON_MAC"));
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
                PublicIncludePaths.Add(includesPath);
                PublicAdditionalLibraries.Add(libsPath);
            }
            else
            {
                string[] items = pythonHome.Split(';');
                PublicIncludePaths.Add(items[0]);
                PublicAdditionalLibraries.Add(items[1]);
            }
            Definitions.Add(string.Format("UNREAL_ENGINE_PYTHON_ON_LINUX"));
        }

        string enableThreads = System.Environment.GetEnvironmentVariable("UEP_ENABLE_THREADS");
        if (!string.IsNullOrEmpty(enableThreads))
        {
            Definitions.Add("UEPY_THREADING");
            System.Console.WriteLine("*** Enabled Python Threads support ***");
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

    private string DiscoverPythonPath(string[] knownPaths)
    {
        // insert the PYTHONHOME content as the first known path
        List<string> paths = new List<string>(knownPaths);
        string environmentPath = System.Environment.GetEnvironmentVariable("PYTHONHOME");
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
        foreach (string path in linuxKnownIncludesPaths)
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
        foreach (string path in linuxKnownLibsPaths)
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
