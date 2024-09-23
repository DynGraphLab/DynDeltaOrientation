def _gurobi_impl(repo_ctx):
    gurobi_home = repo_ctx.os.environ.get("GUROBI_HOME", "")
    repo_ctx.symlink(
        gurobi_home or "/opt/gurobi951/linux64",
        "gurobi-distro",
    )

    # Emit the generated BUILD.bazel file.
    repo_ctx.template(
        "BUILD",
        Label("//utils/workspace/gurobi:" +
              "BUILD.bazel.in"),
        substitutions = {
            "{gurobi_home}": gurobi_home,
        },
        executable = False,
    )

gurobi_home = repository_rule(
    environ = ["GUROBI_HOME"],
    local = True,
    implementation = _gurobi_impl,
)
