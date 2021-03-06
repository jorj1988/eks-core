import "../EksBuild" as Eks;

Eks.Library {
  name: "EksCore"
  toRoot: "../../"

  Export {
    Depends { name: "cpp" }
    cpp.includePaths: [ "include" ]
  }

  Properties {
    condition: buildtools.windows && buildtools.debug
    cpp.dynamicLibraries: [ "DbgHelp" ]
  }

}
