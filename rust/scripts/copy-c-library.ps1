# Copy the source code of the C library into `rust/ittapi-sys/c-library`. This is a workaround for
# Windows developers that may not be able to use POSIX symlinks. Also see:
# https://github.com/git-for-windows/git/wiki/Symbolic-Links.
$cLibrary = $MyInvocation.MyCommand.Path | Split-Path -Parent | Split-Path -Parent | Split-Path -Parent
$linkLocation = Join-Path -Path $cLibrary -ChildPath "rust" | Join-Path -ChildPath "ittapi-sys" | Join-Path -ChildPath "c-library"
Remove-Item $linkLocation -Force -Recurse
robocopy $cLibrary $linkLocation /e /xd ".git" "build" "rust"
