const std = @import("std");
const builtin = @import("builtin");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{ .name = "SdlAudioPlayer", .target = target, .optimize = optimize });

    exe.addIncludePath(b.path("src"));
    exe.addIncludePath(b.path("deps/SDL/include"));
    exe.addIncludePath(b.path("deps/SDL_image/include"));
    exe.addIncludePath(b.path("deps/SDL_ttf/include"));

    const extension = if (builtin.target.os.tag == .macos) ".dylib" else ".so";
    const libSdlPath = "deps/SDL/build/libSDL3" ++ extension;
    const libSdlImgPath = "deps/SDL_image/build/libSDL3_image" ++ extension;
    const libSdlTtfPath = "deps/SDL_ttf/build/libSDL3_ttf" ++ extension;

    exe.addObjectFile(b.path(libSdlPath));
    exe.addObjectFile(b.path(libSdlImgPath));
    exe.addObjectFile(b.path(libSdlTtfPath));

    var sources = std.ArrayList([]const u8).init(b.allocator);
    {
        var dir = try std.fs.cwd().openDir("src", .{ .iterate = true });
        var walker = try dir.walk(b.allocator);
        defer walker.deinit();
        const allowed_exts = [_][]const u8{ ".c", ".cpp", ".cxx", ".c++", ".cc" };
        while (try walker.next()) |entry| {
            const ext = std.fs.path.extension(entry.basename);
            const include_file = for (allowed_exts) |e| {
                if (std.mem.eql(u8, ext, e))
                    break true;
            } else false;
            if (include_file) {
                try sources.append(b.dupe(entry.path));
            }
        }
    }
    exe.addCSourceFiles(.{ .root = b.path("src"), .files = sources.items, .flags = &.{} });
    exe.linkLibC();
    b.installArtifact(exe);
    b.installLibFile(libSdlPath, "../bin/libs/libSDL3.0" ++ extension);
    b.installLibFile(libSdlImgPath, "../bin/libs/libSDL3_image.0" ++ extension);
    b.installLibFile(libSdlTtfPath, "../bin/libs/libSDL3_ttf.0" ++ extension);
    exe.root_module.addRPathSpecial("@executable_path/libs");
}
