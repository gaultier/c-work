make -n | awk -v P="$(pwd)" '
BEGIN { print "[" }
/([A-Za-z_]+\.c)/ {
match($0, /[A-Za-z_]+\.c/);
if (NR != 1) print ",";
print "{\"file\":\"" substr($0, RSTART, RLENGTH) "\",\"command\":\"" $0 "\", \"directory\":\"" P "\"}"
}
END { print "]" }' > compile_commands.json
