#!/bin/bash
#
# RGW Bucket Object Limits Check Script
#
# This script helps administrators check bucket object counts and
# identify buckets that may benefit from resharding or quota adjustments.
#
# Usage: ./rgw_bucket_limits_check.sh [options]
#   -b <bucket>    Check specific bucket
#   -a             Check all buckets
#   -t <threshold> Set objects-per-shard threshold (default: 100000)
#   -w             Show warnings for buckets approaching limits
#   -h             Show this help message
#

set -e

THRESHOLD=100000
CHECK_ALL=false
SHOW_WARNINGS=false
SPECIFIC_BUCKET=""

# Color codes for output
RED='\033[0;31m'
YELLOW='\033[1;33m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

usage() {
    cat << EOF
Usage: $0 [options]

Options:
    -b <bucket>    Check specific bucket
    -a             Check all buckets
    -t <threshold> Set objects-per-shard threshold (default: 100000)
    -w             Show warnings for buckets approaching limits
    -h             Show this help message

Examples:
    $0 -b mybucket              # Check specific bucket
    $0 -a -w                    # Check all buckets with warnings
    $0 -a -t 50000              # Check all with custom threshold

This script requires radosgw-admin to be available in PATH.
EOF
    exit 1
}

# Parse command line arguments
while getopts "b:at:wh" opt; do
    case $opt in
        b) SPECIFIC_BUCKET="$OPTARG" ;;
        a) CHECK_ALL=true ;;
        t) THRESHOLD="$OPTARG" ;;
        w) SHOW_WARNINGS=true ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Check if radosgw-admin is available
if ! command -v radosgw-admin &> /dev/null; then
    echo -e "${RED}Error: radosgw-admin not found in PATH${NC}"
    exit 1
fi

