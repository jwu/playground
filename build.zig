const std = @import("std");
const rl = @import("raylib");

const src_dir = "src/bin";
const default_example = "core_basic_window";

pub fn build(b: *std.Build) !void {
  if (std.mem.endsWith(u8, b.install_prefix, "zig-out")) {
    b.resolveInstallPrefix(b.pathFromRoot("target"), .{});
  }

  const target = b.standardTargetOptions(.{});
  const optimize = b.standardOptimizeOption(.{});
  const platform = b.option(rl.PlatformBackend, "platform", "raylib platform backend") orelse .glfw;

  const raylib_dep = b.dependency("raylib", .{
    .target = target,
    .optimize = optimize,
    .platform = platform,
  });
  const raylib = raylib_dep.artifact("raylib");
  const cglm_dep = b.dependency("cglm", .{});
  const cglm_include = cglm_dep.path("include");
  const raygui_dep = b.dependency("raygui", .{});
  const raygui_src = raygui_dep.path("src");

  const examples_step = b.step("examples", "Build all examples");
  const run_step = b.step("run", "Run the default example");
  const compile_commands_step = b.step("compile_commands", "Generate compile_commands.json for clangd");

  var db: std.Io.Writer.Allocating = .init(b.allocator);
  const db_writer = &db.writer;
  try db_writer.writeAll("[\n");

  const root = b.pathFromRoot(".");
  const raylib_src = raylib_dep.path("src").getPath2(b, null);

  var found_default_example = false;
  var first_db_entry = true;
  var dir = b.build_root.handle.openDir(b.graph.io, src_dir, .{ .iterate = true }) catch |err| switch (err) {
    error.FileNotFound => return std.log.warn("{s}/ does not exist", .{src_dir}),
    else => return err,
  };
  defer dir.close(b.graph.io);

  var it = dir.iterate();
  while (try it.next(b.graph.io)) |entry| {
    if (entry.kind != .file or !std.mem.eql(u8, std.fs.path.extension(entry.name), ".c")) continue;

    const name = std.fs.path.stem(entry.name);
    const source = b.pathJoin(&.{ src_dir, entry.name });
    const exe = addExample(b, name, source, target, optimize, raylib, cglm_include, raygui_src);

    examples_step.dependOn(&b.addInstallArtifact(exe, .{}).step);

    const run = b.addRunArtifact(exe);
    run.cwd = b.path(".");

    b.step(name, b.fmt("Run {s}", .{name})).dependOn(&run.step);
    if (std.mem.eql(u8, default_example, name)) {
      found_default_example = true;
      run_step.dependOn(&run.step);
    }

    try addCompileCommand(db_writer, root, raylib_src, cglm_include.getPath2(b, null), raygui_src.getPath2(b, null), source, &first_db_entry);
  }

  try db_writer.writeAll("]\n");
  const update_db = b.addUpdateSourceFiles();
  update_db.addBytesToSource(db.written(), "compile_commands.json");
  compile_commands_step.dependOn(&update_db.step);

  if (!found_default_example) {
    std.log.warn("default example '{s}' not found", .{default_example});
  }
}

fn addExample(
  b: *std.Build,
  name: []const u8,
  source: []const u8,
  target: std.Build.ResolvedTarget,
  optimize: std.builtin.OptimizeMode,
  raylib: *std.Build.Step.Compile,
  cglm_include: std.Build.LazyPath,
  raygui_src: std.Build.LazyPath,
) *std.Build.Step.Compile {
  const mod = b.createModule(.{
    .target = target,
    .optimize = optimize,
    .link_libc = true,
  });
  mod.addCMacro("PLATFORM_DESKTOP", "");
  mod.addIncludePath(cglm_include);
  mod.addIncludePath(raygui_src);
  mod.addCSourceFile(.{ .file = b.path(source) });
  mod.linkLibrary(raylib);

  return b.addExecutable(.{
    .name = name,
    .root_module = mod,
    .use_lld = target.result.os.tag == .windows,
  });
}

fn addCompileCommand(
  writer: *std.Io.Writer,
  root: []const u8,
  raylib_src: []const u8,
  cglm_include: []const u8,
  raygui_src: []const u8,
  source: []const u8,
  first: *bool,
) !void {
  if (first.*) {
    first.* = false;
  } else {
    try writer.writeAll(",\n");
  }

  try writer.writeAll("  { \"directory\": ");
  try std.json.Stringify.value(root, .{}, writer);
  try writer.writeAll(", \"file\": ");
  try std.json.Stringify.value(source, .{}, writer);
  try writer.writeAll(", \"arguments\": [\"cc\", \"-std=c99\", \"-DPLATFORM_DESKTOP\", \"-I\", ");
  try std.json.Stringify.value(raylib_src, .{}, writer);
  try writer.writeAll(", \"-I\", ");
  try std.json.Stringify.value(cglm_include, .{}, writer);
  try writer.writeAll(", \"-I\", ");
  try std.json.Stringify.value(raygui_src, .{}, writer);
  try writer.writeAll(", \"-c\", ");
  try std.json.Stringify.value(source, .{}, writer);
  try writer.writeAll("] }");
}
