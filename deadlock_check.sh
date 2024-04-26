#!/bin/bash
NAME="proj2"
NUM_EXECUTIONS=100

TIMEOUT=10

echo "🚀 starting $NUM_EXECUTIONS executions"

for ((i = 1; i <= NUM_EXECUTIONS; i++)); do
    L=$((RANDOM % 20000 + 1)) # Random number between 1 and 20000
    Z=$((RANDOM % 10 + 1))    # Random number between 1 and 10
    K=$((RANDOM % 91 + 10))   # Random number between 10 and 100
    TL=$((RANDOM % 10001))    # Random number between 0 and 10000
    TB=$((RANDOM % 1001))     # Random number between 0 and 1000

    PROGRAM="./$NAME $L $Z $K $TL $TB"

    echo "😳 run $i $PROGRAM"

    start_time=$(date +%s%3N)
    timeout $TIMEOUT $PROGRAM
    end_time=$(date +%s%3N)
    duration=$((end_time - start_time))
    if [ $? -eq 124 ]; then
        echo "💀 timed out - ran for $TIMEOUT, running: $PROGRAM"
        exit 1
    else
        echo "✅ run $i ok - done in ${duration}ms"
    fi

done

echo "🎉 congrats! $NUM_EXECUTIONS executions done successfully"
