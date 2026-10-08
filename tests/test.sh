#!/bin/sh

reussi=0
rate=0

echo "Test Makefile-Variables:"
echo ""
for i in $(seq 1 11); do
    ./minimake -f tests/Makefile-Variables/Makefile$i >tests/Makefile-Variables/output$i.txt 2>&1
    if diff -u tests/Makefile-Variables/expected$i.txt tests/Makefile-Variables/output$i.txt >/dev/null; then
        echo "Test $i Variables : reussi"
        reussi=$((reussi+1))
    else
        echo ""
        echo "Difference:"
        diff -u tests/Makefile-Variables/expected$i.txt \
            tests/Makefile-Variables/output$i.txt | grep -vE '^(\+\+\+|---|@@ )'
        echo "Test $i Variables : echoue"
        rate=$((rate+1))
    fi
done

echo ""
echo "Test Makefile-Rules:"
echo ""

for i in $(seq 1 7); do
    ./minimake -f tests/Makefile-Rules/Makefile$i >tests/Makefile-Rules/output$i.txt 2>&1
    if diff -u tests/Makefile-Rules/expected$i.txt tests/Makefile-Rules/output$i.txt >/dev/null; then
        echo "Test $i Rules : reussi"
        reussi=$((reussi+1))
    else
        echo ""
        echo "Difference:"
        diff -u tests/Makefile-Rules/expected$i.txt \
            tests/Makefile-Rules/output$i.txt | grep -vE '^(\+\+\+|---|@@ )'
        echo "Test $i Rules : echoue"
        rate=$((rate+1))
    fi
done

echo ""
echo "Test Makefile-Variables-Special:"
echo ""

for i in $(seq 1 5); do
    ./minimake -f  tests/Makefile-Variables-Special/Makefile$i > tests/Makefile-Variables-Special/output$i.txt 2>&1
    if diff -u  tests/Makefile-Variables-Special/expected$i.txt  tests/Makefile-Variables-Special/output$i.txt >/dev/null; then
        echo "Test $i Variables-Special : reussi"
        reussi=$((reussi+1))
    else
        echo ""
        echo "Difference:"
        diff -u   tests/Makefile-Variables-Special/expected$i.txt  tests/Makefile-Variables-Special/output$i.txt | grep -vE '^(\+\+\+|---|@@ )'
        echo "Test $i Variables-Special : echoue"
        rate=$((rate+1))
    fi
done


echo "Test Makefile-Excution:"
echo ""
for i in $(seq 1 7); do
    ./minimake -f tests/Makefile-Excution/Makefile$i >tests/Makefile-Excution/output$i.txt 2>&1
    if diff -u tests/Makefile-Excution/expected$i.txt tests/Makefile-Excution/output$i.txt >/dev/null; then
        echo "Test $i Execution : reussi"
        reussi=$((reussi+1))
    else
        echo ""
        echo "Difference:"
        diff -u tests/Makefile-Excution/expected$i.txt \
            tests/Makefile-Excution/output$i.txt | grep -vE '^(\+\+\+|---|@@ )'
        echo "Test $i Execution : echoue"
        rate=$((rate+1))
    fi
done


rm tests/Makefile-Variables/output*
rm tests/Makefile-Rules/output*
rm tests/Makefile-Variables-Special/output*
rm tests/Makefile-Excution/output*

echo ""
echo ""
echo "Test reussi : $reussi"
echo "Test rate : $rate"
echo "$((rate + reussi))"
