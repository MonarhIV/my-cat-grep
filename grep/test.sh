#!/bin/bash

GREPCMD="grep"
S21GREPCMD="./s21_grep"

echo -e "Hello world\nThis is a test\nAnother line with Test\nNothing here\nTESTING CASES" > test_file1.txt
echo -e "Test line one\nLine two\nAnother Test case\nNo match here" > test_file2.txt
echo -e "Completely unrelated content\nStill nothing to match\nAbsolutely no tests here" > test_file3.txt
echo -e "pattern\nSome other line\nPattern\nAnother line with pattern" > test_file4.txt
echo -e "pattern in file1\nSome other content in file1" > file1.txt
echo -e "pattern in file2\nSome other content in file2" > file2.txt
echo -e "pattern\nPattern in file3\nOther content in file3" > file3.txt
echo -e "pattern" > patterns.txt

run_test() {
    local description=$1
    local command=$2
    echo "Running: $description"
    echo "$command"
    eval "$command > grep_output.txt"
    eval "${command/$GREPCMD/$S21GREPCMD} > s21grep_output.txt"

    if diff -q grep_output.txt s21grep_output.txt > /dev/null; then
        echo "Test PASSED"
    else
        echo "Test FAILED"
        echo "Diff:"
        diff -u grep_output.txt s21grep_output.txt
        echo "---- Expected (grep) ----"
        cat grep_output.txt
        echo "---- Actual (s21_grep) ----"
        cat s21grep_output.txt
    fi
    echo ""
}

#с одиночными флагами
run_test "Basic search" "$GREPCMD 'test' test_file1.txt"
run_test "Case-insensitive search (-i)" "$GREPCMD -i 'test' test_file1.txt"
run_test "Invert match (-v)" "$GREPCMD -v 'test' test_file1.txt"
run_test "Count matching lines (-c)" "$GREPCMD -c 'test' test_file1.txt"
run_test "Matching files (-l)" "$GREPCMD -l 'test' test_file1.txt test_file2.txt"
run_test "Line numbers (-n)" "$GREPCMD -n 'test' test_file1.txt"
run_test "Only matched part (-o)" "$GREPCMD -o 'test' test_file1.txt"
run_test "Patterns from file (-f)" "$GREPCMD -f patterns.txt test_file1.txt"
run_test "Suppress errors (-s)" "$GREPCMD -s 'nonexistentpattern' test_file1.txt nonexistent_file.txt"

#с комбинированными флагами
run_test "Combined flags (-i -n)" "$GREPCMD -i -n 'test' test_file1.txt"
run_test "Combined flags (-v -c)" "$GREPCMD -v -c 'test' test_file1.txt"
run_test "Combined flags (-l -c)" "$GREPCMD -l -c 'test' test_file1.txt"

#с несколькими паттернами
run_test "Multiple patterns (-e)" "$GREPCMD -e 'test' -e 'Hello' test_file1.txt"
run_test "Patterns from file and multiple files (-f)" "$GREPCMD -f patterns.txt test_file1.txt test_file2.txt"

#с несколькими файлами
run_test "Search in two files" "$GREPCMD 'test' test_file1.txt test_file2.txt"
run_test "Search in three files" "$GREPCMD 'test' test_file1.txt test_file2.txt test_file3.txt"
run_test "Case-insensitive search in multiple files" "$GREPCMD -i 'test' test_file1.txt test_file2.txt"
run_test "Invert match in multiple files" "$GREPCMD -v 'test' test_file1.txt test_file2.txt"
run_test "Count matching lines in multiple files" "$GREPCMD -c 'test' test_file1.txt test_file2.txt"
run_test "Matching files (-l) in multiple files" "$GREPCMD -l 'test' test_file1.txt test_file2.txt test_file3.txt"
run_test "Line numbers in multiple files" "$GREPCMD -n 'test' test_file1.txt test_file2.txt"

rm -f grep_output.txt s21grep_output.txt test_file1.txt test_file2.txt test_file3.txt test_file4.txt file1.txt file2.txt file3.txt patterns.txt

echo "All tests completed."
