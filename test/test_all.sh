#!/bin/sh

AMIS="../src/amis $*"
DIFF="perl model_diff.pl 5"

ALGORITHMS="GIS GISMAP BFGS BFGSMAP LMVM LMVMMAP BLMVMBC BLMVMBCMAP OWBFGSBC OWBFGSBCMAP"
ALGORITHMS_AP="GIS GISMAP BFGS BFGSMAP LMVM LMVMMAP BLMVMBC BLMVMBCMAP OWBFGSBC OWBFGSBCMAP AP"

TEST_BINARY="simple combine pack variable multifile reference refapprox tree packtree forest reftree fix crf crf_amis crf_tree"
TEST_NONBINARY="$TEST_BINARY nonbinary"

######################################################################

num_tests=0
num_success=0
num_fail=0
num_fail_threshold=10

exec_test() {
    feature=$1
    shift
    test_file_list=$1
    shift
    algo_list=$1
    shift
    additional_args=$*
    for test_file in $test_file_list ; do
	for algo in $algo_list ; do
	    num_tests=`expr $num_tests + 1`


	    command="$AMIS -f $feature -a $algo $additional_args $test_file.conf"
#	    command="$AMIS -f $feature -o $test_file.$algo.test -a $algo $additional_args $test_file.conf"


	    test_spec="$test_file $feature $algo"
	    echo $command
	    if $command > /dev/null 2>&1
		then
		if $DIFF $test_file.output $test_file.$algo.test > /dev/null 2>&1
		    then
		    echo "$test_spec $additional_args: SUCCESS"
		    num_success=`expr $num_success + 1`
		else
		    echo "$test_spec $additional_args: FAIL"
		    num_fail=`expr $num_fail + 1`
		fi
	    else
		echo "FAILED: $command"
		exit 1
	    fi
	done
    done
}

exec_test binary "$TEST_BINARY" "$ALGORITHMS"
exec_test binary "$TEST_BINARY" "$ALGORITHMS_AP" --parameter-type lambda
exec_test binary "$TEST_BINARY" "$ALGORITHMS" --event-on-file true
exec_test integer "$TEST_NONBINARY" "$ALGORITHMS"
exec_test integer "$TEST_NONBINARY" "$ALGORITHMS_AP" --parameter-type lambda
exec_test integer "$TEST_NONBINARY" "$ALGORITHMS" --event-on-file true
exec_test real "$TEST_NONBINARY" "$ALGORITHMS"
exec_test real "$TEST_NONBINARY" "$ALGORITHMS_AP" --parameter-type lambda
exec_test real "$TEST_NONBINARY" "$ALGORITHMS" --event-on-file true

echo "$num_success tests passed."
if [ $num_fail -gt 0 ]; then
    echo "$num_fail tests failed."
    if [ $num_fail -gt $num_fail_threshold ]; then
	exit 1
    fi
fi

