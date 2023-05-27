const std = @import("std");

pub fn addCSource(exe: *std.build.CompileStep) void {
    exe.addCSourceFiles(
        &.{
            "src/globalvars.c",
            "src/bound.c",
            "src/readparam.c",
            "src/main.c",
            "src/nrsrc/nrutil.c",
            "src/save.c",
            "src/load.c",
        },
        &.{
            "-Wall",
            "-W",
        },
    );
    exe.linkLibC();
}

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "MoreBound",
        .target = target,
        .optimize = optimize,
    });
    addCSource(exe);

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    const install_step = b.getInstallStep();
    run_cmd.step.dependOn(install_step);

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    // const run_step = b.step("run", "Run the app");
    // run_step.dependOn(&run_cmd.step);

    const test_cmd = b.addSystemCommand(&.{ "time", "./zig-out/bin/MoreBound", "snapshot_720", "bndparam.txt" });
    test_cmd.step.dependOn(install_step);

    const diff_cmd = b.addSystemCommand(&.{ "diff", "snapshot_720_b.txt", "test_720.backup.txt" });
    diff_cmd.step.dependOn(&test_cmd.step);

    const test_step = b.step("test", "Run test");
    test_step.dependOn(&diff_cmd.step);
}
