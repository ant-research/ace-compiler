# The artifact is distributed without a .git directory, so tolerate the
# absence of a git repository instead of printing a fatal error.
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE AIR_GIT_COMMIT
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT AIR_GIT_COMMIT)
  set (AIR_GIT_COMMIT "unknown")
endif ()

# file(WRITE "${CMAKE_BINARY_DIR}/git_commit.txt" "Commit: ${AIR_GIT_COMMIT}\n")