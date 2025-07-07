#!/bin/bash

S21_CAT="./s21_cat"
CAT="/bin/cat"

TEST_FILE="test_file.txt"

echo -e "Line 1 of the file\nLine 2 of the file\n\nLine 3\nLine 4" > $TEST_FILE

test_flag() {
    FLAG=$1

    echo "Testing flag: $FLAG with file: $TEST_FILE"

    $S21_CAT $FLAG $TEST_FILE > s21_cat_result.txt
    $CAT $FLAG $TEST_FILE > cat_result.txt

    if diff -q s21_cat_result.txt cat_result.txt > /dev/null; then
        echo "PASS: $FLAG"
    else
        echo "FAIL: $FLAG"
        echo "Diff:"
        diff s21_cat_result.txt cat_result.txt
    fi
}

SHORT_FLAGS="-b -n -e -s -t -v"

for FLAG in $SHORT_FLAGS; do
    test_flag "$FLAG"
done

rm -f s21_cat_result.txt cat_result.txt $TEST_FILE