# Function to analyze a single bucket
analyze_bucket() {
    local bucket="$1"

    echo "Analyzing bucket: $bucket"
    echo "----------------------------------------"

    # Get bucket stats
    local stats=$(radosgw-admin bucket stats --bucket="$bucket" 2>/dev/null)

    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Failed to get stats for bucket $bucket${NC}"
        return 1
    fi

    # Parse JSON output (requires jq if available, otherwise use grep/sed)
    if command -v jq &> /dev/null; then
        local num_objects=$(echo "$stats" | jq -r '.usage."rgw.main".num_objects // 0')
        local num_shards=$(echo "$stats" | jq -r '.num_shards // 0')
        local size=$(echo "$stats" | jq -r '.usage."rgw.main".size // 0')
        local quota_max_objects=$(echo "$stats" | jq -r '.bucket_quota.max_objects // -1')
        local quota_enabled=$(echo "$stats" | jq -r '.bucket_quota.enabled // false')
    else
        # Fallback to grep/sed parsing
        local num_objects=$(echo "$stats" | grep -o '"num_objects":[[:space:]]*[0-9]*' | head -1 | grep -o '[0-9]*$')
        local num_shards=$(echo "$stats" | grep -o '"num_shards":[[:space:]]*[0-9]*' | grep -o '[0-9]*$')
        local size=$(echo "$stats" | grep -o '"size":[[:space:]]*[0-9]*' | head -1 | grep -o '[0-9]*$')
        local quota_max_objects=$(echo "$stats" | grep -o '"max_objects":[[:space:]]*-\?[0-9]*' | head -1 | grep -o '-\?[0-9]*$')
        local quota_enabled=$(echo "$stats" | grep -o '"enabled":[[:space:]]*\(true\|false\)' | head -1 | grep -o '\(true\|false\)$')

        # Set defaults if parsing failed
        num_objects=${num_objects:-0}
        num_shards=${num_shards:-0}
        size=${size:-0}
        quota_max_objects=${quota_max_objects:--1}
        quota_enabled=${quota_enabled:-false}
    fi

    # Calculate objects per shard
    local objs_per_shard=0
    if [ "$num_shards" -gt 0 ]; then
        objs_per_shard=$((num_objects / num_shards))
    else
        objs_per_shard=$num_objects
    fi

    # Convert size to human readable
    local size_human=$(numfmt --to=iec-i --suffix=B "$size" 2>/dev/null || echo "${size} bytes")

    echo "  Total Objects: $num_objects"
    echo "  Total Size: $size_human"
    echo "  Number of Shards: $num_shards"
    echo "  Objects per Shard: $objs_per_shard"

    # Check quota
    if [ "$quota_enabled" = "true" ]; then
        echo "  Quota Enabled: Yes"
        if [ "$quota_max_objects" -gt 0 ]; then
            echo "  Quota Max Objects: $quota_max_objects"
            local quota_usage=$((num_objects * 100 / quota_max_objects))
            echo "  Quota Usage: ${quota_usage}%"

            if [ "$quota_usage" -ge 90 ]; then
                echo -e "  ${RED}WARNING: Quota usage above 90%${NC}"
            elif [ "$quota_usage" -ge 75 ] && [ "$SHOW_WARNINGS" = true ]; then
                echo -e "  ${YELLOW}WARNING: Quota usage above 75%${NC}"
            fi
        fi
    else
        echo "  Quota Enabled: No"
    fi

    # Check if resharding is needed
    if [ "$objs_per_shard" -gt "$THRESHOLD" ]; then
        echo -e "  ${RED}RECOMMENDATION: Consider resharding (${objs_per_shard} > ${THRESHOLD} objects/shard)${NC}"

        # Calculate recommended shards
        local recommended_shards=$((num_objects / THRESHOLD + 1))

        # Suggest nearest prime
        local primes=(7 11 23 53 101 211 503 1009 2003 5003 10007 20011 50021 100003)
        local suggested_shards=$recommended_shards
        for prime in "${primes[@]}"; do
            if [ "$prime" -ge "$recommended_shards" ]; then
                suggested_shards=$prime
                break
            fi
        done

        echo "  Recommended shards: $suggested_shards (nearest prime to $recommended_shards)"
        echo "  Command: radosgw-admin bucket reshard --bucket=$bucket --num-shards=$suggested_shards"
    elif [ "$objs_per_shard" -gt $((THRESHOLD * 75 / 100)) ] && [ "$SHOW_WARNINGS" = true ]; then
        echo -e "  ${YELLOW}INFO: Approaching threshold (${objs_per_shard} objects/shard, threshold: ${THRESHOLD})${NC}"
    else
        echo -e "  ${GREEN}OK: Within recommended limits${NC}"
    fi

    # Check resharding status
    local reshard_status=$(radosgw-admin reshard status --bucket="$bucket" 2>/dev/null || echo "")
    if echo "$reshard_status" | grep -q "in-progress"; then
        echo -e "  ${YELLOW}INFO: Resharding currently in progress${NC}"
    fi

    echo ""
}

# Main execution
if [ -n "$SPECIFIC_BUCKET" ]; then
    analyze_bucket "$SPECIFIC_BUCKET"
elif [ "$CHECK_ALL" = true ]; then
    echo "Fetching list of all buckets..."

    # Get list of all buckets
    if command -v jq &> /dev/null; then
        buckets=$(radosgw-admin bucket list 2>/dev/null | jq -r '.[]')
    else
        # Fallback parsing
        buckets=$(radosgw-admin bucket list 2>/dev/null | grep -o '"[^"]*"' | tr -d '"')
    fi

    if [ -z "$buckets" ]; then
        echo "No buckets found or error fetching bucket list"
        exit 1
    fi

    bucket_count=$(echo "$buckets" | wc -l)
    echo "Found $bucket_count buckets"
    echo ""

    current=0
    for bucket in $buckets; do
        current=$((current + 1))
        echo "[$current/$bucket_count]"
        analyze_bucket "$bucket"
    done
else
    echo "Error: Must specify either -b <bucket> or -a"
    usage
fi

echo "Analysis complete."
