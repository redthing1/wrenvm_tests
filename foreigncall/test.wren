import "builtin" for Test

System.print("foreign call test")

class ProgramMeta {
    construct new() { }

    name { "foreign call test" }
    platforms { ["windows", "linux", "macos"] }
    hashes {{
        "windows" : [
            "hash1",
            "hash2"
        ],
        "linux" : [
            "hash1",
            "hash2"
        ],
        "macos" : [
            "hash1",
            "hash2"
        ]
    }}
}

class Script {
    static run() {
        System.print("script is now running")

        return Test.cool_func(ProgramMeta.new())
    }
}
