#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Basic command execution" {
    run ./dsh <<EOF
echo "hello world"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "hello world" ]]
}

@test "cd command changes directory" {
    run ./dsh <<EOF
cd /tmp
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "/tmp" ]]
}

@test "cd with no arguments stays in same directory" {
    current=$(pwd)
    run ./dsh <<EOF
cd
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "${current}" ]]
}

@test "Command with multiple arguments" {
    run ./dsh <<EOF
echo arg1 arg2 arg3
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "arg1 arg2 arg3" ]]
}

@test "Quoted arguments preserved" {
    run ./dsh <<EOF
echo "hello    world"
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "hello    world" ]]
}

@test "Command not found handling" {
    run ./dsh <<EOF
nonexistentcommand
rc
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "Command not found in PATH" ]]
    [[ "${output}" =~ "2" ]]  # ENOENT is usually 2
}

@test "Permission denied handling" {
    touch testfile
    chmod 000 testfile
    run ./dsh <<EOF
./testfile
rc
exit
EOF
    rm -f testfile
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "Permission denied" ]]
    [[ "${output}" =~ "13" ]]  # EACCES is usually 13
}

@test "Exit command works" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Return code command shows last exit status" {
    run ./dsh <<EOF
true
rc
false
rc
exit
EOF
    [ "$status" -eq 0 ]
    [[ "${output}" =~ "0" ]]  # true returns 0
    [[ "${output}" =~ "1" ]]  # false returns 1
}