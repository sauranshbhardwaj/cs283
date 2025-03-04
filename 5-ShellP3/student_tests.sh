#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check simple pipe - ls | wc" {
    run ./dsh <<EOF                
ls | wc -l
EOF
    # Assertions
    [ "$status" -eq 0 ]
}

@test "Check built-in command 'exit'" {
    run ./dsh <<EOF                
exit
EOF
    # Verify output contains 'exiting...'
    [[ "$output" == *"exiting..."* ]]
    [ "$status" -eq 0 ]
}

@test "Check built-in command 'cd'" {
    run ./dsh <<EOF                
cd /tmp
pwd
EOF
    # Verify we changed directory to /tmp
    [[ "$output" == *"/tmp"* ]]
    [ "$status" -eq 0 ]
}

@test "Check triple pipe - ls | grep .c | wc -l" {
    run ./dsh <<EOF                
ls | grep ".c" | wc -l
EOF
    # Assertions - just check it runs
    [ "$status" -eq 0 ]
}

@test "Check filtering with pipes - ls | grep dshlib" {
    run ./dsh <<EOF                
ls | grep dshlib
EOF
    # Verify output contains dshlib.c and dshlib.h
    [[ "$output" == *"dshlib.c"* ]] || [[ "$output" == *"dshlib.h"* ]]
    [ "$status" -eq 0 ]
}

@test "Check error handling for command not found" {
    run ./dsh <<EOF                
nonexistentcommand
EOF
    # Should return 0 for the shell itself, even if the command fails
    [ "$status" -eq 0 ]
}

@test "Check max commands limit" {
    # Create a pipeline with CMD_MAX+1 commands (assuming CMD_MAX is 8)
    run ./dsh <<EOF                
ls | grep a | grep b | grep c | grep d | grep e | grep f | grep g | grep h | grep i
EOF
    # Verify that error message is printed
    [[ "$output" == *"error: piping limited to"* ]]
    [ "$status" -eq 0 ]
}

# Extra credit tests for redirection

@test "Check output redirection with > operator" {
    run ./dsh <<EOF                
echo "hello, world" > test_output.txt
cat test_output.txt
rm test_output.txt
EOF
    # Verify output contains the text that was redirected
    [[ "$output" == *"hello, world"* ]]
    [ "$status" -eq 0 ]
}

@test "Check input redirection with < operator" {
    run ./dsh <<EOF
echo "test input data" > test_input.txt
cat < test_input.txt
rm test_input.txt
EOF
    # Verify output contains the text that was redirected
    [[ "$output" == *"test input data"* ]]
    [ "$status" -eq 0 ]
}

@test "Check output redirection with append >> operator" {
    run ./dsh <<EOF
echo "line 1" > test_append.txt
echo "line 2" >> test_append.txt
cat test_append.txt
rm test_append.txt
EOF
    # Verify both lines are present (line 1 wasn't overwritten)
    [[ "$output" == *"line 1"* ]]
    [[ "$output" == *"line 2"* ]]
    [ "$status" -eq 0 ]
}

@test "Check redirection with pipes" {
    run ./dsh <<EOF
ls | grep ".c" > test_pipe_redirect.txt
cat test_pipe_redirect.txt
rm test_pipe_redirect.txt
EOF
    # Verify we got some .c files
    [[ "$output" == *".c"* ]]
    [ "$status" -eq 0 ]
}